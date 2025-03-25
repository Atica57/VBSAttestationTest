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

#pragma once

#include <winenclave.h>
#include <bcrypt.h>
#include <wchar.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#define STR_SIZE 1024
#define BUFFER_SIZE 32'768
// enclave.c 쪽 헤더파일

typedef struct ReportDataInfo {
    HRESULT hr;
    BYTE Report[BUFFER_SIZE];
    UINT32 ReportSize;
}ReportDataInfo;
