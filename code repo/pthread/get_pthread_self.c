
#include <pthread.h>
#include <stdio.h>

int main(int argc, char **argv)
{
	pthread_t tid = pthread_self();
	printf("Pthread self tid is:%ld\n", tid);

	return 0;
}

