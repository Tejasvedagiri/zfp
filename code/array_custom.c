#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "zfp.h"
#include "zfp/internal/zfp/macros.h"
#include <time.h>


int main(int argc, char* argv[])
{
    char* inpath = "C:\\Projects\\git\\zfp\\code\\input";
    char* zfppath = "C:\\Projects\\git\\zfp\\code\\zfp_compressed_file";
    char* outpath = "C:\\Projects\\git\\zfp\\code\\decompressed";
    double tolerance = 0;
    uint precision = 0;
    zfp_type type = zfp_type_double;
    size_t typesize = 0;
    uint dims = 0;
    size_t nx = 256;
    size_t ny = 256;
    //size_t nz = 1000;
    //size_t nw = 1;
    size_t count = 0;
    double rate = 0;
    clock_t start_time;
    clock_t end_time;
    clock_t total_time;

    char mode = "R";
    zfp_exec_policy exec = zfp_exec_serial;
    uint threads = 0;
    uint chunk_size = 0;

    /* local variables */
    zfp_field* field = NULL;

    bitstream* stream = NULL;
    void* fi = NULL;
    void* fo = NULL;
    void* buffer = NULL;
    size_t rawsize = 0;
    size_t zfpsize = 0;
    size_t bufsize = 0;

    zfp_stream* zfp = NULL;
    zfp = zfp_stream_open(NULL);



    for (size_t i = 1; i < argc; i++) {
        switch (argv[i][1]) {
        case 'w':
            inpath = "C:\\Projects\\git\\zfp\\code\\input";
            zfppath = "C:\\Projects\\git\\zfp\\code\\zfp_compressed_file";
            outpath = "C:\\Projects\\git\\zfp\\code\\decompressed";
            break;
        case 'l':
            inpath = "/data/input";
            zfppath = "/data/zfp_compressed_file";
            outpath = "/data/decompressed";
            break;
        case 'R':
            zfp_stream_set_reversible(zfp);
            break;
        case 'a':
            zfp_stream_set_accuracy(zfp, tolerance);
            break;
        case 'p':
            zfp_stream_set_precision(zfp, precision);
        case 'o':
            printf("Out path ==> %s", argv[i]);
            outpath = argv[i];
        case 'x':
            exec = zfp_exec_cuda;
        default:
            break;

        }
    }

    typesize = zfp_type_size(type);
    count = nx * ny;
        //* nz * nw;

    
    field = zfp_field_alloc();

    FILE* input_file = fopen(inpath, "rb");
    rawsize = typesize * count;
    printf("File size ==> %d\n", rawsize);
    fi = malloc(rawsize);
    if (!fi) {
        fprintf(stderr, "cannot allocate memory\n");
        return EXIT_FAILURE;
    }
    size_t read_count = fread(fi, typesize, count, input_file);
    printf("File count ==> %d\n", read_count);
    fclose(input_file);
    zfp_field_set_pointer(field, fi);
    zfp_field_set_type(field, type);
    zfp_field_set_size_2d(field, nx, ny );
        //nz, nw);
    zfp_stream_set_reversible(zfp);
    switch (mode) {
    case 'R':
        zfp_stream_set_reversible(zfp);
        break;
    case 'a':
        zfp_stream_set_accuracy(zfp, tolerance);
        break;
    case 'p':
        zfp_stream_set_precision(zfp, precision);
        break;
    case 'r':
        zfp_stream_set_rate(zfp, rate, type, dims, zfp_false);
        break;
    }
    //zfp_stream_set_execution(zfp, exec);
    bufsize = zfp_stream_maximum_size(zfp, field);
    printf("Buffer size ==> %d\n", bufsize);
    buffer = malloc(bufsize);
    stream = stream_open(buffer, bufsize);
    zfp_stream_set_bit_stream(zfp, stream);


    if (!zfp_stream_set_execution(zfp, exec)) {
        fprintf(stderr, "cuda execution not available\n");
    }
    else {
        printf("Cuda set");
    }

    start_time = clock();
    zfpsize = zfp_compress(zfp, field);
    end_time = clock();
    total_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    printf("Total time ==> %d\n", total_time);

    printf("Comprseed size ==> %d \n", zfpsize);
    FILE* out_compressed_file =  fopen(zfppath, "wb");
    if (!out_compressed_file) {
        fprintf(stderr, "cannot create compressed file\n");
        return EXIT_FAILURE;
    }
    fwrite(buffer, 1, zfpsize, out_compressed_file);
    fclose(out_compressed_file);

    zfp_stream_rewind(zfp);

    typesize = zfp_type_size(type);
    nx = MAX(field->nx, 1u);
    ny = MAX(field->ny, 1u);
    //nz = MAX(field->nz, 1u);
    //nw = MAX(field->nw, 1u);
    count = nx * ny;
        //* nz * nw;
    rawsize = typesize * count;
    fo = malloc(rawsize);
    printf("FO size ==> %d\n", rawsize);
    zfp_field_set_pointer(field, fo);
    start_time = clock();
    zfpsize = zfp_decompress(zfp, field);
    end_time = clock();
    total_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("Total time ==> %d", total_time);

    printf("\nDecompressed size ==> %d\n", zfpsize);
    FILE* out_decompressed_file =  fopen(outpath, "wb");
    fwrite(fo, typesize, count, out_decompressed_file);
    fclose(out_decompressed_file);

    zfp_field_free(field);
    zfp_stream_close(zfp);
    stream_close(stream);
    free(buffer);
    free(fi);
    free(fo);

    return EXIT_SUCCESS;


}
