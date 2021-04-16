#include <stdio.h>
#include <input_manager.h>


int NetworkDeviceInit(void)
{
    return 0;
}

int NetworkDeviceExit(void)
{

    return 0;
}

int NetworkGetInputEvent(pInputEvent event_in)
{
	
    event_in->iType = INPUT_TYPE_NET;
    
    return 0;
}

static InputDevice g_tNetworkInputDevice = {
    .cName = "Network",
    .DeviceInit = NetworkDeviceInit,
    .DeviceExit = NetworkDeviceExit,
    .GetInputEvent = NetworkGetInputEvent,
};


#if 1
int main(int argc, const char *argv[])
{
	
    return 0;
}
#endif
