//not in use rn
#include <ntddk.h>

extern bool LoadImageNotifyRoutineRegistered;

VOID LoadImageNotifyRoutine(_In_opt_ PUNICODE_STRING FullImageName,_In_ HANDLE ProcessId,_In_ PIMAGE_INFO ImageInfo);