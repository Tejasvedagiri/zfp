#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "zfp.h"

/* compress or decompress array */
struct setupVars
{
  zfp_stream *stream;
  zfp_field *field;
  bitstream *bs;
  void *buffer;
  size_t streamSize;
};

static int
compress(double *array, size_t nx, size_t ny, double tolerance, zfp_bool decompress)
{
  printf("1\n");
  struct setupVars *bundle = malloc(sizeof(struct setupVars));

  printf("2\n");
  bundle->stream = zfp_stream_open(NULL);
  printf("3\n");

  bundle->field = zfp_field_2d(array, zfp_type_int32, nx, ny);

  size_t bufferSize = zfp_stream_maximum_size(bundle->stream, bundle->field);
  bundle->buffer = malloc(bufferSize);
  memset(bundle->buffer, 0, bufferSize);
  printf("4");
  bundle->bs = stream_open(bundle->buffer, bufferSize);
  stream_skip(bundle->bs, stream_word_bits + 1);
  printf("5");
  bundle->streamSize = stream_size(bundle->bs);
  printf("6");
  zfp_bool a = zfp_stream_set_execution(bundle->stream, zfp_exec_cuda);
  printf("7 %d", a);

  printf("8");

  size_t zfpsize = zfp_compress(bundle->stream, bundle->field);

  printf("Compressed %d", zfpsize);
  /*
  fwrite(bundle->buffer, 1, zfpsize, stdout);
  */

  return 0;
}

int main(int argc, char *argv[])
{
  zfp_bool decompress = (argc == 2 && !strcmp(argv[1], "-d"));

  size_t nx = 256;
  size_t ny = 256;

  double *array = malloc(nx * ny * sizeof(double));

  /* initialize array to be compressed */
  size_t i, j, k;
  for (j = 0; j < ny; j++)
    for (i = 0; i < nx; i++)
    {
      double x = 2.0 * i / nx;
      double y = 2.0 * j / ny;
      array[i + j] = 1;
    }

  return compress(array, nx, ny, 1e-3, decompress);
}