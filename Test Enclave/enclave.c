//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

/*
    Defines the code that will be loaded into the VBS enclave.
--*/

#include "precomp.h"

typedef struct ReportData {
    PVOID Report;
    UINT32 ReportSize;
}ReportData;

// VBS enclave configuration

const IMAGE_ENCLAVE_CONFIG __enclave_config = {
    sizeof(IMAGE_ENCLAVE_CONFIG),
    IMAGE_ENCLAVE_MINIMUM_CONFIG_SIZE,
    IMAGE_ENCLAVE_POLICY_DEBUGGABLE,    // DO NOT SHIP DEBUGGABLE ENCLAVES TO PRODUCTION
    0,
    0,
    0,
    { 0xFE, 0xFE },    // family id
    { 0x01, 0x01 },    // image id
    0,                 // version
    0,                 // SVN
    0x10000000,        // size
    16,                // number of threads
    IMAGE_ENCLAVE_FLAG_PRIMARY_IMAGE
};

ULONG InitialCookie;
UINT8 MyEnclaveData[1024];

BOOL
DllMain(
    _In_ HINSTANCE hinstDLL,
    _In_ DWORD dwReason,
    _In_ LPVOID lpvReserved
)
{
    UNREFERENCED_PARAMETER(hinstDLL);
    UNREFERENCED_PARAMETER(lpvReserved);

    if (dwReason == DLL_PROCESS_ATTACH) {
        //dll에서 console창에 출력하기 위한 설정
        //AllocConsole(); 
        //FILE* fp;
        //freopen_s(&fp, "CONOUT$", "w", stdout);
        //printf("Console initialized in DLL\n");

        InitialCookie = 0xDADAF00D;
		strcpy_s((char*)MyEnclaveData, sizeof(MyEnclaveData), "This is Enclave for crypto test and attestation test");
    }

    return TRUE;
}

void*
CALLBACK
CallEnclaveTest(
    _In_ void* Context
)
{
    WCHAR String[32];
    swprintf_s(String, ARRAYSIZE(String), L"%s\n", L"CallEnclaveTest started");
    OutputDebugStringW(String);

    return (void*)((ULONG_PTR)(Context) ^ InitialCookie);
}
//
//void*
//CALLBACK
//CallEnclaveCryptoTest(
//
//)
//{
//    WCHAR String[1024];
//    swprintf_s(String, ARRAYSIZE(String), L"%s\n", L"CallEnclaveCryptoTest started");
//    OutputDebugStringW(String);
//
//
//}

void* 
CALLBACK
CallEnclaveAttestationReport(
    _In_ void* Context
)
{
    WCHAR String[1024];
    swprintf_s(String, ARRAYSIZE(String), L"%s\n", L"CallEnclaveAttestationReport started");
    OutputDebugStringW(String);
    //printf("CallEnclaveAttestationReport started\n");

    PVOID Report = NULL;
    UINT32 BufferSize = 0;
    UINT32* OutputSize = NULL;
    ReportData* ReportDT = NULL;

    HRESULT res = EnclaveGetAttestationReport(MyEnclaveData, Report, BufferSize, OutputSize);
    if (res != S_OK) {
        //printf("CallEnclaveAttestationReport failed. result is not S_OK\n");
        return res;
    }
    else {
        //printf("CallEnclaveAttestationReport have return values\n");
		ReportDT->Report = Report;
		ReportDT->ReportSize = *OutputSize;
    }
    return (void*)ReportDT;
}

void*
CALLBACK
CallEnclaveGetEnclaveInformationTest(
    //no input!
)
{
    WCHAR String[1024];
    swprintf_s(String, ARRAYSIZE(String), L"%s\n", L"CallEnclaveAttestationReport started");
    OutputDebugStringW(String);

    UINT32 InformationSize = 176;
    ENCLAVE_INFORMATION* EnclaveInformation = NULL;
    HRESULT res = EnclaveGetEnclaveInformation(InformationSize, EnclaveInformation);
	if (res != S_OK) {
		return res;
	}
    return (void*)EnclaveInformation;
}