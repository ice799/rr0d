#ifndef BUFFERING_H
#define BUFFERING_H



extern int bufferindex;
extern  int insertbufferindex;


void cleaningbuffer(void);
void insert_cmd(char *);
void recall_oldcmd(int );

void insertinbuffer(char *);
void displaybuffer(void);
void bufferdown(void);

void bufferup(void);


void bufferbigdown(void); 

void bufferbigup(void);

void startbuffer(void);

void endbuffer(void);
 
void restorebuffer(void);


void log_line_in_buffer(char *ptr);

#define BUFFERMAXCMD 40
extern char buff_cmd[BUFFERMAXCMD][80];
extern int insertcmdindex;
extern int cmdindex;

#endif /* BUFFERING_H */
