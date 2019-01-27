#pragma once
#ifndef MOUSE_CONFIG_WMI_H
#define MOUSE_CONFIG_WMI_H

#include "base.h"
#include <stdio.h>
#include <wbemidl.h>

#define QUERY_BUFFER_SIZE 512
#define MOUSE_DEVICE L"Win32_PointingDevice"
#define KEYBOARD_DEVICE L"Win32_Keyboard"

ULONG CountDeviceById(IWbemServices *services, LPCWSTR device, LPCWSTR deviceId) {
  HRESULT hr = 0;
  ULONG returnedCount = 0;
  IEnumWbemClassObject *results = NULL;

  BSTR language = SysAllocString(L"WQL");
  wchar_t queryBuffer[QUERY_BUFFER_SIZE] = {0};

  swprintf_s(queryBuffer, QUERY_BUFFER_SIZE,
             L"SELECT * FROM %ls WHERE DeviceID like '%%%ls%%'", device, deviceId);

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

ULONG CountMouseById(IWbemServices *services, LPCWSTR deviceId) {
  return CountDeviceById(services, MOUSE_DEVICE, deviceId);
}

ULONG CountKeyboardById(IWbemServices *services, LPCWSTR deviceId) {
  return CountDeviceById(services, KEYBOARD_DEVICE, deviceId);
}

#define DeviceIdMaxLen 256
#define DEFAULT_CFG_ID L"Default"

typedef struct _MouseConfig {
  DWORD mouseSpeed;
  DWORD mouseParams[3];
} MouseConfig;

typedef struct _MouseConfigId {
  MouseConfig cfg;
  wchar_t deviceId[DeviceIdMaxLen];
} MouseConfigId;

typedef struct _KeyboardConfig {
  BOOL numLock;
  BOOL capsLock;
  BOOL scrollLock;
  wchar_t deviceId[DeviceIdMaxLen];
} KeyboardConfig;


void ExecMouseConfig(const MouseConfigId *cfg, size_t len, IWbemServices *services);

void ExecKeyboardConfig(const KeyboardConfig *cfg, size_t len, IWbemServices *services);

void ExecConfig(MouseConfigId *mouseCfg, size_t mouseCfgLen,
                const KeyboardConfig *kbdCfg, size_t kbdCfgLen) {
  if ((mouseCfg == NULL || mouseCfgLen < 1) && (kbdCfg == NULL || kbdCfgLen < 1)) {
    return;
  }

  HRESULT hr = 0;
  IWbemLocator *locator = NULL;
  IWbemServices *services = NULL;

  // BSTR strings we'll use (http://msdn.microsoft.com/en-us/library/ms221069.aspx)
  BSTR resource = SysAllocString(L"ROOT\\CIMV2");
  BSTR language = SysAllocString(L"WQL");

  // initialize COM
  CoInitializeEx(0, COINIT_MULTITHREADED);
  CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT,
                       RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);

  // connect to WMI
  hr = CoCreateInstance(&CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER,
                        &IID_IWbemLocator, (LPVOID *) &locator);
  EXIT_ON_ERROR(hr, CoCreateInstance);
  COM_CALL(hr, locator, ConnectServer, resource, NULL, NULL, NULL, 0, NULL, NULL, &services);
  if (services == NULL) {
    goto Exit;
  }
  if (mouseCfg != NULL && mouseCfgLen > 0) {
    ExecMouseConfig(mouseCfg, mouseCfgLen, services);
  }
  if (kbdCfg != NULL && kbdCfgLen > 0) {
    ExecKeyboardConfig(kbdCfg, kbdCfgLen, services);
  }

  COM_CALL(hr, services, Release);
  Exit:
  if (locator != NULL) {
    locator->lpVtbl->Release(locator);
  }
  SysFreeString(language);
  SysFreeString(resource);
  // unwind everything else we've allocated
  CoUninitialize();
}

#endif //MOUSE_CONFIG_WMI_H
