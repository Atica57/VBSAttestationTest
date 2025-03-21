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
UINT8 MyEnclaveData[STR_SIZE];

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
		strcpy_s((char*)MyEnclaveData, sizeof(MyEnclaveData), "This is Enclave for attestation test");
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

void* 
CALLBACK
CallEnclaveGetAttestationReport(
    _In_ void* Context
)
{
    WCHAR String[1024];
    swprintf_s(String, ARRAYSIZE(String), L"%s\n", L"CallEnclaveAttestationReport started");
    OutputDebugStringW(String);
    //printf("CallEnclaveAttestationReport started\n");

    ReportDataInfo ReportData;
    UINT32 BufferSize = BUFFER_SIZE;

    HRESULT hr = EnclaveGetAttestationReport(MyEnclaveData, ReportData.Report, BufferSize, &ReportData.ReportSize);
	ReportData.hr = hr;
    return (void*)&ReportData;
}

void*
CALLBACK
CallEnclaveVerifyAttestationReport(
    _In_ void* ReportData
)
{
    WCHAR String[1024];
    swprintf_s(String, ARRAYSIZE(String), L"%s\n", L"CallEnclaveVerifyAttestationReport started");
    OutputDebugStringW(String);
    //printf("CallEnclaveAttestationReport started\n");

    HRESULT hr = EnclaveVerifyAttestationReport(
        ENCLAVE_TYPE_VBS, 
		((ReportDataInfo*)ReportData)->Report,
		((ReportDataInfo*)ReportData)->ReportSize
        );
    
    return (void*)hr;
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