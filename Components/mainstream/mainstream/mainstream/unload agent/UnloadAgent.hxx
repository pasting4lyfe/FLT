#pragma once
#include <ntddk.h>
#include <Shared/shared_buffers.hxx>
#include <deployment/ioctl_settings.hxx>
#include <Callbacks/PsSetLoadImageNotifyRoutine/psSetLoadImageNotifyRoutine.hxx>

namespace n_unload_agent {
	VOID UnloadRoutine(_In_ WDFDRIVER Driver);
}