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

#include <windows.h>
#include <stdio.h>
#include <wil/resource.h>
#include <wil/result_macros.h>
#include <fstream>
#include <iostream>
#define STR_SIZE 1024
#define BUFFER_SIZE 32'768
// enclapp.cpp 쪽 헤더파일

typedef struct ReportDataInfo {
    HRESULT hr;
    BYTE Report[BUFFER_SIZE];
    UINT32 ReportSize;
}ReportDataInfo;