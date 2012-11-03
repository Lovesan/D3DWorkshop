#include "StdAfx.h"
#include "D3DWSurface.hpp"
#include "D3DWContext.hpp"
#include "D3DWTexture.hpp"

D3DWSurface::D3DWSurface()
{
  _drawing = FALSE;
}

D3DWSurface::~D3DWSurface()
{
}

STDMETHODIMP D3DWSurface::GetTexture(ID3DWTexture **oTex)
{
  if(!oTex)
    return E_POINTER;
  *oTex = NULL;
  _tex->AddRef();
  *oTex = _tex;
  return S_OK;
}

STDMETHODIMP D3DWSurface::GetSize(UINT *oWidth, UINT *oHeight)
{
  if(oWidth)
    *oWidth = _width;
  if(oHeight)
    *oHeight = _height;
  return S_OK;
}

STDMETHODIMP D3DWSurface::BeginDraw()
{
  HRESULT hr;
  if(_drawing)
    return HRESULT_FROM_WIN32(ERROR_INVALID_STATE);
  V_HR(_tex->Lock2D());
  _rt->BeginDraw();
  _drawing = TRUE;
  return S_OK;
}

STDMETHODIMP D3DWSurface::EndDraw()
{
  HRESULT hr;
  if(!_drawing)
    return HRESULT_FROM_WIN32(ERROR_INVALID_STATE);
  V_HR(_rt->EndDraw());
  V_HR(_tex->Unlock2D());
  _drawing = FALSE;
  return hr;
}

STDMETHODIMP D3DWSurface::Clear(FLOAT clearColor[4])
{
  HRESULT hr;
  if(!_drawing)
    return HRESULT_FROM_WIN32(ERROR_INVALID_STATE);
  if(!clearColor)
    return E_POINTER;
  D2D1_COLOR_F color = D2D1::ColorF(
    clearColor[0],
    clearColor[1],
    clearColor[2],
    clearColor[3]);
  V_HR(_tex->Clear(clearColor));
  _rt->Clear(color);
  _clearBrush->SetColor(color);
  _rt->FillRectangle(D2D1::RectF(0, 0, (FLOAT)_width, (FLOAT)_height), _clearBrush);
  return S_OK;
}

STDMETHODIMP D3DWSurface::ClearDepthStencil()
{
  HRESULT hr;
  V_HR(_tex->ClearDepthStencil());
  return S_OK;
}

HRESULT D3DWSurface::Create(D3DWContext *ctx, D3DWTexture *tex, D3DWSurface **oSurface, IDXGIKeyedMutex **oKm)
{
  if(!oSurface || !tex)
    return E_POINTER;
  *oSurface = NULL;
  ComContainedObject<D3DWSurface> *surface = new (std::nothrow) ComContainedObject<D3DWSurface>(tex);
  if(!surface)
    return E_OUTOFMEMORY;
  HRESULT hr = surface->Initialize(ctx, tex, oKm);
  if(FAILED(hr))
  {
    SafeDelete(surface);
    return hr;
  }
  *oSurface = surface;
  return S_OK;
}

HRESULT D3DWSurface::Initialize(D3DWContext *ctx, D3DWTexture *tex, IDXGIKeyedMutex **oKm)
{
  HRESULT hr;
  if(!ctx || !oKm)
    return E_POINTER;

  _ctx = ctx;
  _tex = tex;
  *oKm = NULL;

  V_HR(_ctx->Get2DFactory(&_factory));
  
  ComPtr<ID3D11Resource> res;
  V_HR(tex->GetRes(&res));

  ComPtr<IDXGIResource> dxgiResource;
  V_HR(res.QueryInterface(&dxgiResource));

  HANDLE share;
  V_HR(dxgiResource->GetSharedHandle(&share));

  ComPtr<ID3D10Device1> device10;
  V_HR(ctx->GetDevice10(&device10));
  device10->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

  ComPtr<ID3D10Texture2D> tex10;
  V_HR_ASSERT(device10->OpenSharedResource(share, IID_PPV_ARGS(&tex10)), "Unable to open shared resource");

  ComPtr<IDXGISurface> dxgiSurface;
  V_HR(tex10.QueryInterface(&dxgiSurface));

  ComPtr<IDXGIKeyedMutex> km;
  V_HR(dxgiSurface.QueryInterface(&km));

  D2D1_RENDER_TARGET_PROPERTIES rtp;
  SecureZeroMemory(&rtp, sizeof(rtp));
  _factory->GetDesktopDpi(&rtp.dpiX, &rtp.dpiY);
  rtp.minLevel = D2D1_FEATURE_LEVEL_10;
  rtp.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
  rtp.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
  rtp.usage = D2D1_RENDER_TARGET_USAGE_NONE;  
  D2D1_RENDER_TARGET_TYPE rtTypes[] =
  {
    D2D1_RENDER_TARGET_TYPE_HARDWARE,
    D2D1_RENDER_TARGET_TYPE_SOFTWARE
  };
  for(int i = 0; i<ARRAYSIZE(rtTypes); ++i)
  {
    rtp.type = rtTypes[i];
    hr = _factory->CreateDxgiSurfaceRenderTarget(dxgiSurface, rtp, &_rt);
    if(SUCCEEDED(hr))
      break;
  }
  V_HR_ASSERT(hr, "Unable to create D2D1 render target");

  V_HR(_rt->CreateSolidColorBrush(D2D1::ColorF(0), &_clearBrush));
  D2D1_SIZE_F size = _rt->GetSize();
  _width = (UINT)size.width;
  _height = (UINT)size.height;

  km->AddRef();
  *oKm = km;

  return S_OK;
}