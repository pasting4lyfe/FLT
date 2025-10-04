#include "wizard.hxx"
#include <deployment/ioctl_settings.hxx>
#include <Packet Handler/Packet_Handler.hxx>//hand me a frigging packet yo
#include <wdf.h> 

NTSTATUS n_wizard::n_allocateDevice(_In_ PDRIVER_OBJECT Driver_Object) {
    DEVICE_CONTEXT* driverExt = nullptr;
    NTSTATUS status = IoAllocateDriverObjectExtension(
        Driver_Object,
        KTAG_EXTENSION,
        sizeof(DEVICE_CONTEXT),
        (PVOID*)&driverExt
    );
    if (!NT_SUCCESS(status)) return status;
    return status;
}

NTSTATUS n_wizard::EvtDeviceAdd(
    _In_ WDFDRIVER Driver,
    _Inout_ PWDFDEVICE_INIT DeviceInit
)
{
    UNREFERENCED_PARAMETER(Driver);
    NTSTATUS status;
    WDFDEVICE device;
    WDF_OBJECT_ATTRIBUTES wdfDeviceAttributes;
    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&wdfDeviceAttributes, DEVICE_CONTEXT);
    wdfDeviceAttributes.EvtCleanupCallback = n_wizard::EvtDeviceContextCleanup;
    status = WdfDeviceCreate(&DeviceInit, &wdfDeviceAttributes, &device);
    if (!NT_SUCCESS(status)) { DbgPrint("Failed to Create Driver Device\n"); return status; }
    DbgPrint("Created Device\n");
    PDEVICE_CONTEXT devContext = GetDeviceContext(device);
    RtlZeroMemory(devContext, sizeof(DEVICE_CONTEXT));
    devContext->device = device;
    devContext->PoolTaag = 'tsCS';
    WDF_IO_QUEUE_CONFIG queueCFG;
    WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(&queueCFG, WdfIoQueueDispatchParallel);
    queueCFG.EvtIoDeviceControl = EvtDeviceHandler;
    WDFQUEUE queue;
    status = WdfIoQueueCreate(device, &queueCFG, WDF_NO_OBJECT_ATTRIBUTES, &queue);
    if (!NT_SUCCESS(status)) {
        DbgPrint("Failed to Create a WDF Io Queue\n");
        return status;
    }
    DbgPrint("Queued WDF\n");
    DECLARE_CONST_UNICODE_STRING(symLink, L"\\??\\NimBusSym");
    status = WdfDeviceCreateSymbolicLink(device, &symLink);
    if (!NT_SUCCESS(status)) {
        DbgPrint("Failed to Create Symbolic Link\n");
        return status;
    }
    DbgPrint("Created WDF Interface\n");
    IoInitializeRemoveLock(&devContext->remove_lock, devContext->PoolTaag, 0, 0);
    return STATUS_SUCCESS;
}

VOID n_wizard::EvtDeviceContextCleanup(_In_ WDFOBJECT DeviceObject) {
    PDEVICE_CONTEXT context = GetDeviceContext(DeviceObject);
    IoReleaseRemoveLockAndWait(&context->remove_lock, (PVOID)DeviceObject);
}

#include <Shared/shared_buffers.hxx>
#include <Shared_Packet.hxx>
#include <unload agent/UnloadAgent.hxx>

void n_wizard::EvtDeviceHandler(
    _In_ WDFQUEUE Queue,
    _In_ WDFREQUEST Request,
    _In_ SIZE_T OutputBufferLength,
    _In_ SIZE_T InputBufferLength,
    _In_ ULONG IoControlCode
)
{
    DbgPrint("EvtIoDeviceControl entered\n");
    UNREFERENCED_PARAMETER(InputBufferLength);
    UNREFERENCED_PARAMETER(OutputBufferLength);
    PDEVICE_CONTEXT ctx = GetDeviceContext(WdfIoQueueGetDevice(Queue));
    NTSTATUS status = IoAcquireRemoveLock(&ctx->remove_lock, (PVOID)Request);
    if (!NT_SUCCESS(status))
    {
        WdfRequestComplete(Request, status);
        return;
    }
    if (IoControlCode == TELEMETRY_GLOBAL_PACKAGEHEAD)
    {
        //DbgPrint("TELEMETRY GLOBAL PACKAGE HEAD ACCEPTED\n");
        if (InputBufferLength < sizeof(n_communication::s_packet) ||
            OutputBufferLength < sizeof(n_communication::s_packet))
        {
            DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Buffer too small\n");
            status = STATUS_BUFFER_TOO_SMALL;
            goto Complete;
        }
        n_communication::s_packet* pIn = nullptr;
        n_communication::s_packet* pOut = nullptr;
        status = WdfRequestRetrieveInputBuffer(Request, sizeof(n_communication::s_packet), (PVOID*)&pIn, nullptr);
        if (!NT_SUCCESS(status) || !pIn) {
            DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Failed to retrieve input buffer\n");
            status = STATUS_INVALID_PARAMETER;
            goto Complete;
        }
        status = WdfRequestRetrieveOutputBuffer(Request, sizeof(n_communication::s_packet), (PVOID*)&pOut, nullptr);
        if (!NT_SUCCESS(status) || !pOut) {
            DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Failed to retrieve output buffer\n");
            status = STATUS_INVALID_PARAMETER;
            goto Complete;
        }
        if (pIn->u_auth_cookie != n_communication::u_cookie) {
            DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Failed to verify security cookie\n");
            status = STATUS_ACCESS_DENIED;
            goto Complete;
        }
        //DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "Validated security cookie, matching packet acquired\n");
        status = n_packet_handler::ProcessPacket(pIn, pOut, InputBufferLength, OutputBufferLength);
    Complete:
        WdfRequestCompleteWithInformation(Request, status,
            NT_SUCCESS(status) ? sizeof(n_communication::s_packet) : 0);

        IoReleaseRemoveLock(&ctx->remove_lock, Request);
        return;
    }

    WdfRequestComplete(Request, STATUS_INVALID_DEVICE_REQUEST);
    IoReleaseRemoveLock(&ctx->remove_lock, Request);
}

VOID n_wizard::EvtDriverUnload(_In_ WDFDRIVER Driver)
{
    n_unload_agent::UnloadRoutine(Driver);
}