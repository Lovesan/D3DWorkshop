#ifndef __STDAFX_H__
#define __STDAFX_H__

#if !defined(UNICODE) && !defined(_UNICODE)
#error Direct3D Workshop requires unicode build
#endif // UNICODE || _UNICODE

#ifndef UNICODE
#define UNICODE
#endif // UNICODE

#ifndef _UNICODE
#define _UNICODE
#endif // _UNICODE

#include <cstdarg>
#include <cstddef>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cstdio>
#include <new>
#include <memory>
#include <vector>
#include <string>
#include <map>
#include <sstream>
#include <iostream>
#include <tchar.h>
#include <windows.h>
#include <windowsx.h>
#include <wincodec.h>
#include <dwmapi.h>
#include <strsafe.h>
#include <d3d11.h>
#include <d3d10_1.h>
#include <d3d10.h>
#include <dxgi.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <d2d1.h>
#include <d2derr.h>
#include "Resource.h"
#include "ComUtils.hpp"

#endif // __STDAFX_H__