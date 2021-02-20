#include <atari.h>
#include <conio.h>
#include <peekpoke.h>

#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define ROUND(A, B) (A + (B - (A%B)))

// GTIA graphics bitmasks
#define GFX_9 0x40
#define GFX_10 0x80
#define GFX_11 0xC0

#define BUFF_SIZE 0x22B0
#define BUFF_BOUNDARIES = BUFF_SIZE / 0x1000

typedef unsigned char byte;

#pragma bss-name (push,"DLIST_R")  
extern byte dlist_red[]; 
#pragma bss-name (pop)
#pragma bss-name (push,"DLIST_G")  
extern byte dlist_green[]; 
#pragma bss-name (pop)
#pragma bss-name (push,"DLIST_B")  
extern byte dlist_blue[]; 
#pragma bss-name (pop)
#pragma bss-name (push,"SCREEN_R")  
extern byte screen_red[]; 
#pragma bss-name (pop)
#pragma bss-name (push,"SCREEN_G")  
extern byte screen_green[]; 
#pragma bss-name (pop)
#pragma bss-name (push,"SCREEN_B")  
extern byte screen_blue[]; 
#pragma bss-name (pop)

void render_red(void);
void render_green(void);
void render_blue(void);

int main()
{
    //cprintf("%p %p, %p, %p\n\r", &dlist, screen_red, screen_green, screen_blue);
    unsigned b = 0;
    byte row = 0, col = 0;
    byte SDMCTL = OS.sdmctl; //PEEK(0x22F);

    // Update all of the display lists to properly use their memory.
    memset(screen_red, 0x11, BUFF_SIZE);
    memset(screen_green, 0x44, BUFF_SIZE);
    memset(screen_blue, 0x88, BUFF_SIZE);

    //*(byte*)0x73fd = DL_JMP;
    //*(unsigned*)0x73fe = 0x7400;

    // Turn off ANTIC
    SDMCTL = OS.sdmctl;
    OS.sdmctl = 0; //POKE(0x22F, 0); 

    for(b = 0; b < BUFF_SIZE; ++b)
    {       
        row = (byte)(b / (unsigned)40);
        col = (byte)(b % (unsigned)40);

        //screen_red[b] = b / (BUFF_SIZE/255);
        // First quad, red
        if((row < 120) && (col < 30))
        {
            if(col < 30)
                screen_red[b] = 0xff;
            if(col > 10)
                screen_green[b] = 0xff;
        }
        if((row > 90) && (col > 10))
            screen_blue[b] = 0xff; //(byte)(((unsigned)row * 15)/40);
        
    }

    // Enable graphics 9
    OS.gprior |=  GFX_9;
    
    // Set maximum luminance
    OS.color1 = 15;

    // Start rendering with blue data
    OS.sdlst = dlist_blue;

    // Start rotating colors
    OS.vdslst = render_blue;
    ANTIC.nmien = 0x80 | 0x40;

    // Turn On ANTIC
    OS.sdmctl = SDMCTL;

    return 0;
}
