/****************************************************************************
*
* PROGRAM: CON_SAMP.C
*
* PURPOSE: Simple console application for calling CVXDSAMP (C VxD Sample) VxD
*
* FUNCTIONS:
*  main() - Console application calls VMM and VKD through CVXDSAMP which
*           supports DeviceIoControl. CVXDSAMP will return values to this
*           application through this same DeviceIoControl interface.
*
* SPECIAL INSTRUCTIONS:
*
****************************************************************************/

#include <stdio.h>
#include <windows.h>
#include <vmm.h>
#include <vxdldr.h>

#define SPY_APIFUNC_1 1
#define SPY_APIFUNC_2 2
#define SPY_APIFUNC_3 3

int main()
{
    HANDLE      hCVxD = 0;
    DWORD       cbBytesReturned;
    DWORD       dwErrorCode;
    DWORD       RetInfo[1];
#if 0
    DWORD       ErrorCode;
#endif

    /*
     * Dynamically load and prepare to call CVXDSAMP
     * The CREATE_NEW flag is not necessary
     */

    hCVxD = CreateFile("\\\\.\\SPY.VXD", 0,0,0,
                        CREATE_NEW, FILE_FLAG_DELETE_ON_CLOSE, 0);

    if ( hCVxD == INVALID_HANDLE_VALUE )
    {
        dwErrorCode = GetLastError();
        if ( dwErrorCode == ERROR_NOT_SUPPORTED )
        {
            printf("Unable to open VxD, \n device does not support DeviceIOCTL\n");
        }
        else
        {
            printf("Unable to open VxD, Error code: %lx\n", dwErrorCode);
        }
    }
    else
    {
        if(!DeviceIoControl(hCVxD, SPY_APIFUNC_3,
                (LPVOID)NULL, 0,
                (LPVOID)RetInfo, sizeof(RetInfo),
                &cbBytesReturned, NULL)){
                printf("Unable to create C:\\FileCall.Spy file! Aborting!");
                CloseHandle(hCVxD);
                exit(1);
        }
        DeviceIoControl(hCVxD, SPY_APIFUNC_1,
                (LPVOID)NULL, 0,
                (LPVOID)RetInfo, sizeof(RetInfo),
                &cbBytesReturned, NULL);
        MessageBox(NULL, "Press OK to unhook spy\nLogging to C:\\FileCall.Spy", "SPY HOOKED", MB_OK);

		system("pause");

        DeviceIoControl(hCVxD, SPY_APIFUNC_2,
                (LPVOID)NULL, 0,
                (LPVOID)RetInfo, sizeof(RetInfo),
                &cbBytesReturned, NULL);
	/* Dynamically unload SPY */
        if(!CloseHandle(hCVxD))
                printf("Unable to close\n");

    }
    return(0);
}
