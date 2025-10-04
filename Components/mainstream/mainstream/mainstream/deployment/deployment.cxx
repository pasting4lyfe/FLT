#include <deployment/ioctl_settings.hxx>
#include <io wizard/wizard.hxx>
#include <Shared/shared_buffers.hxx>
#include <Callbacks/Callback_wizard.hxx>
#include <Exported/Exported.hxx>

#ifdef ALLOC_PRAGMA
#pragma alloc_text (INIT, DriverEntry)
#endif

extern "C" NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject,_In_ PUNICODE_STRING RegistryPath)
{

    if (NT_SUCCESS(n_export_table::AutoResolveExports())) {
       DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "Resolved Unexported Function\n.");
    }
    else {};

    WDF_DRIVER_CONFIG wdfconfig;
    PDEVICE_CONTEXT driverExt = nullptr;
    NTSTATUS status = IoAllocateDriverObjectExtension(DriverObject,KTAG_EXTENSION,sizeof(DEVICE_CONTEXT),(PVOID*)&driverExt);
    if (!NT_SUCCESS(status)) {
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,"IoAllocateDriverObjectExtension failed: 0x%X\n", status);
        return status;}
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "Allocated Driver Object Extension\n");
    WDF_DRIVER_CONFIG_INIT(&wdfconfig, n_wizard::EvtDeviceAdd);
    wdfconfig.EvtDriverUnload = n_wizard::EvtDriverUnload;
    status = WdfDriverCreate(DriverObject, RegistryPath, WDF_NO_OBJECT_ATTRIBUTES, &wdfconfig, WDF_NO_HANDLE);
    if (!NT_SUCCESS(status)) {
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,"WdfDriverCreate failed: 0x%X\n", status);
        return status;}
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "Created WDF Driver\n");

    status = n_callback_wizard::InstallCallback();
    if (status) {
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "Callbacks Installed.\n");
        return status;
    }

    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "Allocating Signal for sub-based Communication Entry.\n");
    buffer.ActivateSectionEntry();
    buffer.SectionUnlocked ? buffer.startSubConnection( ) : DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "Section Locked.\n");

    return status;
}
