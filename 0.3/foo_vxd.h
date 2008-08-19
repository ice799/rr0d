
#ifndef _FOO_VXD_H
#define _FOO_VXD_H

#ifdef __cplusplus
extern "C" {
#endif

#define FOO_VXD_MAJOR_VER    1		// Major version number
#define FOO_VXD_MINOR_VER    0		// Minor version number
#define FOO_VXD_DEVICE_ID    UNDEFINED_DEVICE_ID	// Device ID for this VxD
#define FOO_VXD_INIT_ORDER   UNDEFINED_INIT_ORDER	// Initialization order

///////////////////
// Define Win32 DeviceIoControl codes
//
#define FOO_VXD_CTL_CODE(IoctlIndex)  CTL_CODE(FILE_DEVICE_UNKNOWN,(IoctlIndex|0x800),METHOD_BUFFERED,FILE_ANY_ACCESS)

#define IOCTL_FOO_VXD_GET_VERSION    FOO_VXD_CTL_CODE(1)


#endif