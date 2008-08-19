#ifndef COMMAND_H
#define COMMAND_H

extern char cmd_result[80];
extern unsigned int adresse_ret_1;
extern unsigned int adresse_ret_2;



extern unsigned int edit_mode;

int chose_command(char* command);
int chose_reg(char* reg);

unsigned int chose_symb(char* symb);


int parse_command(char* );

#endif /* COMMAND_H */
