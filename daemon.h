#pragma once
#ifndef MOUSE_CONFIG_DAEMON_H
#define MOUSE_CONFIG_DAEMON_H

#include "mouse.h"
#include "timer.h"
#include <dbt.h>
#include <usbiodef.h>

#define WND_CLASS_NAME L"MouseConfigDaemonClass"

BOOL DoRegisterDeviceInterfaceToHwnd(
    IN GUID InterfaceClassGuid,
    IN HWND hWnd,
    OUT HDEVNOTIFY *hDeviceNotify
) {
  DEV_BROADCAST_DEVICEINTERFACE NotificationFilter = {
      .dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE),
      .dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE,
      .dbcc_classguid = InterfaceClassGuid
  };

  *hDeviceNotify = RegisterDeviceNotification(
      hWnd,                       // events recipient
      &NotificationFilter,        // type of device
      DEVICE_NOTIFY_WINDOW_HANDLE // type of recipient handle
  );

  if (NULL == *hDeviceNotify) {
    printError(TEXT("RegisterDeviceNotification"));
    return FALSE;
  }

  return TRUE;
}

INT_PTR WINAPI WinProcCallback(
    HWND hWnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam
) {
  LRESULT lRet = 1;
  static HDEVNOTIFY hDeviceNotify;
  static HWND hEditWnd;

  switch (message) {
    case WM_CREATE:
      //
      // This is the actual registration., In this example, registration
      // should happen only once, at application startup when the window
      // is created.
      //
      // If you were using a service, you would put this in your main code
      // path as part of your service initialization.
      //
      if (!DoRegisterDeviceInterfaceToHwnd(
          GUID_DEVINTERFACE_USB_DEVICE,
          hWnd,
          &hDeviceNotify)) {
        // Terminate on failure.
        printError(TEXT("DoRegisterDeviceInterfaceToHwnd"));
        ExitProcess(1);
      }

      break;

    case WM_SETFOCUS:
      break;

    case WM_SIZE:
      break;

    case WM_DEVICECHANGE: {
      // Output some messages to the window.
      switch (wParam) {
        case DBT_DEVICEARRIVAL:
        case DBT_DEVICEREMOVECOMPLETE:
          if (mouseCfg != NULL && cfgCount > 0) {
            StartTimer();
          }
          break;
        case DBT_DEVNODES_CHANGED:
          break;
        default:
          break;
      }
    }
      break;
    case WM_CLOSE:
      if (!UnregisterDeviceNotification(hDeviceNotify)) {
        printError(L"UnregisterDeviceNotification");
      }
      DestroyWindow(hWnd);
      break;

    case WM_DESTROY:
      PostQuitMessage(0);
      break;

    default:
      // Send all other messages on to the default windows handler.
      lRet = DefWindowProc(hWnd, message, wParam, lParam);
      break;
  }

  return lRet;
}

BOOL InitWindowClass() {
  WNDCLASSEXW wndClass = {
      .cbSize = sizeof(WNDCLASSEXW),
      .style = CS_OWNDC,
      .hInstance = (HINSTANCE) (GetModuleHandle(NULL)),
      .lpfnWndProc = (WNDPROC) (WinProcCallback),
      .cbClsExtra = 0,
      .cbWndExtra = 0,
      .lpszClassName = WND_CLASS_NAME,
      .lpszMenuName = NULL
  };
  if (!RegisterClassEx(&wndClass)) {
    printError(L"RegisterClassEx");
    return FALSE;
  }
  return TRUE;
}

#endif //MOUSE_CONFIG_DAEMON_H
