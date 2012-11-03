#include <cstdlib>
#include <new>
#include "../D3DWorkshop/ComUtils.hpp"
#include "../D3DWorkshop/D3DW.h"

class COM_NO_VTABLE D3DWTriangleExample
  : public ID3DWWindowEvents
{
public:
  BEGIN_INTERFACE_MAP
    INTERFACE_MAP_ENTRY(ID3DWWindowEvents)
  END_INTERFACE_MAP

  static HRESULT Create(D3DWTriangleExample **oApp)
  {
    if(!oApp)
      return E_POINTER;
    *oApp = NULL;
    D3DWTriangleExample *app = new (std::nothrow) ComObject<D3DWTriangleExample>();
    if(!app)
      return E_OUTOFMEMORY;
    HRESULT hr = app->Initialize();
    if(FAILED(hr))
    {
      SafeDelete(app);
      return hr;
    }
    app->AddRef();
    *oApp = app;
    return S_OK;
  }

  virtual ~D3DWTriangleExample() { }

  HRESULT Run()
  {
    return _wnd->ShowDialog();
  }

  STDMETHODIMP OnLoaded() { return S_OK; }

  STDMETHODIMP OnClosed() { return S_OK; }

  STDMETHODIMP OnResize(UINT width, UINT height)
  {
    return _ctx->SetSize(width, height);
  } 

  STDMETHODIMP OnRender()
  {
    HRESULT hr;
    FLOAT clearColor[4] = { 0.0f, 0.7f, 1.0f, 1.0f };
    V_HR(_ctx->Clear(clearColor));
    V_HR(_ctx->ClearDepthStencil());
    V_HR(_mesh->Draw(_effect));
    return S_OK;
  }

  STDMETHODIMP OnKeyDown(DWORD vkKey) { return S_OK; }

  STDMETHODIMP OnKeyUp(DWORD vkKey)
  {
    HRESULT hr;
    if(VK_SPACE == vkKey)
    {
      BOOL fullscreen;
      V_HR(_wnd->GetFullscreenMode(&fullscreen));
      fullscreen = !fullscreen;
      V_HR(_wnd->SetFullscreenMode(fullscreen));
    }
    else if(VK_ESCAPE == vkKey)
    {
      V_HR(_wnd->Close());
    }
    return S_OK;
  }

  STDMETHODIMP OnMouseDown(INT x, INT y, D3DW_MOUSE_BUTTON button) { return S_OK; }

  STDMETHODIMP OnMouseUp(INT x, INT y, D3DW_MOUSE_BUTTON button) { return S_OK; }

  STDMETHODIMP OnMouseDoubleClick(INT x, INT y, D3DW_MOUSE_BUTTON button) { return S_OK; }

  STDMETHODIMP OnMouseEnter(INT x, INT y) { return S_OK; }

  STDMETHODIMP OnMouseMove(INT x, INT y) { return S_OK; }

  STDMETHODIMP OnMouseLeave(INT x, INT y) { return S_OK; }

  STDMETHODIMP OnMouseWheel(INT x, INT y, INT delta) { return S_OK; }

  STDMETHODIMP OnGotCapture(INT x, INT y) { return S_OK; }

  STDMETHODIMP OnLostCapture(INT x, INT y) { return S_OK; } 

  STDMETHODIMP OnGotFocus() { return S_OK; }

  STDMETHODIMP OnLostFocus() { return S_OK; } 

protected:
  D3DWTriangleExample() { }
  
private:
  static const LPCSTR _shaders;

  ComPtr<ID3DWWindow> _wnd;
  ComPtr<ID3DWContext> _ctx;
  ComPtr<ID3DWEffect> _effect;
  ComPtr<ID3DWMesh> _mesh;

  D3DWTriangleExample(const D3DWTriangleExample& rhs);

  HRESULT Initialize()
  {
    HRESULT hr;

    V_HR(D3DWCreateWindow(
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      NULL,
      &_wnd));

    V_HR(_wnd->SetTitle(L"D3DW triangle example"));

    V_HR(_wnd->GetContext(&_ctx));

    V_HR(_ctx->CreateEffectFromMemory(
      _shaders, strlen(_shaders), "VS", NULL, "PS", &_effect));

    FLOAT vertices[] =
    {
      /*  Position           Color         */
      -0.7f, -0.7f, 0.0f,   1.0f, 0.0f, 0.0f,
       0.0f,  0.7f, 0.0f,   0.0f, 1.0f, 0.0f,
       0.7f, -0.7f, 0.0f,   0.0f, 0.0f, 1.0f,
    };
    UINT32 indices[] =
    {
      0, 1, 2,
    };
    V_HR(_ctx->CreateMesh(
      vertices, sizeof(FLOAT)*6, 3, indices, 3, D3DW_TOPOLOGY_TRIANGLE, FALSE, &_mesh));

    V_HR(_wnd->SetRenderMode(FALSE, TRUE));

    V_HR(_wnd->AddEventSink(this));
    
    return S_OK;
  }
};

const LPCSTR D3DWTriangleExample::_shaders =
    "void VS(float3 pos : POSITION, float3 color : COLOR,"
    "        out float4 oPos : SV_POSITION, out float3 oColor : COLOR)"
    "{ oPos = float4(pos, 1); oColor = color;  }"
    "float4 PS(float4 pos : SV_POSITION, float3 color : COLOR) : SV_TARGET"
    "{ return float4(color, 1); }";

INT WINAPI WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdLine, INT cmdShow)
{
  HRESULT hr;
  hr = OleInitialize(NULL);
  if(FAILED(hr)) return (INT)hr;
  {
    ComPtr<D3DWTriangleExample> app;
    hr = D3DWTriangleExample::Create(&app);
    if(FAILED(hr)) goto exit;
    hr = app->Run();
  }
exit:
  OleUninitialize();
  return (INT)hr;
}