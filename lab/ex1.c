#include <stdio.h>

int main()
{
  int x = 1;
  printf("Hello x = %d\n", x);

  //
  // Put in-line assembly here to increment
  // the value of x by 1 using in-line assembly
  //

    __asm__("movl %1, %%eax;" "incl %%eax;" "movl %%eax, %0;"
      :"=r"(x)    /* y is output operand */
      :"r"(x)        /* x is input operand */
      );    /* %eax is clobbered register */

  printf("Hello x = %d after increment\n", x);

  if(x == 2){
    printf("OK\n");
  }
  else{
    printf("ERROR\n");
  }
}
