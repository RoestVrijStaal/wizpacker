#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

char * unpackFileFromVariable(const char *fileName, const char *fileData, unsigned long fileSize)
{
	const char tmpPath[] = "/tmp/";
	char *tmpfileName = malloc(strlen(fileName)+strlen(tmpPath)+1);
	sprintf(tmpfileName, "%s%s", tmpPath, fileName);
	int outDescriptor = creat(tmpfileName, S_IRUSR + S_IWUSR);
	if ( -1 == outDescriptor )
	{
		free(tmpfileName);
		return 0x0;
	}
	unsigned long writed = write(outDescriptor, fileData, fileSize);
	if (writed != fileSize )
	{
		free(tmpfileName);
		return 0x0;
	}
	close(outDescriptor);
	return tmpfileName;
}
