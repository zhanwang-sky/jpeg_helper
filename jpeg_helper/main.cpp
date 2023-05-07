//
//  main.cpp
//  jpeg_helper
//
//  Created by jichen on 2023/4/8.
//

#include <iostream>
#include "colorspace_helper.hpp"
#include "jpeg_helper.hpp"
#include "ppm_helper.hpp"

using std::cout;
using std::cerr;
using std::endl;

unsigned char rgb_buf[12032 * 9024 * 3]; // one billion pixels ~ 310MB
unsigned char ycc_buf[12032 * 9024 * 3];

int main(int argc, char* argv[]) {
  int width = 0;
  int height = 0;
  int rc = 0;

  if (argc != 4) {
    cerr << "Usage: ./jpeg_helper <input.ppm> <output.jpg> <output.ppm>\n";
    exit(EXIT_FAILURE);
  }

  rc = ppm_helper::read_ppm(argv[1], &width, &height, rgb_buf, sizeof(rgb_buf));
  if (rc <= 0) {
    cerr << "ppm_helper: Fail to read ppm file \"" << argv[1] << "\", rc=" << rc << endl;
    exit(EXIT_FAILURE);
  }
  cout << "ppm_helper::read_ppm: "
       << "width=" << width << ", height=" << height << ", rgb_size=" << rc << endl;

  colorspace_helper::rgb_to_ycc(width, height, rgb_buf, ycc_buf);

  rc = jpeg_helper::compress_jpeg(argv[2], width, height, ycc_buf, jpeg_helper::JHCS_YCC);
  if (rc <= 0) {
    cerr << "jpeg_helper: Fail to compress JPEG file \"" << argv[2] << "\", rc=" << rc << endl;
    exit(EXIT_FAILURE);
  }
  cout << "jpeg_helper::compress_jpeg: "
       << "width=" << width << ", height=" << height << ", rgb_size=" << rc << endl;

  rc = jpeg_helper::decompress_jpeg(argv[2], &width, &height, ycc_buf, sizeof(ycc_buf), jpeg_helper::JHCS_YCC);
  if (rc <= 0) {
    cerr << "jpeg_helper: Fail to decompress JPEG file \"" << argv[2] << "\", rc=" << rc << endl;
    exit(EXIT_FAILURE);
  }
  cout << "jpeg_helper::decompress_jpeg: "
       << "width=" << width << ", height=" << height << ", rgb_size=" << rc << endl;

  colorspace_helper::ycc_to_rgb(width, height, ycc_buf, rgb_buf);

  rc = ppm_helper::write_ppm(argv[3], width, height, rgb_buf);
  if (rc <= 0) {
    cerr << "ppm_helper: Fail to write ppm file \"" << argv[3] << "\", rc=" << rc << endl;
    exit(EXIT_FAILURE);
  }
  cout << "ppm_helper::write_ppm: "
       << "width=" << width << ", height=" << height << ", rgb_size=" << rc << endl;

  return 0;
}
