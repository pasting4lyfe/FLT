#pragma once
#include <ntddk.h>

#define d_slot_size      2048
#define d_slots_pring    1024
#define d_mask         (d_slots_pring - 1)
#define PKT_TYPE_UNLOAD   1
#define d_cookie  0xA55A5A5AULL

typedef struct s_unload {
    BOOLEAN b_StartUnloadRoutine; //from km->mf
    BOOLEAN b_SignalUnloadRotuine;  //from mf->km 
    ULONG   u_Error;        //0
} unload;

typedef struct s_slot {
    volatile ULONG64 u_seq;   
    volatile ULONG32 u_length; 
    ULONG32 u32_type;          
    UCHAR u_payload[d_slot_size - sizeof(ULONG64) - sizeof(ULONG32) - sizeof(ULONG32)];
} slot;

typedef struct s_ring {
    volatile LONG64 l64_head; 
    volatile LONG64 l64_tail; 
    UCHAR u_pad[64 - (sizeof(LONG64) * 2 % 64)];
    slot s_slots[d_slots_pring];
} ring;

typedef struct shared_ctrl {
    ULONG64 u64_cookie;
    ULONG32 u32_version;
    ULONG32 u32_flags;
    ring r_ringKMtoMF; 
    ring r_ringMFtoKM; 
} shared_ctrl, *pshared_ctrl;