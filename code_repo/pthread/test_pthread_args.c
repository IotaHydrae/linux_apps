
#include <pthread.h>
#include <stdio.h>
#include <unistd.h> /* for sleep() */
#include <error.h>

typedef struct person{
	char *name;
	int id;
}ming, *pming;

void *fun(void *data)
{
	pming per = (pming)data;
	pthread_t tid = pthread_self();
	printf("The tid of fun thread is %ld \n",tid);

	printf("addr =%p\n",data);
	printf("%s name:%s\n",__FUNCTION__,per->name);
	printf("%s id:%d\n",__FUNCTION__,per->id);
}

void *fun2(void *data)
{
	int *a = (int *)data;
	printf("%s val of data %d\n",__FUNCTION__, *a);
}

void *fun3(void *data)
{
	int b = (int)(long)data;
	printf("%s val of data %d\n",__FUNCTION__, b);
	
}


int main()
{
	pthread_t tid;
	ming minge;
	int a = 19;
	int b = 20;
	minge.name = "chel";
	minge.id = 197384265;
	
	pthread_create(&tid, NULL, fun, (void *)&minge);
	pthread_create(&tid, NULL, fun2, (void *)&a);
	pthread_create(&tid, NULL, fun3, (void *)(long)b);


	printf("The tid of main thread is %ld\n", pthread_self());
	/* unsigned int sleep(unsigned int seconds); */
	/* 线程顺序执行不受控制 */
	sleep(1); /* 这里加入sleep查看fun线程输出 */
	return 0;
}



