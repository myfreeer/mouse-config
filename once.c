#include "base.h"
#include "wmi.h"
#include "mouse.h"
#include "ini.h"

#ifdef NO_START_FILES
#include "mainv.h"
#endif

int main() {
  wchar_t pathBuf[MAX_PATH] = {0};
  GetModuleNameExtW(pathBuf, L".ini", MAX_PATH);
  DWORD count = 0;
  MouseConfigId *cfg = readIniFile(pathBuf, &count);
  if (cfg != NULL) {
    ProcessMouseConfig(cfg, count);
    free(cfg);
  }
  return 0;
}