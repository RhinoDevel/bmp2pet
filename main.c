
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

int main(int argc, char* argv[])
{
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
		
	// TODO: Implement!
		
    Bmp_delete(bmp);
    return 0;
}
