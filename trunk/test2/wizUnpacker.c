#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define WIZUNPACK_FILENAMESIZE 512

char * unpackFile(char * me, char * fileName)
{
	unsigned int entries = 0x0;
	char *extractedFile = NULL;
	const char magic[] = {0,'w','i','z','P','a','c','k','e','r',0};
	char isMagic[11];
	int containerDesc = open(me, O_RDONLY);
	if (-1 == containerDesc )
	{
		#ifdef _DEBUG
		printf("Unable to open myself\n");
		#endif
		return NULL;
	}
	size_t containerFileSize = lseek(containerDesc, 0x0, SEEK_END);
	lseek(containerDesc, -11 , SEEK_END);
	read(containerDesc, isMagic, 11);
	if ( memcmp(magic, isMagic, 11) != 0x0 )
	{
		#ifdef _DEBUG
		printf("Don't seems a valid wizPacker file\n");
		#endif
		close(containerDesc);
		return NULL;		
	}

	lseek(containerDesc, (11+sizeof(entries))*-1 , SEEK_END);
	read(containerDesc, &entries, sizeof(entries));
	#ifdef _DEBUG
	printf("Entries in file: %i\n", entries);
	#endif
	unsigned int c;
	size_t currentOffset = lseek(containerDesc, 0x0 , SEEK_CUR);
	currentOffset-=sizeof(entries); // substract last read

	for (c=entries;c>0;c--)
	{
		
		unsigned long int length;
		unsigned long int offset;
		char *containedFilename = malloc(WIZUNPACK_FILENAMESIZE);		// arbitrary size
		if ( NULL == containedFilename )
		{
			#ifdef _DEBUG
			printf("Unable to allocate size for filename aborting\n");
			#endif 
			close(containerDesc);
			return NULL;
		}
		currentOffset-=sizeof(length)+sizeof(offset);	 // substract table entry size
		currentOffset = lseek(containerDesc, currentOffset , SEEK_SET);
		#ifdef _DEBUG
		printf("Current offset %x\n", currentOffset);
		#endif 
		read(containerDesc, &offset, sizeof(offset));
		read(containerDesc, &length, sizeof(length));
		#ifdef _DEBUG
		printf("Entry number %u\n", c);
		printf("Length: %lu\n", length);
		printf("Offset: 0x%lx\n", offset);
		#endif
		if ( offset >= containerFileSize )
		{
			#ifdef _DEBUG
			printf("Current offset %x is out of container bounds ¿corrupted entry? aborting\n", currentOffset);
			#endif 
			free(containedFilename);
			close(containerDesc);
			return NULL;
		}
		lseek(containerDesc, offset , SEEK_SET);
		read(containerDesc, containedFilename, WIZUNPACK_FILENAMESIZE);
		if ( strcmp(containedFilename, fileName) == 0x0)
		{
			#ifdef _DEBUG
			printf("File found '%s'\n", fileName);
			lseek(containerDesc, offset + strlen(fileName)+1, SEEK_SET);
			#endif 
			void *data = malloc(length);
			if ( NULL == data )
			{
				#ifdef _DEBUG
				printf("Unable to alloc file size of '%s' = %lu\n", fileName, length);
				#endif 
				free(containedFilename);
				close(containerDesc);
				return NULL;
			}
			read(containerDesc, data, length); //@todo protect this ;)
			int fileOutDescriptor = creat(fileName, S_IRUSR | S_IWUSR);
			if ( -1 == fileOutDescriptor )
			{
				#ifdef _DEBUG
				printf("Unable to create file '%s'\n", fileName);
				#endif 
				free(data);
				free(containedFilename);
				close(containerDesc);
				return NULL;
			}
			write(fileOutDescriptor, data, length); //@todo protect this ;)
			close(fileOutDescriptor);
			#ifdef _DEBUG
			printf("file ready at '%s'\n", fileName);
			#endif 
			free(data);
			close(containerDesc);
			return containedFilename;

		}
		currentOffset-=sizeof(length)+sizeof(offset);
		currentOffset = lseek(containerDesc, currentOffset , SEEK_SET);
		free(containedFilename);
	}
	close(containerDesc);
	return extractedFile;
}

