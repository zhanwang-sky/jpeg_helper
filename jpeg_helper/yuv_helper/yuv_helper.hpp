//
//  yuv_helper.hpp
//  jpeg_helper
//
//  Created by jichen on 2023/6/18.
//

#ifndef yuv_helper_hpp
#define yuv_helper_hpp

namespace yuv_helper {

enum color_spec {
  CS_BT601 = 0,
  CS_JPEG = CS_BT601,
  CS_BT709 = 1,
  CS_BT2020 = 2
};

enum pixel_format {
  FMT_YCbCr = 0,
  FMT_YUY2 = 1,
  FMT_YVYU = 2,
  FMT_UYVY = 3,
  FMT_I420 = 4,
  FMT_YV12 = 5,
  FMT_NV12 = 6
};

void RGB_to_PackedYUV(color_spec cs, pixel_format fmt, int width, int height,
                      const unsigned char* rgb_buf, unsigned char* yuv_buf);

void PackedYUV_to_RGB(color_spec cs, pixel_format fmt, int width, int height,
                      const unsigned char* yuv_buf, unsigned char* rgb_buf);

void RGB_to_PlanarYUV(color_spec cs, pixel_format fmt, int width, int height,
                      const unsigned char* rgb_buf,
                      const int strides[], unsigned char* planes[]);

void PlanarYUV_to_RGB(color_spec cs, pixel_format fmt, int width, int height,
                      const int strides[], const unsigned char* planes[],
                      unsigned char* rgb_buf);

}

#endif /* yuv_helper_hpp */
