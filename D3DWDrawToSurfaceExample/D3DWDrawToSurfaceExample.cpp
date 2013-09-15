#include <cstdlib>
#include <new>
#include <memory>
#include <windows.h>
#include <DirectXMath.h>
#include "../D3DWorkshop/ComUtils.hpp"
#include "../D3DWorkshop/D3DW.h"
#include "Resource.h"

using namespace DirectX;

template<class T, unsigned int align>
class aligned_object
{
public:
  aligned_object()
  {
    _p = (T*)_aligned_malloc(sizeof(T), align);
    if(!_p)
      throw std::bad_alloc();
    new (_p) T();
  }
  aligned_object(const T& copy)
  {
    _p = (T*)_aligned_malloc(sizeof(T), align);
    if(!_p)
      throw std::bad_alloc();
    *_p = copy;
  }
  ~aligned_object()
  {
    _aligned_free(_p);
  }

  inline operator T&()
  {
    return *_p;
  }

  inline T& operator=(const T& rhs)
  {
    return *_p = rhs;
  }

  inline T& get()
  {
    return *_p;
  }

private:
  T *_p;
};

class COM_NO_VTABLE D3DWDrawToSurfaceExample
  : public ID3DWWindowEvents
{
public:
  BEGIN_INTERFACE_MAP
    INTERFACE_MAP_ENTRY(ID3DWWindowEvents)
  END_INTERFACE_MAP

  static HRESULT Create(D3DWDrawToSurfaceExample **oApp)
  {
    if(!oApp)
      return E_POINTER;
    *oApp = NULL;
    D3DWDrawToSurfaceExample *app = new (std::nothrow) ComObject<D3DWDrawToSurfaceExample>();
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

  virtual ~D3DWDrawToSurfaceExample() { }

  HRESULT Run()
  {
    return _wnd->ShowDialog();
  }

  STDMETHODIMP OnLoaded() { return S_OK; }

  STDMETHODIMP OnClosed() { return S_OK; }

  STDMETHODIMP OnResize(UINT width, UINT height)
  {
    _proj = XMMatrixPerspectiveFovLH(XM_PIDIV4, width / (FLOAT)height, 0.0001f, 100.0f);
    return _ctx->SetSize(width, height);
  } 

  STDMETHODIMP OnRender()
  {
    HRESULT hr;
    FLOAT clearColor[4] = { 0.0f, 0.7f, 1.0f, 1.0f };
    V_HR(_ctx->Clear(clearColor));
    V_HR(_ctx->ClearDepthStencil());

    XMMATRIX worldViewProj;

    FLOAT angle;
    V_HR(_animation->Query(&angle));
    
    FLOAT surfaceClearColor[4] = { 0.0f, 1.0f, 0.7f, 1.0f };
    V_HR(_surface->BeginDraw());
    V_HR(_surface->Clear(surfaceClearColor));
    V_HR(_surface->ClearDepthStencil());
    V_HR(_surface->EndDraw());
        
    XMMATRIX proj = XMMatrixPerspectiveFovLH(XM_PIDIV4, 1, 0.001f, 100.0f);
    _world = XMMatrixRotationX(angle*2) * XMMatrixRotationY(angle*2);
    worldViewProj = _world.get() * _view.get() * proj;
    _cb->Update(&worldViewProj, sizeof(worldViewProj));

    V_HR(_effect->SetTexture("Texture", _tex));
    V_HR(_mesh->DrawToSurface(_effect, _surface));

    _world = XMMatrixRotationX(-angle) * XMMatrixRotationY(-angle);
    worldViewProj = _world.get() * _view.get() * _proj.get();
    _cb->Update(&worldViewProj, sizeof(worldViewProj));

    V_HR(_effect->SetTexture("Texture", _surfaceTex));
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
  D3DWDrawToSurfaceExample() { }
  
private:
  struct Vertex
  {
    XMFLOAT3 pos;
    XMFLOAT2 tex;
  };

  ComPtr<ID3DWWindow> _wnd;
  ComPtr<ID3DWContext> _ctx;
  ComPtr<ID3DWEffect> _effect;
  ComPtr<ID3DWMesh> _mesh;
  ComPtr<ID3DWConstantBuffer> _cb;
  ComPtr<ID3DWSampler> _sampler;
  ComPtr<ID3DWTexture> _surfaceTex;
  ComPtr<ID3DWSurface> _surface;
  ComPtr<ID3DWTexture> _tex;
  ComPtr<ID3DWFloatAnimation> _animation;
  aligned_object<XMMATRIX, 16> _world;
  aligned_object<XMMATRIX, 16> _view;
  aligned_object<XMMATRIX, 16> _proj;

  D3DWDrawToSurfaceExample(const D3DWDrawToSurfaceExample& rhs);

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

    V_HR(_wnd->SetTitle(L"D3DW Draw To Surface example"));

    V_HR(_wnd->GetContext(&_ctx));

    Vertex vertices[] =
    {
      { XMFLOAT3(-1, -1, -1), XMFLOAT2(0, 1) },
      { XMFLOAT3(-1,  1, -1), XMFLOAT2(0, 0) },
      { XMFLOAT3( 1,  1, -1), XMFLOAT2(1, 0) },
      { XMFLOAT3( 1, -1, -1), XMFLOAT2(1, 1) },

      { XMFLOAT3( 1, -1,  1), XMFLOAT2(0, 1) },
      { XMFLOAT3( 1,  1,  1), XMFLOAT2(0, 0) },
      { XMFLOAT3(-1,  1,  1), XMFLOAT2(1, 0) },
      { XMFLOAT3(-1, -1,  1), XMFLOAT2(1, 1) },

      { XMFLOAT3(-1,  1, -1), XMFLOAT2(0, 1) },
      { XMFLOAT3(-1,  1,  1), XMFLOAT2(0, 0) },
      { XMFLOAT3( 1,  1,  1), XMFLOAT2(1, 0) },
      { XMFLOAT3( 1,  1, -1), XMFLOAT2(1, 1) },

      { XMFLOAT3(-1, -1,  1), XMFLOAT2(0, 1) },
      { XMFLOAT3(-1, -1, -1), XMFLOAT2(0, 0) },
      { XMFLOAT3( 1, -1, -1), XMFLOAT2(1, 0) },
      { XMFLOAT3( 1, -1,  1), XMFLOAT2(1, 1) },

      { XMFLOAT3(-1, -1,  1), XMFLOAT2(0, 1) },
      { XMFLOAT3(-1,  1,  1), XMFLOAT2(0, 0) },
      { XMFLOAT3(-1,  1, -1), XMFLOAT2(1, 0) },
      { XMFLOAT3(-1, -1, -1), XMFLOAT2(1, 1) },

      { XMFLOAT3( 1, -1, -1), XMFLOAT2(0, 1) },
      { XMFLOAT3( 1,  1, -1), XMFLOAT2(0, 0) },
      { XMFLOAT3( 1,  1,  1), XMFLOAT2(1, 0) },
      { XMFLOAT3( 1, -1,  1), XMFLOAT2(1, 1) },
    };

    UINT32 indices[] =
    {
      0, 1, 2,
      2, 3, 0,

      4, 5, 6,
      6, 7, 4,

      8, 9, 10,
      10, 11, 8,

      12, 13, 14,
      14, 15, 12,

      16, 17, 18,
      18, 19, 16,

      20, 21, 22,
      22, 23, 20
    };

    V_HR(_ctx->CreateMesh(
      vertices, sizeof(Vertex), ARRAYSIZE(vertices), indices, ARRAYSIZE(indices), D3DW_TOPOLOGY_TRIANGLE, &_mesh));

    V_HR(_ctx->CreateEffectFromResource(
      GetModuleHandle(NULL), MAKEINTRESOURCE(IDS_SHADER), MAKEINTRESOURCE(RT_SHADER), "VS", NULL, "PS", &_effect));

    V_HR(_ctx->CreateTexture(400, 400, &_surfaceTex));
    V_HR(_surfaceTex->GetSurface(&_surface));

	V_HR(_ctx->CreateTextureFromResource(
		GetModuleHandle(NULL),
		MAKEINTRESOURCE(IDS_TEXTURE),
		MAKEINTRESOURCE(RT_TEXTURE),
		FALSE,
		&_tex));


    V_HR(_ctx->CreateConstantBuffer(sizeof(XMMATRIX), &_cb));

    V_HR(_ctx->CreateSampler(16, D3DW_TEXTURE_MODE_WRAP, NULL, &_sampler));

    V_HR(_effect->SetConstantBuffer("Matrices", _cb));

    V_HR(_effect->SetTexture("Texture", _tex));

    V_HR(_effect->SetSampler("Sampler", _sampler));
    
    _world = XMMatrixIdentity();

    XMVECTOR eye = XMVectorSet(-3, 3, -3, 1),
             at = XMVectorSet(0, 0, 0, 1),
             up = XMVectorSet(0, 1, 0, 1);
    _view = XMMatrixLookAtLH(eye, at, up);

    UINT width, height;
    V_HR(_ctx->GetSize(&width, &height));

    _proj = XMMatrixPerspectiveFovLH(XM_PIDIV4, width / (FLOAT)height, 0.0001f, 100.0f);

    V_HR(_ctx->CreateFloatAnimation(0, XM_2PI, 15, TRUE, FALSE, &_animation));
    V_HR(_animation->Start());

    V_HR(_wnd->SetRenderMode(TRUE, TRUE));

    V_HR(_wnd->AddEventSink(this));
    
    return S_OK;
  }
};

INT WINAPI WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdLine, INT cmdShow)
{
  HRESULT hr;
  hr = OleInitialize(NULL);
  if(FAILED(hr)) return (INT)hr;
  {
    ComPtr<D3DWDrawToSurfaceExample> app;
    hr = D3DWDrawToSurfaceExample::Create(&app);
    if(FAILED(hr)) goto exit;
    hr = app->Run();
  }
exit:
  OleUninitialize();
  return (INT)hr;
}