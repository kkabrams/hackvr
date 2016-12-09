#include <stdio.h>

int main() {
 ungetc('a',stdin);
 ungetc('b',stdin);
 printf("%c",fgetc(stdin));
 printf("%c",fgetc(stdin));
 printf("\n");
}
