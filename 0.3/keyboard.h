#ifndef KEYBOARD_H
#define KEYBOARD_H

#define KBENTER         -1
#define KBF11           -2
#define KBF12           -3
#define KBDEL           -4
#define KBCURSORUP      -5
#define KBCURSORDOWN    -6
#define KBLEFTALT       -7
#define KBRIGHTSHIFT    -8
#define KBPAGEUP        -9
#define KBPAGEDOWN      -10

#define KBLEFTSHIFT		-11

#define KBCURSORLEFT	-12
#define KBCURSORRIGHT	-13

#define KBESC	        -14


#define KBUP			-128






unsigned int read_scancode(void);

void handle_scancode(unsigned char );

char scan_to_ascii(int );




unsigned int WaitForKeyboard(void);
void DrainOutputBuffer(void);
unsigned int SendKeyboardCommand(unsigned char);
unsigned char  print_keystroke(void);
unsigned int myWaitForKeyboard(unsigned char*);

unsigned int _read_scancode(void);



#endif

