#pragma once
#include <ntddk.h>
#include <wdf.h>
#include <initguid.h>   
#include <wdfrequest.h>
#include <wdmguid.h>
#include <ntintsafe.h>
#include <ntstrsafe.h>

inline const ULONG TELEMETRY_GLOBAL_PACKAGEHEAD = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800,METHOD_BUFFERED,FILE_ANY_ACCESS);
struct DEVICE_CONTEXT {
	WDFDEVICE device;
	IO_REMOVE_LOCK remove_lock;
	ULONG PoolTaag;
};
using PDEVICE_CONTEXT = DEVICE_CONTEXT*;
WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(DEVICE_CONTEXT, GetDeviceContext);
inline PDEVICE_CONTEXT GetDeviceContextFromRequest(WDFREQUEST request) {
	return GetDeviceContext(WdfIoQueueGetDevice(WdfRequestGetIoQueue(request)));
}
extern "C" DRIVER_INITIALIZE DriverEntry;
#define KTAG_EXTENSION (PVOID)'xtag'
inline PDEVICE_CONTEXT GetDriverExtension(PDRIVER_OBJECT DriverObject)
{
    PDEVICE_CONTEXT driverExt = nullptr;
    driverExt = (PDEVICE_CONTEXT)IoGetDriverObjectExtension(
        DriverObject,
        KTAG_EXTENSION
    );
    return driverExt;
}