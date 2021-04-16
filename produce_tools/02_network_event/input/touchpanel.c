#include <stdio.h>
#include <tslib.h>
#include <input_manager.h>

static struct tsdev *g_ts;

int TouchpanelDeviceInit(void)
{
    g_ts = ts_setup(NULL, 0);
    if(!g_ts) {
        printf("error on ts_setup\n");
        return -1;
    }
    return 0;
}

int TouchpanelDeviceExit(void)
{
    ts_close(g_ts);
    return 0;
}

int TouchpanelGetInputEvent(pInputEvent event_in)
{
    struct ts_sample smp;
    int ret;
    ret = ts_read(g_ts, &smp, 1);
    event_in->iType = INPUT_TYPE_TOUCH;
    event_in->iX = smp.x;
    event_in->iY = smp.y;
    event_in->iPressure = smp.pressure;
    event_in->tv = smp.tv;
    return 0;
}

static InputDevice g_tTouchpanelInputDevice = {
    .cName = "Touchpanel",
    .DeviceInit = TouchpanelDeviceInit,
    .DeviceExit = TouchpanelDeviceExit,
    .GetInputEvent = TouchpanelGetInputEvent,
};


#if 0
int main(int argc, const char *argv[])
{
    InputEvent event;
    int ret;
    g_tTouchpanelInputDevice.DeviceInit();
    while(1) {
        ret = g_tTouchpanelInputDevice.GetInputEvent(&event);
        if(ret) {
            printf("error on GetInputEvent\n");
            return -1;
        } else {
			printf("time        :%d\n", (int)event.tv.tv_sec);
            printf("Type        :%d\n", event.iType);
            printf("iX          :%d\n", event.iX);
            printf("iY          :%d\n", event.iY);
            printf("iPressure   :%d\n\n\n", event.iPressure);
        }
    }
    return 0;
}
#endif
