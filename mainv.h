// Small alternative startfile for main()
// Build with gcc perms:
// -nostartfiles
#pragma once
#ifndef STARTFILE_MAIN_H
#define STARTFILE_MAIN_H

#include <windows.h>

// workaround undefined reference to `_pei386_runtime_relocator'
#define main() _main()

int main();

int __cdecl mainCRTStartup() {
  const int exitCode = main();

  ExitProcess(exitCode);
  return exitCode;
}

#endif
