#include "pastry-like/config.h"
#include "pastry-like/netio.h"
#include <windows.h>

int winsock_init(void){
    WSADATA wsad;
    short int wVersion = MAKEWORD(2,0);
    int err;
    err = WSAStartup(wVersion, &wsad);
    if(err != 0)
	return 1;

    if (HIWORD(wVersion) != HIWORD(wsad.wVersion) || LOWORD(wVersion) != LOWORD(wsad.wVersion)) {
	WSACleanup();
	return 1;
    }
    return 0;
}

void winsock_cleanup(void){
    //netio_shutdown();
    WSACleanup();
}

BOOL APIENTRY DllMain(HANDLE hModule,
                      DWORD ul_reason_for_call,
                      LPVOID lpReserved)
{
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
	if(winsock_init() != 0)
	    return FALSE;
        break;
    case DLL_PROCESS_DETACH:
		winsock_cleanup();
        break;
    }

    return TRUE;
}
