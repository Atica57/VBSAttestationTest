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

/*++

    This app demonstrates the life cycle of a VBS enclave including
    making function calls into the enclave.

--*/

#include "precomp.h"

HRESULT Run()
{
    if (!IsEnclaveTypeSupported(ENCLAVE_TYPE_VBS))
    {
        printf("VBS Enclave not supported\n");
        return E_NOTIMPL;
    }

    // Create the enclave
    constexpr ENCLAVE_CREATE_INFO_VBS CreateInfo
    {
        ENCLAVE_VBS_FLAG_DEBUG, // Flags
        { 0x10, 0x20, 0x30, 0x40, 0x41, 0x31, 0x21, 0x11 }, // OwnerID
    };

    PVOID Enclave = CreateEnclave(GetCurrentProcess(),
        nullptr, // Preferred base address
        0x10000000, // size
        0,
        ENCLAVE_TYPE_VBS,
        &CreateInfo,
        sizeof(ENCLAVE_CREATE_INFO_VBS),
        nullptr);
    RETURN_LAST_ERROR_IF_NULL(Enclave);

    // Ensure we terminate and delete the enclave even if something goes wrong.
    auto cleanup = wil::scope_exit([&]
        {
            // fWait = TRUE means that we wait for all threads in the enclave to terminate.
            // This is necessary because you cannot delete an enclave if it still has
            // running threads.
            LOG_IF_WIN32_BOOL_FALSE(TerminateEnclave(Enclave, TRUE));

            // Delete the enclave.
            LOG_IF_WIN32_BOOL_FALSE(DeleteEnclave(Enclave));
        });

    // Load enclave module with SEM_FAILCRITICALERRORS enabled to suppress
    // the error message dialog.
    {
        DWORD previousMode = GetThreadErrorMode();
        SetThreadErrorMode(previousMode | SEM_FAILCRITICALERRORS, nullptr);
        auto restoreErrorMode = wil::scope_exit([&]
            {
                SetThreadErrorMode(previousMode, nullptr);
            });
        RETURN_IF_WIN32_BOOL_FALSE(LoadEnclaveImageW(Enclave, L"vbsenclave.dll"));
    }

    // Initialize the enclave with one thread.
    // Once initialized, no more DLLs can be loaded into the enclave.
    ENCLAVE_INIT_INFO_VBS InitInfo{};

    InitInfo.Length = sizeof(ENCLAVE_INIT_INFO_VBS);
    InitInfo.ThreadCount = 1;

    RETURN_IF_WIN32_BOOL_FALSE(InitializeEnclave(GetCurrentProcess(),
        Enclave,
        &InitInfo,
        InitInfo.Length,
        nullptr));

    // Locate the function in the enclave.
    PENCLAVE_ROUTINE Routine = reinterpret_cast<PENCLAVE_ROUTINE>(
        GetProcAddress(reinterpret_cast<HMODULE>(Enclave), "CallEnclaveGetAttestationReport"));
    RETURN_LAST_ERROR_IF_NULL(Routine);

	// Call the function. Attestation Report test.
    ULONG_PTR Input = 0x1234;
    void* ReportPtr;

    RETURN_IF_WIN32_BOOL_FALSE(CallEnclave(Routine, reinterpret_cast<void*>(Input), TRUE /* fWaitForThread */, &ReportPtr));

    // 제대로 레포트를 받았는지 확인
    if (!ReportPtr)
    {
        printf("Unexpected result from enclave\n");
    }
    else {//check value
		ReportDataInfo* Report = reinterpret_cast<ReportDataInfo*>(ReportPtr);
        printf("HRESULT: %d\n", Report->hr);
		printf("Report Size: %d\n", Report->ReportSize);
		//printf("Report: %s\n", Report->Report);
		printf("Attestation finished.\n");
    }

    ////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////Verify the attestation report//////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////
    
    // Locate the function in the enclave.
    PENCLAVE_ROUTINE Routine2 = reinterpret_cast<PENCLAVE_ROUTINE>(
            GetProcAddress(reinterpret_cast<HMODULE>(Enclave), "CallEnclaveVerifyAttestationReport"));
    RETURN_LAST_ERROR_IF_NULL(Routine2);

    // Call the function. Attestation Report test.
    ReportDataInfo* ReportData = reinterpret_cast<ReportDataInfo*>(ReportPtr);
    void* VerifyOutput;

    RETURN_IF_WIN32_BOOL_FALSE(CallEnclave(Routine2, ReportData, TRUE /* fWaitForThread */, &VerifyOutput));

    // 제대로 verify를 했는지 확인
    HRESULT verify_hr = reinterpret_cast<HRESULT>(VerifyOutput);
	if (FAILED(verify_hr))
    {
        printf("The error is occured during verification\n");
    }
    else {//check value
		printf("HRESULT is S_OK. The attestation report is verified successfully\n");
    }

    // Destructor of "cleanup" variable will terminate and delete the enclave.

    return S_OK;
}
//
//HRESULT CryptoTest() {
//
//    return S_OK;
//}

int
main(
    [[maybe_unused]] _In_ int argc,
    [[maybe_unused]] _In_reads_(argc) char** argv
)
{
    // Print diagnostic messages to the console for developer convenience.
    wil::SetResultLoggingCallback([](wil::FailureInfo const& failure) noexcept
        {
            wchar_t message[1024];
            wil::GetFailureLogString(message, ARRAYSIZE(message), failure);
            wprintf(L"Diagnostic message: %ls\n", message);
        });

    HRESULT hr = Run();
    //HRESULT hr = CryptoTest();
    
    if (hr == HRESULT_FROM_WIN32(ERROR_INVALID_IMAGE_HASH))
    {
        wprintf(L"If you developer-signed the DLL, make sure that you have enabled test signing.\n");
    }
	else if (hr == HRESULT_FROM_WIN32(ERROR_NOT_FOUND))
	{
        wprintf(L"Element not found.\n");
		wprintf(L"Make sure that the enclave DLL is in the same directory as the app.\n");
	}
    else {
        wprintf(L"Success!\n");
    }

    return SUCCEEDED(hr) ? EXIT_SUCCESS : EXIT_FAILURE;
}
