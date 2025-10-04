#pragma once
#include <Windows.h>

inline const ULONG TELEMETRY_GLOBAL_PACKAGEHEAD = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800,METHOD_BUFFERED,FILE_ANY_ACCESS);

namespace n_communication {

	inline ULONG u_cookie = 0xA84Fa;

	namespace n_iat_lookup {
		typedef struct s_iat_table {
			const char* import;
			const char* processname;
			ULONG_PTR returnAddress;
		}iat_table, p_iat_table;
	}

	namespace n_callback_packet {
		typedef enum e_process_signature_status {
			ProcSig_Unknown = NULL,
			ProcSig_Signed,
			ProcSig_Unsigned,
			ProcSig_Invalid
		} process_signature_status;

		//typedef struct 

		typedef struct s_process_info {
			ULONG u_version;
			HANDLE h_pid;
			HANDLE h_ppid;
			process_signature_status e_signature_status;
			WCHAR wc_image_path[260];
			LARGE_INTEGER i_time_stamp;
		} process_info, * pprocess_info;
	}

	enum class RequestType : UINT64 {
		exclude = NULL,
		auth_test,//1
		unload,
		lockdown,
		unlock,
		iat_lookup
	};

	typedef struct data_pool {
		n_iat_lookup::s_iat_table iat_table;
		BOOLEAN test1;
	}data_pool;

	typedef struct s_packet {
		RequestType type;
		BOOLEAN RingRelated;
		data_pool Data;
		ULONG u_auth_cookie;
	}packet;

}