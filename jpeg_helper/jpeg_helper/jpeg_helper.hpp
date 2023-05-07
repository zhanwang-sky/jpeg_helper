//
//  jpeg_helper.hpp
//  jpeg_helper
//
//  Created by jichen on 2023/4/8.
//

#ifndef jpeg_helper_hpp
#define jpeg_helper_hpp

namespace jpeg_helper {

enum color_space {
  JHCS_RGB,
  JHCS_YCC
};

int compress_jpeg(const char* filename, int width, int height,
                  unsigned char* buf, color_space cs = JHCS_RGB);

int decompress_jpeg(const char* filename, int* width, int* height,
                    unsigned char* buf, int bufsz, color_space cs = JHCS_RGB);

}

#endif /* jpeg_helper_hpp */
