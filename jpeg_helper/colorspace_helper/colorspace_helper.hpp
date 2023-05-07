//
//  colorspace_helper.hpp
//  jpeg_helper
//
//  Created by jichen on 2023/5/7.
//

#ifndef colorspace_helper_hpp
#define colorspace_helper_hpp

namespace colorspace_helper {

void rgb_to_ycc(int width, int height, const unsigned char* rgb_buf, unsigned char* ycc_buf);

void ycc_to_rgb(int width, int height, const unsigned char* ycc_buf, unsigned char* rgb_buf);

}

#endif /* colorspace_helper_hpp */
