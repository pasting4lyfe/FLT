#pragma once
#include <ntddk.h>
#include <ntintsafe.h>
//device io control global packages
//shared between um agent and mainstream drv
//nigga
namespace n_communication {

	inline ULONG u_cookie = 0xA84Fa;

	enum class RequestType : UINT64 {
		exclude = NULL,
		auth_test,//1
		unload,
		lockdown,
		unlock
	};

	typedef struct data_pool {
		BOOLEAN test1;
	}data_pool;

	typedef struct s_packet {
		RequestType type;
		BOOLEAN RingRelated;
		data_pool Data;
		ULONG u_auth_cookie;
	}s_packet;


}
