#include "zfp.h"

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int main()
{
	size_t nx = 256;
	size_t ny = 256;
	float *array = malloc(nx * ny * sizeof(zfp_type_float));

	for (int i = 0; i < nx * ny; i++)
	{
		array[i] = (float)0.131;
	}

	FILE* fp = fopen("C:\\Projects\\git\\zfp\\code\\input", "w");
	// check for error here

	for (unsigned i = 0; i < nx*ny; i++) {
		fprintf(fp, "%.2f\n", array[i]);
		// check for error here too
	}

	fclose(fp);
}
float getRandom() {
	float low = 0;
	float high = 1;
	return (rand() / (float)(RAND_MAX)) * abs(low - high) + low;
}

