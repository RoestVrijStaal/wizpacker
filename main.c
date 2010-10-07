#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define MAJOR_VERSION 0
#define MINOR_VERSION 1

#define SIZE_OF_LINES 20

void showHelp(int argc, char * argv[])
{
	fprintf(stdout, "\nWIZfilePacker v.%d.%d\n", MAJOR_VERSION, MINOR_VERSION);
	fprintf(stdout, "-----------------------\n\n");
	fprintf(stdout, "Usage %s <input file> <output file.h>\n\n", argv[0]);

}

int main(int argc, char * argv[])
{
	if ( argc < 3 )
	{
		fprintf(stderr, "Error: Invalid arguments\n");
		showHelp(argc, argv);
		return EXIT_FAILURE;
	}
	// open files
	int inDescriptor = open(argv[1], O_RDONLY);
	if ( -1 == inDescriptor )
	{
		fprintf(stderr, "Error: unable to open in file '%s' ABORTING\n", argv[1]);
		return EXIT_FAILURE;
	}
	int outDescriptor = creat(argv[2], S_IRUSR + S_IWUSR);
	if ( -1 == inDescriptor )
	{
		fprintf(stderr, "Error: unable to open out file '%s' ABORTING\n", argv[2]);
		close(inDescriptor);
		return EXIT_FAILURE;
	}
	// obtain file size
	off_t fileSize = lseek(inDescriptor, 0, SEEK_END);
	lseek(inDescriptor, 0, SEEK_SET);

	// convert filename into a C variable
	char *variable = malloc(strlen(argv[1])+1);
	if ( 0x0 == variable )
	{
		fprintf(stderr, "Error: unable to allocate file variable ABORTING\n");
		close(outDescriptor);
		close(inDescriptor);
		return EXIT_FAILURE;
	}
	strcpy(variable, argv[1]);
	char *ptr = variable;
	while (*ptr != 0x0)
	{
		// @todo if the file have numbers? ;)

		if ( ( *ptr < 65 ) || ( *ptr > 122 ) )
		{
			*ptr = 0x5f; // replace by '_'
		}
		ptr++;
	}
	char *declarationFirstPart = "const char wiz_filepacker_";
	char *declarationSecondPart = "_data [] = { ";
	char *declarationThirdPart = " };\n";
	char *valuePrototipe = "0x00, ";
	unsigned long newlines = fileSize / SIZE_OF_LINES;
	// filesize must be multiplied by strlen("0x00, ")
	char *declaration = malloc(strlen(declarationFirstPart) + strlen(variable) +
				strlen(declarationSecondPart) + (fileSize * strlen(valuePrototipe)) +
				strlen(declarationThirdPart)+1+newlines);
	if ( 0x0 == declaration )
	{
		fprintf(stderr, "Error: unable to allocate string data ABORTING\n");
		free(variable);
		close(outDescriptor);
		close(inDescriptor);
		return EXIT_FAILURE;
	}
	sprintf(declaration, "%s%s%s", declarationFirstPart, variable, declarationSecondPart);

	unsigned char *fileContents = malloc(fileSize);
	if ( 0x0 == fileContents )
	{
		fprintf(stderr, "Error: unable to allocate in file contents ABORTING\n");
		free(declaration);
		free(variable);
		close(outDescriptor);
		close(inDescriptor);
		return EXIT_FAILURE;
	}
	read(inDescriptor, fileContents, fileSize);
	off_t position;
	char *currentValue = malloc(strlen(valuePrototipe)+1);
	for (position = 0; position<fileSize;position++)
	{
		sprintf(currentValue, "0x%.2x, ", fileContents[position] );
		sprintf(declaration+strlen(declaration), "%s", currentValue);
		if ( 0x0 == (position % SIZE_OF_LINES ) )
		{
			sprintf(declaration, "%s\n", declaration );
		}
	}
	char *endPos = declaration+strlen(declaration)-2;
	sprintf(endPos, "%s", declarationThirdPart);
	write(outDescriptor, declaration, strlen(declaration));
	const char *originalFileNameDeclarationOne = "const char wiz_filepacker_";
	const char *originalFileNameDeclarationTwo = " [] = \"";
	const char *originalFileNameDeclarationEnd = "\";\n";
	write(outDescriptor, originalFileNameDeclarationOne, strlen(originalFileNameDeclarationOne));
	write(outDescriptor, variable, strlen(variable));
	write(outDescriptor, originalFileNameDeclarationTwo, strlen(originalFileNameDeclarationTwo));
	write(outDescriptor, argv[1], strlen(argv[1]));
	write(outDescriptor, originalFileNameDeclarationEnd, strlen(originalFileNameDeclarationEnd));

	const char *originalFileSizeOne = "const unsigned long wiz_filepacker_";
	const char *originalFileSizeTwo ="_size = ";
	const char *originalFileSizeEnd = ";\n";
	char *sizeAsString = malloc(32);
	sprintf(sizeAsString,"%lu", fileSize);
	write(outDescriptor, originalFileSizeOne, strlen(originalFileSizeOne));
	write(outDescriptor, variable, strlen(variable));
	write(outDescriptor, originalFileSizeTwo, strlen(originalFileSizeTwo));
	write(outDescriptor, sizeAsString, strlen(sizeAsString));
	write(outDescriptor, originalFileSizeEnd, strlen(originalFileSizeEnd));
	free(sizeAsString);


	close(inDescriptor);
	close(outDescriptor);

	free(declaration);
	free(variable);
	free(fileContents);

	return EXIT_SUCCESS;
}
