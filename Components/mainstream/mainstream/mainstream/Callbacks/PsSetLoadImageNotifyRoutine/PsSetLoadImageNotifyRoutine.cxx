#include "psSetLoadImageNotifyRoutine.hxx"
#include <Shared_Packet.hxx>
#include <Exported/Exported.hxx>
#include <Definition/WIN.hxx>
#include <ntimage.h>


bool LoadImageNotifyRoutineRegistered = false;

VOID LoadImageNotifyRoutine(_In_opt_ PUNICODE_STRING FullImageName,_In_ HANDLE ProcessId,_In_ PIMAGE_INFO ImageInfo)
{
    UNREFERENCED_PARAMETER(ImageInfo);
    if (FullImageName && FullImageName->Buffer) {

    }
    if (ProcessId != PsGetCurrentProcessId()) {
        PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)ImageInfo->ImageBase;
        if (dosHeader->e_magic == IMAGE_DOS_SIGNATURE) {
            PIMAGE_NT_HEADERS ntHeaders = (PIMAGE_NT_HEADERS)((PUCHAR)ImageInfo->ImageBase + dosHeader->e_lfanew);
            if (ntHeaders->Signature == IMAGE_NT_SIGNATURE) {
                if (ntHeaders->OptionalHeader.Subsystem == IMAGE_SUBSYSTEM_WINDOWS_GUI ||
                    ntHeaders->OptionalHeader.Subsystem == IMAGE_SUBSYSTEM_WINDOWS_CUI) {
                    DbgPrint("new process instances spawned -> pid=%p name=%wZ virtualaddy=%p size=%lu\n", ProcessId, FullImageName, ImageInfo->ImageBase, (ULONG)ImageInfo->ImageSize);
                }
            }
        }
    }
}

