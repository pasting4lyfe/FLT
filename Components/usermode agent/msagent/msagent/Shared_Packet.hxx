#pragma once
#include <Windows.h>

inline const ULONG TELEMETRY_GLOBAL_PACKAGEHEAD = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800,METHOD_BUFFERED,FILE_ANY_ACCESS);
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
	};

	typedef struct s_packet {
		RequestType type;
		BOOLEAN RingRelated;
		data_pool Data;
		ULONG u_auth_cookie;
	}packet;

}
