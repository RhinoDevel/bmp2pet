
// MT, 2020sep21

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "Deb.h"
#include "Sys.h"
#include "FileSys.h"
#include "Bmp.h"

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
        
	// TODO: Implement!
		
    Bmp_delete(bmp);
    return 0;
}
