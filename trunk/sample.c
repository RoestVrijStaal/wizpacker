#include <stdlib.h>
#include "unpacker.c"

// include your generated packed files as headers
#include "samplePacked.h"

int main(int argc, char * argv[])
{
	// call this to obtain file full path
	char * filename = unpackFileFromVariable(wiz_filepacker_sample_c, wiz_filepacker_sample_c_data, wiz_filepacker_sample_c_size);
	if ( NULL == filename )
	{
		printf("Unable to unpack\n");
		return EXIT_FAILURE;
	}
	printf("The filename is availiable at %s\n", filename);
	free(filename);
	return EXIT_SUCCESS;
}
