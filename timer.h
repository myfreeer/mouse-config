#pragma once
#ifndef MOUSE_CONFIG_TIMER_H
#define MOUSE_CONFIG_TIMER_H

#include "base.h"
#include "daemon.h"
#include "mouse.h"

#define TIMER_DELAY 200

MouseConfigId *mouseCfg = NULL;
DWORD cfgCount = 0;

_Atomic UINT_PTR timer = 0;

void TimerProc(
    HWND UNUSED Arg1,
    UINT UNUSED Arg2,
    UINT_PTR UNUSED Arg3,
    DWORD UNUSED Arg4
) {
  timer = 0;
  if (mouseCfg != NULL && cfgCount > 0) {
    ProcessMouseConfig(mouseCfg, cfgCount);
  }
}

void StartTimer(void) {
  if (timer != 0) {
    KillTimer(NULL, timer);
  }
  timer = SetTimer(NULL, 0, TIMER_DELAY, TimerProc);
}

#endif //MOUSE_CONFIG_TIMER_H
