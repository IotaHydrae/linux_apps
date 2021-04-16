

#ifndef _INPUT_MANAGER_H_
#define _INPUT_MANAGER_H_

#ifndef NULL
    #define NULL (void *)0
#endif

typedef enum {
    INPUT_TYPE_TOUCH = 1,
    INPUT_TYPE_NET
} INPUT_TYPE;

typedef struct InputEvent {
    int iType;
    int iX;
    int iY;
    int iPressure;
    char str[1024];
    struct timeval tv;
} InputEvent, *pInputEvent;

typedef struct InputDevice {
    char *cName;
    int (*DeviceInit)(void);
    int (*DeviceExit)(void);

    int (*GetInputEvent)(pInputEvent event);
    struct InputDevice *next;
} InputDevice, *pInputDevice;


void ResgisterInputDevice(pInputDevice ptinputdev);
void InputInit(void);
void InputDeviceInit(void);


#endif

