#pragma once
#include <ntddk.h>
typedef struct _WIN_CERTIFICATE {
    DWORD dwLength;
    WORD  wRevision;
    WORD  wCertificateType;
    BYTE  bCertificate[ANYSIZE_ARRAY];
} WIN_CERTIFICATE, * PWIN_CERTIFICATE;

#define WIN_CERT_REVISION_2_0 0x0200
#define WIN_CERT_TYPE_X509     0x0001
#define WIN_CERT_TYPE_PKCS_SIGNED_DATA 0x0002
#ifndef IMAGE_DIRECTORY_ENTRY_SECURITY
#define IMAGE_DIRECTORY_ENTRY_SECURITY 4
#endif