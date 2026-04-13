#pragma once

#include "Arduino.h"

// Definizione compatibile di FONT_STORAGE per ESP32 e AVR
#ifdef ESP32
        #define FONT_STORAGE const
#else
        #include <avr/pgmspace.h>
        #define FONT_STORAGE const PROGMEM
#endif


struct FontType {
        const uint8_t __0[8];
        const uint8_t __1[8];
        const uint8_t __2[8];
        const uint8_t __3[8];
        const uint8_t __4[8];
        const uint8_t __5[8];
        const uint8_t __6[8];
        const uint8_t __7[8];
        const uint8_t __8[8];
        const uint8_t __9[8];

        const uint8_t __A[8];
        const uint8_t __B[8];
        const uint8_t __C[8];
        const uint8_t __D[8];
        const uint8_t __E[8];
        const uint8_t __F[8];
        const uint8_t __G[8];
        const uint8_t __H[8];
        const uint8_t __I[8];
        const uint8_t __J[8];
        const uint8_t __K[8];
        const uint8_t __L[8];
        const uint8_t __M[8];
        const uint8_t __N[8];
        const uint8_t __O[8];
        const uint8_t __P[8];
        const uint8_t __Q[8];
        const uint8_t __R[8];
        const uint8_t __S[8];
        const uint8_t __T[8];
        const uint8_t __U[8];
        const uint8_t __V[8];
        const uint8_t __W[8];
        const uint8_t __X[8];
        const uint8_t __Y[8];
        const uint8_t __Z[8];

        const uint8_t __a[8];
        const uint8_t __b[8];
        const uint8_t __c[8];
        const uint8_t __d[8];
        const uint8_t __e[8];
        const uint8_t __f[8];
        const uint8_t __g[8];
        const uint8_t __h[8];
        const uint8_t __i[8];
        const uint8_t __j[8];
        const uint8_t __k[8];
        const uint8_t __l[8];
        const uint8_t __m[8];
        const uint8_t __n[8];
        const uint8_t __o[8];
        const uint8_t __p[8];
        const uint8_t __q[8];
        const uint8_t __r[8];
        const uint8_t __s[8];
        const uint8_t __t[8];
        const uint8_t __u[8];
        const uint8_t __v[8];
        const uint8_t __w[8];
        const uint8_t __x[8];
        const uint8_t __y[8];
        const uint8_t __z[8];

        const uint8_t __space[8];
        const uint8_t __exc_mark[8];
        const uint8_t __quotes[8];
        const uint8_t __sharp[8];
        const uint8_t __dollar[8];
        const uint8_t __percent[8];
        const uint8_t __ampersand[8];
        const uint8_t __apex[8];
        const uint8_t __par_open[8];
        const uint8_t __par_close[8];
        const uint8_t __asterisk[8];
        const uint8_t __plus[8];
        const uint8_t __comma[8];
        const uint8_t __minus[8];
        const uint8_t __dot[8];
        const uint8_t __slash[8];

        const uint8_t __colon[8];
        const uint8_t __semicolon[8];   
        const uint8_t __left_ang[8];   
        const uint8_t __equal[8];
        const uint8_t __right_ang[8];
        const uint8_t __quest_mark[8];

        // special characters
        const uint8_t __heart[8];
        const uint8_t __heart_fill[8];
        const uint8_t __smile[8];
        const uint8_t __sad[8];
        const uint8_t __flash[8];
        const uint8_t __flash_bold[8];
};

