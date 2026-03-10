#include "avr/dtostrf.h"
#include <string.h>

char *dtostrf(double val, signed char width, unsigned char prec, char *sout)
{
    char *p = sout;
    int neg = 0;

    if (val < 0.0) {
        neg = 1;
        val = -val;
    }

    /* Round to the requested precision. */
    double rounding = 0.5;
    for (unsigned char i = 0; i < prec; i++) {
        rounding *= 0.1;
    }
    val += rounding;

    /* Integer part. */
    unsigned long ipart = (unsigned long)val;
    double fpart = val - (double)ipart;

    /* Convert integer part to string (reversed). */
    char ibuf[12];
    int ilen = 0;
    if (ipart == 0) {
        ibuf[ilen++] = '0';
    } else {
        while (ipart > 0) {
            ibuf[ilen++] = '0' + (char)(ipart % 10);
            ipart /= 10;
        }
    }

    /* Write sign and integer digits. */
    if (neg) {
        *p++ = '-';
    }
    for (int i = ilen - 1; i >= 0; i--) {
        *p++ = ibuf[i];
    }

    /* Fractional part. */
    if (prec > 0) {
        *p++ = '.';
        for (unsigned char i = 0; i < prec; i++) {
            fpart *= 10.0;
            int digit = (int)fpart;
            *p++ = '0' + (char)digit;
            fpart -= digit;
        }
    }
    *p = '\0';

    /* Right-justify within the requested width. */
    int len = (int)strlen(sout);
    if (len < width) {
        int pad = width - len;
        memmove(sout + pad, sout, (size_t)len + 1);
        memset(sout, ' ', (size_t)pad);
    }

    return sout;
}
