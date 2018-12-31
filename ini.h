#pragma once
#ifndef MOUSE_CONFIG_INI_H
#define MOUSE_CONFIG_INI_H


#include "mouse.h"

// get path to file with the same path and name of module, with different extension
BOOL GetModuleNameExtW(LPWSTR lpFileName, LPCWSTR lpExt, DWORD nSize) {
  DWORD ret = GetModuleFileNameW(NULL, lpFileName, nSize);
  if (ret == 0) return (BOOL) ret;
  WCHAR *szDirEnd = wcsrchr(lpFileName, L'\\');
  if (!szDirEnd) return 0;
  WCHAR *szFileExt = wcsrchr(szDirEnd, L'.');
  if (szFileExt) {
    return wcscpy_s(szFileExt, nSize + lpFileName - szFileExt, lpExt) == 0;
  } else {
    return wcscat_s(lpFileName, nSize, lpExt) == 0;
  }
}

MouseConfigId *readIniFile(LPWSTR lpFileName, DWORD *countPtr) {
  if (lpFileName == NULL || countPtr == NULL) {
    return NULL;
  }
  HANDLE hFile = CreateFileW(lpFileName, FILE_READ_ATTRIBUTES, FILE_SHARE_READ,
                             NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (hFile == INVALID_HANDLE_VALUE) {
    return NULL;
  }
  LARGE_INTEGER fileSize = {0};
  if (!GetFileSizeEx(hFile, &fileSize)) {
    return NULL;
  }
  wchar_t *sectionBuf = calloc(fileSize.LowPart, sizeof(wchar_t));
  if (sectionBuf == NULL) {
    return NULL;
  }
  DWORD length = GetPrivateProfileSectionW(L"Mouse", sectionBuf, fileSize.LowPart, lpFileName);
  DWORD count = 0;
  for (DWORD i = 0; i < length; ++i) {
    if (sectionBuf[i] == L'\0') {
      ++count;
    }
  }
  *countPtr = count;
  if (count < 1) {
    free(sectionBuf);
    return NULL;
  }

  MouseConfigId *cfg = calloc(count, sizeof(MouseConfigId));
  if (cfg == NULL) {
    free(sectionBuf);
    return NULL;
  }
  DWORD index = 0;
  DWORD lastOffset = 0;
  wchar_t *deviceId = sectionBuf;
  for (DWORD i = 0; i < length; ++i) {
    if (sectionBuf[i] == L'=') {
      wcsncpy_s(cfg[index].deviceId, DeviceIdMaxLen, deviceId, i - lastOffset);
      DWORD *mouseParams = cfg[index].cfg.mouseParams;
      swscanf_s(sectionBuf + i + 1, L"%u,%u,%u,%u", &cfg[index].cfg.mouseSpeed,
                mouseParams, mouseParams + 1, mouseParams + 2);
      ++index;
    } else if (sectionBuf[i] == L'\0') {
      lastOffset = i + 1;
      deviceId = sectionBuf + lastOffset;
    }
  }
  free(sectionBuf);
  return cfg;
}

#endif //MOUSE_CONFIG_INI_H
