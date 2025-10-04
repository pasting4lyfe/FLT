#pragma once

#include <windows.h>
#include <string>
#include <utility>
#include <cassert>
#include <Shared_Packet.hxx>

class cWizardComm
{
public://no default, :(
    cWizardComm() noexcept;
    ~cWizardComm() noexcept;
    bool InitializeFunction();  
    void Close();             
    void PrintDeviceHandle();
    bool HelloDexterMorgan();
    BYTE IATLookUp(const char* import, const char* processname);
    bool SendRequest(_In_ const n_communication::packet& rfcInPacket,_Out_ n_communication::packet& rfcOutPacket,_In_ DWORD dwTimeoutMs = 5000);
    DWORD GetLastErrorValue() const noexcept { return m_dwLastError; }
    bool IsOpen() const noexcept { return (m_hDevice != INVALID_HANDLE_VALUE && m_hDevice != nullptr); }
    static constexpr const wchar_t* v_nltDevicePath = L"\\\\.\\NimBusSym";
private:
    HANDLE m_hDevice;
    DWORD m_dwLastError;
};