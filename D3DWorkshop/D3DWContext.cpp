#include "StdAfx.h"
#include "D3DWContext.hpp"
#include "D3DWWindow.hpp"
#include "D3DWEffect.hpp"
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
  sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
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

  V_HR(SetAnisotropyLevel(0));

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
  _rtv.Release();
  _dsv.Release();

  HRESULT hr;

  V_HR_ASSERT(_swc->ResizeBuffers(2, width, height, DXGI_FORMAT_UNKNOWN, 0), "Unable to resize swap chain buffers");

  ComPtr<ID3D11Texture2D> backBuffer;
  V_HR_ASSERT(_swc->GetBuffer(0, IID_PPV_ARGS(&backBuffer)), "Unable to obtain back buffer");
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

STDMETHODIMP D3DWContext::GetAnisotropyLevel(UINT *oLevel)
{
  if(!oLevel)
    return E_POINTER;
  *oLevel = _afLevel;
  return S_OK;
}
  
STDMETHODIMP D3DWContext::SetAnisotropyLevel(UINT level)
{
  HRESULT hr;
  if(_afLevel == level)
    return S_OK;
  if(0 != level && 2 != level && 4 != level && 8 != level && 16 != level)
    return E_INVALIDARG;
  D3D11_SAMPLER_DESC sd;
  SecureZeroMemory(&sd, sizeof(sd));
  sd.AddressU = sd.AddressV = sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
  sd.ComparisonFunc = D3D11_COMPARISON_LESS;
  sd.MipLODBias = 0;
  sd.MinLOD = 0;
  sd.MaxLOD = D3D11_FLOAT32_MAX;
  if(0 == level)
  {
    sd.MaxAnisotropy = 0;
    sd.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
  }
  else
  {
    sd.MaxAnisotropy = level;
    sd.Filter = D3D11_FILTER_COMPARISON_ANISOTROPIC;
  }
  ComPtr<ID3D11SamplerState> sampler;
  hr = _device->CreateSamplerState(&sd, &sampler);
  if(SUCCEEDED(hr))
  {
    _dc->VSSetSamplers(0, 1, sampler.AddressOf());
    _dc->GSSetSamplers(0, 1, sampler.AddressOf());
    _dc->PSSetSamplers(0, 1, sampler.AddressOf());
    _sampler = sampler;
    _afLevel = level;
  }
  return hr;
}

STDMETHODIMP D3DWContext::Clear(FLOAT clearColor[4])
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
  LPVOID vertexData, SIZE_T vertexSize, UINT nVertices, UINT32 *indices, UINT nIndices, D3DW_TOPOLOGY topology, BOOL canMap, ID3DWMesh **oMesh)
{
  if(!oMesh)
    return E_POINTER;
  *oMesh = NULL;
  D3DWMesh *mesh = NULL;
  HRESULT hr = D3DWMesh::Create(vertexData, vertexSize, nVertices, indices, nIndices, topology, canMap, this, &mesh);
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