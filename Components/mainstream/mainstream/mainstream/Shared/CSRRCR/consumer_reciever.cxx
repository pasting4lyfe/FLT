#include "consumer_reciever.hxx"

namespace n_unloader {
	reciever rec;
}
namespace n_unloader {
	consumer cos;
}

namespace n_unloader {
    NTSTATUS reciever::SendUnloadPacket(ring* ring, const s_unload* pkt) {
        static_assert(sizeof(s_unload) <= sizeof(((slot*)0)->u_payload),"dude just get rid of the compiler error L13.consumer_reciever.hxx");
        LONG64 idx = InterlockedExchangeAdd64(&ring->l64_tail, 1);
        slot* slot = &ring->s_slots[idx & d_mask];
        RtlCopyMemory(slot->u_payload, pkt, sizeof(*pkt));
        KeMemoryBarrier();
        slot->u_length = sizeof(*pkt);
        slot->u32_type = PKT_TYPE_UNLOAD;
        KeMemoryBarrier();
        slot->u_seq = idx + 1;

        return STATUS_SUCCESS;
    }

}

namespace n_unloader {
    NTSTATUS consumer::RecvUnloadPacket(ring* ring, s_unload* outPkt)//this should be on the minifilters side, but till we dont got one we will store it here
    {
        if (ring->l64_head >= ring->l64_tail) {
            return STATUS_NO_MORE_ENTRIES; 
        }
        slot* slot = &ring->s_slots[ring->l64_head & d_mask];
        KeMemoryBarrier(); //gotta sync
        if (slot->u32_type != PKT_TYPE_UNLOAD ||
            slot->u_length != sizeof(s_unload)) {
            return STATUS_INVALID_BUFFER_SIZE;
        }
        RtlCopyMemory(outPkt, slot->u_payload, sizeof(s_unload));
        KeMemoryBarrier();
        ring->l64_head++; 
        return STATUS_SUCCESS;
    }
}

namespace n_unloader {
    NTSTATUS SignalUnload() {//yayayayayayayayyahttps://www.youtube.com/watch?v=7qkYigb2yd4&list=LL&index=8yayayayayayayayyayayayaayayyayaya
        NTSTATUS status;
        s_unload packet = { 0 };
        packet.b_StartUnloadRoutine = TRUE;
        packet.b_SignalUnloadRotuine = FALSE;
        packet.u_Error = 0;
        status = rec.SendUnloadPacket(&buffer.gCtrl->r_ringKMtoMF, &packet);
        if (NT_SUCCESS(status)) {
            log("Unload Packet Sent.", NULL);
            return status;
        }
        return status;
    }
}