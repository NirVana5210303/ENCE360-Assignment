#include <stdio.h>
#include <stdlib.h>

#define N 1000000

int main()
{
  int sum = 0;
  int i = 0;
  for (i = 0; i < N; i++)
  {
    sum += i;
  }
  printf("Sum: %d\n", sum);
  return 0;
}
