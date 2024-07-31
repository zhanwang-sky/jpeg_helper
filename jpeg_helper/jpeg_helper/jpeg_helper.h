//
//  jpeg_helper.h
//  jpeg_helper
//
//  Created by zhanwang-sky on 2024/7/31.
//

#ifndef jpeg_helper_h
#define jpeg_helper_h

int jpeg_compress(const char* filename, int width, int height,
                  unsigned char* buf, int quality);

int jpeg_decompress(const char* filename, int* width, int* height,
                    unsigned char* buf, int bufsz);

#endif /* jpeg_helper_h */
