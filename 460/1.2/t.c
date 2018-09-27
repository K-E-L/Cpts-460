/*******************************************************
*                      t.c file                        *
*******************************************************/
/* typedef unsigned char  u8; */
/* typedef unsigned short u16; */
/* typedef unsigned long  u32; */

#include <string.h>
#include "ext2.h"

#define TRK 18
#define CYL 36
#define BLK 1024

typedef struct ext2_group_desc  GD;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR;
GD    *gp;
INODE *ip;
DIR   *dp;

char buf1[BLK], buf2[BLK];
int color = 0x0A;
u8 ino;

u16 blkno;
char buf[BLK];
char *cp;

void print_two_digits(u16 num) {
  u16 ten, tens;
  ten = 10;
  tens = num % ten;
  putc((num - tens) / ten + '0');
  putc(tens + '0');
  prints("\n\r");
}

void getblk(u16 blk, char *buf)
{
  readfd( (2*blk)/CYL, ( (2*blk)%CYL)/TRK, ((2*blk)%CYL)%TRK, buf);
  /* readfd( blk/18, ((blk)%18)/9, ( ((blk)%18)%9)<<1, buf); */
}

int main()
{
  u16    i, iblk, one;
  char   c, temp[64];

  one = 1;

  prints("read block# 2 (GD)\n\r");
  getblk(2, buf1);

// 1. WRITE YOUR CODE to get iblk = bg_inode_table block number
  gp = (GD *)buf1;
  iblk = gp->bg_inode_table;

  prints("inode_block="); putc(iblk+'0'); prints("\n\r");

// 2. WRITE YOUR CODE to get root inode
  prints("read inodes begin block to get root inode\n\r");

  /* let    INODE *ip -> INODE of /boot/mtx */
  getblk(iblk, buf1);
  ip = (INODE *)buf1 + 1;

  for (i=0; i<15; i++){
    if (ip->i_block[i]) {
      prints("iblock[");
      putc(i + '0');
      prints("]=");
      print_two_digits(ip->i_block[i]);
    }
  }

  /* getblk(ip->i_block[0], buf1); */


// 3. WRITE YOUR CODE to step through the data block of root inode
  prints("read data block of root DIR\n\r");

  /* setes(0x1000);            // ES = 0x1000 => BIOS loads disk block to (ES, buf) */

  blkno = 21;

  /* while (blkno > 0) { */
  /*   putc('h'); */
  /*   getblk((u16)blkno, 0);    // buf = 0 => memory address = (ES, 0) */
  /*   inces();                  // inc ES by 1KB/16 = 0x40 */
  /*   blkno -= one; */
  /* } */
  /* repeat loop for next blkno until no more */


  /* puts(temp); */

   
// 4. print file names in the root directory /


  return 1;
}

int prints(char *s)
{
  unsigned int i = 0;
  for(i = 0; i < strlen(s); i++) {
    putc(s[i]);
  }
  return 0;
}

int gets(char *s)
{
  int i = 1;
  char temp_char = getc();
  if( temp_char == '\r' ) {
    s[0] = 0;
    return 1;
  }
  s[0] = temp_char;
  putc(s[0]);
  while( temp_char != '\r' ) {
    temp_char = getc();
    s[i] = temp_char;
    putc(s[i]);
    i++;
  }
  return 0;
}

