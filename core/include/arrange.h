/*
 *  Copyright � 2006-2011 SplinterGU (Fenix/Bennugd)
 *  Copyright � 2002-2006 Fenix Team (Fenix)
 *  Copyright � 1999-2002 Jos� Luis Cebri�n Pag�e (Fenix)
 *
 *  This file is part of Bennu - Game Development
 *
 *  This software is provided 'as-is', without any express or implied
 *  warranty. In no event will the authors be held liable for any damages
 *  arising from the use of this software.
 *
 *  Permission is granted to anyone to use this software for any purpose,
 *  including commercial applications, and to alter it and redistribute it
 *  freely, subject to the following restrictions:
 *
 *     1. The origin of this software must not be misrepresented; you must not
 *     claim that you wrote the original software. If you use this software
 *     in a product, an acknowledgment in the product documentation would be
 *     appreciated but is not required.
 *
 *     2. Altered source versions must be plainly marked as such, and must not be
 *     misrepresented as being the original software.
 *
 *     3. This notice may not be removed or altered from any source
 *     distribution.
 *
 */

#ifndef __ARRANGE_ST_H
    #define __ARRANGE_ST_H

    #include <stdint.h>

    #define __BGD_LIL_ENDIAN 1234
    #define __BGD_BIG_ENDIAN 4321

    #if defined(__hppa__) || \
        defined(__m68k__) || \
        defined(mc68000) || \
        defined(_M_M68K) || \
        (defined(__MIPS__) && defined(__MISPEB__)) || \
        defined(__ppc__) || \
        defined(__POWERPC__) || \
        defined(_M_PPC) || \
        defined(__sparc__) || \
        defined(GEKKO)
        #define __BYTEORDER  __BGD_BIG_ENDIAN
    #else
        #define __BYTEORDER  __BGD_LIL_ENDIAN
    #endif

    /* ---------------------------------------------------------------------- */
    /* Trucos de portabilidad                                                 */
    /* ---------------------------------------------------------------------- */

    #if __BYTEORDER == __BGD_LIL_ENDIAN
        #define ARRANGE_DWORD(x)
        #define ARRANGE_WORD(x)

        #define ARRANGE_DWORDS(x,c)
        #define ARRANGE_WORDS(x,c)
    #else
        #if defined(__GNUC__) && ( defined(__powerpc__) || defined(__ppc__) || defined(GEKKO) )
            /* PowerPC optimized assembly code */
            static __inline__ void DO_Swap16(uint16_t * D)
            {
                uint16_t result;
	            __asm__("rlwimi %0,%2,8,16,23" : "=&r" (result) : "0" (*D >> 8), "r" (*D));

	            *D = result;
            }

            static __inline__ void DO_Swap32(uint32_t * D)
            {
                uint32_t result;

	            __asm__("rlwimi %0,%2,24,16,23" : "=&r" (result) : "0" (*D>>24), "r" (*D));
	            __asm__("rlwimi %0,%2,8,8,15"   : "=&r" (result) : "0" (result),    "r" (*D));
	            __asm__("rlwimi %0,%2,24,0,7"   : "=&r" (result) : "0" (result),    "r" (*D));

	            *D = result;
            }
        #else
            static __inline__ void DO_Swap16(uint16_t * D) {
                *D = ((*D<<8)|(*D>>8));
            }

            static __inline__ void DO_Swap32(uint32_t * D) {
                *D = ((*D<<24)|((*D<<8)&0x00FF0000)|((*D>>8)&0x0000FF00)|(*D>>24));
            }
        #endif


        #define ARRANGE_DWORD(x)    DO_Swap32(x)
        #define ARRANGE_WORD(x)     DO_Swap16(x)

        #define ARRANGE_DWORDS(x,c) {               \
            int __n;                                \
            uint32_t * __p = (uint32_t *)(x);       \
            for (__n = 0 ; __n < (int)(c) ; __n++)  \
                ARRANGE_DWORD(&__p[__n]);           \
            }
        #define ARRANGE_WORDS(x,c) {                \
            int __n;                                \
            uint16_t * __p = (uint16_t *)(x);       \
            for (__n = 0 ; __n < (int)(c) ; __n++)  \
                ARRANGE_WORD(&__p[__n]);            \
            }
    #endif
#endif
