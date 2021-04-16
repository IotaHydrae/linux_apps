
/**
* @file
*/

#ifndef _DISP_MANAGER_H_
#define _DISP_MANAGER_H_
#include <stdio.h>
#include <errno.h>

#ifndef NULL
    #define NULL (void *)0
#endif

#define HANDLE_ERROR(msg) do{ \
        perror(msg);return(-1);}while(0);

typedef struct disp_buf {
    int iXres;
    int iYres;
    int iBpp;
    unsigned char *cBuf_base;
} disp_buf, *pdisp_buf;

typedef struct disp_area {
    int iX1;
    int iY1;
    int iWidth;
    int iHeight;
} disp_area, *pdisp_area;

typedef struct  disp_opr {
    char *cName;
    int (*device_init)(void);
    int (*device_exit)(void);

    int (*get_buffer)(pdisp_buf buf);
    int (*flush_area)(pdisp_buf buf, pdisp_area area);

    struct disp_opr *next;
} disp_opr, *pdisp_opr;

void register_disp_drivers(pdisp_opr opr);
int select_default_display(char *name);
int init_default_display(void);
pdisp_buf get_display_buffer();

void put_pixel(int x, int y, unsigned int color);
int flush_display_area(pdisp_buf buf, pdisp_area area);
void display_init(void);

void FrameBufferInit(void);


#endif

