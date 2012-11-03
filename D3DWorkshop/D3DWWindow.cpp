#include "StdAfx.h"
#include <windowsx.h>
#include "D3DWWindow.hpp"
#include "D3DWContext.hpp"

HMODULE D3DWWindow::_moduleHandle = NULL;

D3DWWindow::D3DWWindow()
{
  _hwnd = NULL;
  _renderAlways = TRUE;
  _pauseOnResize = FALSE;
  _resizing = FALSE;
  _context = NULL;
  _alive = FALSE;
  _mouseInside = FALSE;
}

D3DWWindow::~D3DWWindow()
{
  SafeDelete(_context);
  if(_hwnd)
  {
    DestroyWindow(_hwnd);
    _hwnd = NULL;
  }
  if(!_className.empty())
  {
    UnregisterClass(_className.c_str(), GetModuleHandle(NULL));
    _className.clear();
  }
}

void D3DWWindow::OnLoaded()
{
  if(!_alive)
    return;
  int nEvents = _events.size();
  for(int i = 0; i<nEvents; ++i)
    _events[i]->OnLoaded();
}

void D3DWWindow::OnClosed()
{
  if(!_alive)
    return;
  _alive = FALSE;
  int nEvents = _events.size();
  for(int i = 0; i<nEvents; ++i)
    _events[i]->OnClosed();
}

void D3DWWindow::OnRender()
{
  if(!_alive)
    return;
  if(!_resizing || !_pauseOnResize)
  {
    int nEvents = _events.size();
    for(int i = 0; i<nEvents; ++i)
      _events[i]->OnRender();
  }
  _context->Present();
}

void D3DWWindow::OnResize()
{
  if(!_alive)
    return;  
  RECT rc;
  GetClientRect(_hwnd, &rc);
  UINT width = rc.right - rc.left;
  UINT height = rc.bottom - rc.top;
  if(!(width > 1 && height > 1))
    return;
  if(!_resizing || !_pauseOnResize)  
  {
    int nEvents = _events.size();
    for(int i = 0; i<nEvents; ++i)
      _events[i]->OnResize(width, height);
  }
  OnRender();
}

void D3DWWindow::OnKeyDown(DWORD vkKey)
{
  if(!_alive)
    return;
  int nEvents = _events.size();
  for(int i = 0; i<nEvents; ++i)
    _events[i]->OnKeyDown(vkKey);
}

void D3DWWindow::OnKeyUp(DWORD vkKey)
{
  if(!_alive)
    return;
  int nEvents = _events.size();
  for(int i = 0; i<nEvents; ++i)
    _events[i]->OnKeyUp(vkKey);
}

void D3DWWindow::OnMouseDown(INT x, INT y, D3DW_MOUSE_BUTTON button)
{
  if(!_alive)
    return;
  int nEvents = _events.size();
  for(int i = 0; i<nEvents; ++i)
    _events[i]->OnMouseDown(x, y, button);
}

void D3DWWindow::OnMouseUp(INT x, INT y, D3DW_MOUSE_BUTTON button)
{
  if(!_alive)
    return;
  int nEvents = _events.size();
  for(int i = 0; i<nEvents; ++i)
    _events[i]->OnMouseUp(x, y, button);
}

void D3DWWindow::OnMouseDoubleClick(INT x, INT y, D3DW_MOUSE_BUTTON button)
{
  if(!_alive)
    return;
  int nEvents = _events.size();
  for(int i = 0; i<nEvents; ++i)
    _events[i]->OnMouseDoubleClick(x, y, button);
}

void D3DWWindow::OnMouseEnter(INT x, INT y)
{
  if(!_alive)
    return;
  int nEvents = _events.size();
  for(int i = 0; i<nEvents; ++i)
    _events[i]->OnMouseEnter(x, y);
}

void D3DWWindow::OnMouseMove(INT x, INT y)
{
  if(!_alive)
    return;
  if(!_mouseInside)
  {
    OnMouseEnter(x, y);
    _mouseInside = TRUE;
    TRACKMOUSEEVENT tme = {0};
    tme.cbSize = sizeof(tme);
    tme.dwFlags = TME_LEAVE;
    tme.dwHoverTime = HOVER_DEFAULT;
    tme.hwndTrack = _hwnd;
    TrackMouseEvent(&tme);
  }
  int nEvents = _events.size();
  for(int i = 0; i<nEvents; ++i)
    _events[i]->OnMouseMove(x, y);
}

void D3DWWindow::OnMouseLeave(INT x, INT y)
{
  if(!_alive)
    return;
  int nEvents = _events.size();
  for(int i = 0; i<nEvents; ++i)
    _events[i]->OnMouseLeave(x, y);
}

void D3DWWindow::OnMouseWheel(INT x, INT y, INT delta)
{
  if(!_alive)
    return;
  int nEvents = _events.size();
  for(int i = 0; i<nEvents; ++i)
    _events[i]->OnMouseWheel(x, y, delta);
}

void D3DWWindow::OnGotFocus()
{
  if(!_alive)
    return;
  int nEvents = _events.size();
  for(int i = 0; i<nEvents; ++i)
    _events[i]->OnGotFocus();
}

void D3DWWindow::OnLostFocus()
{
  if(!_alive)
    return;
  int nEvents = _events.size();
  for(int i = 0; i<nEvents; ++i)
    _events[i]->OnLostFocus();
}

void D3DWWindow::OnGotCapture()
{
  if(!_alive)
    return;
  POINT p;
  if(!GetCursorPos(&p))
    return;
  SetLastError(ERROR_SUCCESS);
  if(!MapWindowPoints(GetDesktopWindow(), _hwnd, &p, 1))
  {
    if(GetLastError())
      return;
  }
  int nEvents = _events.size();
  for(int i = 0; i<nEvents; ++i)
    _events[i]->OnGotCapture(p.x, p.y);
}

void D3DWWindow::OnLostCapture()
{
  if(!_alive)
    return;
  POINT p;
  if(!GetCursorPos(&p))
    return;
  SetLastError(ERROR_SUCCESS);
  if(!MapWindowPoints(GetDesktopWindow(), _hwnd, &p, 1))
  {
    if(GetLastError())
      return;
  }
  int nEvents = _events.size();
  for(int i = 0; i<nEvents; ++i)
    _events[i]->OnLostCapture(p.x, p.y);
}

STDMETHODIMP D3DWWindow::AddEventSink(ID3DWWindowEvents *events)
{
  int nEvents = _events.size();
  for(int i = 0; i<nEvents; ++i)
  {
    if(_events[i] == events)
      return S_FALSE;
  }
  _events.push_back(events);
  return S_OK;
}

STDMETHODIMP D3DWWindow::RemoveEventSink(ID3DWWindowEvents *events)
{
  int nEvents = _events.size();
  for(int i = 0; i<nEvents; ++i)
  {
    if(_events[i] == events)
    {
      _events.erase(_events.begin()+i);
      return S_OK;
    }
  }
  return S_FALSE;
}

STDMETHODIMP D3DWWindow::GetRenderMode(BOOL *oRenderAlways, BOOL *oPauseOnResize)
{
  if(oRenderAlways)
    *oRenderAlways = _renderAlways;
  if(oPauseOnResize)
    *oPauseOnResize = _pauseOnResize;
  return S_OK;
}

STDMETHODIMP D3DWWindow::SetRenderMode(BOOL renderAlways, BOOL pauseOnResize)
{
  _renderAlways = renderAlways;
  _pauseOnResize = pauseOnResize;
  return S_OK;
}

STDMETHODIMP D3DWWindow::GetContext(ID3DWContext **oCtx)
{
  if(!oCtx)
    return E_POINTER;
  *oCtx = NULL;
  _context->AddRef();
  *oCtx = _context;
  return S_OK;
}

STDMETHODIMP D3DWWindow::GetFullscreenMode(BOOL *oFullscreen)
{
  HRESULT hr;
  if(!_alive)
    return HRESULT_FROM_WIN32(ERROR_INVALID_STATE);
  if(!oFullscreen)
    return E_POINTER;
  ComPtr<IDXGISwapChain> swc;
  V_HR(_context->GetSwapChain(&swc));
  V_HR(swc->GetFullscreenState(oFullscreen, NULL));
  return S_OK;
}

STDMETHODIMP D3DWWindow::SetFullscreenMode(BOOL fullscreen)
{
  HRESULT hr;
  if(!_alive)
    return HRESULT_FROM_WIN32(ERROR_INVALID_STATE);
  ComPtr<IDXGISwapChain> swc;
  V_HR(_context->GetSwapChain(&swc));
  V_HR(swc->SetFullscreenState(fullscreen, NULL));
  return S_OK;
}

STDMETHODIMP D3DWWindow::GetSize(UINT *oWidth, UINT *oHeight)
{
  if(!_alive)
    return HRESULT_FROM_WIN32(ERROR_INVALID_STATE);
  RECT rc;
  GetClientRect(_hwnd, &rc);
  if(oWidth)
    *oWidth = rc.right - rc.left;
  if(oHeight)
    *oHeight = rc.bottom - rc.top;
  return S_OK;
}

STDMETHODIMP D3DWWindow::SetSize(UINT width, UINT height)
{
  if(!_alive)
    return HRESULT_FROM_WIN32(ERROR_INVALID_STATE);
  RECT r;
  if(!GetWindowRect(_hwnd, &r))
    return HRESULT_FROM_LAST_ERROR();
  if(!SetWindowPos(_hwnd, NULL, 0, 0, (INT)width, (INT)height, SWP_NOZORDER | SWP_NOMOVE))
    return HRESULT_FROM_LAST_ERROR();
  return S_OK;
}

STDMETHODIMP D3DWWindow::GetPosition(INT *oX, INT *oY)
{
  if(!_alive)
    return HRESULT_FROM_WIN32(ERROR_INVALID_STATE);
  HWND parent = GetParent(_hwnd);
  if(!parent)
    parent = GetDesktopWindow();
  POINT p = {0};
  SetLastError(ERROR_SUCCESS);
  if(!MapWindowPoints(_hwnd, parent, &p, 1))    
  {
    DWORD err = GetLastError();
    if(err)
      return HRESULT_FROM_WIN32(err);
  }
  if(oX)
    *oX = p.x;
  if(oY)
    *oY = p.y;
  return S_OK;
}

STDMETHODIMP D3DWWindow::SetPosition(INT x, INT y)
{
  if(!_alive)
    return HRESULT_FROM_WIN32(GetLastError());
  if(!SetWindowPos(_hwnd, NULL, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE))
    return HRESULT_FROM_LAST_ERROR();
  return S_OK;
}

STDMETHODIMP D3DWWindow::Show()
{
  if(!_alive)
    return HRESULT_FROM_WIN32(ERROR_INVALID_STATE);
  ShowWindow(_hwnd, SW_SHOW);
  UpdateWindow(_hwnd);
  return S_OK;
}

STDMETHODIMP D3DWWindow::ShowDialog()
{
  HRESULT hr;
  if(!_alive)
    return HRESULT_FROM_WIN32(ERROR_INVALID_STATE);
  V_HR(Show());
  MSG msg = {0};
  while(_alive)
  {
    if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
    else
    {
      if(_renderAlways)
        OnRender();
      else
        WaitMessage();
    }
  }
  return S_OK;
}

STDMETHODIMP D3DWWindow::Hide()
{
  if(!_alive)
    return HRESULT_FROM_WIN32(ERROR_INVALID_STATE);
  ShowWindow(_hwnd, SW_HIDE);
  return S_OK;
}

STDMETHODIMP D3DWWindow::Close()
{
  if(!_alive)
    return HRESULT_FROM_WIN32(ERROR_INVALID_STATE);
  DestroyWindow(_hwnd);
  return S_OK;
}
  
STDMETHODIMP D3DWWindow::GetTitle(LPCWSTR *oTitle)
{
  if(!_alive)
    return HRESULT_FROM_WIN32(ERROR_INVALID_STATE);
  if(!oTitle)
    return E_POINTER;
  *oTitle = NULL;
  SIZE_T size = SendMessage(_hwnd, WM_GETTEXTLENGTH, 0, 0);
  _title.clear();
  _title.resize(size+1, 0);
  if(size > 0)
  {
    SendMessage(_hwnd, WM_GETTEXT, size+1, (LPARAM)_title.data());
  }
  *oTitle = _title.c_str();
  return S_OK;
}

STDMETHODIMP D3DWWindow::SetTitle(LPCWSTR title)
{
  if(!_alive)
    return HRESULT_FROM_WIN32(ERROR_INVALID_STATE);
  if(!SetWindowText(_hwnd, title))
    return HRESULT_FROM_LAST_ERROR();
  return S_OK;
}

STDMETHODIMP D3DWWindow::GetIcon(HICON *oIcon)
{
  if(!_alive)
    return HRESULT_FROM_WIN32(ERROR_INVALID_STATE);
  if(!oIcon)
    return E_POINTER;
  *oIcon = (HICON)SendMessage(_hwnd, WM_GETICON, 1, 0);
  return S_OK;
}

STDMETHODIMP D3DWWindow::SetIcon(HICON icon)
{
  if(!_alive)
    return HRESULT_FROM_WIN32(ERROR_INVALID_STATE);
  SendMessage(_hwnd, WM_SETICON, ICON_BIG, (LPARAM)icon);
  SendMessage(_hwnd, WM_SETICON, ICON_SMALL, (LPARAM)icon);
  return S_OK;
}

STDMETHODIMP D3DWWindow::GetMouseCapture(BOOL *oCaptured)
{
  if(!_alive)
    return HRESULT_FROM_WIN32(ERROR_INVALID_STATE);
  if(!oCaptured)
    return E_POINTER;
  *oCaptured = (GetCapture() == _hwnd);
  return S_OK;
}

STDMETHODIMP D3DWWindow::SetMouseCapture(BOOL captured)
{
  if(!_alive)
    return HRESULT_FROM_WIN32(ERROR_INVALID_STATE);
  if(captured)
  {
    if(GetCapture() != _hwnd)
    {
      SetCapture(_hwnd);      
      OnGotCapture();
      return S_OK;
    }
    return S_FALSE;
  }
  else if(GetCapture() != _hwnd)
    return S_FALSE;  
  else if(!ReleaseCapture())
    return HRESULT_FROM_LAST_ERROR();
  else
    return S_OK;
}

STDMETHODIMP D3DWWindow::GetKeyboardFocus(BOOL *oFocused)
{
  if(!_alive)
    return HRESULT_FROM_WIN32(ERROR_INVALID_STATE);
  if(!oFocused)
    return E_POINTER;
  *oFocused = (GetFocus() == _hwnd);
  return S_OK;
}

STDMETHODIMP D3DWWindow::SetKeyboardFocus(BOOL focused)
{
  if(!_alive)
    return HRESULT_FROM_WIN32(ERROR_INVALID_STATE);
  if(focused)
  {
    SetFocus(_hwnd);
    return S_OK;
  }
  else if(GetFocus() != _hwnd)
    return S_FALSE;  
  else if(!SetFocus(NULL))
    return HRESULT_FROM_LAST_ERROR();
  else
    return S_OK;
}

HRESULT D3DWWindow::SetModuleHandle(HMODULE module)
{
  _moduleHandle = module;
  return S_OK;
}

HRESULT D3DWWindow::Create(UINT width, UINT height, INT x, INT y, HWND parent, D3DWWindow **oWnd)
{
  if(!oWnd)
    return E_POINTER;
  *oWnd = NULL;
  ComObject<D3DWWindow> *wnd = new (std::nothrow) ComObject<D3DWWindow>();
  if(!wnd)
    return E_OUTOFMEMORY;
  HRESULT hr = wnd->Initialize(width, height, x, y, parent);
  if(FAILED(hr))
  {
    SafeDelete(wnd);
    return hr;
  }
  wnd->AddRef();
  *oWnd = wnd;
  return S_OK;
}

HRESULT D3DWWindow::Initialize(UINT width, UINT height, INT x, INT y, HWND parent)
{
  IUnknown *comCtxToken = NULL;
  HRESULT hr = CoGetContextToken((ULONG_PTR*)&comCtxToken);
  if(FAILED(hr))
    return CO_E_NOTINITIALIZED;
  
  WNDCLASSEX wc = {0};
  wc.cbSize = sizeof(wc);  
  wc.hInstance = GetModuleHandle(NULL);
  wc.hCursor = LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW));
  wc.hIcon = LoadIcon(_moduleHandle, MAKEINTRESOURCE(IDI_DX_ICON));
  wc.lpfnWndProc = WindowProc;
  _className = L"D3DWorkshop_";
  for(int i = 0; i<10; ++i)
    _className.push_back((WCHAR)(rand() % ('Z' - 'A') + 'A'));
  wc.lpszClassName = _className.c_str();
  wc.style = CS_DBLCLKS | CS_VREDRAW | CS_HREDRAW;

  if(!RegisterClassEx(&wc))
  {
    _className.clear();
    return HRESULT_FROM_LAST_ERROR();
  }

  _hwnd = CreateWindowEx(
    WS_EX_COMPOSITED,
    wc.lpszClassName,
    L"",
    parent? WS_CHILD : WS_OVERLAPPEDWINDOW,
    x,
    y,
    width,
    height,
    parent,
    NULL,
    wc.hInstance,
    NULL);
  if(!_hwnd)
  {
    UnregisterClass(wc.lpszClassName, wc.hInstance);
    _className.clear();
    return HRESULT_FROM_LAST_ERROR();
  }

  SetWindowLongPtr(_hwnd, GWLP_USERDATA, (LONG_PTR)this);

  if(!parent)
  {
    MARGINS m = { -1 };
    DwmExtendFrameIntoClientArea(_hwnd, &m);
  }

  hr = D3DWContext::Create(_hwnd, this, &_context);
  if(FAILED(hr))
  {
    DestroyWindow(_hwnd);
    _hwnd = NULL;
    UnregisterClass(wc.lpszClassName, wc.hInstance);
    _className.clear();
    return hr;
  }

  _alive = TRUE;
  SendMessage(_hwnd, WM_D3DW_WINDOW_LOADED, 0, 0);

  return S_OK;
}

LRESULT CALLBACK D3DWWindow::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  D3DWWindow *wnd = (D3DWWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
  if(!wnd)
    return DefWindowProc(hwnd, msg, wParam, lParam);
  switch(msg)
  {
    case WM_D3DW_WINDOW_LOADED:
      wnd->OnLoaded();
      return 0;
    case WM_DESTROY:
      wnd->OnClosed();
      return 0;
    case WM_PAINT:
      {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        wnd->OnRender();
        EndPaint(hwnd, &ps);
      }
      return 0;
    case WM_SETFOCUS:
      wnd->OnGotFocus();
      return 0;
    case WM_KILLFOCUS:
      wnd->OnLostFocus();
      return 0;
    case WM_CAPTURECHANGED:
      wnd->OnLostCapture();
      return 0;
    case WM_SYSKEYDOWN:
    case WM_KEYDOWN:
      wnd->OnKeyDown((DWORD)wParam);
      return 0;
    case WM_SYSKEYUP:
    case WM_KEYUP:
      wnd->OnKeyUp((DWORD)wParam);
      return 0;
    case WM_MOUSEWHEEL:
      wnd->OnMouseWheel(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), ((SHORT)HIWORD(wParam))/120);
      return 0;
    case WM_MOUSEMOVE:
      wnd->OnMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
      return 0;
    case WM_MOUSELEAVE:
      wnd->OnMouseLeave(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
      wnd->_mouseInside = FALSE;
      return 0;
    case WM_LBUTTONDOWN:
      wnd->OnMouseDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), D3DW_MOUSE_BUTTON_LEFT);
      return 0;
    case WM_LBUTTONUP:
      wnd->OnMouseUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), D3DW_MOUSE_BUTTON_LEFT);
      return 0;
    case WM_LBUTTONDBLCLK:
      wnd->OnMouseDoubleClick(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), D3DW_MOUSE_BUTTON_LEFT);
      return 0;
    case WM_MBUTTONDOWN:
      wnd->OnMouseDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), D3DW_MOUSE_BUTTON_MIDDLE);
      return 0;
    case WM_MBUTTONUP:
      wnd->OnMouseUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), D3DW_MOUSE_BUTTON_MIDDLE);
      return 0;
    case WM_MBUTTONDBLCLK:
      wnd->OnMouseDoubleClick(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), D3DW_MOUSE_BUTTON_MIDDLE);
      return 0;
    case WM_RBUTTONDOWN:
      wnd->OnMouseDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), D3DW_MOUSE_BUTTON_RIGHT);
      return 0;
    case WM_RBUTTONUP:
      wnd->OnMouseUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), D3DW_MOUSE_BUTTON_RIGHT);
      return 0;
    case WM_RBUTTONDBLCLK:
      wnd->OnMouseDoubleClick(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), D3DW_MOUSE_BUTTON_RIGHT);
      return 0;
    case WM_XBUTTONDOWN:
      wnd->OnMouseDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam),
        (1 == HIWORD(wParam)) ?  D3DW_MOUSE_BUTTON_X1 : D3DW_MOUSE_BUTTON_X2);
      return 0;
    case WM_XBUTTONUP:
      wnd->OnMouseUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam),
        (1 == HIWORD(wParam)) ?  D3DW_MOUSE_BUTTON_X1 : D3DW_MOUSE_BUTTON_X2);
      return 0;
    case WM_XBUTTONDBLCLK:
      wnd->OnMouseDoubleClick(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam),
        (1 == HIWORD(wParam)) ?  D3DW_MOUSE_BUTTON_X1 : D3DW_MOUSE_BUTTON_X2);
      return 0;
    case WM_ENTERSIZEMOVE:
      wnd->_resizing = TRUE;
      return 0;
    case WM_EXITSIZEMOVE:
      wnd->_resizing = FALSE;
      wnd->OnResize();
      return 0;
    case WM_SIZE:
      if(SIZE_RESTORED != wParam)
        wnd->_resizing = FALSE;
      wnd->OnResize();    
      return 0;
    default:
      return DefWindowProc(hwnd, msg, wParam, lParam);
  }
}

EXTERN_C HRESULT WINAPI D3DWCreateWindow(
  UINT width, UINT height, INT x, INT y, HWND parent, ID3DWWindow **oWindow)
{
  if(!oWindow)
    return E_POINTER;
  *oWindow = NULL;
  D3DWWindow *wnd = NULL;
  HRESULT hr = D3DWWindow::Create(width, height, x, y, parent, &wnd);
  V_HR(hr);
  *oWindow = wnd;
  return S_OK;
}