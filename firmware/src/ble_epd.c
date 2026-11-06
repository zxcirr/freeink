/*
 * Minimal BLE image transfer service for ZKC21V / ZKC42V.
 *
 * Service: 6E400001-B5A3-F393-E0A9-E50E24DCCA9E
 *   CMD  : 6E400002-...  write + notify
 *   DATA : 6E400003-...  write (chunks: u32 offset LE + payload)
 *
 * Protocol (little-endian):
 *   CMD  [opcode, seq, payload...]
 *   NTF  [opcode|0x80, seq, status, payload...]
 *
 *   0x01 INFO   -> 0x81 status, version, width u16, height u16, format,
 *                       max_chunk u16
 *   0x02 BEGIN  width u16, height u16, format, total u32, crc32 u32
 *               -> 0x82 status, max_chunk u16
 *   0x03 COMMIT crc32 u32 -> 0x83 status; later 0xE2 display done
 *   0x04 ABORT  -> 0x84 status
 *   0x05 CLEAR  -> 0x85 status; later 0xE2 done
 *
 * Image format 2 = two 1bpp planes:
 *   plane 0: 1 = white, 0 = black
 *   plane 1: 1 = no red, 0 = red
 *
 * This is intentionally a small, self-contained protocol. It is not the
 * InkTag protocol and does not add any InkTag branding.
 */

#include <string.h>
#include "om.h"
#include "om_driver.h"
#include "omble.h"
#include "board.h"
#include "epd.h"
#include "ble_epd.h"
#include "led.h"

#define EPD_SERVICE_UUID_BYTES \
    { 0x9E, 0xCA, 0xDC, 0x24, 0x0E, 0xE5, 0xA9, 0xE0, \
      0x93, 0xF3, 0xA3, 0xB5, 0x01, 0x00, 0x40, 0x6E }
#define EPD_CMD_UUID_BYTES \
    { 0x9E, 0xCA, 0xDC, 0x24, 0x0E, 0xE5, 0xA9, 0xE0, \
      0x93, 0xF3, 0xA3, 0xB5, 0x02, 0x00, 0x40, 0x6E }
#define EPD_DATA_UUID_BYTES \
    { 0x9E, 0xCA, 0xDC, 0x24, 0x0E, 0xE5, 0xA9, 0xE0, \
      0x93, 0xF3, 0xA3, 0xB5, 0x03, 0x00, 0x40, 0x6E }

#define CMD_INFO        0x01
#define CMD_BEGIN       0x02
#define CMD_COMMIT      0x03
#define CMD_ABORT       0x04
#define CMD_CLEAR       0x05
#define EVT_DONE        0xE2
#define EVT_ERROR       0xE6

#define STATUS_OK       0x00
#define STATUS_INVALID  0x01
#define STATUS_BUSY     0x02
#define STATUS_CRC      0x03
#define STATUS_SIZE     0x04
#define STATUS_STATE    0x05

#define INVALID_CONN    0xFF

static const uint8_t service_uuid[16] = EPD_SERVICE_UUID_BYTES;
static const uint8_t cmd_uuid[16] = EPD_CMD_UUID_BYTES;
static const uint8_t data_uuid[16] = EPD_DATA_UUID_BYTES;

static const ob_gatt_item_t atts[] = {
    { ob_att_char_def, OB_UUID_16BIT, OB_ATT_PROP_READ },
    { cmd_uuid,  OB_UUID_128BIT, OB_ATT_PROP_WRITE | OB_ATT_PROP_NTF, 0, 0 },
    { ob_att_cccd_def, OB_UUID_16BIT, OB_ATT_PROP_READ | OB_ATT_PROP_WRITE },
    { ob_att_char_def, OB_UUID_16BIT, OB_ATT_PROP_READ },
    { data_uuid, OB_UUID_128BIT, OB_ATT_PROP_WRITE | OB_ATT_PROP_WRITE_CMD, 0, 0 },
};

static const ob_gatt_serv_t serv = {
    service_uuid, OB_UUID_128BIT,
    sizeof(atts) / sizeof(atts[0]), atts
};

static uint16_t s_start_handle;
static uint8_t s_conn_idx = INVALID_CONN;
static uint16_t s_mtu = 23;

static uint8_t s_image[EPD_IMAGE_BYTES];
static uint32_t s_expected;
static uint32_t s_received;
static uint32_t s_crc_expected;
static uint16_t s_width;
static uint16_t s_height;
static uint8_t s_format;
static uint8_t s_transfer_active;
static volatile uint8_t s_pending;   /* 1 = display image, 2 = clear */
static uint8_t s_pending_seq;

static uint8_t s_adv[] = {
    0x02, 0x01, 0x06,
    0x07, 0x09, 'Z', 'K', 'C', '4', '2', 'V'
};

static uint8_t s_scan[] = {
    0x11, 0x07,
    0x9E, 0xCA, 0xDC, 0x24, 0x0E, 0xE5, 0xA9, 0xE0,
    0x93, 0xF3, 0xA3, 0xB5, 0x01, 0x00, 0x40, 0x6E
};

#if defined(BOARD_ZKC21V)
static uint8_t s_random_addr[6] = { 0xC0, 0x21, 0x00, 0x00, 0x5A, 0xA5 };
#else
static uint8_t s_random_addr[6] = { 0xC0, 0x42, 0x00, 0x00, 0x5A, 0xA5 };
#endif
static ob_adv_param_t s_adv_param;
static ob_data_t s_adv_data = { s_adv, sizeof(s_adv) };
static ob_data_t s_scan_data = { s_scan, sizeof(s_scan) };

static uint16_t rd16(const uint8_t *p)
{
    return (uint16_t)p[0] | ((uint16_t)p[1] << 8);
}

static uint32_t rd32(const uint8_t *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

static uint32_t crc32(const uint8_t *data, uint32_t len)
{
    uint32_t crc = 0xFFFFFFFFu;
    while (len--) {
        crc ^= *data++;
        for (int i = 0; i < 8; i++)
            crc = (crc >> 1) ^ (0xEDB88320u & (0u - (crc & 1u)));
    }
    return ~crc;
}

static uint16_t max_chunk(void)
{
    /* DATA payload = 4-byte offset + chunk. ATT overhead = 3 bytes. */
    if (s_mtu <= 7)
        return 1;
    uint16_t n = s_mtu - 3 - 4;
    return n > 240 ? 240 : n;
}

static void notify(const uint8_t *data, uint16_t len)
{
    if (s_conn_idx == INVALID_CONN)
        return;
    ob_gatts_hvx_t hvx = {
        OB_HANDLE_VALUE_NTF,
        s_start_handle + 2,     /* CMD value handle */
        data,
        len,
        0
    };
    ob_gatts_send_hvx(s_conn_idx, &hvx);
}

static void reply(uint8_t opcode, uint8_t seq, uint8_t status,
                  const uint8_t *payload, uint16_t len)
{
    uint8_t buf[24];
    if (len > sizeof(buf) - 3)
        len = sizeof(buf) - 3;
    buf[0] = opcode | 0x80;
    buf[1] = seq;
    buf[2] = status;
    if (len)
        memcpy(&buf[3], payload, len);
    if (status != STATUS_OK)
        led_set_pattern(LED_ERROR);
    notify(buf, len + 3);
}

static void reset_transfer(void)
{
    s_expected = 0;
    s_received = 0;
    s_crc_expected = 0;
    s_width = 0;
    s_height = 0;
    s_format = 0;
    s_transfer_active = 0;
}

static void adv_start(void)
{
    s_adv_param.own_addr_type = OB_ADV_ADDR_TYPE_RANDOM;
    s_adv_param.prim_phy = OB_ADV_PHY_1M;
    s_adv_param.secd_phy = OB_ADV_PHY_1M;
    s_adv_param.tx_pwr = 0;
    s_adv_param.filter_policy = OB_ADV_FILTER_NONE;
    s_adv_param.prim_ch_map = OB_ADV_CH_ALL;
    s_adv_param.prim_intv_min = 0xA0;
    s_adv_param.prim_intv_max = 0xA0;
    s_adv_param.adv_properties = OB_ADV_PROP_LEGACY_IND;
    s_adv_param.local_addr = s_random_addr;
    s_adv_param.peer_addr = NULL;
    ob_gap_adv_start(0, &s_adv_param, &s_adv_data, &s_scan_data);
}

static void handle_command(const uint8_t *data, uint16_t len)
{
    if (len < 2)
        return;

    uint8_t opcode = data[0];
    uint8_t seq = data[1];
    const uint8_t *p = data + 2;
    uint16_t plen = len - 2;

    switch (opcode) {
    case CMD_INFO: {
        uint8_t out[8];
        uint16_t chunk = max_chunk();
        out[0] = 0x01;                 /* protocol version */
        out[1] = EPD_WIDTH & 0xFF;
        out[2] = EPD_WIDTH >> 8;
        out[3] = EPD_HEIGHT & 0xFF;
        out[4] = EPD_HEIGHT >> 8;
        out[5] = EPD_DEFAULT_FORMAT; /* 2 = dual 1bpp, 3 = 2bpp indexed */
        out[6] = chunk & 0xFF;
        out[7] = chunk >> 8;
        reply(opcode, seq, STATUS_OK, out, sizeof(out));
        break;
    }
    case CMD_BEGIN: {
        if (plen < 13) {
            reply(opcode, seq, STATUS_INVALID, NULL, 0);
            break;
        }
        uint16_t w = rd16(p);
        uint16_t h = rd16(p + 2);
        uint8_t fmt = p[4];
        uint32_t total = rd32(p + 5);
        uint32_t crc = rd32(p + 9);

        if (w != EPD_WIDTH || h != EPD_HEIGHT || fmt != EPD_DEFAULT_FORMAT ||
            total != EPD_IMAGE_BYTES || total > sizeof(s_image)) {
            reply(opcode, seq, STATUS_SIZE, NULL, 0);
            break;
        }

        reset_transfer();
        s_width = w;
        s_height = h;
        s_format = fmt;
        s_expected = total;
        s_crc_expected = crc;
        s_transfer_active = 1;

        uint8_t mc[2] = { max_chunk() & 0xFF, max_chunk() >> 8 };
        led_set_pattern(LED_TRANSFER);
        reply(opcode, seq, STATUS_OK, mc, 2);
        break;
    }
    case CMD_COMMIT: {
        if (plen < 4) {
            reply(opcode, seq, STATUS_INVALID, NULL, 0);
            break;
        }
        uint32_t crc = rd32(p);
        if (!s_transfer_active) {
            reply(opcode, seq, STATUS_STATE, NULL, 0);
            break;
        }
        if (s_received != s_expected) {
            reply(opcode, seq, STATUS_SIZE, NULL, 0);
            break;
        }
        if (crc != s_crc_expected || crc32(s_image, s_expected) != s_crc_expected) {
            reply(opcode, seq, STATUS_CRC, NULL, 0);
            break;
        }
        s_transfer_active = 0;
        s_pending_seq = seq;
        s_pending = 1;
        led_set_pattern(LED_DISPLAY);
        reply(opcode, seq, STATUS_OK, NULL, 0);
        break;
    }
    case CMD_ABORT:
        reset_transfer();
        led_set_pattern(LED_CONNECTED);
        reply(opcode, seq, STATUS_OK, NULL, 0);
        break;
    case CMD_CLEAR:
        reset_transfer();
        s_pending_seq = seq;
        s_pending = 2;
        led_set_pattern(LED_DISPLAY);
        reply(opcode, seq, STATUS_OK, NULL, 0);
        break;
    default:
        reply(opcode, seq, STATUS_INVALID, NULL, 0);
        break;
    }
}

static void handle_data(const uint8_t *data, uint16_t len)
{
    if (!s_transfer_active || len < 4)
        return;
    uint32_t offset = rd32(data);
    uint16_t n = len - 4;
    if (offset != s_received || offset + n > s_expected) {
        return; /* host will fail on commit CRC/size */
    }
    memcpy(&s_image[offset], data + 4, n);
    s_received += n;
}

static void ble_event_cb(uint16_t evt_id, const omble_evt_t *evt)
{
    switch (evt_id) {
    case OB_GAP_EVT_CONNECTED:
        s_conn_idx = evt->gap.conn_idx;
        s_mtu = 23;
        reset_transfer();
        led_set_connected(1);
        break;

    case OB_GAP_EVT_DISCONNECTED:
        s_conn_idx = INVALID_CONN;
        reset_transfer();
        led_set_connected(0);
        adv_start();
        break;

    case OB_GATT_EVT_MTU_EXCHANGED:
        s_mtu = evt->gatt.mtu_exchanged.mtu;
        break;

    case OB_GATTS_EVT_WRITE_REQ: {
        uint16_t hdl = evt->gatt.write_req.att_hdl;
        const uint8_t *data = evt->gatt.write_req.data;
        uint16_t len = evt->gatt.write_req.len;

        if (hdl == s_start_handle + 2)
            handle_command(data, len);
        else if (hdl == s_start_handle + 5)
            handle_data(data, len);
        break;
    }

    default:
        break;
    }
}

void ble_epd_init(void)
{
    uint16_t handle = 0;

    /* Update the advertised name without relying on a writable string. */
    const char *name = BLE_NAME;
    uint8_t n = 0;
    while (name[n] && n < 6) {
        s_adv[5 + n] = (uint8_t)name[n];
        n++;
    }
    s_adv[3] = n + 1;

    ob_gatts_add_service(&serv, &handle);
    s_start_handle = handle;
    ob_event_callback_reg(ble_event_cb);
    adv_start();
    led_set_connected(0);
}

void ble_epd_task(void)
{
    if (!s_pending)
        return;

    uint8_t op = s_pending;
    uint8_t seq = s_pending_seq;
    s_pending = 0;

    if (op == 1) {
        if (s_format == 3) {
            epd_show_indexed(s_image);
        } else {
            epd_show(s_image, s_image + EPD_PLANE_BYTES);
        }
        led_set_pattern(LED_SUCCESS);
        uint8_t done[3] = { EVT_DONE, seq, STATUS_OK };
        notify(done, sizeof(done));
    } else if (op == 2) {
        epd_clear();
        led_set_pattern(LED_SUCCESS);
        uint8_t done[3] = { EVT_DONE, seq, STATUS_OK };
        notify(done, sizeof(done));
    }
}
