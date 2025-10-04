#include <Callbacks/Callback_wizard.hxx>

namespace n_callback_wizard {
	NTSTATUS InstallCallback() {
        NTSTATUS status;
        status = PsSetLoadImageNotifyRoutine(LoadImageNotifyRoutine);
        if (NT_SUCCESS(status)) {
            LoadImageNotifyRoutineRegistered = true;
        }
        else {
            DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Failed to register LoadImageNotifyRoutine: 0x%X\n", status);
            LoadImageNotifyRoutineRegistered = false;
        }
        return status;
	}
	VOID UnInstallCallback() {
        if (LoadImageNotifyRoutineRegistered) {
            PsRemoveLoadImageNotifyRoutine(LoadImageNotifyRoutine);
            DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Uninstalled PsLetLoadImageNotifyRoutine Callback\n");
        }
	}
}