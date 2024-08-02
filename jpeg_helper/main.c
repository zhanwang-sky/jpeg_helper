//
//  main.c
//  jpeg_helper
//
//  Created by zhanwang-sky on 2024/7/31.
//

#include <stdio.h>
#include <stdlib.h>
#include "jpeg_helper.h"

unsigned char buf[8192 * 6144 * 3];

int main(int argc, char* argv[]) {
  int w, h;
  int rc = 0;

  if (argc != 3) {
    fprintf(stderr, "Usage: ./jpeg_helper <input.jpg> <output.jpg>\n");
    exit(EXIT_FAILURE);
  }

  rc = jpeg_decompress(argv[1], &w, &h, buf, sizeof(buf));
  if (rc != 0) {
    fprintf(stderr, "failed to decompress JPEG file, rc=%d\n", rc);
    exit(EXIT_FAILURE);
  }

  printf("dimension: %d * %d\n", w, h);

  rc = jpeg_compress(argv[2], w, h, buf, 95);
  if (rc != 0) {
    fprintf(stderr, "failed to compress JPEG file, rc=%d\n", rc);
    exit(EXIT_FAILURE);
  }

  printf("Done.\n");

  return 0;
}
