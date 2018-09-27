/*********  t.c file *********************/

#include <string.h>

void prints(char *s)
{
  unsigned int i = 0;
  for(i = 0; i < strlen(s); i++) {
    putc(s[i]);
  }
}

void gets(char *s)
{
  int i = 1;
  char temp_char = getc();
  if( temp_char == '\r' ) {
    s[0] = 0;
    return;
  }
  s[0] = temp_char;
  putc(s[0]);
  while( temp_char != '\r' ) {
    temp_char = getc();
    s[i] = temp_char;
    putc(s[i]);
    i++;
  }
}

char ans[64];

int main()
{
  // only works for 2 letters
  while(1){
    prints("What's your name? ");
    gets(ans); prints("\n\r");

    if (ans[0]==0){
      prints("return to assembly and hang\n\r");
      return 1;
    }
    prints("Welcome "); prints(ans); prints("\n\r");
  }
}
  

