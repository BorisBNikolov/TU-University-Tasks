#include<stdio.h>
int main() {
  int array[10][10]={{1,1,1,2,1,1,1,1,1,1},{1,1,2,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1,1,1}};
  for (size_t i = 0; i < 99-1; i++) {
    printf("%d\n", array[i][i]);
  }
  return 0;
}
