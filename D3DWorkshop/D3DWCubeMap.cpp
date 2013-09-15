#include "StdAfx.h"
#include "D3DWCubeMap.hpp"
#include "D3DWContext.hpp"
#include "D3DWTexture.hpp"

D3DWCubeMap::D3DWCubeMap()
{
}

D3DWCubeMap::~D3DWCubeMap()
{
}

STDMETHODIMP D3DWCubeMap::GetRes(ID3D11Resource **oRes)
{
  if(!oRes)
    return E_POINTER;
  *oRes = NULL;
  _tex->AddRef();
  *oRes = _tex;
  return S_OK;    
}

STDMETHODIMP D3DWCubeMap::GetSrv(ID3D11ShaderResourceView **oSrv)
{
  if(!oSrv)
    return E_POINTER;
  *oSrv = NULL;
  _srv->AddRef();
  *oSrv = _srv;
  return S_OK;
}

STDMETHODIMP D3DWCubeMap::GetRtv(ID3D11RenderTargetView **oRtv)
{
  if(!oRtv)
    return E_POINTER;
  *oRtv = NULL;
  _rtv->AddRef();
  *oRtv = _rtv;
  return S_OK;
}

STDMETHODIMP D3DWCubeMap::SetAsTarget()
{
  _dc->OMSetRenderTargets(1, _rtv.AddressOf(), _dsv);
  _dc->RSSetViewports(1, &_vp);
  return S_OK;
}

STDMETHODIMP D3DWCubeMap::Clear(const FLOAT clearColor[4])
{
  if(!clearColor)
    return E_POINTER;
  _dc->ClearRenderTargetView(_rtv, clearColor);
  return S_OK;
}

STDMETHODIMP D3DWCubeMap::ClearDepthStencil()
{
  _dc->ClearDepthStencilView(_dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 255);
  return S_OK;
}

STDMETHODIMP D3DWCubeMap::GetContext(ID3DWContext **oCtx)
{
  if(!oCtx)
    return E_POINTER;
  *oCtx = NULL;
  _ctx->AddRef();
  *oCtx = _ctx;
  return S_OK;
}

STDMETHODIMP D3DWCubeMap::GetByteSize(SIZE_T *oSize)
{
  if(!oSize)
    return E_POINTER;
  *oSize = _byteSize;
  return S_OK;
}

STDMETHODIMP D3DWCubeMap::GetSideSize(UINT *oSize)
{
  if(!oSize)
    return E_POINTER;
  *oSize = (UINT)_vp.Width;
  return S_OK;
}

HRESULT D3DWCubeMap::Create(
  ID3DWTexture *posX,
  ID3DWTexture *negX,
  ID3DWTexture *posY,
  ID3DWTexture *negY,
  ID3DWTexture *posZ,
  ID3DWTexture *negZ,
  D3DWContext *ctx,
  D3DWCubeMap **oCubeMap)
{
  if(!oCubeMap)
    return E_POINTER;
  *oCubeMap = NULL;
  ComObject<D3DWCubeMap> *cubeMap = new (std::nothrow) ComObject<D3DWCubeMap>();
  if(!cubeMap)
    return E_OUTOFMEMORY;
  HRESULT hr = cubeMap->Initialize(posX, negX, posY, negY, posZ, negZ, ctx);
  if(FAILED(hr))
  {
    SafeDelete(cubeMap);
    return hr;
  }
  cubeMap->AddRef();
  *oCubeMap = cubeMap;
  return S_OK;
}

HRESULT D3DWCubeMap::Initialize(
  ID3DWTexture *posX,
  ID3DWTexture *negX,
  ID3DWTexture *posY,
  ID3DWTexture *negY,
  ID3DWTexture *posZ,
  ID3DWTexture *negZ,
  D3DWContext *ctx)
{
  HRESULT hr;
  if(!posX || !negX || !posY || !negY || !posZ || !negZ || !ctx)
    return E_POINTER;

  V_HR(ctx->GetDevice(&_device));
  V_HR(ctx->GetDc(&_dc));

  UINT width, height;

  ComPtr<ID3DWTextureInternals> texInternals;
  V_HR(posX->QueryInterface(IID_PPV_ARGS(&texInternals)));
  V_HR(posX->GetSize(&width, &height));

  if(width > height)
    height = width;
  else
    width = height;

  UINT rowPitch = (width * 32 + 7) / 8;
  UINT size = rowPitch * height;

  D3D11_TEXTURE2D_DESC td = {0};
  td.ArraySize = 6;
  td.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
  td.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  td.MipLevels = 1;
  td.Width = width;
  td.Height = height;
  td.SampleDesc.Count = 1;
  td.SampleDesc.Quality = 0;
  td.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
  V_HR_ASSERT(_device->CreateTexture2D(&td, NULL, &_tex), "Unable to create cube texture");

  std::unique_ptr<BYTE[]> buffer(new BYTE[size]);

  V_HR(texInternals->GetRgbaData(width, height, buffer.get()));
  _dc->UpdateSubresource(_tex, 0, NULL, buffer.get(), rowPitch, size);
  texInternals.Release();
  V_HR(negX->QueryInterface(IID_PPV_ARGS(&texInternals)));
  V_HR(texInternals->GetRgbaData(width, height, buffer.get()));
  _dc->UpdateSubresource(_tex, 1, NULL, buffer.get(), rowPitch, size);
  texInternals.Release();
  V_HR(posY->QueryInterface(IID_PPV_ARGS(&texInternals)));
  V_HR(texInternals->GetRgbaData(width, height, buffer.get()));
  _dc->UpdateSubresource(_tex, 2, NULL, buffer.get(), rowPitch, size);
  texInternals.Release();
  V_HR(negY->QueryInterface(IID_PPV_ARGS(&texInternals)));
  V_HR(texInternals->GetRgbaData(width, height, buffer.get()));
  _dc->UpdateSubresource(_tex, 3, NULL, buffer.get(), rowPitch, size);
  texInternals.Release();
  V_HR(posZ->QueryInterface(IID_PPV_ARGS(&texInternals)));
  V_HR(texInternals->GetRgbaData(width, height, buffer.get()));
  _dc->UpdateSubresource(_tex, 4, NULL, buffer.get(), rowPitch, size);
  texInternals.Release();
  V_HR(negZ->QueryInterface(IID_PPV_ARGS(&texInternals)));
  V_HR(texInternals->GetRgbaData(width, height, buffer.get()));
  _dc->UpdateSubresource(_tex, 5, NULL, buffer.get(), rowPitch, size);

  D3D11_SHADER_RESOURCE_VIEW_DESC srvD;
  SecureZeroMemory(&srvD, sizeof(srvD));
  srvD.Format = td.Format;
  srvD.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
  srvD.TextureCube.MipLevels = 1;
  srvD.TextureCube.MostDetailedMip = 0;
  V_HR(_device->CreateShaderResourceView(_tex, &srvD, &_srv));

  D3D11_RENDER_TARGET_VIEW_DESC rtvD;
  SecureZeroMemory(&rtvD, sizeof(rtvD));
  rtvD.Format = td.Format;  
  rtvD.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
  rtvD.Texture2DArray.ArraySize = td.ArraySize;
  rtvD.Texture2DArray.FirstArraySlice = 0;
  rtvD.Texture2DArray.MipSlice = 0;
  V_HR(_device->CreateRenderTargetView(_tex, &rtvD, &_rtv));

  _vp.MinDepth = 0;
  _vp.MaxDepth = 1;
  _vp.TopLeftX = 0;
  _vp.TopLeftY = 0;
  _vp.Width = (FLOAT)width;
  _vp.Height = (FLOAT)height;

  td.BindFlags = D3D11_BIND_DEPTH_STENCIL;
  td.Format = DXGI_FORMAT_D32_FLOAT;
  td.MiscFlags = 0;
  ComPtr<ID3D11Texture2D> ds;
  V_HR_ASSERT(_device->CreateTexture2D(&td, NULL, &ds), "Unable to create depth stencil for cube texture");

  D3D11_DEPTH_STENCIL_VIEW_DESC dsvD;
  SecureZeroMemory(&dsvD, sizeof(dsvD));
  dsvD.Format = td.Format;
  dsvD.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
  dsvD.Texture2DArray.ArraySize = td.ArraySize;
  dsvD.Texture2DArray.FirstArraySlice = 0;
  dsvD.Texture2DArray.MipSlice = 0;
  V_HR_ASSERT(_device->CreateDepthStencilView(ds, &dsvD, &_dsv), "Unable to create DSV for cube map");

  _byteSize = size * 6;

  return S_OK;
}