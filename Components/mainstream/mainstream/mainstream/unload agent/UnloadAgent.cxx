#include "UnloadAgent.hxx"
#include <Shared/CSRRCR/consumer_reciever.hxx>




	VOID n_unload_agent::UnloadRoutine(_In_ WDFDRIVER Driver) {



		PDRIVER_OBJECT driverObject = WdfDriverWdmGetDriverObject(Driver);
		PDEVICE_CONTEXT driverExt = GetDriverExtension(driverObject);
		if (!driverExt) return;
		if (buffer.gSectionHandle != (HANDLE)(LONG_PTR)-1) {
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "Unload Routine Verified, Shutting Down Shared Sections.\n");
			buffer.CleanUp();
		}//for now we just shut down by ourself, tmw or later this evening ill implement the unload callback 
		else { DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "Unload Routine Verified.\n"); }
		n_callback_wizard::UnInstallCallback();
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "Unload Routine Completed\n");
	}
