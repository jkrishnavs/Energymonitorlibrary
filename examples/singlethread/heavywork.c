#include<stdio.h>
#define NUMMAX 50000

int main(int argc,char *argv[]){
  int i,j,k;
  
  for(i=NUMMAX;i>0;i--){
    k = i;
    for(j=NUMMAX;j>0;j--)
      k--;
  }

  printf("%d",i+j+k);
  return 0;
}
