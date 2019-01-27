#pragma once

#ifndef MOUSE_CONFIG_KEYBOARD_H
#define MOUSE_CONFIG_KEYBOARD_H

#include "wmi.h"
#include <stdbool.h>

void SetKeyState(const BOOL bState, const WORD keyCode) {
  INPUT input = {0};
  SHORT keyState = GetKeyState(keyCode);
  if ((bState && !(keyState & 1)) ||
      (!bState && (keyState & 1))) {
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = keyCode;
    SendInput(1, &input, sizeof(input));

    input.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &input, sizeof(input));
  }
}

void setKeyboard(const KeyboardConfig *cfg) {
  SetKeyState(cfg->numLock, VK_NUMLOCK);
  SetKeyState(cfg->capsLock, VK_CAPITAL);
  SetKeyState(cfg->scrollLock, VK_SCROLL);
}

void ExecKeyboardConfig(const KeyboardConfig *cfg, const size_t len, IWbemServices *services) {
  const KeyboardConfig *defaultCfg = NULL;
  bool isSet = false;
  ULONG returnedCount = 0;
  for (size_t i = 0; i < len; ++i) {
    KeyboardConfig keyboardConfig = cfg[i];
    if (keyboardConfig.deviceId[0] == L'\0') {
      continue;
    }
    if (memcmp(DEFAULT_CFG_ID, keyboardConfig.deviceId, sizeof(DEFAULT_CFG_ID)) == 0) {
      defaultCfg = cfg + i;
      continue;
    }

    returnedCount = CountKeyboardById(services, keyboardConfig.deviceId);
    if (returnedCount > 0) {
      isSet = true;
      setKeyboard(&keyboardConfig);
      break;
    }
  }
  if (!isSet && defaultCfg) {
    setKeyboard(defaultCfg);
  }
}

#endif //MOUSE_CONFIG_KEYBOARD_H
