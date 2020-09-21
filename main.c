
// MT, 2020sep21

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "Deb.h"
#include "Sys.h"
#include "FileSys.h"
#include "Bmp.h"

static void print_bmp(struct Bmp const * const b)
{
	static char const fg = 'O';
	static char const bg = '.';
	
	for(int row = 0;row < b->d.h;++row)
    {
        unsigned char * const row_ptr = b->p + 3 * row * b->d.w;

        for(int col = 0;col < b->d.w;++col)
        {
            unsigned char * const col_ptr = row_ptr + 3 * col;
			char c = fg;
			
			if(col_ptr[0] == 0 && col_ptr[1] == 0 && col_ptr[2] == 0)
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
		Deb_line(
            "Error: Failed to load bitmap file!")
        return 2;
    }

    Sys_log_line(
        true, true, "Bitmap resolution = %d x %d pixels.", bmp->d.w, bmp->d.h);
        
	print_bmp(bmp);
		
	// TODO: Implement!
		
    Bmp_delete(bmp);
    return 0;
}
