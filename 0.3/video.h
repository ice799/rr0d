#ifndef VIDEO_H
#define VIDEO_H

#define dump_screen_size (80*25*2)

extern char *table[];
extern char  screen_line0[];
extern char  screen_line1[];
extern char  screen_line2[];
extern char  screen_line3[];
extern char  screen_line4[];
extern char  screen_line5[];
extern char  screen_line6[];
extern char  screen_line7[];
extern char  screen_line8[];
extern char  screen_line9[];

extern char  screen_line10[];
extern char  screen_line11[];
extern char  screen_line12[];
extern char  screen_line13[];
extern char  screen_line14[];
extern char  screen_line15[];
extern char  screen_line16[];
extern char  screen_line17[];
extern char  screen_line18[];
extern char  screen_line19[];

extern char  screen_line20[];
extern char  screen_line21[];
extern char  screen_line22[];
extern char  screen_line23[];
extern char  screen_line24[];
extern char  screen_line25[];
extern char  screen_line26[];
extern char  screen_line27[];
extern char  screen_line28[];
extern char  screen_line29[];

extern char  screen_line30[];
extern char  screen_line31[];
extern char  screen_line32[];
extern char  screen_line33[];
extern char  screen_line34[];
extern char  screen_line35[];
extern char  screen_line36[];
extern char  screen_line37[];
extern char  screen_line38[];
extern char  screen_line39[];

extern char *table_color[];

extern unsigned int data_win_y;
extern unsigned int data_win_dim;

extern unsigned int code_win_y;
extern unsigned int code_win_dim;

extern unsigned int buff_win_y;
extern unsigned int buff_win_dim;

extern unsigned int cmd_win_y;
extern unsigned int cmd_win_dim;

extern unsigned int screen_offset_x;
extern unsigned int screen_offset_y;

extern unsigned int screen_bpp;

extern unsigned int code_dim_line[10];

#define SCREEN_LINE 40

typedef struct tagBITMAPFILEHEADER { /* bmfh */
    unsigned int   bfSize; 
    short     bfReserved1; 
    short     bfReserved2; 
    unsigned int   bfOffBits; 
} BITMAPFILEHEADER; 


typedef struct tagBITMAPINFOHEADER{ /* bmih */
    unsigned int  biSize; 
    unsigned int   biWidth; 
    unsigned int   biHeight; 
    short int   biPlanes; 
    short int   biBitCount ;
    int  biCompression; 
    int  biSizeImage; 
    unsigned int   biXPelsPerMeter; 
    unsigned int   biYPelsPerMeter; 
    int  biClrUsed; 
    int  biClrImportant; 
} BITMAPINFOHEADER; 

#define BITMAP_ID 0x4d42

extern unsigned int font_width;
extern unsigned int font_height;

extern unsigned int font_x;
extern unsigned int font_y;
extern unsigned int font_bpp;

extern unsigned int screen_adresse ;

extern unsigned char font_data[];

int test(void);
void prepare_screen(void);


int LoadBMP(char* , unsigned int* , unsigned int* , unsigned int *, unsigned char *);

void my_outb(unsigned short port, unsigned char value);
void read_mem_base(void);
void restore_mem_base(void) ;
void reset_mem_to_0(void);

void put_XY_char(int x, int y, unsigned char a);

void print_char(unsigned char , unsigned int , unsigned int );
void Put_String(char* , unsigned int , unsigned int );
void PutString(unsigned long ,unsigned long ,char *,char );

void dump_screen(void);
void restore_screen(int);



void displaymessage(char *);

void WriteVCon(unsigned int ,unsigned ,char *);

void delline(int );

void ClearAW(void);
void ClearDW(void);
void ClearCW(void);
void cls(void);


void DisplayText(int ,int ,char *);

void Refresh_AW(void);
void Refresh_DW(void);
void Refresh_CW(void);
void Refresh_ALL(void);
void cursor_top_right(void);
void Refresh_Display(void);

#ifdef VIDEO_AA
void Start_Display(void);
void End_Display(void);
#endif /* VIDEO_AA */

void InsertCommandKey(char );
void ClearCommand(void);

void DeleteCommandKey(void);

void deblin_change_color(char *,char *);
void deblin_console_off(void); 
void deblin_console_on(void);
void test_scr(void);

void print_AW(char* ptr, int ligne);
void print_DW(char* ptr, int ligne);
void print_CW(char* ptr, int ligne);

void update_cursor(unsigned int x, unsigned int y);
void erase_cursor(void);

extern unsigned int cursor_X;
extern unsigned int cursor_Y;
extern unsigned int cursor_old_color;
extern unsigned int cursor_color;



#endif /* VIDEO_H */
