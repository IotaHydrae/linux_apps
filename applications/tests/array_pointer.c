#include <stdio.h>

#define MAX 5

int main(int argc, char **argv)
{
    int a[MAX] = {1,1,1,1,1};

    int *p;
    int (*pa)[5];

    p=a;

    // printf("%p, %p\n", p, a);

    // for(int i=0;i<MAX;i++){
    //     printf("%d, %d\n", p[i], a[i]);
    // }

    pa=&a;
    printf("%p, %p\n", pa, a);
    for(int i=0;i<MAX;i++){
        printf("%p, %d, %d\n", *pa+i, *(*pa+i), a[i]);
    }
}