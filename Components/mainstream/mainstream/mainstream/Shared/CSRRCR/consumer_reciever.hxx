#pragma once
#include <Shared/shared_buffers.hxx>

namespace n_unloader {
	struct reciever {
		NTSTATUS SendUnloadPacket(ring* ring, const s_unload* pkt);
	}; extern reciever rec;
	struct consumer {
		NTSTATUS RecvUnloadPacket(ring* ring, s_unload* outPkt);
	}; extern consumer cos;
	NTSTATUS SignalUnload();//this is our main function to start unloading  not the other ones :)
}

namespace n_shared {
	//more coming soon
}