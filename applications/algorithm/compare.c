#include <stdio.h>

int compare(int num[], int len)
{
	int i,j,tmp;	
	for(j=0;j<len-1;j++){
		for(i=0;i<len-1-j;i++){
			if(num[i] > num[i+1]){
			tmp=num[i];
			num[i] = num[i+1];
			num[i+1] = tmp;	
			}
		}
	}

	return 0;
}

int main(int argc, char **argv)
{
	int num[]={9,6,5,3,7,2,4};
	int len=sizeof(num)/sizeof(int);
	printf("len %d\n", len);

	for(int i=0;i<len;i++){
		printf("%d,", num[i]);
	}
	printf("\n");
	compare(num, len);

	for(int i=0;i<len;i++){
		printf("%d,", num[i]);
	}
	printf("\n");
	return 0;
}
