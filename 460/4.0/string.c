// YOUR string.c file for

/* strcmp(), strlen(), strncmp(), etc. */

int strlen(char *s) {
  int count = 0;
  while(*s) {
    count++;
    *s++;
  }

  return count;
}

int strcmp(char *s1, char *s2) {
  // 0 means they are equal, 1 if they are not
  int same = 0;
  int i;
  int len_s1 = strlen(s1);
  int len_s2 = strlen(s2);
  if (len_s1 > len_s2) {
    for(i = 0; i < len_s1; i++) {
      if(s1[i] != s2[i]) {
        same = 1;
      }
    }
  }
  else {
    for(i = 0; i < len_s1; i++) {
      if(s2[i] != s1[i]) {
        same = 1;
      }
    }
  }

  return same;
}

void strcpy(char *dest, char *src) { // : copies string src to dest
  int len_s1 = strlen(dest);
  int len_s2 = strlen(src);
  int i;

  for(i = 0; i < len_s2; i++) {
    dest[i] = src[i];
  }
  
  if(len_s1 >= len_s2) {
    for(i = len_s2; i < len_s1; i++) {
      dest[i] = '\0';
    }
  }
}

void strncpy(char *dest, char *src, int n) { /* : copies only n bytes */
  int i;
  for(i = 0; i < n; i++) {
    dest[i] = src[i];
  }
}

int strncmp(char *s1, char *s2, int n) { /* : compares n bytes */
  // 0 means they are equal, 1 if they are not
  int same = 0;
  int i;
  for(i = 0; i < n; i++) {
    if (s1[i] != s2[i]) {
      same = 1;
    }
  }

  return same;
}

void memncpy(char *dest, char *src, int n) { /* : copies n bytes from src to dest */ 
  int i;
  for(i = 0; i < n; i++) {
    if (src[i] == '\0') { break; }
    dest[i] = src[i];
  }
}

void memset(char *dest, char c, int n) { /* : fills dest with n CONSTANT bytes of c */ 
  int i;
  for(i = 0; i < n; i++) {
    dest[i] = c;
  }
}

  
