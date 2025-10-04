#include <Shared/shared_buffers.hxx>

VOID s_shared::ActivateSectionEntry() {
    buffer.SectionUnlocked = TRUE;
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "0::0\n");
}

NTSTATUS s_shared::startSubConnection()
{
    if (!buffer.SectionUnlocked)
        return STATUS_UNSATISFIED_DEPENDENCIES;

    UNICODE_STRING uSectionName;
    RtlInitUnicodeString(&uSectionName, L"\\BaseNamedObjects\\NimbusCTRLBridge"); 
    OBJECT_ATTRIBUTES objAttr;
    InitializeObjectAttributes(&objAttr,&uSectionName,OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE,NULL,NULL);
    LARGE_INTEGER maxSize;
    maxSize.QuadPart = sizeof(shared_ctrl);
    NTSTATUS status = ZwCreateSection(&buffer.gSectionHandle,SECTION_ALL_ACCESS,&objAttr,     &maxSize,PAGE_READWRITE,SEC_COMMIT,NULL);      
    if (!NT_SUCCESS(status)) {
        log("ZwCreateSection Failed 0x%X\n", status);
        return status;
    }
    SIZE_T viewSize = 0;
    status = ZwMapViewOfSection(buffer.gSectionHandle,ZwCurrentProcess(),&buffer.gSharedView,0,0,NULL,&viewSize,ViewUnmap,0,PAGE_READWRITE);
    if (!NT_SUCCESS(status)) {
        log("ZwMapViewOfSection Failed 0x%X\n", status);
        ZwClose(buffer.gSectionHandle);
        buffer.gSectionHandle = NULL;
        return status;
    }
    buffer.gCtrl = (pshared_ctrl)buffer.gSharedView;
    RtlZeroMemory(buffer.gCtrl, sizeof(shared_ctrl));
    buffer.gCtrl->u64_cookie = d_cookie;
    buffer.gCtrl->u32_version = 1;
    log("Created Shared Entry for Sub-Drivers.\n", status);
    return STATUS_SUCCESS;
}

void s_shared::CleanUp()
{
    if (buffer.gSharedView) {
        ZwUnmapViewOfSection(NtCurrentProcess(), buffer.gSharedView);
        buffer.gSharedView = NULL;
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "Unampped Memory Sections.\n");
    }
    if (buffer.gSectionHandle) {
        ZwClose(buffer.gSectionHandle);
        buffer.gSectionHandle = NULL;
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "Cleared Section Handle.\n");
    }
    buffer.gCtrl = NULL;
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "NULLED CTRL.\n");
}
