//
//  jpeg_helper.hpp
//  jpeg_helper
//
//  Created by jichen on 2023/4/8.
//

#ifndef jpeg_helper_hpp
#define jpeg_helper_hpp

namespace jpeg_helper {

int compress_jpeg(const char* filename, int width, int height, unsigned char* buf);

int decompress_jpeg(const char* filename, int* width, int* height,
                    unsigned char* buf, int bufsz);

}

#endif /* jpeg_helper_hpp */
