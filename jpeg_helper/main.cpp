//
//  main.cpp
//  jpeg_helper
//
//  Created by jichen on 2023/4/8.
//

#include <iostream>
#include "jpeg_helper.hpp"

using std::cout;
using std::cerr;
using std::endl;

unsigned char imgbuf[12032 * 9024 * 3]; // one billion pixels ~ 310MB

int main(int argc, char* argv[]) {
  int width = 0;
  int height = 0;
  int rc = 0;

  if (argc != 3) {
    cerr << "Usage: ./jpeg_helper <input.jpg> <output.jpg>\n";
    exit(EXIT_FAILURE);
  }

  rc = jpeg_helper::decompress_jpeg(argv[1], &width, &height, imgbuf, sizeof(imgbuf));
  if (rc <= 0) {
    cerr << "Fail to decompress JPEG file, rc=" << rc << endl;
    exit(EXIT_FAILURE);
  }
  cout << "decompression: "
       << "width=" << width << ",height=" << height << ",img_size=" << rc << endl;

  rc = jpeg_helper::compress_jpeg(argv[2], width, height, imgbuf);
  if (rc <= 0) {
    cerr << "Fail to compress JPEG file, rc=" << rc << endl;
    exit(EXIT_FAILURE);
  }
  cout << "compression: "
       << "width=" << width << ",height=" << height << ",img_size=" << rc << endl;

  return 0;
}
