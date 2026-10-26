/* SPDX-License-Identifier: MIT
 * Readable reconstruction of seven original GameSir ARM64 row helpers.
 * Analysis artifact: not linked into Mesa and not a successful Ghidra C export.
 * Domain: little-endian, ordinary non-volatile byte buffers, valid non-wrapping
 * row extents and 64-bit flat addresses. Scalar overlap handling loads a whole
 * source pixel before writing it; vector blocks are selected only for disjoint
 * complete rows. This preserves row bytes, not SIMD/fault/race timing.
 * Build, exhaustive arithmetic and row/alias validation remain deferred.
 */
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

_Static_assert(UINTPTR_MAX == UINT64_MAX, "Original helpers use 64-bit addresses");

enum gs_conversion {
   GS_U8_SCALED,
   GS_S8_SCALED,
   GS_S8_NORM,
   GS_S16_NORM,
};

static uint8_t
gs_convert(unsigned raw, enum gs_conversion kind)
{
   if (kind == GS_U8_SCALED)
      return raw ? UINT8_MAX : 0;

   int32_t x = (int32_t)raw;
   if (kind == GS_S16_NORM) {
      if (x >= 32768)
         x -= 65536;
      if (x < 0)
         x = 0;
      /* Original UMLAL: y=x*255+16383. UMULL/UZP2/shift7 implements
       * floor(y/32767) for the bounded y range; reciprocal0x01000201.
       * Original high32 temporary stays below65536 before narrow+shift.
       */
      uint32_t y = (uint32_t)x * 255u + 16383u;
      return (uint8_t)(((uint64_t)y * UINT64_C(0x01000201)) >> 39);
   }

   if (x >= 128)
      x -= 256;
   if (kind == GS_S8_SCALED)
      return x > 0 ? UINT8_MAX : 0;
   if (x < 0)
      x = 0;
   /* Original scalar ORR and vector ADD+USRA agree on x in[0,127]. */
   return (uint8_t)(((unsigned)x << 1) | ((unsigned)x >> 6));
}

static void
gs_pixel(uint8_t rgba[4], const uint8_t *src, enum gs_conversion kind, bool bgr)
{
   unsigned raw[4];
   if (kind == GS_S16_NORM) {
      for (unsigned c = 0; c < 4; c++)
         raw[c] = (unsigned)src[2 * c] | ((unsigned)src[2 * c + 1] << 8);
      for (unsigned c = 0; c < 4; c++)
         rgba[c] = gs_convert(raw[c], kind);
   } else {
      raw[0] = src[0];
      raw[1] = src[1];
      raw[2] = src[2];
      rgba[0] = gs_convert(raw[bgr ? 2 : 0], kind);
      rgba[1] = gs_convert(raw[1], kind);
      rgba[2] = gs_convert(raw[bgr ? 0 : 2], kind);
      rgba[3] = UINT8_MAX;
   }
}

static void
gs_block(uint8_t *dst, const uint8_t *src, unsigned count,
         enum gs_conversion kind, bool bgr)
{
   uint8_t rgba[16][4];
   unsigned stride = kind == GS_S16_NORM ? 8 : 3;
   /* Native LD3/LD4 reads the block before ST4/TBL stores its result. */
   for (unsigned i = 0; i < count; i++)
      gs_pixel(rgba[i], src + i * stride, kind, bgr);
   memcpy(dst, rgba, count * 4);
}

static void
gs_row(uint8_t *dst, const uint8_t *src, unsigned width,
       enum gs_conversion kind, bool bgr)
{
   if (width == 0)
      return;

   unsigned stride = kind == GS_S16_NORM ? 8 : 3;
   unsigned narrow = kind == GS_S16_NORM ? 4 : 8;
   uintptr_t input_end = (uintptr_t)src + (uint64_t)width * stride;
   uintptr_t output_end = (uintptr_t)dst + (uint64_t)width * 4;
   bool disjoint = (uintptr_t)dst >= input_end || (uintptr_t)src >= output_end;
   unsigned done = 0;

   if (width >= narrow && disjoint) {
      /* Original main blocks:16pixels. RGB8 remainder:8pixels.
       * RGBA16 remainder:4pixels, repeated for4/8/12remaining pixels.
       */
      while (width - done >= 16) {
         gs_block(dst + (uint64_t)done * 4, src + (uint64_t)done * stride,
                  16, kind, bgr);
         done += 16;
      }
      while (width - done >= narrow) {
         gs_block(dst + (uint64_t)done * 4, src + (uint64_t)done * stride,
                  narrow, kind, bgr);
         done += narrow;
      }
   }

   while (done < width) {
      uint8_t rgba[4];
      gs_pixel(rgba, src + (uint64_t)done * stride, kind, bgr);
      uint8_t *out = dst + (uint64_t)done * 4;
      out[3] = rgba[3];
      out[0] = rgba[0];
      out[1] = rgba[1];
      out[2] = rgba[2];
      done++;
   }
}

/* Original ELF0xb10b1c,size320. */
void gs_restored_r8g8b8_uscaled_unpack_rgba_8unorm(uint8_t *d, const uint8_t *s, unsigned n)
{ gs_row(d, s, n, GS_U8_SCALED, false); }
/* Original ELF0xb10e60,size316. */
void gs_restored_b8g8r8_uscaled_unpack_rgba_8unorm(uint8_t *d, const uint8_t *s, unsigned n)
{ gs_row(d, s, n, GS_U8_SCALED, true); }
/* Original ELF0xb13de4,size320. */
void gs_restored_r8g8b8_sscaled_unpack_rgba_8unorm(uint8_t *d, const uint8_t *s, unsigned n)
{ gs_row(d, s, n, GS_S8_SCALED, false); }
/* Original ELF0xb14128,size320. */
void gs_restored_b8g8r8_sscaled_unpack_rgba_8unorm(uint8_t *d, const uint8_t *s, unsigned n)
{ gs_row(d, s, n, GS_S8_SCALED, true); }
/* Original ELF0xb12420,size380. */
void gs_restored_r8g8b8_snorm_unpack_rgba_8unorm(uint8_t *d, const uint8_t *s, unsigned n)
{ gs_row(d, s, n, GS_S8_NORM, false); }
/* Original ELF0xb12920,size380. */
void gs_restored_b8g8r8_snorm_unpack_rgba_8unorm(uint8_t *d, const uint8_t *s, unsigned n)
{ gs_row(d, s, n, GS_S8_NORM, true); }
/* Original ELF0xb0e5ec,size940. Alpha is converted from signed16, not filled. */
void gs_restored_r16g16b16a16_snorm_unpack_rgba_8unorm(uint8_t *d, const uint8_t *s, unsigned n)
{ gs_row(d, s, n, GS_S16_NORM, false); }
