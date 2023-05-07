//
//  ppm_helper.hpp
//  jpeg_helper
//
//  Created by jichen on 2023/5/7.
//

#ifndef ppm_helper_hpp
#define ppm_helper_hpp

namespace ppm_helper {

int write_ppm(const char* filename, int width, int height,
              const unsigned char* buf);

int read_ppm(const char* filename, int* width, int* height,
             unsigned char* buf, int bufsz);

}

#endif /* ppm_helper_hpp */
