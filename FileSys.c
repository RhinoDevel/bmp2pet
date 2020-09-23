
// MT, 2016mar19

#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h>
#include "Deb.h"
#include "FileSys.h"

/** Source: http://stackoverflow.com/questions/8236/how-do-you-determine-the-size-of-a-file-in-c
 */
off_t FileSys_GetFileSize(char const * const inPath)
{
    struct stat s;

    assert(inPath!=NULL);

    if(stat(inPath, &s)==0)
    {
        return s.st_size; // *** RETURN ***
    }

    return -1;
}

unsigned char * FileSys_loadFile(
    char const * const inPath, off_t * const inOutSize)
{
    *inOutSize = -1;

    off_t const signed_size = FileSys_GetFileSize(inPath);

    if(signed_size==-1)
    {
        Deb_line("Error: Failed to get size of file \"%s\"!", inPath)
        return NULL;
    }

    FILE * const file = fopen(inPath, "rb");

    if(file==NULL)
    {
        Deb_line("Error: Failed to open source file \"%s\"!", inPath)
        return NULL;
    }

    size_t const size = (size_t)signed_size;
    unsigned char * const buf = malloc(size*sizeof(*buf));

    if(fread(buf, sizeof(*buf), size, file)!=size)
    {
        Deb_line("Error: Failed to completely load character ROM file content!")
        return NULL;
    }

    fclose(file);
    *inOutSize = signed_size;
    return buf;
}

bool FileSys_saveFile(
	char const * const path,
	size_t const size,
	unsigned char const * const bytes)
{
    assert(path != NULL);
    assert(bytes != NULL);

    FILE * const file = fopen(path, "wb");

	if(file == NULL)
    {
        Deb_line("Error: Failed to open destination file \"%s\"!", path)
        return false;
    }

    if(fwrite(bytes, 1, size, file) != size)
	{
        Deb_line("Error: Failed to completely save bytes to file!")
        return false;
    }
	
    fclose(file);
	return true;
}
