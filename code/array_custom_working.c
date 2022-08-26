#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "zfp.h"

/* compress or decompress array */
struct setupVars {
  zfp_stream* stream;
  zfp_field* field;
  bitstream* bs;
  void* buffer;
  size_t streamSize;
};

static int
compress(double* array, size_t nx, size_t ny, double tolerance, zfp_bool decompress)
{int status = 0;    /* return value: 0 = success */
  zfp_type type;     /* array scalar type */
  zfp_field* field;  /* array meta data */
  zfp_stream* zfp;   /* compressed stream */
  void* buffer;      /* storage for compressed stream */
  size_t bufsize;    /* byte size of compressed buffer */
  bitstream* stream; /* bit stream to write to or read from */
  size_t zfpsize;    /* byte size of compressed stream */

  /* allocate meta data for the 3D array a[nz][ny][nx] */
  type = zfp_type_double;
  field = zfp_field_2d(array, type, nx, ny);
  
  /* allocate meta data for a compressed stream */
  zfp = zfp_stream_open(NULL);

  /* set compression mode and parameters via one of four functions */
/*  zfp_stream_set_reversible(zfp); */
/*  zfp_stream_set_rate(zfp, rate, type, zfp_field_dimensionality(field), zfp_false); */
/*  zfp_stream_set_precision(zfp, precision); */
  zfp_stream_set_accuracy(zfp, tolerance);

  /* allocate buffer for compressed data */
  bufsize = zfp_stream_maximum_size(zfp, field);
  buffer = malloc(bufsize);

  /* associate bit stream with allocated buffer */
  stream = stream_open(buffer, bufsize);
  zfp_stream_set_bit_stream(zfp, stream);
  zfp_stream_rewind(zfp);

 zfp_bool a =  zfp_stream_set_execution(zfp, zfp_exec_cuda);
  printf("7 %d", a);
  zfpsize = zfp_compress(zfp, field);
  if (!zfpsize) {
    fprintf(stderr, "compression failed\n");
    status = EXIT_FAILURE;
  }
  else
    fwrite(buffer, 1, zfpsize, stdout);
 
  zfpsize = fread(buffer, 1, bufsize, stdin);
  zfp_decompress(zfp, field);

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