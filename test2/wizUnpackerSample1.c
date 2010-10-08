#include "wizUnpacker.c"

int main(int argc, char * argv [])
{
	char *fullPathFilename = unpackFile(argv[0], "testfile.bin");
	if ( NULL != fullPathFilename )
	{
		printf("Result: '%s'\n", fullPathFilename);
		free(fullPathFilename);
		return EXIT_SUCCESS;
	}
	printf("Unable to unpack file\n");
	return EXIT_FAILURE;
}
