#include <libcookai/cookai.h>
#include <win32/cookai.h>
#include <windows.h>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>

DWORD WINAPI cookai_thread(LPVOID option){
    HANDLE cookai_mutex;

    cookai_mutex = CreateMutex(0, FALSE, L"local\\libcookai-ng_mutex");
#if 0
	while(1){
		tick(10000);
    }
#endif
    CloseHandle(cookai_mutex);
    return TRUE;
}

HANDLE win32_threaded_cookai_initialize(char *my_host, char *my_service,
				      int type, char *remote_host, char *remote_service, DWORD *threadID){
    static HANDLE handle = 0;
    if(handle != 0){
		return handle;
    }
#if 0
	if(cookai_initialize(my_host, my_service,
		type, remote_host, remote_service) == FALSE)
		return 0;
#endif
    handle = CreateThread(NULL, 0, cookai_thread, NULL, 0, threadID);

    return handle;
}