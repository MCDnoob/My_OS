#include <stdio.h>
#include <malloc.h>

int main() {
  void *addr = malloc(528*1024);
  printf("addr内存起始地址：%x\n", addr);
  printf("使用cat /proc/%d/maps查看内存分配\n",getpid());

  while(1);

  free(addr);
  printf("释放了内存\n");

  getchar();
  return 0;
}

