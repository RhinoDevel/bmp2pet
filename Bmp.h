
// MT, 2016aug23

#ifndef MT_BMP
#define MT_BMP

#include "Dim.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * - RGB
 * - 8 bits per color channel.
 */
struct Bmp
{
    unsigned char * const p; // Pixel data.
    struct Dim const d; // Image dimensions.
};

void Bmp_delete(struct Bmp * const inBmp);

void Bmp_save(struct Bmp const * const inBmp, char const * const inFilePath);

/** Load bitmap from file.
 *
 * - Caller takes ownership. Free by calling Bmp_delete().
 */
struct Bmp * Bmp_load(char const * const inFilePath);

/** Create a bitmap with given dimensions.
 *  Pixel values may be random.
 *
 * - Caller takes ownership. Free by calling Bmp_delete().
 */
struct Bmp * Bmp_create(int inWidth, int inHeight);

#ifdef __cplusplus
}
#endif

#endif // MT_BMP
