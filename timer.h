#pragma once
#ifndef MOUSE_CONFIG_TIMER_H
#define MOUSE_CONFIG_TIMER_H

#include "base.h"
#include "daemon.h"
#include "mouse.h"
#include "keyboard.h"

#define TIMER_DELAY 200

MouseConfigId *mouseCfg = NULL;
DWORD mouseCfgCount = 0;
KeyboardConfig *kbdCfg = NULL;
DWORD kbdCfgCount = 0;

_Atomic UINT_PTR timer = 0;

void TimerProc(
    HWND UNUSED Arg1,
    UINT UNUSED Arg2,
    UINT_PTR UNUSED Arg3,
    DWORD UNUSED Arg4
) {
  if ((mouseCfg != NULL && mouseCfgCount > 0) || (kbdCfg != NULL && kbdCfgCount > 0)) {
    ExecConfig(mouseCfg, mouseCfgCount, kbdCfg, kbdCfgCount);
  }
  if (timer != 0) {
    KillTimer(NULL, timer);
    timer = 0;
  }
}

void StartTimer(void) {
  if (timer != 0) {
    KillTimer(NULL, timer);
  }
  timer = SetTimer(NULL, 0, TIMER_DELAY, TimerProc);
}

#endif //MOUSE_CONFIG_TIMER_H
