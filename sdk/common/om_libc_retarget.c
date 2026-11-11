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
 * @file     om_libc_retarget.c
 * @brief
 * @date     06. Aug 2020
 * @author   OnMicro SW Team
 *
 * @version
 * Version 1.0
 *  - Initial release
 *
 * @{
 */

/*********************************************************************
 * INCLUDES
 */
#include <stdio.h>
#include "om_device.h"
#ifdef CONFIG_SHELL
#include "shell.h"
#endif

/*********************************************************************
 * MACROS
 */


/*********************************************************************
 * PUBLIC FUNCTIONS
 */

void ser_write(const char *ptr, int len)
{
    for(; len>0; --len, ++ptr)
    {
#ifdef CONFIG_SHELL
        if(*ptr == '\n')
            shell_out('\r');
        shell_out(*ptr);
#endif
    }
}

#if defined(__CC_ARM) ||  defined(__ICCARM__) || defined (__ARMCC_VERSION)

#if !defined(__GNUC__)
struct __FILE { int handle; /* Add whatever you need here */ };
FILE __stdout;
FILE __stdin;
#endif

int fputc(int c, FILE *f)
{
    ser_write((const char *)&c, 1);
    return c;
}

int fgetc(FILE *f)
{
    return -1;
}

int ferror(FILE *f)
{
    return EOF;
}

void _ttywrch(int c)
{
    ser_write((const char *)&c, 1);
}

void _sys_exit(int return_code)
{
    while(1);
}

#elif defined(__GNUC__)

#include <sys/stat.h>
#include <assert.h>

// Default _sbrk just return 'end' value (defined in linker).
caddr_t _sbrk(int incr)
{
#ifdef CONFIG_STACK_DEBUG
    // In Stack Debug mode, can't use the real the stack for sbrk, it will effect co_stack_check()
    static uint8_t fake_stack[2048];
    uint8_t * const stack_base  = fake_stack;
    uint8_t * const stack_limit = fake_stack + sizeof(fake_stack);
#else
    extern unsigned __StackLimit;
    uint8_t * const stack_base  = (uint8_t *)&__StackLimit;
    uint8_t * const stack_limit = (uint8_t *)__get_MSP();
#endif

    static uint8_t* psbrk = stack_base;
    uint8_t*        prev  = psbrk;
    uint8_t*        next  = psbrk + incr;

    // Use assert repace 'return -1'
    // Not use OM_ASSERT, it may trigger next this fault
#ifdef NDEBUG
    while (next >= stack_limit);
#else
    assert (next < stack_limit);
#endif

    psbrk = next;

    return (caddr_t) prev;
}

int _write(int file, const char * ptr, int len)
{
    ser_write(ptr, len);
    return len;
}

int _read(int file, char * ptr, int len)
{
    return 0;
}

int _close(int file) {
    return -1;
}
int _lseek(int file, int offset, int whence) {
    return -1;
}
int _fstat(int file, struct stat *st) {
    return -1;
}
int _isatty(int file) {
    return 0;
}

#endif

/** @} */


