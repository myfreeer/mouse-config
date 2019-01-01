#pragma once
#ifndef MOUSE_CONFIG_MOUSE_H
#define MOUSE_CONFIG_MOUSE_H

#include "wmi.h"
#include <stdbool.h>

typedef struct _MouseConfig {
  DWORD mouseSpeed;
  DWORD mouseParams[3];
} MouseConfig;
#define DeviceIdMaxLen 256
typedef struct _MouseConfigId {
  MouseConfig cfg;
  wchar_t deviceId[DeviceIdMaxLen];
} MouseConfigId;
#define DEFAULT_CFG_ID L"Default"

void setMouse(MouseConfig cfg) {
  SystemParametersInfo(SPI_SETMOUSE, 0, cfg.mouseParams, SPIF_SENDCHANGE);
  intptr_t mouseSpeed = cfg.mouseSpeed;
  if (mouseSpeed < 1) {
    mouseSpeed = 1;
  }
  if (mouseSpeed > 20) {
    mouseSpeed = 20;
  }
  SystemParametersInfo(SPI_SETMOUSESPEED, 0, (PVOID) mouseSpeed, SPIF_SENDCHANGE);
}

void ProcessMouseConfig(MouseConfigId *cfg, size_t len) {
  if (cfg == NULL || len < 1) {
    return;
  }

  HRESULT hr = 0;
  ULONG returnedCount = 0;
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
  MouseConfigId *defaultCfg = NULL;
  bool isSet = false;
  for (size_t i = 0; i < len; ++i) {
    MouseConfigId mouseConfigId = cfg[i];
    if (mouseConfigId.deviceId[0] == L'\0') {
      continue;
    }
    if (memcmp(DEFAULT_CFG_ID, mouseConfigId.deviceId, sizeof(DEFAULT_CFG_ID)) == 0) {
      defaultCfg = cfg + i;
      continue;
    }

    returnedCount = CountMouseById(services, mouseConfigId.deviceId);
    if (returnedCount > 0) {
      isSet = true;
      setMouse(mouseConfigId.cfg);
      break;
    }
  }
  if (!isSet && defaultCfg) {
    setMouse(defaultCfg->cfg);
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


#endif //MOUSE_CONFIG_MOUSE_H
