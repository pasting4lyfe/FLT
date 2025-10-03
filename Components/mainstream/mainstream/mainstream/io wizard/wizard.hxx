#pragma once
#include <ntddk.h>
#include <wdf.h>


namespace n_wizard {
	extern "C"
	{
		EVT_WDF_DRIVER_DEVICE_ADD EvtDeviceAdd;
		EVT_WDF_DRIVER_UNLOAD     EvtDriverUnload;
		EVT_WDF_IO_QUEUE_IO_DEVICE_CONTROL EvtDeviceHandler;
		EVT_WDF_OBJECT_CONTEXT_CLEANUP EvtDeviceContextCleanup;
	}
	NTSTATUS n_allocateDevice(_In_ PDRIVER_OBJECT Driver_Object);
}