#ifndef __D3DWWINDOW_HPP__
#define __D3DWWINDOW_HPP__

#include <string>
#include <vector>
#include <windows.h>
#include "ComUtils.hpp"
#include "D3DW.h"

#ifndef __D3DWCONTEXT_DEFINED__
class D3DWContext;
#endif // __D3DWCONTEXT_DEFINED__

#define WM_D3DW_WINDOW_LOADED (WM_USER+1)

class COM_NO_VTABLE D3DWWindow :
  public ID3DWWindow
{
public:
  BEGIN_INTERFACE_MAP
    INTERFACE_MAP_ENTRY(ID3DWWindow)
  END_INTERFACE_MAP

  static HRESULT Create(UINT width, UINT height, INT x, INT y, HWND parent, D3DWWindow **oWindow);
  static HRESULT SetModuleHandle(HMODULE module);
  virtual ~D3DWWindow();

  STDMETHODIMP Render();
  STDMETHODIMP AddEventSink(ID3DWWindowEvents *events);
  STDMETHODIMP RemoveEventSink(ID3DWWindowEvents *events);
  STDMETHODIMP GetRenderMode(BOOL *oRenderAlways, BOOL *oPauseOnResize);
  STDMETHODIMP SetRenderMode(BOOL renderAlways, BOOL pauseOnResize);
  STDMETHODIMP GetFullscreenMode(BOOL *oFullscreen);
  STDMETHODIMP SetFullscreenMode(BOOL fullscreen);
  STDMETHODIMP GetContext(ID3DWContext **oCtx);
  STDMETHODIMP GetSize(UINT *oWidth, UINT *oHeight);
  STDMETHODIMP SetSize(UINT width, UINT height);
  STDMETHODIMP GetPosition(INT *oX, INT *oY);
  STDMETHODIMP SetPosition(INT x, INT y);
  STDMETHODIMP Show();
  STDMETHODIMP ShowDialog();
  STDMETHODIMP Hide();
  STDMETHODIMP Close();  
  STDMETHODIMP GetTitle(LPCWSTR *oTitle);
  STDMETHODIMP SetTitle(LPCWSTR title);
  STDMETHODIMP GetIcon(HICON *oIcon);
  STDMETHODIMP SetIcon(HICON icon);
  STDMETHODIMP GetMouseCapture(BOOL *oCaptured);
  STDMETHODIMP SetMouseCapture(BOOL captured);
  STDMETHODIMP GetKeyboardFocus(BOOL *oFocused);
  STDMETHODIMP SetKeyboardFocus(BOOL focused);

protected:  
  D3DWWindow();

private:
  static HMODULE _moduleHandle;
  std::wstring _className;
  HWND _hwnd;
  std::wstring _title;
  BOOL _renderAlways;
  BOOL _pauseOnResize;
  BOOL _resizing;
  BOOL _alive;
  D3DWContext *_context;
  std::vector<ID3DWWindowEvents*> _events;
  BOOL _mouseInside;

  D3DWWindow(const D3DWWindow& copy);
  HRESULT Initialize(UINT width, UINT height, INT x, INT y, HWND parent);
  void OnLoaded();
  void OnClosed();
  void OnRender();
  void OnResize();
  void OnKeyDown(DWORD vkKey);
  void OnKeyUp(DWORD vkKey);
  void OnMouseDown(INT x, INT y, D3DW_MOUSE_BUTTON button);
  void OnMouseUp(INT x, INT y, D3DW_MOUSE_BUTTON button);
  void OnMouseDoubleClick(INT x, INT y, D3DW_MOUSE_BUTTON button);
  void OnMouseEnter(INT x, INT y);
  void OnMouseMove(INT x, INT y);
  void OnMouseLeave(INT x, INT y);
  void OnMouseWheel(INT x, INT y, INT delta);  
  void OnGotCapture();
  void OnLostCapture();
  void OnGotFocus();
  void OnLostFocus();
  static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

#define __D3DWWINDOW_DEFINED__

#endif // __D3DWWINDOW_HPP__