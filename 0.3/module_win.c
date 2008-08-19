#include <ntddk.h>

#define TRIVIAL_DEVICE_NAME_U     L"\\Device\\CANCELSAMP"
#define TRIVIAL_DOS_DEVICE_NAME_U L"\\DosDevices\\CancelSamp"


#define CSAMP_KDPRINT(_x_) \
                DbgPrint("CANCEL.SYS: ");\
                DbgPrint _x_;

typedef struct _DEVICE_EXTENSION{

    BOOLEAN Spare;
    
}  DEVICE_EXTENSION, *PDEVICE_EXTENSION;

NTSTATUS
DriverEntry(
    IN PDRIVER_OBJECT  DriverObject,
    IN PUNICODE_STRING registryPath
);

NTSTATUS
TrivialCreateClose (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

VOID
TrivialUnload(
    IN PDRIVER_OBJECT DriverObject
    );

NTSTATUS
TrivialCleanup(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
);


#ifdef ALLOC_PRAGMA
#pragma alloc_text( INIT, DriverEntry )
#pragma alloc_text( PAGE, TrivialCreateClose)
#pragma alloc_text( PAGE, TrivialUnload)
#endif // ALLOC_PRAGMA


extern int init_rr0d(void);
extern void cleanup_rr0d(void);

NTSTATUS
DriverEntry(
    IN PDRIVER_OBJECT  DriverObject,
    IN PUNICODE_STRING RegistryPath
    )
{
    NTSTATUS            status = STATUS_SUCCESS;
    UNICODE_STRING      unicodeDeviceName;   
    UNICODE_STRING      unicodeDosDeviceName;  
    PDEVICE_OBJECT      deviceObject;
    PDEVICE_EXTENSION   devExtension;
    HANDLE              threadHandle;

    UNREFERENCED_PARAMETER (RegistryPath);

    CSAMP_KDPRINT(("--DriverEntry Enter \n"));
    
    //
    // Initialize UNICODE_STRING structures with NT and DOS device names
    //
    (void) RtlInitUnicodeString( &unicodeDeviceName, TRIVIAL_DEVICE_NAME_U);
    (void) RtlInitUnicodeString( &unicodeDosDeviceName, TRIVIAL_DOS_DEVICE_NAME_U );



    //
    // Create DOS device
    //
    status = IoCreateDevice(
                DriverObject,
                sizeof(DEVICE_EXTENSION),
                &unicodeDeviceName,
                FILE_DEVICE_UNKNOWN,
                0,
                (BOOLEAN) FALSE,
                &deviceObject
                );

    if (!NT_SUCCESS(status))
    {
        return status;
    }

    //
    // Create the DOS name for the device
    //
    status = IoCreateSymbolicLink(
                (PUNICODE_STRING) &unicodeDosDeviceName,
                (PUNICODE_STRING) &unicodeDeviceName
                );

    if (!NT_SUCCESS(status))
    {
        IoDeleteDevice(deviceObject);
        return status;
    }

    // Nothing in our device extension
    //devExtension = deviceObject->DeviceExtension;
    
    //
    // Initialize our IRP handling and unload functions
    //
    DriverObject->MajorFunction[IRP_MJ_CREATE]  = TrivialCreateClose;
    DriverObject->MajorFunction[IRP_MJ_CLOSE]   = TrivialCreateClose;
    DriverObject->MajorFunction[IRP_MJ_CLEANUP] = TrivialCleanup;

    DriverObject->DriverUnload = TrivialUnload;


    init_rr0d();

    //  Nothing to set in our flags, since we don't do any actual I/O
    //deviceObject->Flags |= DO_BUFFERED_IO;

    return status;
}



NTSTATUS
TrivialCreateClose(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
{
    PIO_STACK_LOCATION   irpStack;
    NTSTATUS             status = STATUS_SUCCESS;

    PAGED_CODE ();

    //
    // Get a pointer to the current location in the Irp.
    //
    CSAMP_KDPRINT(("TrivialCreateClose \n"));

    irpStack = IoGetCurrentIrpStackLocation(Irp);

    switch(irpStack->MajorFunction)
    {
        case IRP_MJ_CREATE:
            Irp->IoStatus.Information = 0;
            break;

        case IRP_MJ_CLOSE:
            Irp->IoStatus.Information = 0;
            break;

        default:
            status = STATUS_INVALID_PARAMETER;
            break;
    }

    //
    // Save Status for return and complete Irp
    //
    Irp->IoStatus.Status = status;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return status;
}

NTSTATUS
TrivialCleanup(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
)
{

    PDEVICE_EXTENSION     devExtension;
    LIST_ENTRY             tempQueue;   
    PLIST_ENTRY            thisEntry;
    PIRP                   pendingIrp;
    PIO_STACK_LOCATION    pendingIrpStack, irpStack;


    CSAMP_KDPRINT(("TrivialCleanup \n"));

    //devExtension = DeviceObject->DeviceExtension;
    //irpStack = IoGetCurrentIrpStackLocation(Irp);

    //
    // Finally complete the cleanup IRP
    //
    Irp->IoStatus.Information = 0;
    Irp->IoStatus.Status = STATUS_SUCCESS;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return STATUS_SUCCESS;
}

VOID
TrivialUnload(
    IN PDRIVER_OBJECT DriverObject
    )
{
    PDEVICE_OBJECT       deviceObject = DriverObject->DeviceObject;
    UNICODE_STRING       uniWin32NameString;
    PDEVICE_EXTENSION    devExtension = deviceObject->DeviceExtension;

    PAGED_CODE();

    CSAMP_KDPRINT(("TrivialUnload \n"));
    
    
    cleanup_rr0d();
    
    //
    // Delete our DOS link and then our device
    //
    RtlInitUnicodeString( &uniWin32NameString, TRIVIAL_DOS_DEVICE_NAME_U );
    IoDeleteSymbolicLink( &uniWin32NameString );

    IoDeleteDevice( deviceObject );
 
    return;
}


