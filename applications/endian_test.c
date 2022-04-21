#include <stdio.h>

int checkCPU()
{
	union w
	{
		int a;
		char b;
	}c;

	c.a = 1;
	return (c.a == c.b);
}

int checkCPU2()
{
	union a{
		struct b{
			unsigned char r:3;
			unsigned char g:2;
			unsigned char b:3;	
		}ch;
		unsigned char full;
	}c;

	//printf("%ld\n",sizeof(c));

	c.ch.r = 0x3;

	printf("0x%x\n", c.full);
	return (c.ch.r == c.full);
}

int
main(int argc, char **argv)
{
	unsigned int num, *p;

	p = &num;

	num = 0;

	*(char *)p = 0xff;
	if(num == 0xff)
	{
		printf("little\n");
	}
	else
	{
		printf("big\n");
	}

	int ret = 0;

	ret = checkCPU();

	if(ret){
	
		printf("little\n");
	}
	else{

		printf("big\n");
	}

	ret  = 	checkCPU2();
	if(ret){
	
		printf("little\n");
	}
	else{

		printf("big\n");
	}

	return 0;
}
