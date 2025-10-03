#pragma once
#include <ntddk.h>
#include <Shared/PacketIndex.hxx>

__forceinline VOID log(PCSTR Cause, NTSTATUS status) {
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,
		Cause,status);
}

struct s_shared {
	HANDLE gSectionHandle = NULL;
	PVOID  gSharedView = NULL;
	pshared_ctrl gCtrl = NULL;
	BOOLEAN SectionUnlocked = 0;
	VOID CleanUp();
	VOID ActivateSectionEntry();
	NTSTATUS startSubConnection();
};
extern s_shared buffer;