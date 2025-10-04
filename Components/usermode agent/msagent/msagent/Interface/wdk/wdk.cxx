#include "wdk.hxx"
#include <Shared_Packet.hxx>
#include <iostream>

#pragma comment(lib, "Kernel32.lib")

cWizardComm::cWizardComm() noexcept :
    m_hDevice(INVALID_HANDLE_VALUE),
    m_dwLastError(0)
{
}

cWizardComm::~cWizardComm() noexcept
{
    Close();
}

void cWizardComm::Close()
{
    if (IsOpen()) {
        CloseHandle(m_hDevice);
        m_hDevice = INVALID_HANDLE_VALUE;
    }
}

void cWizardComm::PrintDeviceHandle() {
    std::cout << "Device handle -> " << this->m_hDevice << std::endl;
}

bool cWizardComm::InitializeFunction()
{
    if (IsOpen())
        return true;

    m_hDevice = CreateFileW(
        v_nltDevicePath,
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
        nullptr
    );

    if (m_hDevice == INVALID_HANDLE_VALUE || m_hDevice == nullptr) {
        m_dwLastError = GetLastError();
        return false;
    }

    m_dwLastError = 0;
    return true;
}

bool cWizardComm::SendRequest(_In_ const n_communication::packet& rfcInPacket,_Out_ n_communication::packet& rfcOutPacket,_In_ DWORD dwTimeoutMs)
{
    if (!IsOpen()) {
        m_dwLastError = ERROR_INVALID_HANDLE;
        return false;
    }
    DWORD dwBytesReturned = 0;
    HANDLE hEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!hEvent) {
        m_dwLastError = GetLastError();
        return false;
    }
    OVERLAPPED ov = {};
    ov.hEvent = hEvent;
    BOOL bRes = DeviceIoControl(m_hDevice,TELEMETRY_GLOBAL_PACKAGEHEAD,(LPVOID)&rfcInPacket,sizeof(rfcInPacket),(LPVOID)&rfcOutPacket,sizeof(rfcOutPacket),&dwBytesReturned,&ov);
    if (!bRes) {
        DWORD dwErr = GetLastError();
        if (dwErr != ERROR_IO_PENDING) {
            m_dwLastError = dwErr;
            CloseHandle(hEvent);
            return false;
        }
        DWORD dwWait = WaitForSingleObject(hEvent, dwTimeoutMs);
        if (dwWait != WAIT_OBJECT_0) {
            CancelIoEx(m_hDevice, &ov);
            GetOverlappedResult(m_hDevice, &ov, &dwBytesReturned, TRUE);
            m_dwLastError = (dwWait == WAIT_TIMEOUT) ? ERROR_TIMEOUT : GetLastError();
            CloseHandle(hEvent);
            return false;
        }
        BOOL bGot = GetOverlappedResult(m_hDevice, &ov, &dwBytesReturned, FALSE);
        if (!bGot) {
            m_dwLastError = GetLastError();
            CloseHandle(hEvent);
            return false;
        }
    }
    if (dwBytesReturned < sizeof(n_communication::packet)) {
        size_t szLeft = sizeof(n_communication::packet) - dwBytesReturned;
        if (szLeft > 0) {
            char* pBytePtr = reinterpret_cast<char*>(&rfcOutPacket);
            memset(pBytePtr + dwBytesReturned, 0, szLeft);
        }
    }
    m_dwLastError = 0;
    CloseHandle(hEvent);
    return true;
}

auto cWizardComm::HelloDexterMorgan() -> bool {
    n_communication::s_packet pIn;
    n_communication::s_packet pOut;
    pIn.type = n_communication::RequestType::auth_test;
    pIn.u_auth_cookie = n_communication::u_cookie;
    pIn.RingRelated = FALSE;
    pIn.Data.test1 = TRUE;
    bool success = SendRequest(pIn, pOut);
    if (success) {
        std::cout << "Hello Dexter -> " << static_cast<int>(pOut.Data.test1) << std::endl;
        return true;
    }
    else {
        std::cout << "byebye Kyle Butler -> " << GetLastError() << std::endl;//dexter kills trinity
        return false;
    }
    //bay harbour gooner
}


auto cWizardComm::IATLookUp(const char* import, const char* processname) -> BYTE {
    n_communication::s_packet pIn;
    n_communication::s_packet pOut;
    pIn.u_auth_cookie = n_communication::u_cookie;
    pIn.type = n_communication::RequestType::iat_lookup;
    pIn.RingRelated = FALSE;
    pIn.Data.iat_table.import = import;
    pIn.Data.iat_table.processname = processname;
    bool success = SendRequest(pIn, pOut);
    if (success) {
        std::cout << "Returned IAT -> 0x" << std::hex << pOut.Data.iat_table.returnAddress << std::endl;
        system("pause");
        return TRUE;
    }
    else {
        std::cout << "Failed to Io Control, Func -> IATLookUp " << std::endl;
        return FALSE;
    }
}