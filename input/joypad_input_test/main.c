#include <stdio.h>

extern int InitJoypadInput(void);
extern int GetJoypadInput(void);

int
main(int argc, char **argv)
{
	int ret;
	InitJoypadInput();

	while(1){
		ret = GetJoypadInput();
		printf("%d\n", ret);
	}
	return 0;
}
