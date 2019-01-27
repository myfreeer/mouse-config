#pragma once
#ifndef MOUSE_CONFIG_MOUSE_H
#define MOUSE_CONFIG_MOUSE_H

#include "wmi.h"
#include <stdbool.h>

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

void ExecMouseConfig(const MouseConfigId *cfg, const size_t len, IWbemServices *services) {
  const MouseConfigId *defaultCfg = NULL;
  bool isSet = false;
  ULONG returnedCount = 0;
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
}

#endif //MOUSE_CONFIG_MOUSE_H
