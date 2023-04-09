//
//  jpeg_helper.cpp
//  jpeg_helper
//
//  Created by jichen on 2023/4/8.
//

#include <csetjmp>
#include <cstdio>
#include <jpeglib.h>
#include "jpeg_helper.hpp"

#define JH_COLOR_SPACE JCS_RGB
#define JH_COLOR_COMPONENTS 3

struct jh_error_mgr {
  struct jpeg_error_mgr jerr;
  int msg_code;
  jmp_buf setjmp_buffer;
};

typedef struct jh_error_mgr* jh_error_ptr;

static void jh_error_exit(j_common_ptr cinfo) {
  jh_error_ptr p_errmgr = (jh_error_ptr) cinfo->err; // base addr of jh_error_mgr

  /* Save message code */
  p_errmgr->msg_code = cinfo->err->msg_code;

  /* Always display the message. */
  (*cinfo->err->output_message)(cinfo);

  /* Return control to the setjmp point */
  longjmp(p_errmgr->setjmp_buffer, 1);
}

int jpeg_helper::compress_jpeg(const char* filename, int width, int height,
                               unsigned char* buf) {
  JDIMENSION row_stride = 0;
  JDIMENSION realsz = 0;
  FILE* p_file = NULL;
  JSAMPROW row_pointer[1] = {0};
  struct jpeg_compress_struct cinfo = {0};
  struct jh_error_mgr errmgr = {0};

  /* Sanity check */
  if (!filename || !width || !height || !buf) {
    return -(__INT_MAX__);
  }

  /* Open file for writing */
  p_file = fopen(filename, "wb");
  if (!p_file) {
    return -(__INT_MAX__ - 1);
  }

  /* We set up the normal JPEG error routines, then override error_exit. */
  cinfo.err = jpeg_std_error(&errmgr.jerr);
  errmgr.jerr.error_exit = jh_error_exit;
  /* Establish the setjmp return context for jh_error_exit to use. */
  if (setjmp(errmgr.setjmp_buffer)) {
    jpeg_destroy_compress(&cinfo);
    fclose(p_file);
    return -(errmgr.msg_code);
  }

  /* Now we can initialize the JPEG compression object. */
  jpeg_create_compress(&cinfo);

  jpeg_stdio_dest(&cinfo, p_file);

  /* First we supply a description of the input image.
   * Four fields of the cinfo struct must be filled in:
   */
  cinfo.image_width = width;
  cinfo.image_height = height;
  cinfo.input_components = JH_COLOR_COMPONENTS;
  cinfo.in_color_space = JH_COLOR_SPACE;
  jpeg_set_defaults(&cinfo);

  /* Now you can set any non-default parameters you wish to. */
  jpeg_set_quality(&cinfo, 100, TRUE);

  /* TRUE ensures that we will write a complete interchange-JPEG file. */
  jpeg_start_compress(&cinfo, TRUE);

  row_stride = width * JH_COLOR_COMPONENTS;
  while (cinfo.next_scanline < cinfo.image_height) {
    row_pointer[0] = &buf[cinfo.next_scanline * row_stride];
    jpeg_write_scanlines(&cinfo, row_pointer, 1);
  }
  realsz = cinfo.next_scanline * row_stride;

  jpeg_finish_compress(&cinfo);

  jpeg_destroy_compress(&cinfo);

  fclose(p_file);

  return realsz;
}

int jpeg_helper::decompress_jpeg(const char* filename, int* width, int* height,
                                 unsigned char* buf, int bufsz) {
  JDIMENSION row_stride = 0;
  JDIMENSION realsz = 0;
  FILE* p_file = NULL;
  JSAMPROW row_pointer[1] = {0};
  struct jpeg_decompress_struct cinfo = {0};
  struct jh_error_mgr errmgr = {0};

  /* Sanity check */
  if (!filename || !width || !height || !buf || !bufsz) {
    return -(__INT_MAX__);
  }

  /* Open JPEG file for reading */
  p_file = fopen(filename, "rb");
  if (!p_file) {
    return -(__INT_MAX__ - 1);
  }

  /* We set up the normal JPEG error routines, then override error_exit. */
  cinfo.err = jpeg_std_error(&errmgr.jerr);
  errmgr.jerr.error_exit = jh_error_exit;
  /* Establish the setjmp return context for jh_error_exit to use. */
  if (setjmp(errmgr.setjmp_buffer)) {
    jpeg_destroy_decompress(&cinfo);
    fclose(p_file);
    return -(errmgr.msg_code);
  }

  /* Now we can initialize the JPEG decompression object. */
  jpeg_create_decompress(&cinfo);

  jpeg_stdio_src(&cinfo, p_file);

  jpeg_read_header(&cinfo, TRUE);
  /* We can ignore the return value from jpeg_read_header since
   *   (a) suspension is not possible with the stdio data source, and
   *   (b) we passed TRUE to reject a tables-only JPEG file as an error.
   * See libjpeg.txt for more info.
   */

  // debug
  fprintf(stderr, "image_width=%u,image_height=%u,components=%d,color_space=%d\n",
          cinfo.image_width, cinfo.image_height,
          cinfo.num_components, cinfo.jpeg_color_space);

  /* Decompression processing parameters --- these fields must be set before
   * calling jpeg_start_decompress().  Note that jpeg_read_header() initializes
   * them to default values.
   */
  cinfo.out_color_space = JH_COLOR_SPACE;

  jpeg_start_decompress(&cinfo);
  /* We can ignore the return value since suspension is not possible
   * with the stdio data source.
   */

  /* We may need to do some setup of our own at this point before reading
   * the data.  After jpeg_start_decompress() we have the correct scaled
   * output image dimensions available, as well as the output colormap
   * if we asked for color quantization.
   */
  row_stride = cinfo.output_width * cinfo.output_components;
  realsz = row_stride * cinfo.output_height;
  if (bufsz < realsz) {
    realsz = 0;
    goto buf_too_small;
  }
  *width = cinfo.output_width;
  *height = cinfo.output_height;

  while (cinfo.output_scanline < cinfo.output_height) {
    row_pointer[0] = &buf[cinfo.output_scanline * row_stride];
    jpeg_read_scanlines(&cinfo, row_pointer, 1);
  }

buf_too_small:
  jpeg_finish_decompress(&cinfo);
  /* We can ignore the return value since suspension is not possible
   * with the stdio data source.
   */

  jpeg_destroy_decompress(&cinfo);

  fclose(p_file);

  return realsz;
}
