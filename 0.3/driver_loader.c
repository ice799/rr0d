//tx to Eliyas Yakub

#include <windows.h>
#include <winioctl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define DRIVER_FUNC_INSTALL     0x01
#define DRIVER_FUNC_REMOVE      0x02
#define DRIVER_NAME       "cancel"

//
// function prototypes
//
BOOLEAN
SetupDriverName(
    PUCHAR DriverLocation
    );

BOOLEAN
ManageDriver(
    IN LPCTSTR  DriverName,
    IN LPCTSTR  ServiceName,
    IN USHORT   Function
    );

//
// Main function
//

VOID _cdecl main( ULONG argc, PCHAR argv[] )
{
    UCHAR driverLocation[8*MAX_PATH];   // Just make it long.  Doesn't have to be "safe"
    HANDLE hDevice;
    BOOLEAN r;
    DWORD   d;

    char * devicePath = "\\\\.\\Cancel";

    //
    // Get driver directory
    //
    if (argc > 1) {
        strcpy(driverLocation, argv[1]);
    } else {
        strcpy(driverLocation, ".");
        d = GetCurrentDirectory(MAX_PATH, driverLocation);
        if (d <= 0) {
            printf("Could not get current directory.\n");
            getchar();
            return;
        }
    }
    
    

    //
    // Setup full path to driver name.
    //
    if (!SetupDriverName(driverLocation)) {
        printf("cannot setup driver name\n");
        getchar();
        return;
    }

    //
    // Be sure driver (and service) are unloaded/removed.
    //
    printf("Be sure driver (and service) are unloaded/removed.\n");
    (void) ManageDriver (DRIVER_NAME, driverLocation, DRIVER_FUNC_REMOVE);
    

    //
    // Install driver.
    //
    printf("Install service and driver.\n");
    r = ManageDriver(DRIVER_NAME, driverLocation, DRIVER_FUNC_INSTALL);
    if (!r) {
            printf("Unable to install driver: %s\n", driverLocation);
            return;
    }

    printf("Driver loaded: enter to remove it\n");
    getchar();
    printf("remove driver\n");
    (void) ManageDriver (DRIVER_NAME, driverLocation, DRIVER_FUNC_REMOVE);
    
    
    return ;
    
}

BOOLEAN
SetupDriverName(
    PUCHAR DriverLocation
    )
{
    HANDLE fileHandle;

    //
    // Setup path name to driver file.
    //
    strcat(DriverLocation, "\\");
    strcat(DriverLocation, DRIVER_NAME);
    strcat(DriverLocation, ".sys");

    //
    // Insure driver file is in the specified directory.
    //
    fileHandle = CreateFile (DriverLocation,
                             GENERIC_READ,
                             0,
                             NULL,
                             OPEN_EXISTING,
                             FILE_ATTRIBUTE_NORMAL,
                             NULL
                            );
    if (fileHandle == INVALID_HANDLE_VALUE) {
        printf("Driver: %s.SYS is not in the system directory. \n", DRIVER_NAME);
        return FALSE;
    }

    if (fileHandle) {
        CloseHandle(fileHandle);
    }

    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////
//
// Manage Windows drivers via the Service Control Manager APIs
//
/////////////////////////////////////////////////////////////////////////////////////
BOOLEAN
InstallDriver(
    IN SC_HANDLE  SchSCManager,
    IN LPCTSTR    DriverName,
    IN LPCTSTR    ServiceExe
    );


BOOLEAN
RemoveDriver(
    IN SC_HANDLE  SchSCManager,
    IN LPCTSTR    DriverName
    );

BOOLEAN
StartDriver(
    IN SC_HANDLE  SchSCManager,
    IN LPCTSTR    DriverName
    );

BOOLEAN
StopDriver(
    IN SC_HANDLE  SchSCManager,
    IN LPCTSTR    DriverName
    );



BOOLEAN
InstallDriver(
    IN SC_HANDLE  SchSCManager,
    IN LPCTSTR    DriverName,
    IN LPCTSTR    ServiceExe
    )
{
    SC_HANDLE   schService;
    DWORD       err;

    //
    // NOTE: This creates an entry for a standalone driver. If this
    //       is modified for use with a driver that requires a Tag,
    //       Group, and/or Dependencies, it may be necessary to
    //       query the registry for existing driver information
    //       (in order to determine a unique Tag, etc.).
    //

    //
    // Create a new a service object.
    //

    schService = CreateService(SchSCManager,           // handle of service control manager database
                               DriverName,             // address of name of service to start
                               DriverName,             // address of display name
                               SERVICE_ALL_ACCESS,     // type of access to service
                               SERVICE_KERNEL_DRIVER,  // type of service
                               SERVICE_DEMAND_START,   // when to start service
                               SERVICE_ERROR_NORMAL,   // severity if service fails to start
                               ServiceExe,             // address of name of binary file
                               NULL,                   // service does not belong to a group
                               NULL,                   // no tag requested
                               NULL,                   // no dependency names
                               NULL,                   // use LocalSystem account
                               NULL                    // no password for service account
                               );

    if (schService == NULL) {
        err = GetLastError();
        if (err == ERROR_SERVICE_EXISTS) {
            return TRUE;            // Ignore.
        } else {
            printf("CreateService failed!  Error = %d \n", err );
            return FALSE;           // Failure.
        }
    }

    //
    // Close the service object.
    //
    if (schService) {
        CloseServiceHandle(schService);
    }

    return TRUE;                    // Success.
}

BOOLEAN
ManageDriver(
    IN LPCTSTR  DriverName,
    IN LPCTSTR  ServiceName,
    IN USHORT   Function
    )
{

    SC_HANDLE   schSCManager;

    BOOLEAN rCode = TRUE;

    //
    // Insure (somewhat) that the driver and service names are valid.
    //
    if (!DriverName || !ServiceName) {
        printf("Invalid Driver or Service provided to ManageDriver() \n");
        return FALSE;
    }

    //
    // Connect to the Service Control Manager and open the Services database.
    //
    schSCManager = OpenSCManager(NULL,                   // local machine
                                 NULL,                   // local database
                                 SC_MANAGER_ALL_ACCESS   // access required
                                 );

    if (!schSCManager) {
        printf("Open SC Manager failed! Error = %d \n", GetLastError());
        return FALSE;
    }

    //
    // Do the requested function.
    //
    switch( Function ) {

        case DRIVER_FUNC_INSTALL:
            //
            // Install the driver service.
            //
            if (InstallDriver(schSCManager, DriverName, ServiceName)) { 
                //
                // Start the driver service (i.e. start the driver).
                //
                rCode = StartDriver(schSCManager, DriverName);

            } else {
                rCode = FALSE;          // Indicate an error.
            }
            break;

        case DRIVER_FUNC_REMOVE:
            //
            // Stop the driver and remove the driver service.
            //
            StopDriver(schSCManager, DriverName);
            RemoveDriver(schSCManager, DriverName);

            rCode = TRUE;       // Ignore all errors.
            break;

        default:
            printf("Unknown ManageDriver() function. \n");
            rCode = FALSE;
            break;
    }

    //
    // Close handle to service control manager.
    //
    if (schSCManager) {
        CloseServiceHandle(schSCManager);
    }
    return rCode;
}

BOOLEAN
RemoveDriver(
    IN SC_HANDLE    SchSCManager,
    IN LPCTSTR      DriverName
    )
{
    SC_HANDLE   schService;
    BOOLEAN     rCode;
    DWORD       err;

    //
    // Open the handle to the existing service.
    //
    schService = OpenService(SchSCManager, DriverName, SERVICE_ALL_ACCESS);
    if (schService == NULL) {
        err = GetLastError();
        if (err != ERROR_SERVICE_DOES_NOT_EXIST && err != ERROR_SERVICE_NOT_ACTIVE)
        {
            printf("OpenService failed!  Error = %d \n", err);
        }
        return FALSE;           // Failure.
    }

    //
    // Mark the service for deletion from the service control manager database.
    //
    if (DeleteService(schService)) {
        rCode = TRUE;           // Success.
    } else {
        err = GetLastError();
        if (err != ERROR_SERVICE_DOES_NOT_EXIST && err != ERROR_SERVICE_NOT_ACTIVE)
        {
            printf("DeleteService failed!  Error = %d \n", err);
        }
        rCode = FALSE;          // Failure.
    }

    //
    // Close the service object.
    //
    if (schService) {
        CloseServiceHandle(schService);
    }

    return rCode;
}

BOOLEAN
StartDriver(
    IN SC_HANDLE    SchSCManager,
    IN LPCTSTR      DriverName
    )
{
    SC_HANDLE   schService;
    DWORD       err;

    //
    // Open the handle to the existing service.
    //
    schService = OpenService(SchSCManager, DriverName, SERVICE_ALL_ACCESS);
    if (schService == NULL) {
        printf("OpenService failed!  Error = %d \n", GetLastError());
        return FALSE;           // Failure.
    }

    //
    // Start the execution of the service (i.e. start the driver).
    //
    if (!StartService(schService, 0, NULL)) {

        err = GetLastError();
        if (err == ERROR_SERVICE_ALREADY_RUNNING) {
            return TRUE;        // Ignore.
        } else {
            printf("StartService failure! Error = %d \n", err );
            return FALSE;       // Failure.
        }

    }

    //
    // Close the service object.
    //
    if (schService) {
        CloseServiceHandle(schService);
    }

    return TRUE;
}

BOOLEAN
StopDriver(
    IN SC_HANDLE    SchSCManager,
    IN LPCTSTR      DriverName
    )
{
    BOOLEAN         rCode = TRUE;
    SC_HANDLE       schService;
    SERVICE_STATUS  serviceStatus;
    DWORD           err;

    //
    // Open the handle to the existing service.
    //
    schService = OpenService(SchSCManager, DriverName, SERVICE_ALL_ACCESS); 
    if (schService == NULL) {
        err = GetLastError();
        if (err != ERROR_SERVICE_DOES_NOT_EXIST && err != ERROR_SERVICE_NOT_ACTIVE)
        {
            printf("OpenService failed!  Error = %d \n", err);
        }
        return FALSE;
    }

    //
    // Request that the service stop.
    //
    if (ControlService(schService, SERVICE_CONTROL_STOP, &serviceStatus)) {
        rCode = TRUE;           // Success.
    } else {
        err = GetLastError();
        if (err != ERROR_SERVICE_DOES_NOT_EXIST && err != ERROR_SERVICE_NOT_ACTIVE)
        {
            printf("ControlService failed!  Error = %d \n", err);
        }
        rCode = FALSE;          // Failure.
    }

    //
    // Close the service object.
    //
    if (schService) {
        CloseServiceHandle (schService);
    }

    return rCode;
}


