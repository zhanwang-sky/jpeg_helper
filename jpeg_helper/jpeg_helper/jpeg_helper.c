//
//  jpeg_helper.c
//  jpeg_helper
//
//  Created by zhanwang-sky on 2024/7/31.
//

#include <limits.h>
#include <setjmp.h>
#include <stdio.h>
#include <jpeglib.h>
#include "jpeg_helper.h"

struct my_error_mgr {
  struct jpeg_error_mgr pub; // "public" fields
  jmp_buf setjmp_buffer;     // for return to caller
};

typedef struct my_error_mgr* my_error_ptr;

static void my_error_exit(j_common_ptr cinfo) {
  // cinfo->err really points to a my_error_mgr struct, so coerce pointer.
  my_error_ptr myerr = (my_error_ptr) cinfo->err;

  // Display the message.
  (*cinfo->err->output_message)(cinfo);

  // Return control to the setjmp point.
  longjmp(myerr->setjmp_buffer, 1);
}

int jpeg_compress(const char* filename, int width, int height,
                  unsigned char* buf, int quality) {
  struct jpeg_compress_struct cinfo = {0};
  struct my_error_mgr jerr = {0};
  FILE* outfile = NULL;
  JSAMPROW row_pointer[1] = {0};
  int row_stride = 0;

  // sanity check
  if (!filename || width <= 0 || height <= 0 || !buf) {
    return INT_MIN;
  }

  outfile = fopen(filename, "wb");
  if (!outfile) {
    return INT_MIN + 1;
  }

  // Step 1: allocate and initialize JPEG compression object.

  // We set up the normal JPEG error routines, then override error_exit.
  cinfo.err = jpeg_std_error(&jerr.pub);
  jerr.pub.error_exit = my_error_exit;
  // Establish the setjmp return context for my_error_exit to use.
  if (setjmp(jerr.setjmp_buffer)) {
    // If we get here, the JPEG code has signaled an error.
    // We need to clean up the JPEG object, close the input file, and return.
    jpeg_destroy_compress(&cinfo);
    fclose(outfile);
    return -jerr.pub.msg_code;
  }
  // Now we can initialize the JPEG compression object.
  jpeg_create_compress(&cinfo);

  // Step 2: specify data destination (eg, a file).

  jpeg_stdio_dest(&cinfo, outfile);

  // Step 3: set parameters for compression.

  // First we supply a description of the input image.
  // Four fields of the cinfo struct must be filled in:
  cinfo.image_width = width;
  cinfo.image_height = height;
  cinfo.input_components = 3;
  cinfo.in_color_space = JCS_RGB;
  jpeg_set_defaults(&cinfo);
  // Now you can set any non-default parameters you wish to.
  jpeg_set_quality(&cinfo, quality, TRUE);

  // Step 4: Start compressor.

  // TRUE ensures that we will write a complete interchange-JPEG file.
  jpeg_start_compress(&cinfo, TRUE);

  // Step 5: while (scan lines remain to be written)
  //           jpeg_write_scanlines(...);

  row_stride = width * 3;

  while (cinfo.next_scanline < cinfo.image_height) {
    row_pointer[0] = &buf[cinfo.next_scanline * row_stride];
    (void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
  }

  // Step 6: Finish compression.

  jpeg_finish_compress(&cinfo);

  // After finish_compress, we can close the output file.
  fclose(outfile);

  // Step 7: release JPEG compression object.

  jpeg_destroy_compress(&cinfo);

  return 0;
}

int jpeg_decompress(const char* filename, int* width, int* height,
                    unsigned char* buf, int bufsz) {
  struct jpeg_decompress_struct cinfo = {0};
  struct my_error_mgr jerr = {0};
  FILE* infile = NULL;
  JSAMPROW row_pointer[1] = {0};
  int row_stride = 0;
  int realsz = 0;

  // sanity check
  if (!filename || !width || !height || bufsz < 0) {
    return INT_MIN;
  }

  infile = fopen(filename, "rb");
  if (!infile) {
    return INT_MIN + 1;
  }

  // Step 1: allocate and initialize JPEG decompression object.

  // We set up the normal JPEG error routines, then override error_exit.
  cinfo.err = jpeg_std_error(&jerr.pub);
  jerr.pub.error_exit = my_error_exit;
  // Establish the setjmp return context for my_error_exit to use.
  if (setjmp(jerr.setjmp_buffer)) {
    // If we get here, the JPEG code has signaled an error.
    // We need to clean up the JPEG object, close the input file, and return.
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);
    return -jerr.pub.msg_code;
  }
  // Now we can initialize the JPEG decompression object.
  jpeg_create_decompress(&cinfo);

  // Step 2: specify data source (eg, a file).

  jpeg_stdio_src(&cinfo, infile);

  // Step 3: read file parameters with jpeg_read_header().

  (void) jpeg_read_header(&cinfo, TRUE);
  // We can ignore the return value from jpeg_read_header since
  //   (a) suspension is not possible with the stdio data source, and
  //   (b) we passed TRUE to reject a tables-only JPEG file as an error.
  // See libjpeg.txt for more info.

  // Step 4: set parameters for decompression.

  // Step 5: Start decompressor.

  (void) jpeg_start_decompress(&cinfo);
  // We can ignore the return value since suspension is not possible
  // with the stdio data source.

  // JSAMPLEs per row in output buffer.
  *width = cinfo.output_width;
  *height = cinfo.output_height;
  row_stride = cinfo.output_width * cinfo.output_components;
  realsz = row_stride * cinfo.output_height;
  if (!buf || bufsz < realsz) {
    goto exit;
  }

  // Step 6: while (scan lines remain to be read)
  //           jpeg_read_scanlines(...);

  while (cinfo.output_scanline < cinfo.output_height) {
    row_pointer[0] = &buf[cinfo.output_scanline * row_stride];
    (void) jpeg_read_scanlines(&cinfo, row_pointer, 1);
  }

  // Step 7: Finish decompression.

  (void) jpeg_finish_decompress(&cinfo);
  // We can ignore the return value since suspension is not possible
  // with the stdio data source.

  realsz = 0;

exit:

  // Step 8: Release JPEG decompression object.

  jpeg_destroy_decompress(&cinfo);

  fclose(infile);

  return realsz;
}
