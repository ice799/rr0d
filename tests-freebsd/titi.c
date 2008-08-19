int main () {
__asm("\
int $0x3;\
");
 getchar();
 return 0;
}
