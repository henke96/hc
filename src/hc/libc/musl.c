/*
musl as a whole is licensed under the following standard MIT license:

----------------------------------------------------------------------
Copyright © 2005-2020 Rich Felker, et al.

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
----------------------------------------------------------------------
*/

#if !hc_LIBC
// Symbols expected by clang in freestanding mode.
void *memset(void *dest, int32_t c, size_t n) hc_NO_BUILTIN
{
    unsigned char *s = dest;
    uint64_t k;

    /* Fill head and tail with minimal branching. Each
     * conditional ensures that all the subsequently used
     * offsets are well-defined and in the dest region. */

    if (!n) return dest;
    s[0] = (unsigned char)c;
    s[n-1] = (unsigned char)c;
    if (n <= 2) return dest;
    s[1] = (unsigned char)c;
    s[2] = (unsigned char)c;
    s[n-2] = (unsigned char)c;
    s[n-3] = (unsigned char)c;
    if (n <= 6) return dest;
    s[3] = (unsigned char)c;
    s[n-4] = (unsigned char)c;
    if (n <= 8) return dest;

    /* Advance pointer to align it at a 4-byte boundary,
     * and truncate n to a multiple of 4. The previous code
     * already took care of any head/tail that get cut off
     * by the alignment. */

    k = -(uint64_t)s & 3;
    s += k;
    n -= k;
    n &= (uint64_t)-4;

    typedef uint32_t __attribute__((__may_alias__)) u32;
    typedef uint64_t __attribute__((__may_alias__)) u64;

    u32 c32 = ((u32)-1)/255 * (unsigned char)c;

    /* In preparation to copy 32 bytes at a time, aligned on
     * an 8-byte bounary, fill head/tail up to 28 bytes each.
     * As in the initial byte-based head/tail fill, each
     * conditional below ensures that the subsequent offsets
     * are valid (e.g. !(n<=24) implies n>=28). */

    *(u32 *)hc_ASSUME_ALIGNED(s+0, 4) = c32;
    *(u32 *)hc_ASSUME_ALIGNED(s+n-4, 4) = c32;
    if (n <= 8) return dest;
    *(u32 *)hc_ASSUME_ALIGNED(s+4, 4) = c32;
    *(u32 *)hc_ASSUME_ALIGNED(s+8, 4) = c32;
    *(u32 *)hc_ASSUME_ALIGNED(s+n-12, 4) = c32;
    *(u32 *)hc_ASSUME_ALIGNED(s+n-8, 4) = c32;
    if (n <= 24) return dest;
    *(u32 *)hc_ASSUME_ALIGNED(s+12, 4) = c32;
    *(u32 *)hc_ASSUME_ALIGNED(s+16, 4) = c32;
    *(u32 *)hc_ASSUME_ALIGNED(s+20, 4) = c32;
    *(u32 *)hc_ASSUME_ALIGNED(s+24, 4) = c32;
    *(u32 *)hc_ASSUME_ALIGNED(s+n-28, 4) = c32;
    *(u32 *)hc_ASSUME_ALIGNED(s+n-24, 4) = c32;
    *(u32 *)hc_ASSUME_ALIGNED(s+n-20, 4) = c32;
    *(u32 *)hc_ASSUME_ALIGNED(s+n-16, 4) = c32;

    /* Align to a multiple of 8 so we can fill 64 bits at a time,
     * and avoid writing the same bytes twice as much as is
     * practical without introducing additional branching. */

    k = 24 + ((uint64_t)s & 4);
    s += k;
    n -= k;

    /* If this loop is reached, 28 tail bytes have already been
     * filled, so any remainder when n drops below 32 can be
     * safely ignored. */

    u64 c64 = c32 | ((u64)c32 << 32);
    for (; n >= 32; n-=32, s+=32) {
        *(u64 *)hc_ASSUME_ALIGNED(s+0, 8) = c64;
        *(u64 *)hc_ASSUME_ALIGNED(s+8, 8) = c64;
        *(u64 *)hc_ASSUME_ALIGNED(s+16, 8) = c64;
        *(u64 *)hc_ASSUME_ALIGNED(s+24, 8) = c64;
    }

    return dest;
}

void *memmove(void *dest, const void *src, size_t n) hc_NO_BUILTIN
{
    typedef __attribute__((__may_alias__)) uint64_t WT;
#define WS (sizeof(WT))

    char *d = dest;
    const char *s = src;

    if (d==s) return d;

    if (d<s) {
        if ((uint64_t)s % WS == (uint64_t)d % WS) {
            while ((uint64_t)d % WS) {
                if (!n--) return dest;
                *d++ = *s++;
            }
            for (; n>=WS; n-=WS, d+=WS, s+=WS) *(WT *)hc_ASSUME_ALIGNED(d, 8) = *(WT *)hc_ASSUME_ALIGNED(s, 8);
        }
        for (; n; n--) *d++ = *s++;
    } else {
        if ((uint64_t)s % WS == (uint64_t)d % WS) {
            while ((uint64_t)(d+n) % WS) {
                if (!n--) return dest;
                d[n] = s[n];
            }
            while (n>=WS) (void)(n-=WS), *(WT *)hc_ASSUME_ALIGNED(d+n, 8) = *(WT *)hc_ASSUME_ALIGNED(s+n, 8);
        }
        while (n) (void)n--, d[n] = s[n];
    }

    return dest;
}

void *memcpy(void *restrict dest, const void *restrict src, size_t n) hc_NO_BUILTIN
{
    unsigned char *d = dest;
    const unsigned char *s = src;

#define LS >>
#define RS <<

    typedef uint32_t __attribute__((__may_alias__)) u32;
    uint32_t w, x;

    for (; (uint64_t)s % 4 && n; n--) *d++ = *s++;

    if ((uint64_t)d % 4 == 0) {
        for (; n>=16; s+=16, d+=16, n-=16) {
            *(u32 *)hc_ASSUME_ALIGNED(d+0, 4) = *(u32 *)hc_ASSUME_ALIGNED(s+0, 4);
            *(u32 *)hc_ASSUME_ALIGNED(d+4, 4) = *(u32 *)hc_ASSUME_ALIGNED(s+4, 4);
            *(u32 *)hc_ASSUME_ALIGNED(d+8, 4) = *(u32 *)hc_ASSUME_ALIGNED(s+8, 4);
            *(u32 *)hc_ASSUME_ALIGNED(d+12, 4) = *(u32 *)hc_ASSUME_ALIGNED(s+12, 4);
        }
        if (n&8) {
            *(u32 *)hc_ASSUME_ALIGNED(d+0, 4) = *(u32 *)hc_ASSUME_ALIGNED(s+0, 4);
            *(u32 *)hc_ASSUME_ALIGNED(d+4, 4) = *(u32 *)hc_ASSUME_ALIGNED(s+4, 4);
            d += 8; s += 8;
        }
        if (n&4) {
            *(u32 *)hc_ASSUME_ALIGNED(d+0, 4) = *(u32 *)hc_ASSUME_ALIGNED(s+0, 4);
            d += 4; s += 4;
        }
        if (n&2) {
            *d++ = *s++; *d++ = *s++;
        }
        if (n&1) {
            *d = *s;
        }
        return dest;
    }

    if (n >= 32) switch ((uint64_t)d % 4) {
    case 1:
        w = *(u32 *)hc_ASSUME_ALIGNED(s, 4);
        *d++ = *s++;
        *d++ = *s++;
        *d++ = *s++;
        n -= 3;
        for (; n>=17; s+=16, d+=16, n-=16) {
            x = *(u32 *)hc_ASSUME_ALIGNED(s+1, 4);
            *(u32 *)hc_ASSUME_ALIGNED(d+0, 4) = (w LS 24) | (x RS 8);
            w = *(u32 *)hc_ASSUME_ALIGNED(s+5, 4);
            *(u32 *)hc_ASSUME_ALIGNED(d+4, 4) = (x LS 24) | (w RS 8);
            x = *(u32 *)hc_ASSUME_ALIGNED(s+9, 4);
            *(u32 *)hc_ASSUME_ALIGNED(d+8, 4) = (w LS 24) | (x RS 8);
            w = *(u32 *)hc_ASSUME_ALIGNED(s+13, 4);
            *(u32 *)hc_ASSUME_ALIGNED(d+12, 4) = (x LS 24) | (w RS 8);
        }
        break;
    case 2:
        w = *(u32 *)hc_ASSUME_ALIGNED(s, 4);
        *d++ = *s++;
        *d++ = *s++;
        n -= 2;
        for (; n>=18; s+=16, d+=16, n-=16) {
            x = *(u32 *)hc_ASSUME_ALIGNED(s+2, 4);
            *(u32 *)hc_ASSUME_ALIGNED(d+0, 4) = (w LS 16) | (x RS 16);
            w = *(u32 *)hc_ASSUME_ALIGNED(s+6, 4);
            *(u32 *)hc_ASSUME_ALIGNED(d+4, 4) = (x LS 16) | (w RS 16);
            x = *(u32 *)hc_ASSUME_ALIGNED(s+10, 4);
            *(u32 *)hc_ASSUME_ALIGNED(d+8, 4) = (w LS 16) | (x RS 16);
            w = *(u32 *)hc_ASSUME_ALIGNED(s+14, 4);
            *(u32 *)hc_ASSUME_ALIGNED(d+12, 4) = (x LS 16) | (w RS 16);
        }
        break;
    case 3:
        w = *(u32 *)hc_ASSUME_ALIGNED(s, 4);
        *d++ = *s++;
        n -= 1;
        for (; n>=19; s+=16, d+=16, n-=16) {
            x = *(u32 *)hc_ASSUME_ALIGNED(s+3, 4);
            *(u32 *)hc_ASSUME_ALIGNED(d+0, 4) = (w LS 8) | (x RS 24);
            w = *(u32 *)hc_ASSUME_ALIGNED(s+7, 4);
            *(u32 *)hc_ASSUME_ALIGNED(d+4, 4) = (x LS 8) | (w RS 24);
            x = *(u32 *)hc_ASSUME_ALIGNED(s+11, 4);
            *(u32 *)hc_ASSUME_ALIGNED(d+8, 4) = (w LS 8) | (x RS 24);
            w = *(u32 *)hc_ASSUME_ALIGNED(s+15, 4);
            *(u32 *)hc_ASSUME_ALIGNED(d+12, 4) = (x LS 8) | (w RS 24);
        }
        break;
    }
    if (n&16) {
        *d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
        *d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
        *d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
        *d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
    }
    if (n&8) {
        *d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
        *d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
    }
    if (n&4) {
        *d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
    }
    if (n&2) {
        *d++ = *s++; *d++ = *s++;
    }
    if (n&1) {
        *d = *s;
    }
    return dest;
}

int32_t memcmp(const void *vl, const void *vr, size_t n) hc_NO_BUILTIN
{
    const unsigned char *l=vl, *r=vr;
    for (; n && *l == *r; n--, l++, r++);
    return n ? *l-*r : 0;
}
#endif
