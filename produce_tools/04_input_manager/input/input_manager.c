#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <input_manager.h>

static pInputDevice g_tInputDevs = NULL;

/* called by each input dev source file */
void ResgisterInputDevice(pInputDevice ptinputdev)
{
	ptinputdev->next = g_tInputDevs;
	g_tInputDevs = ptinputdev;
}

/* register each input device operations */
void InputInit(void)
{
	/* register touchpanel event */
	extern void RegisterTouchpanelInputDevice(void);
	RegisterTouchpanelInputDevice();
	/* register network event */
	extern void RegisterNetworkInputDevice(void);
	RegisterNetworkInputDevice();
}

void InputDeviceInit(void)
{
	/* forch earch input dev, 
	init it and create a thread to handle input event*/
	int ret;
	pthread_t tid;
	
	pInputDevice ptmp = g_tInputDevs;
	while(ptmp){
		/* init device */
		ret = ptmp->DeviceInit();
		if(ret < 0){
			printf("%s init error.\n",ptmp->cName);
		}
		else
		{
			
		}
		

		ptmp = ptmp->next;
	}
}


