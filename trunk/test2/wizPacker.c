#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*
 * File after packer
 * [NORMAL ELF FILE]
 * [FILE 1 NAME (null terminated char array)]
 * [FILE 1 DATA]
 * [FILE 2 NAME (null terminated char array)]
 * [FILE 2 DATA]
 * [FILE ... NAME (null terminated char array)]
 * [FILE ... DATA]
 * [FILE 1 LENGTH (unsigned long int)] (note: only data without filename)
 * [FILE 1 OFFSET (unsigned long int)] (note: points to file name)
 * [FILE 2 LENGTH (unsigned long int)]
 * [FILE 2 OFFSET (unsigned long int)]
 * [FILE ... LENGTH (unsigned long int)]
 * [FILE ... OFFSET (unsigned long int)]
 * [NUMBER OF ENTRIES IN wizPacker (unsigned int)]
 * [VERSION (unsigned char)] = 0
 * [MAGIC NUMBER (null terminated char array 'wizPacker\0')]
 */

int main(int argc, char * argv[])
{
	const char magic[] = "wizPacker";
	const unsigned char version = 0;
	int descOut = open(argv[1], O_WRONLY);
	if ( -1 == descOut )
	{
		printf("Ouch! can't open the output file '%s'\n", argv[1]);
		return EXIT_FAILURE;
	}
	if ( argc < 3)
	{
		printf("Remember to include files to add to '%s'\n", argv[1]);
		return EXIT_FAILURE;

	}
	void * fileTable = malloc((argc-2) * (sizeof(unsigned long int)+ sizeof(unsigned long int)));
	unsigned long int * fileTablePtr = fileTable;
	unsigned int fileTableEntries = 0;

	int filenumber;
	for (filenumber = 2;filenumber < argc;filenumber++)
	{
		size_t fileSizeOut = lseek(descOut, 0x0, SEEK_END);
		printf("Filenumber: %d '%s'\n", filenumber-1, argv[filenumber]);
		int descIn = open(argv[filenumber], O_RDONLY);
		if ( -1 == descIn )
		{
			printf("Ouch! can't open the input file '%s'\n", argv[filenumber]);
			return EXIT_FAILURE;
		}
		size_t fileSizeIn = lseek(descIn, 0x0, SEEK_END);
		
		printf("File size %d\n", fileSizeIn);
		lseek(descIn, 0x0, SEEK_SET);
		void *buffer = malloc(fileSizeIn);
		printf("buffer allocated at: %p\n", buffer);
		size_t readed = read(descIn, buffer, fileSizeIn);
		if ( fileSizeIn != readed )
		{
			printf("Readed data: %d\n", readed);
			printf("Unable to read the input file '%s'\n", argv[filenumber]);
			close(descIn);
			close(descOut);
			free(buffer);
			return EXIT_FAILURE;
		}
		close(descIn);
		write(descOut, argv[filenumber], strlen(argv[filenumber])+1);
		write(descOut, buffer, fileSizeIn);
		free(buffer);
		*fileTablePtr = (unsigned long int)fileSizeOut;
		fileTablePtr++;
		*fileTablePtr = (unsigned long int)fileSizeIn;
		fileTableEntries++;
		printf("Offset at: 0x%.4x\n", fileSizeOut);
		printf("Size: 0x%.4x\n", fileSizeIn);

	}
	write(descOut, fileTable, (argc-1) * (sizeof(unsigned long int)+ sizeof(unsigned long int)));
	write(descOut, &fileTableEntries, sizeof(fileTableEntries));
	write(descOut, &version, sizeof(version));
	write(descOut, &magic, strlen(magic)+1);
	close(descOut);			// safely ignore close
	return EXIT_SUCCESS;
}

