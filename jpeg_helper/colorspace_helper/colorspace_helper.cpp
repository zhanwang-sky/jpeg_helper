//
//  colorspace_helper.cpp
//  jpeg_helper
//
//  Created by jichen on 2023/5/7.
//

#include <climits>
#include <cmath>
#include "colorspace_helper.hpp"

#define CV_DESCALE(x,n) (((x) + (1 << ((n)-1))) >> (n))

// constants for conversion from/to RGB and Gray, YUV, YCrCb according to BT.601
static constexpr int R2Y = 4899; // == 0.299*16384
static constexpr int G2Y = 9617; // == 0.587*16384
static constexpr int B2Y = 1868; // == 0.114*16384
// to YCbCr
static constexpr int YCBI = 9241;  // == 0.564*16384
static constexpr int YCRI = 11682; // == 0.713*16384
// from YCbCr
static constexpr int CR2RI = 22987;  // == 1.403*16384
static constexpr int CR2GI = -11698; // == -0.714*16384
static constexpr int CB2GI = -5636;  // == -0.344*16384
static constexpr int CB2BI = 29049;  // == 1.773*16384

static inline unsigned char saturate_cast(int v) {
  return (unsigned char) ((unsigned) v <= UCHAR_MAX ? v : v > 0 ? UCHAR_MAX : 0);
}

// RGB2YCrCb_i<uchar>
void colorspace_helper::rgb_to_ycc(int width, int height,
                                   const unsigned char* rgb_buf,
                                   unsigned char* ycc_buf) {
  constexpr int shift = 14;
  constexpr int delta = (UCHAR_MAX / 2 + 1) * (1 << shift);
  int r, g, b, y, cb, cr;
  int idx = 0;

  for (int i = 0; i < height; ++i) {
    for (int j = 0; j < width; ++j) {
      r = rgb_buf[idx];
      g = rgb_buf[idx + 1];
      b = rgb_buf[idx + 2];

      y = CV_DESCALE(R2Y * r + G2Y * g + B2Y * b, shift);
      cb = CV_DESCALE(YCBI * (b - y) + delta, shift);
      cr = CV_DESCALE(YCRI * (r - y) + delta, shift);

      ycc_buf[idx] = saturate_cast(y);
      ycc_buf[idx + 1] = saturate_cast(cb);
      ycc_buf[idx + 2] = saturate_cast(cr);

      idx += 3;
    }
  }
}

// YCrCb2RGB_i<uchar>
void colorspace_helper::ycc_to_rgb(int width, int height,
                                   const unsigned char* ycc_buf,
                                   unsigned char* rgb_buf) {
  constexpr int shift = 14;
  constexpr int delta = UCHAR_MAX / 2 + 1;
  int y, cb, cr, r, g, b;
  int idx = 0;

  for (int i = 0; i < height; ++i) {
    for (int j = 0; j < width; ++j) {
      y = ycc_buf[idx];
      cb = ycc_buf[idx + 1];
      cr = ycc_buf[idx + 2];

      r = y + CV_DESCALE(CR2RI * (cr - delta), shift);
      g = y + CV_DESCALE(CB2GI * (cb - delta) + CR2GI * (cr - delta), shift);
      b = y + CV_DESCALE(CB2BI * (cb - delta), shift);

      rgb_buf[idx] = saturate_cast(r);
      rgb_buf[idx + 1] = saturate_cast(g);
      rgb_buf[idx + 2] = saturate_cast(b);

      idx += 3;
    }
  }
}
