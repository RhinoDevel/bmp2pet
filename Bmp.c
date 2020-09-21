
// MT, 2016aug23

#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "Sys.h"
#include "Bmp.h"

static uint16_t const BITS_PER_PIXEL = 24;
static uint16_t const PLANES = 1;
static uint32_t const COMPRESSION = 0;
static uint32_t const X_PIXEL_PER_METER = 0x130B; // 2835, 72 DPI.
static uint32_t const Y_PIXEL_PER_METER = 0x130B; // 2835, 72 DPI.

#pragma pack(push,1)
struct FileHeader
{
    char signature[2];
    uint32_t fileSize;
    uint32_t reserved;
    uint32_t fileOffsetToPixelArr;
};
struct BitmapInfoHeader
{
    uint32_t dibHeaderSize;
    uint32_t width;
    uint32_t height;
    uint16_t planes;
    uint16_t bitsPerPixel;
    uint32_t compression;
    uint32_t imgSize;
    uint32_t yPixelPerMeter;
    uint32_t xPixelPerMeter;
    uint32_t numColorsPalette;
    uint32_t mostImpColor;
};
struct Bitmap
{
    struct FileHeader fileHeader;
    struct BitmapInfoHeader bmpInfoHeader;
};
#pragma pack(pop)

static void flipVertically(unsigned char * const inOutPixels, int const inWidth, int const inHeight)
{
    assert(inOutPixels!=NULL);
    assert(inWidth>0);
    assert(inHeight>0);

    size_t const rowByteWidth = inWidth*3*sizeof(unsigned char); // MT_TODO: TEST: Hard-coded for three channels!
    unsigned char * const rowBuf = (unsigned char *)malloc(rowByteWidth);

    for(int row = 0;row<inHeight/2;++row)
    {
        unsigned char * const rowPtr = inOutPixels+row*rowByteWidth,
            * const otherRowPtr = inOutPixels+(inHeight-1-row)*rowByteWidth;

        memcpy(rowBuf, otherRowPtr, rowByteWidth);
        memcpy(otherRowPtr, rowPtr, rowByteWidth);
        memcpy(rowPtr, rowBuf, rowByteWidth);
    }

    free(rowBuf);
}

static void save(int const inWidth, int const inHeight, unsigned char const * const inPixels, char const * const inFilePath)
{
    assert(!Sys_is_big_endian());
    assert(inWidth>0);
    assert(inHeight>0);
    assert(inPixels!=NULL);
    assert(inFilePath!=NULL);

    struct Bitmap * const bmp = calloc(1, sizeof *bmp);
    uint32_t const pixelByteSize = (inWidth*inHeight*BITS_PER_PIXEL)/8;

    bmp->fileHeader.signature[0] = 'B';
    bmp->fileHeader.signature[1] = 'M';
    bmp->fileHeader.fileSize = sizeof *bmp+pixelByteSize;
    bmp->fileHeader.fileOffsetToPixelArr = sizeof *bmp;

    bmp->bmpInfoHeader.dibHeaderSize =sizeof bmp->bmpInfoHeader;
    bmp->bmpInfoHeader.width = inWidth;
    bmp->bmpInfoHeader.height = -inHeight; // HARD-CODED top to bottom pixel order by negation!
    bmp->bmpInfoHeader.planes = PLANES;
    bmp->bmpInfoHeader.bitsPerPixel = BITS_PER_PIXEL;
    bmp->bmpInfoHeader.compression = COMPRESSION;
    bmp->bmpInfoHeader.imgSize = pixelByteSize;
    bmp->bmpInfoHeader.yPixelPerMeter = Y_PIXEL_PER_METER;
    bmp->bmpInfoHeader.xPixelPerMeter = X_PIXEL_PER_METER;
    bmp->bmpInfoHeader.numColorsPalette = 0;
    bmp->bmpInfoHeader.mostImpColor = 0;

    FILE * const fp = fopen(inFilePath,"wb");

    fwrite(bmp, 1, sizeof *bmp ,fp);
    fwrite(inPixels, 1, pixelByteSize, fp);
    fclose(fp);
    free(bmp);
}

/** Return image data from bitmap file at given path. Not really correctly implemented for all kinds of bitmaps.
 *
 * - See: http://stackoverflow.com/questions/14279242/read-bitmap-file-into-structure#14279511
 */
static unsigned char * load(char const * const inFilePath, int * const inOutWidth, int * const inOutHeight)
{
    assert(!Sys_is_big_endian());
    assert(inFilePath!=NULL);
    assert(inOutWidth!=NULL);
    assert(inOutHeight!=NULL);

    unsigned char * imgData = NULL;
    FILE* filePtr = NULL;
    struct FileHeader fileHeader;
    struct BitmapInfoHeader infoHeader;

    *inOutWidth = -1;
    *inOutHeight = -1;

    filePtr = fopen(inFilePath, "rb");
    if(filePtr==NULL)
    {
        return NULL;
    }

    fread(&fileHeader, sizeof(struct FileHeader), 1, filePtr);

    if(fileHeader.signature[0]!='B' || fileHeader.signature[1]!='M')
    {
        fclose(filePtr);
        return NULL;
    }

    fread(&infoHeader, sizeof(struct BitmapInfoHeader), 1, filePtr);

    fseek(filePtr, fileHeader.fileOffsetToPixelArr, SEEK_SET);

    imgData = malloc(infoHeader.imgSize);
    if (imgData==NULL)
    {
        fclose(filePtr);
        return NULL;
    }

    fread(imgData, infoHeader.imgSize, 1, filePtr);
	
	uint32_t const absHeight = abs(infoHeader.height);

    if(absHeight > 0)
    {
        flipVertically(imgData, infoHeader.width, absHeight);
    }

    fclose(filePtr);
    *inOutWidth = infoHeader.width;
    *inOutHeight = absHeight;
    return imgData;
}

void Bmp_delete(struct Bmp * const inBmp)
{
    assert(inBmp!=NULL);

    assert(inBmp->p!=NULL);
    free(inBmp->p);

    free(inBmp);
}

void Bmp_save(struct Bmp const * const inBmp, char const * const inFilePath)
{
    assert(inBmp!=NULL);
    save(inBmp->d.w, inBmp->d.h, inBmp->p, inFilePath);
}

struct Bmp * Bmp_load(char const * const inFilePath)
{
    struct Bmp * const retVal = malloc(sizeof *retVal);
    struct Dim d;

    assert(retVal != NULL);

	unsigned char * const p = load(inFilePath, &d.w, &d.h);

    if(p == NULL)
	{
        return NULL;
	}

    struct Bmp const buf = (struct Bmp)
    {
        .p = p,
        .d = d
    };
    memcpy(retVal, &buf, sizeof *retVal);
    return retVal;
}

struct Bmp * Bmp_create(int inWidth, int inHeight)
{
    struct Bmp * const retVal = malloc(sizeof *retVal);
    struct Dim const d = (struct Dim)
    {
        .w = inWidth,
        .h = inHeight
    };
    unsigned char * const p = malloc((size_t)(3*d.w*d.h)); // RGB

    assert(retVal!=NULL);

    struct Bmp const buf = (struct Bmp)
    {
        .p = p,
        .d = d
    };
    memcpy(retVal, &buf, sizeof *retVal);
    return retVal;
}
