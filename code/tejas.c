#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "zfp.h"

/* compress or decompress array */
static int
compress(double* array, size_t nx, size_t ny, double tolerance, zfp_bool decompress)
{
  int status = 0;    /* return value: 0 = success */
  zfp_type type;     /* array scalar type */
  zfp_field* field;  /* array meta data */
  zfp_stream* zfp;   /* compressed stream */
  void* buffer;      /* storage for compressed stream */
  size_t bufsize;    /* byte size of compressed buffer */
  bitstream* stream; /* bit stream to write to or read from */
  size_t zfpsize;    /* byte size of compressed stream */

  /* allocate meta data for the 3D array a[nz][ny][nx] */
  type = zfp_type_int32;
  field = zfp_field_2d(array, type, nx, ny);
  printf("1\n");

  /* allocate meta data for a compressed stream */
  zfp = zfp_stream_open(NULL);
 
  
  printf("2\n");
  /* set compression mode and parameters via one of four functions */
/*  zfp_stream_set_reversible(zfp); */
/*  zfp_stream_set_rate(zfp, rate, type, zfp_field_dimensionality(field), zfp_false); */
/*  zfp_stream_set_precision(zfp, precision); */
  zfp_stream_set_accuracy(zfp, tolerance);

  printf("3\n");
  /* allocate buffer for compressed data */
  bufsize = zfp_stream_maximum_size(zfp, field);
  buffer = malloc(bufsize);

  printf("4\n");
  /* associate bit stream with allocated buffer */
  stream = stream_open(buffer, bufsize);
  printf("4.0\n");
  printf("4.1\n");
  zfp_stream_set_bit_stream(zfp, stream);
  printf("4.2\n");
  zfp_stream_rewind(zfp);
  printf("4.3\n");
  

  printf("5\n");
  /* compress or decompress entire array */
  if (decompress) {
    /* read compressed stream and decompress and output array */
    zfp_bool cude_set = zfp_stream_set_execution(zfp, zfp_exec_cuda);
    printf("Here %d\b",cude_set);
    zfpsize = fread(buffer, 1, bufsize, stdin);
    if (!zfp_decompress(zfp, field)) {
      fprintf(stderr, "decompression failed\n");
      status = EXIT_FAILURE;
    }
    else
      fwrite(array, sizeof(double), zfp_field_size(field, NULL), stdout);
  }
  else {
    /* compress array and output compressed stream */
    printf("6\n");
    zfpsize = zfp_compress(zfp, field);
    if (!zfpsize) {
      fprintf(stderr, "compression failed\n");
      status = EXIT_FAILURE;
    }
    else
      fwrite(buffer, 1, zfpsize, stdout);
  }

  /* clean up */
  zfp_field_free(field);
  zfp_stream_close(zfp);
  stream_close(stream);
  free(buffer);
  free(array);

  return status;
}

int main(int argc, char* argv[])
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