
# reset load and run
define om_reset

# reset
mon reset 1

#Disable WDT
set *0x400e00e8 = 0x6666

# Reset pinmux
set *0x40000080 = 0x00000000
set *0x40000084 = 0x00000000
set *0x40000088 = 0x00001c00
set *0x4000008C = 0x00000000
set *0x40000090 = 0x00000000
set *0x40000094 = 0x00000000
set *0x40000098 = 0x00000000

# Disable All IRQ
set *0xE000E180 = 0xFFFFFFFF
set *0xE000E184 = 0xFFFFFFFF
set *0xE000E010 = 0

# Invalid Cache
set *0xE0042004 = 1
set *0xE0042008 = 0
set *0xE0042020 = 1
set *0xE0042004 = 0
set *0xE0042008 = 1

# load code
restore ../../../hal/device/om6626/rom_lib/current/misc/ram_resident_data_by_rom.hex

# invalid jlink cache and skip compare
mon exec InvalidateCache
mon exec SetCompareMode = 0

# load
load

# set SP/PC
set $sp = *0x50004000
set $pc = *0x50004004

# run
c

end

