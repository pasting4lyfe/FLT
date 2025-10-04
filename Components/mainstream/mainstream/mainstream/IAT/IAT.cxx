#include <IAT/IAT.hxx>

#include <ntimage.h>

UINT32 c_iat::find_proc_by_name(const char* proc_name) {
	PEPROCESS process;
	PUCHAR process_name;
	UINT32 pid = 0;
	if (!proc_name)
		return 0;
	for (HANDLE i = (HANDLE)4; (SIZE_T)i < 0x100000; i = (HANDLE)((SIZE_T)i + 4)) {
		if (NT_SUCCESS(n_export_table::PsLookupProcessByProcessId(i, &process))) {
			process_name = (PUCHAR)n_export_table::PsGetProcessImageFileName(process);
			if (process_name) {
				if (crt::strcmp((const char*)process_name, proc_name) == 0) {
					pid = (UINT32)(SIZE_T)i;
					ObDereferenceObject(process);
					break;
				}
			}
			ObDereferenceObject(process);
		}
	}
	return pid;
}

PBYTE c_iat::find_proc_import(UINT32 pid, const char* import_name) const
{
    constexpr size_t MAX_IMPORTS = 256;
    constexpr ULONG  TAG = 'PMO\0';

    PEPROCESS eprocess = nullptr;
    PBYTE     result = nullptr;

    if (!NT_SUCCESS(n_export_table::PsLookupProcessByProcessId((HANDLE)pid, &eprocess)) || !eprocess)
        return nullptr;

    auto cleanup = [&](PVOID pool = nullptr)
        {
            if (pool) ExFreePoolWithTag(pool, TAG);
            if (eprocess) ObDereferenceObject(eprocess);
        };

    PBYTE base = reinterpret_cast<PBYTE>(n_export_table::PsGetProcessSectionBaseAddress(eprocess));
    if (!base) return cleanup(), nullptr;

    SIZE_T bytes = 0;
    auto read = [&](PVOID dst, PVOID src, SIZE_T size) -> bool
        {
            bytes = 0;
            return NT_SUCCESS(n_export_table::MmCopyVirtualMemory(
                eprocess, src, PsGetCurrentProcess(), dst, size, KernelMode, &bytes
            )) && bytes == size;
        };

    IMAGE_DOS_HEADER dos{};
    if (!read(&dos, base, sizeof(dos))) return cleanup(), nullptr;

    IMAGE_NT_HEADERS64 nt{};
    if (!read(&nt, base + dos.e_lfanew, sizeof(nt))) return cleanup(), nullptr;

    if (dos.e_magic != IMAGE_DOS_SIGNATURE ||
        nt.Signature != IMAGE_NT_SIGNATURE ||
        nt.OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC)
        return cleanup(), nullptr;

    DWORD import_rva = nt.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
    if (!import_rva) return cleanup(), nullptr;

    auto* imports = (IMAGE_IMPORT_DESCRIPTOR*)
        ExAllocatePoolWithTag(NonPagedPool, sizeof(IMAGE_IMPORT_DESCRIPTOR) * MAX_IMPORTS, TAG);
    if (!imports) return cleanup(), nullptr;

    if (!read(imports, base + import_rva, sizeof(IMAGE_IMPORT_DESCRIPTOR) * MAX_IMPORTS))
        return cleanup(imports), nullptr;

    for (size_t i = 0; i < MAX_IMPORTS && imports[i].Name; ++i)
    {
        auto& imp = imports[i];

        char dll_name[256]{};
        read(dll_name, base + imp.Name, sizeof(dll_name) - 1);

        for (DWORD idx = 0;; ++idx)
        {
            IMAGE_THUNK_DATA64 thunk{}, orig{};
            if (!read(&orig, base + imp.OriginalFirstThunk + idx * sizeof(orig), sizeof(orig)) ||
                !orig.u1.AddressOfData)
                break;

            if (IMAGE_SNAP_BY_ORDINAL64(orig.u1.Ordinal))
                continue;

            char func[256]{};
            read(func, base + orig.u1.AddressOfData + FIELD_OFFSET(IMAGE_IMPORT_BY_NAME, Name), sizeof(func) - 1);

            if (crt::strcmp(func, import_name) == 0)
            {
                if (read(&thunk, base + imp.FirstThunk + idx * sizeof(thunk), sizeof(thunk)))
                    result = reinterpret_cast<PBYTE>(thunk.u1.Function);
                cleanup(imports);
                return result;
            }
        }
    }

    cleanup(imports);
    return nullptr;
}

c_iat iat;