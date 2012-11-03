#include "StdAfx.h"
#include "D3DWWindow.hpp"

BOOL WINAPI DllMain(HINSTANCE module, DWORD reason, LPVOID reserved)
{
  D3DWWindow::SetModuleHandle(module);
  if(DLL_PROCESS_ATTACH == reason)
    DisableThreadLibraryCalls(module);
  return TRUE;
}