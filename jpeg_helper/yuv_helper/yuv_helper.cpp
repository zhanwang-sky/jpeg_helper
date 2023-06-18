//
//  yuv_helper.cpp
//  jpeg_helper
//
//  Created by jichen on 2023/6/18.
//

#include <climits>
#include <cmath>
#include "yuv_helper.hpp"

static float color_matrices[][5] = {
  {0.2990f, 0.5870f, 0.1140f, 1.7720f, 1.4020f}, // BT601
  {0.2126f, 0.7152f, 0.0722f, 1.8556f, 1.5748f}, // BT709
  {0.2627f, 0.6780f, 0.0593f, 1.8814f, 1.4746f}  // BT2020
};

static inline unsigned char saturate_cast(float v) {
  int iv = std::roundf(v);
  return (unsigned char) ((iv > UCHAR_MAX) ? UCHAR_MAX : ((iv < 0) ? 0 : iv));
}

void yuv_helper::RGB_to_PackedYUV(color_spec cs, pixel_format fmt,
                                  int width, int height,
                                  const unsigned char* rgb_buf,
                                  unsigned char* yuv_buf) {
  float a = color_matrices[cs][0];
  float b = color_matrices[cs][1];
  float c = color_matrices[cs][2];
  float d = color_matrices[cs][3];
  float e = color_matrices[cs][4];
  float R, G, B, Y, Cb, Cr;
  int idx = 0;

  // XXX TODO: only support YCbCr, fix me
  for (int i = 0; i < height; ++i) {
    for (int j = 0; j < width; ++j) {
      R = (float) rgb_buf[idx];
      G = (float) rgb_buf[idx + 1];
      B = (float) rgb_buf[idx + 2];

      Y = a * R + b * G + c * B;
      Cb = (B - Y) / d;
      Cr = (R - Y) / e;

      yuv_buf[idx] = saturate_cast(Y);
      yuv_buf[idx + 1] = saturate_cast(Cb + 128.f);
      yuv_buf[idx + 2] = saturate_cast(Cr + 128.f);

      idx += 3;
    }
  }
}

void yuv_helper::PackedYUV_to_RGB(color_spec cs, pixel_format fmt,
                                  int width, int height,
                                  const unsigned char* yuv_buf,
                                  unsigned char* rgb_buf) {
  float a = color_matrices[cs][0];
  float b = color_matrices[cs][1];
  float c = color_matrices[cs][2];
  float d = color_matrices[cs][3];
  float e = color_matrices[cs][4];
  float Y, Cb, Cr, R, G, B;
  int idx = 0;

  // XXX TODO: only support YCbCr, fix me
  for (int i = 0; i < height; ++i) {
    for (int j = 0; j < width; ++j) {
      Y = (float) yuv_buf[idx];
      Cb = (float) yuv_buf[idx + 1] - 128.f;
      Cr = (float) yuv_buf[idx + 2] - 128.f;

      R = Y + e * Cr;
      G = Y - (a * e / b) * Cr - (c * d / b) * Cb;
      B = Y + d * Cb;

      rgb_buf[idx] = saturate_cast(R);
      rgb_buf[idx + 1] = saturate_cast(G);
      rgb_buf[idx + 2] = saturate_cast(B);

      idx += 3;
    }
  }
}

void yuv_helper::RGB_to_PlanarYUV(color_spec cs, pixel_format fmt,
                                  int width, int height,
                                  const unsigned char* rgb_buf,
                                  const int strides[],
                                  unsigned char* planes[]) {
  float a = color_matrices[cs][0];
  float b = color_matrices[cs][1];
  float c = color_matrices[cs][2];
  float d = color_matrices[cs][3];
  float e = color_matrices[cs][4];
  float R, G, B, Y, Cb, Cr;

  for (int i = 0; i < height; ++i) {
    for (int j = 0; j < width; ++j) {
      R = (float) rgb_buf[i * width + j];
      G = (float) rgb_buf[i * width + j + 1];
      B = (float) rgb_buf[i * width + j + 2];

      Y = a * R + b * G + c * B;
      planes[0][i * strides[0] + j] = saturate_cast(Y);

      // XXX TODO: only support I420, fix me
      if (i % 2 == 0 && j % 2 == 0) {
        Cb = (B - Y) / d;
        Cr = (R - Y) / e;
        planes[1][(i / 2) * strides[1] + (j / 2)] = saturate_cast(Cb + 128.f);
        planes[2][(i / 2) * strides[2] + (j / 2)] = saturate_cast(Cr + 128.f);
      }
    }
  }
}

void yuv_helper::PlanarYUV_to_RGB(color_spec cs, pixel_format fmt,
                                  int width, int height,
                                  const int strides[],
                                  const unsigned char* planes[],
                                  unsigned char* rgb_buf) {
  float a = color_matrices[cs][0];
  float b = color_matrices[cs][1];
  float c = color_matrices[cs][2];
  float d = color_matrices[cs][3];
  float e = color_matrices[cs][4];
  float Y, Cb, Cr, R, G, B;

  for (int i = 0; i < height; ++i) {
    for (int j = 0; j < width; ++j) {
      Y = (float) planes[0][i * strides[0] + j];
      // XXX TODO: only support I420, fix me
      Cb = (float) planes[1][(i / 2) * strides[1] + (j / 2)] - 128.f;
      Cr = (float) planes[2][(i / 2) * strides[2] + (j / 2)] - 128.f;

      R = Y + e * Cr;
      G = Y - (a * e / b) * Cr - (c * d / b) * Cb;
      B = Y + d * Cb;

      rgb_buf[i * width + j] = saturate_cast(R);
      rgb_buf[i * width + j + 1] = saturate_cast(G);
      rgb_buf[i * width + j + 2] = saturate_cast(B);
    }
  }
}
