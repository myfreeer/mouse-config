#pragma once
#ifndef MOUSE_CONFIG_WMI_H
#define MOUSE_CONFIG_WMI_H

#include "base.h"
#include <stdio.h>
#include <wbemidl.h>

#define QUERY_BUFFER_SIZE 512

ULONG CountMouseById(IWbemServices *services, LPCWSTR deviceId) {
  HRESULT hr = 0;
  ULONG returnedCount = 0;
  IEnumWbemClassObject *results = NULL;

  BSTR language = SysAllocString(L"WQL");
  wchar_t queryBuffer[QUERY_BUFFER_SIZE] = {0};

  swprintf_s(queryBuffer, QUERY_BUFFER_SIZE,
             L"SELECT * FROM Win32_PointingDevice WHERE DeviceID like '%%%ls%%'", deviceId);

  BSTR query = SysAllocString(queryBuffer);

  // issue a WMI query
  COM_CALL(hr, services, ExecQuery, language, query, WBEM_FLAG_BIDIRECTIONAL, NULL, &results);
  // list the query results
  if (results == NULL) {
    goto Exit;
  }

  IWbemClassObject *result = NULL;

  // enumerate the retrieved objects
  COM_CALL(hr, results, Next, WBEM_INFINITE, 1, &result, &returnedCount);

  if (result != NULL) {
    COM_CALL(hr, result, Release);
  }

  Exit:
  SysFreeString(query);
  return returnedCount;
}

#endif //MOUSE_CONFIG_WMI_H
