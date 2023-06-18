//
//  main.cpp
//  jpeg_helper
//
//  Created by jichen on 2023/4/8.
//

#include <iostream>
#include "jpeg_helper.hpp"
#include "ppm_helper.hpp"
#include "yuv_helper.hpp"

using std::cout;
using std::cerr;
using std::endl;

unsigned char rgb_buf[12032 * 9024 * 3]; // one billion pixels ~ 310MB
unsigned char yuv_buf[12032 * 9024 * 3];

int main(int argc, char* argv[]) {
  int width = 0;
  int height = 0;
  int rc = 0;

  if (argc != 4) {
    cerr << "Usage: ./jpeg_helper <input.ppm> <output.jpg> <output.ppm>\n";
    exit(EXIT_FAILURE);
  }

  rc = jpeg_helper::decompress_jpeg(argv[1], &width, &height,
                                    yuv_buf, sizeof(yuv_buf),
                                    jpeg_helper::JHCS_YCC);
  if (rc <= 0) {
    cerr << "jpeg_helper: Fail to decompress JPEG file \"" << argv[1] << "\", rc=" << rc << endl;
    exit(EXIT_FAILURE);
  }
  cout << "jpeg_helper::decompress_jpeg: "
       << "width=" << width << ", height=" << height << ", rgb_size=" << rc << endl;

  yuv_helper::PackedYUV_to_RGB(yuv_helper::CS_JPEG, yuv_helper::FMT_YCbCr,
                               width, height, yuv_buf, rgb_buf);

  rc = ppm_helper::write_ppm(argv[2], width, height, rgb_buf);
  if (rc <= 0) {
    cerr << "ppm_helper: Fail to write ppm file \"" << argv[2] << "\", rc=" << rc << endl;
    exit(EXIT_FAILURE);
  }
  cout << "ppm_helper::write_ppm: "
       << "width=" << width << ", height=" << height << ", rgb_size=" << rc << endl;

  rc = ppm_helper::read_ppm(argv[2], &width, &height, rgb_buf, sizeof(rgb_buf));
  if (rc <= 0) {
    cerr << "ppm_helper: Fail to read ppm file \"" << argv[2] << "\", rc=" << rc << endl;
    exit(EXIT_FAILURE);
  }
  cout << "ppm_helper::read_ppm: "
       << "width=" << width << ", height=" << height << ", rgb_size=" << rc << endl;

  yuv_helper::RGB_to_PackedYUV(yuv_helper::CS_JPEG, yuv_helper::FMT_YCbCr,
                               width, height, rgb_buf, yuv_buf);

  rc = jpeg_helper::compress_jpeg(argv[3], width, height,
                                  yuv_buf, jpeg_helper::JHCS_YCC);
  if (rc <= 0) {
    cerr << "jpeg_helper: Fail to compress JPEG file \"" << argv[3] << "\", rc=" << rc << endl;
    exit(EXIT_FAILURE);
  }
  cout << "jpeg_helper::compress_jpeg: "
       << "width=" << width << ", height=" << height << ", rgb_size=" << rc << endl;

  return 0;
}
