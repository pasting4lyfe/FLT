#include <Exported/Exported.hxx>

namespace n_export_table {

	n_typedef::tPsGetProcessImageFileName PsGetProcessImageFileName = nullptr;
	n_typedef::tPsLookupProcessByProcessId PsLookupProcessByProcessId = nullptr;
	n_typedef::tPsGetProcessSectionBaseAddress PsGetProcessSectionBaseAddress = nullptr;
	n_typedef::tMmCopyVirtualMemory MmCopyVirtualMemory = nullptr;

	NTSTATUS AutoResolveExports() {

		UNICODE_STRING usPsGetProcessImageFileName;
		UNICODE_STRING usPsLookupProcessByProcessId;
		UNICODE_STRING usPsGetProcessSectionBaseAddress;
		UNICODE_STRING usMmCopyVirtualMemory;

		RtlInitUnicodeString(&usPsGetProcessImageFileName, L"PsGetProcessImageFileName");
		PsGetProcessImageFileName = reinterpret_cast<n_typedef::tPsGetProcessImageFileName>(
			MmGetSystemRoutineAddress(&usPsGetProcessImageFileName)
			);
		if (!PsGetProcessImageFileName)
			return STATUS_NOT_FOUND;
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "Exported PsGetProcessImageFileName\n");

		RtlInitUnicodeString(&usPsLookupProcessByProcessId, L"PsLookupProcessByProcessId");
		PsLookupProcessByProcessId = reinterpret_cast<n_typedef::tPsLookupProcessByProcessId>(
			MmGetSystemRoutineAddress(&usPsLookupProcessByProcessId)
			);
		if (!PsLookupProcessByProcessId)
			return STATUS_NOT_FOUND;
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "Exported PsLookupProcessByProcessId\n");

		RtlInitUnicodeString(&usPsGetProcessSectionBaseAddress, L"PsGetProcessSectionBaseAddress");
		PsGetProcessSectionBaseAddress = reinterpret_cast<n_typedef::tPsGetProcessSectionBaseAddress>(
			MmGetSystemRoutineAddress(&usPsGetProcessSectionBaseAddress)
			);
		if (!PsGetProcessSectionBaseAddress)
			return STATUS_NOT_FOUND;
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "Exported PsGetProcessSectionBaseAddress\n");

		RtlInitUnicodeString(&usMmCopyVirtualMemory, L"MmCopyVirtualMemory");
		MmCopyVirtualMemory = reinterpret_cast<n_typedef::tMmCopyVirtualMemory>(
			MmGetSystemRoutineAddress(&usMmCopyVirtualMemory)
			);
		if (!MmCopyVirtualMemory)
			return STATUS_NOT_FOUND;
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "Exported MmCopyVirtualMemory\n");

		return STATUS_SUCCESS;
	}
}
