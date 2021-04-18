
#include <pthread.h>
#include <stdio.h>
#include <unistd.h> /* for sleep() */
#include <error.h>

void *fun(void *data)
{
	pthread_t tid = pthread_self();
	printf("The tid of fun thread is %ld \n",tid);
}

int main()
{
	pthread_t tid;
	int ret = pthread_create(&tid, NULL, fun, NULL);

	printf("The tid of main thread is %ld\n", pthread_self());
	/* unsigned int sleep(unsigned int seconds); */
	/* 线程顺序执行不受控制 */
	sleep(1); /* 这里加入sleep查看fun线程输出 */
	return 0;
}




