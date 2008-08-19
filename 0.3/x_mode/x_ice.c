/* Xlib include files */
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>

#include <X11/extensions/xf86dga.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>





typedef struct
{
    char *ptr;                          // Pointer to the DGA framebuffer
    int  width;                         // Width of the screen
    int  banksize;                      //
    int  memsize;                       //

} TMYWIN;

void CollectDisplayInfo(char *sDisplay, unsigned int fQuery)
{
  //static TXINITPACKET Init;           // Init packet sent to the debugger

    Display *display;                   // Display name to connect to
    XWindowAttributes windowattr;       // Window attributes structure
    int screen_num;                     // Screen number
    Screen *screen_ptr;                 // Pointer to screen structure
    TMYWIN mywin;                       // Structure holding window information

    int event_base, error_base;

    unsigned int display_width, display_height;
    unsigned char* tmp;
    unsigned int* tmp_i;
    int a;
    
    int i;
    
    display = XOpenDisplay(sDisplay);
    
    if( !display )
      {
	fprintf(stderr, "FAILED: Cannot connect to server %s\n", XDisplayName(sDisplay));
	return;
      }
    
    // Get the screen number and pointers
    screen_num = DefaultScreen(display);
    screen_ptr = DefaultScreenOfDisplay(display);

    
    // Get the root window attributes
    if( !XGetWindowAttributes(display, RootWindow(display, screen_num), &windowattr) )
      {
	fprintf(stderr, "FAILED: Unable to get root window attributes\n");
	return;
      }

    display_width  = windowattr.width;
    display_height = windowattr.height;
	
    printf("%d %d\n", display_width, display_height);
		
    // Query the DGA extension - this is specific to XFree86
    if(! XF86DGAQueryExtension(display, &event_base, &error_base) )
      {
	fprintf(stderr, "FAILED: Unable to query the DGA extension\n");
	fprintf(stderr, "Your display does not support DGA extensions!\n");
	return;
      }
    printf("%d %d\n", event_base, error_base& XF86DGADirectPresent);
	    
    if(! XF86DGAGetVideo(display, screen_num, &mywin.ptr, &mywin.width, &mywin.banksize, &mywin.memsize) )
      {
	fprintf(stderr, "FAILED: XF86DGAGetVideo\n");
	return;
      }
    tmp = (unsigned char*)mywin.ptr;
    tmp_i = (unsigned int*)mywin.ptr;
    printf("bank: %X\n", mywin.banksize);
    printf("%p %X %X %X \n",
	   mywin.ptr,
	   mywin.width,
	   mywin.memsize,
	   mywin.memsize/ mywin.width);
    printf("%d\n", windowattr.depth);
 
    for (i=0;i<mywin.width*windowattr.height;i++)
      {
	a = tmp_i[i];
	a&=0xFFFFFF00;

	tmp_i[i]=a;
      }

    return;
}


int main(int argc, char **argp)
{

    CollectDisplayInfo(NULL, 0 );

    return 0 ;
}

