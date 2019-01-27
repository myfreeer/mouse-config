#pragma once
#ifndef MOUSE_CONFIG_INI_H
#define MOUSE_CONFIG_INI_H

#define MOUSE_CONFIG_INI_SECTION L"Mouse"
#define KEYBOARD_CONFIG_INI_SECTION L"Keyboard"

#include "mouse.h"
#include "keyboard.h"

MouseConfigId *parseMouseConfig(const WCHAR *lpFileName, DWORD *countPtr,
                                wchar_t *sectionBuf, DWORD bufSize);

KeyboardConfig *parseKeyboardConfig(const WCHAR *lpFileName, DWORD *countPtr,
                                    wchar_t *sectionBuf, DWORD bufSize);

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

#define READ_INI_FAIL { \
  *mouseCfg = NULL;     \
  *mouseCount = 0;      \
  *kbdCfg = NULL;       \
  *kbdCfgCount = 0;     \
  return;               \
}

void readIniFile(LPCWSTR lpFileName,
                 MouseConfigId **mouseCfg, DWORD *mouseCount,
                 KeyboardConfig **kbdCfg, DWORD *kbdCfgCount) {
  if (lpFileName == NULL ||
      mouseCfg == NULL || mouseCount == NULL ||
      kbdCfg == NULL || kbdCfgCount == NULL) {
    return;
  }
  HANDLE hFile = CreateFileW(lpFileName, FILE_READ_ATTRIBUTES, FILE_SHARE_READ,
                             NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (hFile == INVALID_HANDLE_VALUE) READ_INI_FAIL;
  LARGE_INTEGER fileSize = {{0}};
  if (!GetFileSizeEx(hFile, &fileSize)) READ_INI_FAIL;
  CloseHandle(hFile);
  const DWORD sectionBufSize = fileSize.LowPart;
  wchar_t *sectionBuf = calloc(fileSize.LowPart, sizeof(wchar_t));
  if (sectionBuf == NULL) READ_INI_FAIL;
  *mouseCfg = parseMouseConfig(lpFileName, mouseCount, sectionBuf, sectionBufSize);
  *kbdCfg = parseKeyboardConfig(lpFileName, kbdCfgCount, sectionBuf, sectionBufSize);
  free(sectionBuf);
}

MouseConfigId *parseMouseConfig(const WCHAR *lpFileName, DWORD *countPtr,
                                wchar_t *sectionBuf, DWORD bufSize) {
  ZeroMemory(sectionBuf, bufSize * sizeof(wchar_t));
  DWORD length = GetPrivateProfileSectionW(MOUSE_CONFIG_INI_SECTION,
                                           sectionBuf, bufSize, lpFileName);
  DWORD count = 0;
  for (DWORD i = 0; i < length; ++i) {
    if (sectionBuf[i] == L'\0') {
      ++count;
    }
  }
  *countPtr = count;
  if (count < 1) {
    return NULL;
  }

  MouseConfigId *cfg = calloc(count, sizeof(MouseConfigId));
  if (cfg == NULL) {
    *countPtr = 0;
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
  return cfg;
}

KeyboardConfig *parseKeyboardConfig(const WCHAR *lpFileName, DWORD *countPtr,
                                    wchar_t *sectionBuf, DWORD bufSize) {
  ZeroMemory(sectionBuf, bufSize * sizeof(wchar_t));
  DWORD length = GetPrivateProfileSectionW(KEYBOARD_CONFIG_INI_SECTION,
                                           sectionBuf, bufSize, lpFileName);
  DWORD count = 0;
  for (DWORD i = 0; i < length; ++i) {
    if (sectionBuf[i] == L'\0') {
      ++count;
    }
  }
  *countPtr = count;
  if (count < 1) {
    return NULL;
  }

  KeyboardConfig *cfg = calloc(count, sizeof(KeyboardConfig));
  if (cfg == NULL) {
    *countPtr = 0;
    return NULL;
  }
  DWORD index = 0;
  DWORD lastOffset = 0;
  wchar_t *deviceId = sectionBuf;
  for (DWORD i = 0; i < length; ++i) {
    if (sectionBuf[i] == L'=') {
      wcsncpy_s(cfg[index].deviceId, DeviceIdMaxLen, deviceId, i - lastOffset);
      swscanf_s(sectionBuf + i + 1, L"%u,%u,%u", &(cfg[index].numLock),
                &(cfg[index].capsLock), &(cfg[index].scrollLock));
      ++index;
    } else if (sectionBuf[i] == L'\0') {
      lastOffset = i + 1;
      deviceId = sectionBuf + lastOffset;
    }
  }
  return cfg;
}

#endif //MOUSE_CONFIG_INI_H
