#include "base.h"
#include "mouse.h"
#include "keyboard.h"
#include "ini.h"
#include "daemon.h"

#ifdef NO_START_FILES

void __cdecl WinMainCRTStartup() {
#else
  int WINAPI WinMain(HINSTANCE UNUSED hInstance, HINSTANCE UNUSED hPrevInstance,
                     LPSTR lpCmdLine, int UNUSED nShowCmd) {
#endif
  wchar_t pathBuf[MAX_PATH] = {0};
  GetModuleNameExtW(pathBuf, L".ini", MAX_PATH);
  readIniFile(pathBuf, &mouseCfg, &mouseCfgCount, &kbdCfg, &kbdCfgCount);
  if ((mouseCfg == NULL || mouseCfgCount < 1) &&
      (kbdCfg == NULL || kbdCfgCount < 1)) {
    ExitProcess(1);
  }
  ExecConfig(mouseCfg, mouseCfgCount, kbdCfg, kbdCfgCount);
  if (!InitWindowClass()) {
    ExitProcess(2);
  }

  HWND hWnd = CreateWindowEx(
      WS_EX_CLIENTEDGE | WS_EX_APPWINDOW,
      WND_CLASS_NAME,
      L"",
      WS_OVERLAPPEDWINDOW, // style
      CW_USEDEFAULT, 0,
      640, 480,
      NULL, NULL,
      GetModuleHandleW(NULL),
      NULL);

  if (hWnd == NULL) {
    printError(TEXT("CreateWindowEx: main app window hWnd"));
    ExitProcess(4);
  }
  MSG msg = {0};
  // Main message loop:
  while (GetMessageW(&msg, NULL, 0, 0) > 0) {
    TranslateMessage(&msg);
    DispatchMessageW(&msg);
  }

  ExitProcess((UINT) msg.wParam);

}