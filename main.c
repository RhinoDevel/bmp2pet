
// MT, 2020sep21

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "Deb.h"
#include "Sys.h"
#include "FileSys.h"
#include "Bmp.h"

static inline bool is_background(unsigned char const * const pix_ptr)
{
	return pix_ptr[0] == 0 && pix_ptr[1] == 0 && pix_ptr[2] == 0;
}

static void print_bmp_monochrome(struct Bmp const * const b)
{
	static char const fg = 'O';
	static char const bg = '.';
	static char const sep_h = '-';
	static char const sep_v = '|';
	static char const sep_b = '+';
	
	int col = -1;
	
	printf(" %c", sep_v);
	while(++col < b->d.w)
	{
		printf("%d", col % 10);
	}
	printf("\n");
	printf("%c%c", sep_h, sep_b);
    for(col = 0;col < b->d.w;++col)
	{
		printf("%c", sep_h);
	}
	printf("\n");

	for(int row = 0;row < b->d.h;++row)
    {
        unsigned char * const row_ptr = b->p + 3 * row * b->d.w;

        printf("%d%c", row % 10, sep_v);

        for(col = 0;col < b->d.w;++col)
        {
            unsigned char * const col_ptr = row_ptr + 3 * col;
			char c = fg;
			
			if(is_background(col_ptr))
			{
				c = bg;
			}
			printf("%c", c);
		}
		printf("\n");
	}
}

/**
 * - Caller takes ownership of returned object.
 */
static unsigned char * create_petscii(
	struct Bmp const * const bmp, int * const out_len)
{
	assert(out_len != NULL);
	
	static int const channel_count = 3;
	
	/** CBM/PET PETSCII "big pixel" characters.
	 *
	 *  Ordered from first quadrant at top-right to fourth (clockwise).
	 *
	 *  3|0
	 *  -+-
	 *  2|1
	 */
	static unsigned char const pet_val[16] = {
	         // 3 2 1 0 <= Quadrants of "sub characters" / "big pixels".
		32,  // 0 0 0 0
		124, // 0 0 0 1
		108, // 0 0 1 0
		225, // ...
		123,
		255,
		98,
		254,
		126,
		226, // 1 0 0 1
		127,
		251,
		97,
		236,
		252,
		224
	};
	
	int const out_width = (bmp->d.w + bmp->d.w % 2) / 2,
			out_height = (bmp->d.h + bmp->d.h % 2) / 2,
			channel_width = channel_count * bmp->d.w;
			
	Deb_line(
		"Info: output character width = %d, height = %d.",
		out_width,
		out_height)
		
	*out_len = out_width * out_height;
		
	unsigned char * const ret_val = malloc(*out_len);
	
	for(int out_row = 0;out_row < out_height;++out_row)
	{
		unsigned char * const out_row_ptr = ret_val + out_row * out_width;
		
		for(int out_col = 0;out_col < out_width;++out_col)
		{
			int const sector_3_0_row = 2 * out_row,
					sector_3_2_col = 2 * out_col,
					sector_2_1_row = sector_3_0_row + 1,
					sector_0_1_col = sector_3_2_col + 1;
			unsigned char const * const sector_3_ptr =
					bmp->p + sector_3_0_row * channel_width
						+ channel_count * sector_3_2_col,
				* const sector_2_ptr = sector_3_ptr + channel_width;
			unsigned char * const out_col_ptr = out_row_ptr + out_col;
			unsigned char pet_index = 0;
			bool const sector_2_1_row_exists = sector_2_1_row < bmp->d.h;
			
			// 3|0
			// -+-
			// 2|1
			
			// Sector 3:
			//
			if(!is_background(sector_3_ptr))
			{				
				pet_index |= 8;
			}
			
			// Sector 2:
			//
			if(sector_2_1_row_exists && !is_background(sector_2_ptr))
			{
				pet_index |= 4;
			}
			
			if(sector_0_1_col < bmp->d.w)
			{
				// Sector 1:
				//
				if(sector_2_1_row_exists
					&& !is_background(sector_2_ptr + channel_count))
				{
					pet_index |= 2;
				}
				
				// Sector 0:
				//
				if(!is_background(sector_3_ptr + channel_count))
				{
					pet_index |= 1;
				}
			}
			
			assert(pet_index >= 0 && pet_index < 16);
			
			*out_col_ptr = pet_val[(int)pet_index];
		}
	}
	return ret_val;
}

int main(int argc, char* argv[])
{
	int len = -1;
	
    if(argc != 3)
    {
        Deb_line(
            "Error: Exactly two arguments (src. & dest. files) must be given!")
        return 1;
    }

    struct Bmp * const bmp = Bmp_load(argv[1]);

    if(bmp == NULL)
    {
		Deb_line("Error: Failed to load bitmap file!")
        return 2;
    }

    Sys_log_line(
        true, true, "Bitmap resolution = %d x %d pixels.", bmp->d.w, bmp->d.h);
        
	print_bmp_monochrome(bmp);
	
	unsigned char * const petscii = create_petscii(bmp, &len);
	
	if(!FileSys_saveFile(argv[2], (size_t)len, petscii))
	{
		Deb_line("Error: Failed to save PETSCII to file!")
		return 3;
	}
		
    Bmp_delete(bmp);
	free(petscii);
    return 0;
}
