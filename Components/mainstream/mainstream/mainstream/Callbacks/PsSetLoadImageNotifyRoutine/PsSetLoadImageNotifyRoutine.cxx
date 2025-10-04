#include "psSetLoadImageNotifyRoutine.hxx"

bool LoadImageNotifyRoutineRegistered = false;

VOID LoadImageNotifyRoutine(_In_opt_ PUNICODE_STRING FullImageName,_In_ HANDLE ProcessId,_In_ PIMAGE_INFO ImageInfo)
{
    UNREFERENCED_PARAMETER(ImageInfo);
    if (FullImageName && FullImageName->Buffer) {
        DbgPrint("new process instances spawned -> pid=%p name=%wZ virtualaddy=%p size=%lu\n",ProcessId,FullImageName,ImageInfo->ImageBase,(ULONG)ImageInfo->ImageSize);
    }
}