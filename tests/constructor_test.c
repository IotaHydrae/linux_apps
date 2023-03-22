#include <stdio.h>

static void __attribute__((constructor)) test_init3(void)
{
    printf("%s, look, it called before main\n", __func__);
}

static void __attribute__((constructor)) test_init1(void)
{
    printf("%s, look, it called before main\n", __func__);
}

static void __attribute__((constructor)) test_init2(void)
{
    printf("%s, look, it called before main\n", __func__);
}

static void __attribute__((destructor)) test_deinit(void)
{
    printf("%s, look, it called after main\n", __func__);
}

int main(int argc, char **argv)
{
    printf("Hello World\n");
    return 0;
}