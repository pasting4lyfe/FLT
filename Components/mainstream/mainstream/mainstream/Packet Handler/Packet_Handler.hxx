#pragma once
#include <ntddk.h>
#include <Shared_Packet.hxx>
#include <wdf.h>

namespace n_packet_handler {
	//ghost defs
	NTSTATUS ProcessPacket( _In_ n_communication::s_packet* pIn, _Out_ n_communication::s_packet* pOut,_In_ size_t InputSize,_In_ size_t OutputSize);

}