#include "StdAfx.h"
#include "D3DWContext.hpp"
#include "D3DWWindow.hpp"
#include "D3DWEffect.hpp"
#include "D3DWSampler.hpp"
#include "D3DWConstantBuffer.hpp"
#include "D3DWMesh.hpp"
#include "D3DWTexture.hpp"
#include "D3DWCubeMap.hpp"
#include "D3DWFloatAnimation.hpp"

D3DWContext::D3DWContext()
{
  _afLevel = 1;
}

D3DWContext::~D3DWContext()
{
  if(_dc)
  {
    _dc->ClearState();
    _dc->Flush();
  }
}

HRESULT WINAPI D3DWContext::Create(HWND hwnd, D3DWWindow *window, D3DWContext **oContext)
{
  if(!oContext || !window)
    return E_POINTER;

  ComContainedObject<D3DWContext> *ctx = new (std::nothrow) ComContainedObject<D3DWContext>(window);
  if(!ctx)
    return E_OUTOFMEMORY;
  HRESULT hr = ctx->Initialize(hwnd, window);
  if(FAILED(hr))
  {
    SafeDelete(ctx);
    return hr;
  }
  *oContext = ctx;
  return S_OK;
}

HRESULT D3DWContext::Initialize(HWND hwnd, D3DWWindow *window)
{
  if(!IsWindow(hwnd))
    return E_HANDLE;

  _wnd = window;
  _hwnd = hwnd;

  HRESULT hr;

  RECT rc;
  GetClientRect(hwnd, &rc);
  UINT width = rc.right - rc.left;
  UINT height = rc.bottom - rc.top;  

  DXGI_SWAP_CHAIN_DESC sd = {0};
  sd.BufferCount = 2;
  sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;
  sd.Windowed = TRUE;
  sd.Flags = 0;
  sd.OutputWindow = hwnd;
  sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
  sd.SampleDesc.Count = 1;
  sd.SampleDesc.Quality = D3D11_STANDARD_MULTISAMPLE_PATTERN;
  sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
  sd.BufferDesc.Width = width;
  sd.BufferDesc.Height = height;
  sd.BufferDesc.RefreshRate.Numerator = 60;
  sd.BufferDesc.RefreshRate.Denominator = 1;
  sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
  sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

  DWORD createFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined(DEBUG) || defined(_DEBUG)
  createFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

  D3D_FEATURE_LEVEL featureLevels[] =
  {
    D3D_FEATURE_LEVEL_10_1,
    D3D_FEATURE_LEVEL_10_0,
    D3D_FEATURE_LEVEL_9_3,
    D3D_FEATURE_LEVEL_9_2,
    D3D_FEATURE_LEVEL_9_1
  };

  D3D_FEATURE_LEVEL featureLevel;

  D3D_DRIVER_TYPE driverTypes[] =
  {
    D3D_DRIVER_TYPE_HARDWARE,
    D3D_DRIVER_TYPE_WARP,
    D3D_DRIVER_TYPE_REFERENCE
  };

  for(int i = 0; i<ARRAYSIZE(driverTypes); ++i)
  {
    hr = D3D11CreateDeviceAndSwapChain(
      NULL,
      driverTypes[i],
      NULL,
      createFlags,
      featureLevels,
      ARRAYSIZE(featureLevels),
      D3D11_SDK_VERSION,
      &sd,
      &_swc,
      &_device,
      &featureLevel,
      &_dc);
    if(SUCCEEDED(hr))
      break;
  }
  V_HR_ASSERT(hr, "Unable to create Direct3D device");

  ComPtr<IDXGIDevice> dxgiDevice;
  V_HR_ASSERT(_device.QueryInterface(&dxgiDevice), "Unable to query for DXGI device");

  ComPtr<IDXGIAdapter> adapter;
  V_HR_ASSERT(dxgiDevice->GetParent(IID_PPV_ARGS(&adapter)), "Unable to query for DXGI adapter");

  ComPtr<IDXGIFactory> factory;
  V_HR_ASSERT(adapter->GetParent(IID_PPV_ARGS(&factory)), "Unable to query for DXGI factory");
  
  V_HR(factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER));
  
  DWORD createFlags10 = D3D10_CREATE_DEVICE_BGRA_SUPPORT;
#if defined(DEBUG) || defined(_DEBUG)
  createFlags10 |= D3D10_CREATE_DEVICE_DEBUG;
#endif
  D3D10_DRIVER_TYPE driverTypes10[] =
  {
    D3D10_DRIVER_TYPE_HARDWARE,
    D3D10_DRIVER_TYPE_WARP,
    D3D10_DRIVER_TYPE_REFERENCE
  };
  for(int i = 0; i<ARRAYSIZE(driverTypes10); ++i)
  {
    hr = D3D10CreateDevice1(
      adapter,
      driverTypes10[i],
      NULL,
      createFlags10,
      D3D10_FEATURE_LEVEL_10_0,
      D3D10_1_SDK_VERSION,
      &_device10);
    if(SUCCEEDED(hr))
      break;
  }
  V_HR_ASSERT(hr, "Unable to create Direct3D 10.1 device");
  
  _vp.MinDepth = 0;
  _vp.MaxDepth = 1;
  _vp.TopLeftX = 0;
  _vp.TopLeftY = 0;  

  V_HR(RecreateTargets(width, height));
  
  V_HR_ASSERT(_wic.CoCreateInstance(CLSID_WICImagingFactory), "Unable to create WIC imaging factory");
  
  D2D1_FACTORY_OPTIONS fo;
  fo.debugLevel = D2D1_DEBUG_LEVEL_NONE;
#if defined(DEBUG) || defined(_DEBUG)
  fo.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif // DEBUG || _DEBUG
  V_HR_ASSERT(D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, fo, &_2dFactory), "Unable to create D2D1 factory");
  
  return S_OK;
}

HRESULT D3DWContext::RecreateTargets(UINT width, UINT height)
{
  _srv.Release();
  _rtv.Release();
  _dsv.Release();

  HRESULT hr;

  V_HR_ASSERT(_swc->ResizeBuffers(2, width, height, DXGI_FORMAT_UNKNOWN, 0), "Unable to resize swap chain buffers");

  ComPtr<ID3D11Texture2D> backBuffer;
  V_HR_ASSERT(_swc->GetBuffer(0, IID_PPV_ARGS(&backBuffer)), "Unable to obtain back buffer");
  V_HR_ASSERT(_device->CreateShaderResourceView(backBuffer, NULL, &_srv), "Unable to create SRV");
  V_HR_ASSERT(_device->CreateRenderTargetView(backBuffer, NULL, &_rtv), "Unable to create RTV");
  backBuffer.Release();

  D3D11_TEXTURE2D_DESC td = {0};
  td.BindFlags = D3D11_BIND_DEPTH_STENCIL;
  td.ArraySize = 1;
  td.Format = DXGI_FORMAT_D32_FLOAT;
  td.Width = width;
  td.Height = height;
  td.MipLevels = 1;
  td.SampleDesc.Count = 1;
  td.SampleDesc.Quality = D3D11_STANDARD_MULTISAMPLE_PATTERN;
  ComPtr<ID3D11Texture2D> ds;
  V_HR_ASSERT(_device->CreateTexture2D(&td, NULL, &ds), "Unable to create depth-stencil");
  V_HR_ASSERT(_device->CreateDepthStencilView(ds, NULL, &_dsv), "Unable to create DSV");  

  _dc->OMSetRenderTargets(1, _rtv.AddressOf(), _dsv);

  _vp.Width = (FLOAT)width;
  _vp.Height = (FLOAT)height;

  _dc->RSSetViewports(1, &_vp);

  return S_OK;
}

STDMETHODIMP D3DWContext::GetSize(UINT *oWidth, UINT *oHeight)
{
  if(oWidth)
    *oWidth = (UINT)_vp.Width;
  if(oHeight)
    *oHeight = (UINT)_vp.Height;
  return S_OK;
}

STDMETHODIMP D3DWContext::SetSize(UINT width, UINT height)
{  
  return RecreateTargets(width, height);
}

STDMETHODIMP D3DWContext::SetAsTarget()
{
  _dc->OMSetRenderTargets(1, _rtv.AddressOf(), _dsv);
  _dc->RSSetViewports(1, &_vp);
  return S_OK;
}

STDMETHODIMP D3DWContext::Clear(const FLOAT clearColor[4])
{
  if(!clearColor)
    return E_POINTER;
  _dc->ClearRenderTargetView(_rtv, clearColor);
  return S_OK;
}

STDMETHODIMP D3DWContext::ClearDepthStencil()
{
  _dc->ClearDepthStencilView(_dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 255);
  return S_OK;
}

STDMETHODIMP D3DWContext::Present()
{
  _swc->Present(0, 0);
  return S_OK;
}

HRESULT D3DWContext::GetDevice(ID3D11Device **oDevice)
{
  if(!oDevice)
    return E_POINTER;
  *oDevice = NULL;
  _device->AddRef();
  *oDevice = _device;
  return S_OK;
}

HRESULT D3DWContext::GetDc(ID3D11DeviceContext **oDc)
{
  if(!oDc)
    return E_POINTER;
  *oDc = NULL;
  _dc->AddRef();
  *oDc = _dc;
  return S_OK;
}

HRESULT D3DWContext::GetDevice10(ID3D10Device1 **oDevice)
{
  if(!oDevice)
    return E_POINTER;
  *oDevice = NULL;
  _device10->AddRef();
  *oDevice = _device10;
  return S_OK;
}

HRESULT D3DWContext::GetWic(IWICImagingFactory **oWic)
{
  if(!oWic)
    return E_POINTER;
  *oWic = NULL;
  _wic->AddRef();
  *oWic = _wic;
  return S_OK;
}

HRESULT D3DWContext::Get2DFactory(ID2D1Factory **oFactory)
{
  if(!oFactory)
    return E_POINTER;
  *oFactory = NULL;
  _2dFactory->AddRef();
  *oFactory = _2dFactory;
  return S_OK;
}

STDMETHODIMP D3DWContext::GetRes(ID3D11Resource **oRes)
{
  HRESULT hr;
  if(!oRes)
    return E_POINTER;
  *oRes = NULL;
  ComPtr<ID3D11Texture2D> backBuffer;
  V_HR(_swc->GetBuffer(0, IID_PPV_ARGS(&backBuffer)));
  backBuffer->AddRef();
  *oRes = backBuffer;
  return S_OK;
}

STDMETHODIMP D3DWContext::GetSrv(ID3D11ShaderResourceView **oSrv)
{
  if(!oSrv)
    return E_POINTER;
  *oSrv = NULL;
  _srv->AddRef();
  *oSrv = _srv;
  return S_OK;
}

STDMETHODIMP D3DWContext::GetRtv(ID3D11RenderTargetView **oRtv)
{
  if(!oRtv)
    return E_POINTER;
  *oRtv = NULL;
  _rtv->AddRef();
  *oRtv = _rtv;
  return S_OK;
}

STDMETHODIMP D3DWContext::GetSwapChain(IDXGISwapChain **oSwc)
{
  if(!oSwc)
    return E_POINTER;
  *oSwc = NULL;
  _swc->AddRef();
  *oSwc = _swc;
  return S_OK;
}

STDMETHODIMP D3DWContext::GetWindow(ID3DWWindow **oWindow)
{
  if(!oWindow)
    return E_POINTER;
  *oWindow = NULL;
  _wnd->AddRef();
  *oWindow = _wnd;
  return S_OK;
}

STDMETHODIMP D3DWContext::CreateEffectFromMemory(
    LPCSTR data, SIZE_T size, LPCSTR vsEntry, LPCSTR gsEntry, LPCSTR psEntry, ID3DWEffect **oEffect)
{
  if(!oEffect)
    return E_POINTER;
  *oEffect = NULL;
  D3DWEffect *effect = NULL;
  HRESULT hr = D3DWEffect::CreateFromMemory(data, size, vsEntry, gsEntry, psEntry, this, &effect);
  V_HR(hr);
  *oEffect = effect;
  return S_OK;
}

STDMETHODIMP D3DWContext::CreateEffectFromFile(
  LPCWSTR filename, LPCSTR vsEntry, LPCSTR gsEntry, LPCSTR psEntry, ID3DWEffect **oEffect)
{
  if(!oEffect)
    return E_POINTER;
  *oEffect = NULL;
  D3DWEffect *effect = NULL;
  HRESULT hr = D3DWEffect::CreateFromFile(filename, vsEntry, gsEntry, psEntry, this, &effect);
  V_HR(hr);
  *oEffect = effect;
  return S_OK;
}

STDMETHODIMP D3DWContext::CreateEffectFromResource(
  HMODULE module, LPCWSTR resourceType, LPCWSTR resourceName, LPCSTR vsEntry, LPCSTR gsEntry, LPCSTR psEntry, ID3DWEffect **oEffect)
{
  if(!oEffect)
    return E_POINTER;
  *oEffect = NULL;
  D3DWEffect *effect = NULL;
  HRESULT hr = D3DWEffect::CreateFromResource(module, resourceType, resourceName, vsEntry, gsEntry, psEntry, this, &effect);
  V_HR(hr);
  *oEffect = effect;
  return S_OK;
}

STDMETHODIMP D3DWContext::CreateSampler(
    UINT anisotropyLevel, D3DW_TEXTURE_MODE addressMode, const FLOAT borderColor[4], ID3DWSampler **oSampler)
{
  if(!oSampler)
    return E_POINTER;
  *oSampler = NULL;
  D3DWSampler *sampler = NULL;
  HRESULT hr = D3DWSampler::Create(anisotropyLevel, addressMode, borderColor, this, &sampler);
  V_HR(hr);
  *oSampler = sampler;
  return S_OK;
}

STDMETHODIMP D3DWContext::CreateConstantBuffer(SIZE_T size, ID3DWConstantBuffer **oConstantBuffer)
{
  if(!oConstantBuffer)
    return E_POINTER;
  *oConstantBuffer = NULL;
  D3DWConstantBuffer *cb = NULL;
  HRESULT hr = D3DWConstantBuffer::Create(size, this, &cb);
  V_HR(hr);
  *oConstantBuffer = cb;
  return S_OK;
}

STDMETHODIMP D3DWContext::CreateTexture(
    UINT width, UINT height, ID3DWTexture **oTex)
{
  if(!oTex)
    return E_POINTER;
  *oTex = NULL;
  D3DWTexture *tex = NULL;
  HRESULT hr = D3DWTexture::Create(width, height, this, &tex);
  V_HR(hr);
  *oTex = tex;
  return S_OK;
}

STDMETHODIMP D3DWContext::CreateTextureFromMemory(
    LPVOID data, SIZE_T size, BOOL canDraw, ID3DWTexture **oTex)
{
  if(!oTex)
    return E_POINTER;
  *oTex = NULL;
  D3DWTexture *tex = NULL;
  HRESULT hr = D3DWTexture::CreateFromMemory(data, size, canDraw, this, &tex);
  V_HR(hr);
  *oTex = tex;
  return S_OK;
}

STDMETHODIMP D3DWContext::CreateTextureFromResource(
    HMODULE module, LPCWSTR resourceName, LPCWSTR resourceType, BOOL canDraw, ID3DWTexture **oTex)
{
  if(!oTex)
    return E_POINTER;
  *oTex = NULL;
  D3DWTexture *tex = NULL;
  HRESULT hr = D3DWTexture::CreateFromResource(module, resourceName, resourceType, canDraw, this, &tex);
  V_HR(hr);
  *oTex = tex;
  return S_OK;
}

STDMETHODIMP D3DWContext::CreateTextureFromFile(
    LPCWSTR filename, BOOL canDraw, ID3DWTexture **oTex)
{
  if(!oTex)
    return E_POINTER;
  *oTex = NULL;
  D3DWTexture *tex = NULL;
  HRESULT hr = D3DWTexture::CreateFromFile(filename, canDraw, this, &tex);
  V_HR(hr);
  *oTex = tex;
  return S_OK;
}

STDMETHODIMP D3DWContext::CreateCubeMap(
    ID3DWTexture *posX,
    ID3DWTexture *negX,
    ID3DWTexture *posY,
    ID3DWTexture *negY,
    ID3DWTexture *posZ,
    ID3DWTexture *negZ,
    ID3DWCubeMap **oCubeMap)
{
  if(!oCubeMap)
    return E_POINTER;
  *oCubeMap = NULL;
  D3DWCubeMap *cubeMap = NULL;
  HRESULT hr = D3DWCubeMap::Create(posX, negX, posY, negY, posZ, negZ, this, &cubeMap);
  V_HR(hr);
  *oCubeMap = cubeMap;
  return S_OK;
}

STDMETHODIMP D3DWContext::CreateMesh(
  LPVOID vertexData, SIZE_T vertexSize, UINT nVertices, UINT32 *indices, UINT nIndices, D3DW_TOPOLOGY topology, ID3DWMesh **oMesh)
{
  if(!oMesh)
    return E_POINTER;
  *oMesh = NULL;
  D3DWMesh *mesh = NULL;
  HRESULT hr = D3DWMesh::Create(vertexData, vertexSize, nVertices, indices, nIndices, topology, this, &mesh);
  V_HR(hr);
  *oMesh = mesh;
  return S_OK;
}

STDMETHODIMP D3DWContext::CreateFloatAnimation(
    FLOAT begin, FLOAT end, FLOAT interval, BOOL repeat, BOOL autoReverse, ID3DWFloatAnimation **oAnimation)
{
  if(!oAnimation)
    return E_POINTER;
  *oAnimation = NULL;
  D3DWFloatAnimation *animation = NULL;
  HRESULT hr = D3DWFloatAnimation::Create(begin, end, interval, repeat, autoReverse, this, &animation);
  V_HR(hr);
  *oAnimation = animation;
  return S_OK;
}