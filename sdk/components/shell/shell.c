/* ----------------------------------------------------------------------------
 * Copyright (c) 2020-2030 OnMicro Limited. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of OnMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * -------------------------------------------------------------------------- */

/**
 * @file     shell.c
 * @brief    Simple command line shell
 * @date     18 Feb. 2022
 * @author   OnMicro SW Team
 *
 * @version
 * Version 1.0
 *  - Initial release
 *
 * @{
 */


/*******************************************************************************
 * INCLUDES
 */
#include <stddef.h>
#include <string.h>
#include <stdint.h>
#include "shell_cmd.h"
#include "shell.h"

#ifdef CONFIG_SHELL

/*******************************************************************************
 * EXTERN VARIABLES
 */
extern const shell_cmd_t shell_cmd[];


/*******************************************************************************
 * MACROS
 */


/*******************************************************************************
 * LOCAL FUNCTIONS
 */
static char *_strtok(char *str, const char *delim, char **saveptr)
{
    char *token;

    if (str) {
        *saveptr = str;
    }
    token = *saveptr;

    if (!token) {
        return NULL;
    }

    token += strspn(token, delim);
    *saveptr = strpbrk(token, delim);
    if (*saveptr) {
        *(*saveptr)++ = '\0';
    }

    return *token ? token : NULL;
}

static void list_commands(const shell_cmd_t *scp)
{
    if(scp) {
        while (scp->sc_name != NULL) {
            if(strcmp(scp->sc_name, SHELL_CMD_SPLIT) == 0) {
                shell_printf("\n%s\n", scp->description);
            } else {
                shell_printf("%16s\t%s\n", scp->sc_name, scp->description);
            }
            scp++;
        }
    }
}

static char cmdexec(const shell_cmd_t *scp, char *name, int argc, char *argv[])
{
    if (scp) {
        while (scp->sc_name != NULL) {
            if (strcasecmp(scp->sc_name, name) == 0) {
                scp->sc_function(argc, argv);
                return 0;
            }
            scp++;
        }
    }

    return 1;
}

/**
 * @brief   Shell thread function.
 */
void shell_main(char *line, const shell_cmd_t *scp)
{
    int n;
    char *lp, *cmd, *tokp;
    char *args[SHELL_MAX_ARGUMENTS + 1];

    shell_printf("\n");
    tokp = NULL;
    lp = _strtok(line, " \t", &tokp);
    cmd = lp;
    n = 0;

    while((lp = _strtok(NULL, " \t", &tokp)) != NULL) {
        if(n >= SHELL_MAX_ARGUMENTS) {
            shell_printf("shell: Too many arguments\n");
            cmd = NULL;
            return;
        }
        args[n++] = lp;
    }
    args[n] = NULL;
    if(cmd != NULL) {
        if(strcasecmp(cmd, "help") == 0) {
            shell_printf("Commands:\n            help\thelp\n");
            list_commands(shell_cmd);
            list_commands(scp);
        } else if(cmdexec(scp, cmd, n, args) && cmdexec(shell_cmd, cmd, n, args)) {
            shell_printf("%s\n", cmd);
            shell_printf(" ? Use \'help\' to get commands\n");
        }
    }
}

/**
 * @brief   Reads a whole line from the input channel.
 *
 * @param[in] line        pointer to the line buffer
 * @param[in] line_size   buffer maximum length
 * @return                The operation status.
 * @retval true           the channel was reset or CTRL-D pressed.
 * @retval false          operation successful.
 *
 * @api
 */
bool shell_get_line(char c, char *line, unsigned line_size, unsigned *line_index)
{
    if(c == 4) {
        shell_printf("^D\n");
        *line_index = 0;
        return true;
    }
    if((c == 8) || (c == 127)) {    //ASCII 8: backspace,  127:DEL
        if(*line_index != 0x00) {
            shell_out(c);
            shell_out(0x20);
            shell_out(c);
            (*line_index)--;
            line[*line_index] = '\0';  //lint !e661
        }
        return false;
    }
    if(c == '\r' || c == '\n') {
        shell_printf("\n");
        return true;
    }
    if(c < 0x20) {
        return false;
    }
    if((*line_index) < (line_size - 1)) {
        shell_out(c);
        line[*line_index] = c;
        (*line_index)++;
        return false;
    }

    return false;
}

#endif

/** @} */
