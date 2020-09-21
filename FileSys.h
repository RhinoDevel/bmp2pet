
// MT, 2016mar19

#ifndef MT_FILESYS
#define MT_FILESYS

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

off_t FileSys_GetFileSize(char const * const inPath);

/** Return content of file at given path.
 *
 *  - Returns NULL on error.
 *  - Caller takes ownership of return value.
 */
unsigned char * FileSys_loadFile(
    char const * const inPath, off_t * const inOutSize);

#ifdef __cplusplus
}
#endif

#endif // MT_FILESYS
