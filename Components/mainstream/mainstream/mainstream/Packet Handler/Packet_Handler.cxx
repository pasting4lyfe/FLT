#include <Packet Handler/Packet_Handler.hxx>
#include <IAT/IAT.hxx>


namespace n_packet_handler {


	NTSTATUS ProcessPacket(_In_ n_communication::s_packet* pIn, _Out_ n_communication::s_packet* pOut, _In_ size_t InputSize, _In_ size_t OutputSize) {
		if (!pIn || !pOut || InputSize < sizeof(n_communication::s_packet) || OutputSize < sizeof(n_communication::s_packet)) {
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Failed to Process Usermode Packte\n");
			return STATUS_BUFFER_TOO_SMALL;
		}

		RtlCopyMemory(pOut, pIn, sizeof(n_communication::s_packet));//once we know ts is valid we can format the packet base to the output

		switch (pIn->type) {
		case n_communication::RequestType::exclude: DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "Packet Type -> Excluded\n"); break;

		case n_communication::RequestType::auth_test: {
			pOut->Data.test1 = FALSE;
			break;
		}
		case n_communication::RequestType::unload: {
			break;
		}
		case n_communication::RequestType::iat_lookup: {

			auto pid = iat.find_proc_by_name(pIn->Data.iat_table.processname);
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "PID -> %i\n", pid);
			PBYTE address = iat.find_proc_import(pid, pIn->Data.iat_table.import);
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "IAT Looked up -> %p\n", (const char*)address);
			pOut->Data.iat_table.returnAddress = (ULONG_PTR)address;
			//RtlCopyMemory(pOut->Data.iat_table.returnAddress, address, sizeof(address));
			//pOut->Data.iat_table.returnAddress = address;

			break;
		}
		default: {
			break;
		}
		}


		return STATUS_SUCCESS;
	}
}