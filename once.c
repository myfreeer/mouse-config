#include "base.h"
#include "wmi.h"
#include "mouse.h"
#include "keyboard.h"
#include "ini.h"

#ifdef NO_START_FILES
#include "mainv.h"
#endif

int main() {
  wchar_t pathBuf[MAX_PATH] = {0};
  GetModuleNameExtW(pathBuf, L".ini", MAX_PATH);

  MouseConfigId *mouseCfg = NULL;
  DWORD mouseCfgCount = 0;
  KeyboardConfig *kbdCfg = NULL;
  DWORD kbdCfgCount = 0;
  readIniFile(pathBuf, &mouseCfg, &mouseCfgCount, &kbdCfg, &kbdCfgCount);
  ExecConfig(mouseCfg, mouseCfgCount, kbdCfg, kbdCfgCount);
  if (mouseCfg != NULL) {
    free(mouseCfg);
  }
  if (kbdCfg != NULL) {
    free(kbdCfg);
  }
  return 0;
}