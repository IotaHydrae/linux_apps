/**
 * Copyright (c) 2022 IotaHydrae
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include <stdio.h>


int main(int argc, char **argv)
{
    int ch = '0';

    for(int i=0;i<100;i++,ch++)
        switch (ch)
        {
        case '0'...'9':
            printf("range: 0~9 %c,%d\n", ch, ch);
            break;

        case 'A'...'Z':
            printf("range: A~Z %c,%d\n", ch, ch);
            break;
        
        case 'a'...'z':
            printf("range: a~f %c,%d\n", ch, ch);
            break;

        default:
            break;
        }

    for(int i=0;i<128;i++)
        printf("%c", i);
    printf("\n");

    return 0;
}