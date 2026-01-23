
#include <stdio.h>
#include <stdlib.h>

#include "jpeglib.h"

#define RELEASE_ASSERT(cond, msg)           \
  if(!(cond))                               \
  {                                         \
    printf("FAILED: " #cond ". " msg "\n"); \
    exit(1);                                \
  }

void my_error_exit (j_common_ptr cinfo) {
  RELEASE_ASSERT(false, "my_error_exit exit handler called");
}

struct jpeg_parsed_data {
  JSAMPLE* image_buffer;
  size_t image_buffer_size;
  int image_height;
  int image_width;
};
