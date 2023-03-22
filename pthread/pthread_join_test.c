#include <pthread.h>
#include <stdio.h>
#include <unistd.h> /* for sleep() */
#include <error.h>

void *func(void *arg)
{
    static int tmp = 0;
    tmp = *(int *)arg;
    tmp += 100;
    printf("%s:Addr=%p val=%d\n", __func__, &tmp, tmp);
    pthread_exit((void *)&tmp);
}


int main(int argc, char const *argv[])
{
    /* code */
    pthread_t tid;
    int a = 50;
    void *Tmp = NULL;
    int ret = pthread_create(&tid, NULL, func, (void *)&a);

    pthread_join(tid, &Tmp);
    printf("%s:Addr=%p val=%d\n", __func__, Tmp, *(int *)Tmp);

    return 0;
}
