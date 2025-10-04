#pragma once
#include <ntddk.h>
#include <windef.h>
#include <Exported/Exported.hxx>
#include <Definition/crt.hxx>

class c_iat {
public:
	
	UINT32 find_proc_by_name(const char* proc_name);
	PBYTE find_proc_import(UINT32 pid, const char* import_name) const;

private:


}; extern c_iat iat;