#include "StdAfx.h"
#include "D3DWTexture.hpp"
#include "D3DWSurface.hpp"
#include "D3DWContext.hpp"

const D3DWTexture::WicTranslate D3DWTexture::_wicTranslate[] = 
{
    { GUID_WICPixelFormat128bppRGBAFloat,       DXGI_FORMAT_R32G32B32A32_FLOAT },

    { GUID_WICPixelFormat64bppRGBAHalf,         DXGI_FORMAT_R16G16B16A16_FLOAT },
    { GUID_WICPixelFormat64bppRGBA,             DXGI_FORMAT_R16G16B16A16_UNORM },

    { GUID_WICPixelFormat32bppRGBA,             DXGI_FORMAT_R8G8B8A8_UNORM },
    { GUID_WICPixelFormat32bppBGRA,             DXGI_FORMAT_B8G8R8A8_UNORM }, // DXGI 1.1
    { GUID_WICPixelFormat32bppBGR,              DXGI_FORMAT_B8G8R8X8_UNORM }, // DXGI 1.1

    { GUID_WICPixelFormat32bppRGBA1010102XR,    DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM }, // DXGI 1.1
    { GUID_WICPixelFormat32bppRGBA1010102,      DXGI_FORMAT_R10G10B10A2_UNORM },
    { GUID_WICPixelFormat32bppRGBE,             DXGI_FORMAT_R9G9B9E5_SHAREDEXP },

#ifdef DXGI_1_2_FORMATS

    { GUID_WICPixelFormat16bppBGRA5551,         DXGI_FORMAT_B5G5R5A1_UNORM },
    { GUID_WICPixelFormat16bppBGR565,           DXGI_FORMAT_B5G6R5_UNORM },

#endif // DXGI_1_2_FORMATS

    { GUID_WICPixelFormat32bppGrayFloat,        DXGI_FORMAT_R32_FLOAT },
    { GUID_WICPixelFormat16bppGrayHalf,         DXGI_FORMAT_R16_FLOAT },
    { GUID_WICPixelFormat16bppGray,             DXGI_FORMAT_R16_UNORM },
    { GUID_WICPixelFormat8bppGray,              DXGI_FORMAT_R8_UNORM },

    { GUID_WICPixelFormat8bppAlpha,             DXGI_FORMAT_A8_UNORM },

#if (_WIN32_WINNT >= 0x0602 /*_WIN32_WINNT_WIN8*/)
    { GUID_WICPixelFormat96bppRGBFloat,         DXGI_FORMAT_R32G32B32_FLOAT },
#endif
};

const D3DWTexture::WicConvert D3DWTexture::_wicConvert[] = 
{
    // Note target GUID in this conversion table must be one of those directly supported formats (above).

    { GUID_WICPixelFormatBlackWhite,            GUID_WICPixelFormat8bppGray }, // DXGI_FORMAT_R8_UNORM

    { GUID_WICPixelFormat1bppIndexed,           GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 
    { GUID_WICPixelFormat2bppIndexed,           GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 
    { GUID_WICPixelFormat4bppIndexed,           GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 
    { GUID_WICPixelFormat8bppIndexed,           GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 

    { GUID_WICPixelFormat2bppGray,              GUID_WICPixelFormat8bppGray }, // DXGI_FORMAT_R8_UNORM 
    { GUID_WICPixelFormat4bppGray,              GUID_WICPixelFormat8bppGray }, // DXGI_FORMAT_R8_UNORM 

    { GUID_WICPixelFormat16bppGrayFixedPoint,   GUID_WICPixelFormat16bppGrayHalf }, // DXGI_FORMAT_R16_FLOAT 
    { GUID_WICPixelFormat32bppGrayFixedPoint,   GUID_WICPixelFormat32bppGrayFloat }, // DXGI_FORMAT_R32_FLOAT 

#ifdef DXGI_1_2_FORMATS

    { GUID_WICPixelFormat16bppBGR555,           GUID_WICPixelFormat16bppBGRA5551 }, // DXGI_FORMAT_B5G5R5A1_UNORM

#else

    { GUID_WICPixelFormat16bppBGR555,           GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM
    { GUID_WICPixelFormat16bppBGRA5551,         GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM
    { GUID_WICPixelFormat16bppBGR565,           GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM

#endif // DXGI_1_2_FORMATS

    { GUID_WICPixelFormat32bppBGR101010,        GUID_WICPixelFormat32bppRGBA1010102 }, // DXGI_FORMAT_R10G10B10A2_UNORM

    { GUID_WICPixelFormat24bppBGR,              GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 
    { GUID_WICPixelFormat24bppRGB,              GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 
    { GUID_WICPixelFormat32bppPBGRA,            GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 
    { GUID_WICPixelFormat32bppPRGBA,            GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 

    { GUID_WICPixelFormat48bppRGB,              GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
    { GUID_WICPixelFormat48bppBGR,              GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
    { GUID_WICPixelFormat64bppBGRA,             GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
    { GUID_WICPixelFormat64bppPRGBA,            GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
    { GUID_WICPixelFormat64bppPBGRA,            GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM

    { GUID_WICPixelFormat48bppRGBFixedPoint,    GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT 
    { GUID_WICPixelFormat48bppBGRFixedPoint,    GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT 
    { GUID_WICPixelFormat64bppRGBAFixedPoint,   GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT 
    { GUID_WICPixelFormat64bppBGRAFixedPoint,   GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT 
    { GUID_WICPixelFormat64bppRGBFixedPoint,    GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT 
    { GUID_WICPixelFormat64bppRGBHalf,          GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT 
    { GUID_WICPixelFormat48bppRGBHalf,          GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT 

    { GUID_WICPixelFormat96bppRGBFixedPoint,    GUID_WICPixelFormat128bppRGBAFloat }, // DXGI_FORMAT_R32G32B32A32_FLOAT 
    { GUID_WICPixelFormat128bppPRGBAFloat,      GUID_WICPixelFormat128bppRGBAFloat }, // DXGI_FORMAT_R32G32B32A32_FLOAT 
    { GUID_WICPixelFormat128bppRGBFloat,        GUID_WICPixelFormat128bppRGBAFloat }, // DXGI_FORMAT_R32G32B32A32_FLOAT 
    { GUID_WICPixelFormat128bppRGBAFixedPoint,  GUID_WICPixelFormat128bppRGBAFloat }, // DXGI_FORMAT_R32G32B32A32_FLOAT 
    { GUID_WICPixelFormat128bppRGBFixedPoint,   GUID_WICPixelFormat128bppRGBAFloat }, // DXGI_FORMAT_R32G32B32A32_FLOAT 

    { GUID_WICPixelFormat32bppCMYK,             GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 
    { GUID_WICPixelFormat64bppCMYK,             GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
    { GUID_WICPixelFormat40bppCMYKAlpha,        GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
    { GUID_WICPixelFormat80bppCMYKAlpha,        GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM

#if (_WIN32_WINNT >= 0x0602 /*_WIN32_WINNT_WIN8*/)
    { GUID_WICPixelFormat32bppRGB,              GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM
    { GUID_WICPixelFormat64bppRGB,              GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
    { GUID_WICPixelFormat64bppPRGBAHalf,        GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT 
#endif

    // We don't support n-channel formats
};

D3DWTexture::D3DWTexture()
{
  _surface = NULL;
  _2dLock = FALSE;
  _3dLock = FALSE;
}

D3DWTexture::~D3DWTexture()
{
  SafeDelete(_surface);
}

STDMETHODIMP D3DWTexture::GetContext(ID3DWContext **oCtx)
{
  if(!oCtx)
    return E_POINTER;
  *oCtx = NULL;
  _ctx->AddRef();
  *oCtx = _ctx;
  return S_OK;
}

STDMETHODIMP D3DWTexture::Lock3D()
{
  HRESULT hr;
  if(!_canDraw)
    return S_FALSE;
  if(_3dLock)
    return S_FALSE;
  if(_2dLock)
  {
    V_HR_BREAK("Texture is already locked for 2D");
    return HRESULT_FROM_WIN32(ERROR_INVALID_STATE);
  }
  V_HR(_3dKm->AcquireSync(0, INFINITE));
  _3dLock = TRUE;
  return S_OK;
}

STDMETHODIMP D3DWTexture::Unlock3D()
{
  HRESULT hr;
  if(!_canDraw)
    return S_FALSE;
  if(!_3dLock)
    return HRESULT_FROM_WIN32(ERROR_INVALID_STATE);
  V_HR(_3dKm->ReleaseSync(0));
  _3dLock = FALSE;
  return S_OK;
}

STDMETHODIMP D3DWTexture::Lock2D()
{
  HRESULT hr;
  if(!_canDraw)
    return S_FALSE;
  if(_2dLock)
    return S_FALSE;
  if(_3dLock)
  {
    V_HR_BREAK("Texture is already locked for 3D");
    return HRESULT_FROM_WIN32(ERROR_INVALID_STATE);
  }
  V_HR(_2dKm->AcquireSync(0, INFINITE));
  _2dLock = TRUE;
  return S_OK;
}

STDMETHODIMP D3DWTexture::Unlock2D()
{
  HRESULT hr;
  if(!_canDraw)
    return S_FALSE;
  if(!_2dLock)
    return HRESULT_FROM_WIN32(ERROR_INVALID_STATE);
  V_HR(_2dKm->ReleaseSync(0));
  _2dLock = FALSE;
  return S_OK;
}

STDMETHODIMP D3DWTexture::GetByteSize(SIZE_T *oSize)
{
  if(!oSize)
    return E_POINTER;
  *oSize = _byteSize;
  return S_OK;
}

STDMETHODIMP D3DWTexture::GetSize(UINT *oWidth, UINT *oHeight)
{
  if(oWidth)
    *oWidth = (UINT)_vp.Width;
  if(oHeight)
    *oHeight = (UINT)_vp.Height;
  return S_OK;
}

STDMETHODIMP D3DWTexture::GetSurface(ID3DWSurface **oSurface)
{
  if(!_canDraw)
    return HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
  if(!oSurface)
    return E_POINTER;
  *oSurface = NULL;
  _surface->AddRef();
  *oSurface = _surface;
  return S_OK;
}

STDMETHODIMP D3DWTexture::SetAsTarget()
{
  if(!_canDraw)
    return HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
  _dc->OMSetRenderTargets(1, _rtv.AddressOf(), _dsv);
  _dc->RSSetViewports(1, &_vp);
  return S_OK;
}

STDMETHODIMP D3DWTexture::Clear(FLOAT clearColor[4])
{
  if(!_canDraw)
    return HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
  if(!clearColor)
    return E_POINTER;
  _dc->ClearRenderTargetView(_rtv, clearColor);
  return S_OK;
}

STDMETHODIMP D3DWTexture::ClearDepthStencil()
{
  if(!_canDraw)
    return HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
  _dc->ClearDepthStencilView(_dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 255);
  return S_OK;
}

STDMETHODIMP D3DWTexture::GetFormat(DXGI_FORMAT *oFormat)
{
  if(!oFormat)
    return E_POINTER;
  *oFormat = _format;
  return S_OK;
}

STDMETHODIMP D3DWTexture::GetRes(ID3D11Resource **oRes)
{
  if(!oRes)
    return E_POINTER;
  *oRes = NULL;
  _tex->AddRef();
  *oRes = _tex;
  return S_OK;
}

STDMETHODIMP D3DWTexture::GetSrv(ID3D11ShaderResourceView **oSrv)
{
  if(!oSrv)
    return E_POINTER;
  *oSrv = NULL;
  _srv->AddRef();
  *oSrv = _srv;
  return S_OK;
}

STDMETHODIMP D3DWTexture::GetRgbaData(UINT width, UINT height, LPVOID oData)
{
  HRESULT hr;
  if(!oData)
    return E_POINTER;

  D3D11_TEXTURE2D_DESC td = {0};
  td.ArraySize = 1;
  td.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
  td.Format = _format;
  td.MipLevels = 1;
  td.Usage = D3D11_USAGE_STAGING;
  td.Width = (UINT)_vp.Width;
  td.Height = (UINT)_vp.Height;
  td.SampleDesc.Count = 1;
  ComPtr<ID3D11Texture2D> copy;
  V_HR_ASSERT(_device->CreateTexture2D(&td, NULL, &copy), "Unable to create staging resource");
  _dc->CopySubresourceRegion(copy, 0, 0, 0, 0, _tex, 0, NULL);
  D3D11_MAPPED_SUBRESOURCE src;
  V_HR_ASSERT(_dc->Map(copy, 0, D3D11_MAP_READ, 0, &src), "Unable to map staging texture");

  WICPixelFormatGUID srcFormat;
  UINT bpp;
  for(int i = 0; i<ARRAYSIZE(_wicTranslate); ++i)
  {
    if(_format == _wicTranslate[i].format)
    {
      srcFormat = _wicTranslate[i].wic;
      goto formatOk;
    }
  }
  _dc->Unmap(copy, 0);
  return E_FAIL;
formatOk:
  hr = GetWicFormatBpp(_wic, srcFormat, &bpp);
  if(FAILED(hr))
  {
    _dc->Unmap(copy, 0);
    return hr;
  }
  UINT srcRowPitch = (td.Width * bpp + 7) / 8;
  UINT srcSize = td.Height * srcRowPitch;
  ComPtr<IWICBitmap> srcBitmap;
  hr = _wic->CreateBitmapFromMemory(td.Width, td.Height, srcFormat, srcRowPitch, srcSize, (BYTE*)src.pData, &srcBitmap);
  _dc->Unmap(copy, 0);
  copy.Release();
  V_HR_ASSERT(hr, "Unable to create WIC bitmap from source bytes");

  WICPixelFormatGUID dstFormat = GUID_WICPixelFormat32bppRGBA;
  UINT dstRowPitch = (width * 32 + 7) / 8;
  UINT dstSize = height * dstRowPitch;

  if(srcFormat == dstFormat && td.Width == width && td.Height == height)
  {
    V_HR(srcBitmap->CopyPixels(NULL, dstRowPitch, dstSize, (BYTE*)oData));
  }
  else if(td.Width != width || td.Height != height)
  {
    ComPtr<IWICBitmapScaler> scaler;
    V_HR(_wic->CreateBitmapScaler(&scaler));
    V_HR_ASSERT(scaler->Initialize(srcBitmap, width, height, WICBitmapInterpolationModeFant),
                "Unable to scale image");
    WICPixelFormatGUID scalerFormat;
    V_HR(scaler->GetPixelFormat(&scalerFormat));
    if(scalerFormat == dstFormat)
    {
      V_HR(scaler->CopyPixels(NULL, dstRowPitch, dstSize, (BYTE*)oData));
    }
    else
    {
      ComPtr<IWICFormatConverter> fc;
      V_HR(_wic->CreateFormatConverter(&fc));
      V_HR_ASSERT(fc->Initialize(
                    scaler,
                    dstFormat,
                    WICBitmapDitherTypeErrorDiffusion,
                    NULL,
                    0,
                    WICBitmapPaletteTypeCustom),
                  "Unable to convert image format to RGBA");
      V_HR(fc->CopyPixels(NULL, dstRowPitch, dstSize, (BYTE*)oData));
    }
  }
  else
  {
    ComPtr<IWICFormatConverter> fc;
    V_HR(_wic->CreateFormatConverter(&fc));
    V_HR_ASSERT(fc->Initialize(
                  srcBitmap,
                  dstFormat,
                  WICBitmapDitherTypeErrorDiffusion,
                  NULL,
                  0,
                  WICBitmapPaletteTypeCustom),
                "Unable to convert image format to RGBA");
    V_HR(fc->CopyPixels(NULL, dstRowPitch, dstSize, (BYTE*)oData));
  }
  return S_OK;
}

HRESULT D3DWTexture::Create(UINT width, UINT height, D3DWContext *ctx, D3DWTexture **oTex)
{
  if(!oTex)
    return E_POINTER;
  *oTex = NULL;
  ComObject<D3DWTexture> *tex = new (std::nothrow) ComObject<D3DWTexture>();
  if(!tex)
    return E_OUTOFMEMORY;
  HRESULT hr = tex->Init(width, height, ctx);
  if(FAILED(hr))
  {
    SafeDelete(tex);
    return hr;
  }
  tex->AddRef();
  *oTex = tex;
  return S_OK;
}

HRESULT D3DWTexture::CreateFromMemory(LPVOID data, SIZE_T size, BOOL canDraw, D3DWContext *ctx, D3DWTexture **oTex)
{
  if(!oTex)
    return E_POINTER;
  *oTex = NULL;
  ComObject<D3DWTexture> *tex = new (std::nothrow) ComObject<D3DWTexture>();
  if(!tex)
    return E_OUTOFMEMORY;
  HRESULT hr = tex->InitFromMemory(data, size, canDraw, ctx);
  if(FAILED(hr))
  {
    SafeDelete(tex);
    return hr;
  }
  tex->AddRef();
  *oTex = tex;
  return S_OK;
}

HRESULT D3DWTexture::CreateFromResource(
  HMODULE module, LPCWSTR resourceName, LPCWSTR resourceType, BOOL canDraw, D3DWContext *ctx, D3DWTexture **oTex)
{
  if(!oTex)
    return E_POINTER;
  *oTex = NULL;
  ComObject<D3DWTexture> *tex = new (std::nothrow) ComObject<D3DWTexture>();
  if(!tex)
    return E_OUTOFMEMORY;
  HRESULT hr = tex->InitFromResource(module, resourceName, resourceType, canDraw, ctx);
  if(FAILED(hr))
  {
    SafeDelete(tex);
    return hr;
  }
  tex->AddRef();
  *oTex = tex;
  return S_OK;
}

HRESULT D3DWTexture::CreateFromFile(LPCWSTR filename, BOOL canDraw, D3DWContext *ctx, D3DWTexture **oTex)
{
  if(!oTex)
    return E_POINTER;
  *oTex = NULL;
  ComObject<D3DWTexture> *tex = new (std::nothrow) ComObject<D3DWTexture>();
  if(!tex)
    return E_OUTOFMEMORY;
  HRESULT hr = tex->InitFromFile(filename, canDraw, ctx);
  if(FAILED(hr))
  {
    SafeDelete(tex);
    return hr;
  }
  tex->AddRef();
  *oTex = tex;
  return S_OK;
}

HRESULT D3DWTexture::Init(UINT width, UINT height, D3DWContext *ctx)
{
  HRESULT hr;
  if(!ctx)
    return E_POINTER;

  _ctx = ctx;
  V_HR(ctx->GetWic(&_wic));
  
  V_HR(ctx->GetDevice(&_device));
  V_HR(ctx->GetDc(&_dc));

  UINT twidth, theight, maxsize;

  switch(_device->GetFeatureLevel())
  {
  case D3D_FEATURE_LEVEL_9_1:
  case D3D_FEATURE_LEVEL_9_2:
    maxsize = 2048 /*D3D_FL9_1_REQ_TEXTURE2D_U_OR_V_DIMENSION*/;
    break;

  case D3D_FEATURE_LEVEL_9_3:
    maxsize = 4096 /*D3D_FL9_3_REQ_TEXTURE2D_U_OR_V_DIMENSION*/;
    break;

  case D3D_FEATURE_LEVEL_10_0:
  case D3D_FEATURE_LEVEL_10_1:
    maxsize = 8192 /*D3D10_REQ_TEXTURE2D_U_OR_V_DIMENSION*/;
    break;

  default:
    maxsize = D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION;
    break;
  }

  if(width > maxsize || height > maxsize)
  {
    FLOAT ar = height / (FLOAT)width;
    if(width > height)
    {
      twidth = maxsize;
      theight = (UINT)(maxsize * ar);
    }
    else
    {
      theight = maxsize;
      twidth = (UINT)(maxsize / ar);
    }
  }
  else
  {
    twidth = width;
    theight = height;
  }

  _format = DXGI_FORMAT_B8G8R8A8_UNORM;
  _canDraw = TRUE;
  _byteSize = twidth * theight * 8;

  _vp.MinDepth = 0;
  _vp.MaxDepth = 0;
  _vp.TopLeftX = 0;
  _vp.TopLeftY = 0;
  _vp.Width = (FLOAT)twidth;
  _vp.Height = (FLOAT)theight;

  D3D11_TEXTURE2D_DESC td = {0};
  td.ArraySize = 1;
  td.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
  td.Width = twidth;
  td.Height = theight;
  td.Format = _format;
  td.MipLevels = 1;
  td.SampleDesc.Count = 1;
  td.SampleDesc.Quality = 0;
  td.MiscFlags = D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX;
  td.Usage = D3D11_USAGE_DEFAULT;
  V_HR(_device->CreateTexture2D(&td, NULL, &_tex));

  D3D11_SHADER_RESOURCE_VIEW_DESC srvD;
  SecureZeroMemory(&srvD, sizeof(srvD));
  srvD.Format = _format;
  srvD.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
  srvD.Texture2D.MipLevels = 1;
  srvD.Texture2D.MostDetailedMip = 0;
  V_HR(_device->CreateShaderResourceView(_tex, &srvD, &_srv));

  V_HR(_tex.QueryInterface(&_3dKm));
  V_HR_ASSERT(_device->CreateRenderTargetView(_tex, NULL, &_rtv), "Unable to create texture RTV");
  td.BindFlags = D3D11_BIND_DEPTH_STENCIL;
  td.Format = DXGI_FORMAT_D32_FLOAT;
  td.MiscFlags = 0;
  ComPtr<ID3D11Texture2D> ds;
  V_HR_ASSERT(_device->CreateTexture2D(&td, NULL, &ds), "Unable to create texture depth stencil");
  V_HR_ASSERT(_device->CreateDepthStencilView(ds, NULL, &_dsv), "Unable to create texture dsv");
  V_HR_ASSERT(D3DWSurface::Create(_ctx, this, &_surface, &_2dKm), "Unable to create drawing surface");

  return S_OK;
}

HRESULT D3DWTexture::InitFromMemory(LPVOID data, SIZE_T size, BOOL canDraw, D3DWContext *ctx)
{
  HRESULT hr;

  if(!ctx)
    return E_POINTER;
  V_HR(ctx->GetWic(&_wic));

  ComPtr<IWICStream> stream;
  V_HR(_wic->CreateStream(&stream));

  V_HR_ASSERT(stream->InitializeFromMemory((WICInProcPointer)data, (DWORD)size), "Unable to create WIC memory stream");

  ComPtr<IWICBitmapDecoder> decoder;
  V_HR_ASSERT(_wic->CreateDecoderFromStream(
                stream,
                NULL,
                WICDecodeMetadataCacheOnDemand,
                &decoder),
              "Unable to create WIC decoder from memory stream");

  V_HR(InitFromWicDecoder(canDraw, decoder, ctx));

  return S_OK;
}

HRESULT D3DWTexture::InitFromResource(HMODULE module, LPCWSTR resourceName, LPCWSTR resourceType, BOOL canDraw, D3DWContext *ctx)
{
  if(!module || !resourceName || !resourceType)
    return E_POINTER;

  HRSRC hrsrc = FindResource(module, resourceName, resourceType);
  if(!hrsrc)
    return HRESULT_FROM_WIN32(GetLastError());
  HGLOBAL res = LoadResource(module, hrsrc);
  if(!res)
    return HRESULT_FROM_WIN32(GetLastError());
  LPVOID data = LockResource(res);
  if(!data)
    return HRESULT_FROM_WIN32(GetLastError());
  SIZE_T size = SizeofResource(module, hrsrc);
  
  return InitFromMemory(data, size, canDraw, ctx);
}

HRESULT D3DWTexture::InitFromFile(LPCWSTR filename, BOOL canDraw, D3DWContext *ctx)
{
  HRESULT hr;
  if(!filename || !ctx)
    return E_POINTER; 

  V_HR(ctx->GetWic(&_wic));

  ComPtr<IWICBitmapDecoder> decoder;
  V_HR_ASSERT(_wic->CreateDecoderFromFilename(
                filename,
                NULL,
                GENERIC_READ,
                WICDecodeMetadataCacheOnDemand,
                &decoder),
              "Unable to create WIC decoder from filename");

  V_HR(InitFromWicDecoder(canDraw, decoder, ctx));

  return S_OK;
}

HRESULT D3DWTexture::InitFromWicDecoder(BOOL canDraw, IWICBitmapDecoder *decoder, D3DWContext *ctx)
{
  HRESULT hr;
  if(!decoder) 
    return E_POINTER;

  _ctx = ctx;
  _canDraw = canDraw;

  V_HR(ctx->GetDevice(&_device));
  V_HR(ctx->GetDc(&_dc));

  ComPtr<IWICBitmapFrameDecode> frame;
  V_HR_ASSERT(decoder->GetFrame(0, &frame), "Unable to get WIC bitmap frame");
  
  UINT width, height, maxsize, twidth, theight;
  V_HR(frame->GetSize(&width, &height));

  switch(_device->GetFeatureLevel())
  {
  case D3D_FEATURE_LEVEL_9_1:
  case D3D_FEATURE_LEVEL_9_2:
    maxsize = 2048 /*D3D_FL9_1_REQ_TEXTURE2D_U_OR_V_DIMENSION*/;
    break;

  case D3D_FEATURE_LEVEL_9_3:
    maxsize = 4096 /*D3D_FL9_3_REQ_TEXTURE2D_U_OR_V_DIMENSION*/;
    break;

  case D3D_FEATURE_LEVEL_10_0:
  case D3D_FEATURE_LEVEL_10_1:
    maxsize = 8192 /*D3D10_REQ_TEXTURE2D_U_OR_V_DIMENSION*/;
    break;

  default:
    maxsize = D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION;
    break;
  }

  if(width > maxsize || height > maxsize)
  {
    FLOAT ar = height / (FLOAT)width;
    if(width > height)
    {
      twidth = maxsize;
      theight = (UINT)(maxsize * ar);
    }
    else
    {
      theight = maxsize;
      twidth = (UINT)(maxsize / ar);
    }
  }
  else
  {
    twidth = width;
    theight = height;
  }
  
  WICPixelFormatGUID pf;
  V_HR(frame->GetPixelFormat(&pf));

  WICPixelFormatGUID convertPf = pf;

  UINT bpp;

  hr = TranslateWicPixelFormat(pf, &_format);
  if(FAILED(hr))
  {
    for(int i = 0; i<ARRAYSIZE(_wicConvert); ++i)
    {
      if(_wicConvert[i].source == pf)
      {
        convertPf = _wicConvert[i].target;
        V_HR(TranslateWicPixelFormat(convertPf, &_format));
        V_HR(GetWicFormatBpp(_wic, convertPf, &bpp));
        break;
      }
    }
    if(DXGI_FORMAT_UNKNOWN == _format)
      return HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
  }
  else
  {
    V_HR(GetWicFormatBpp(_wic, pf, &bpp));
  }

  UINT support;
  if(canDraw)
  {
    convertPf = GUID_WICPixelFormat32bppBGRA;
    _format = DXGI_FORMAT_B8G8R8A8_UNORM;
    bpp = 32;
  }
  else
  {
    hr = _device->CheckFormatSupport(_format, &support);
    if(FAILED(hr) || !(support & D3D11_FORMAT_SUPPORT_TEXTURE2D))
    {
      convertPf = GUID_WICPixelFormat32bppRGBA;
      _format = DXGI_FORMAT_R8G8B8A8_UNORM;
      bpp = 32;
    }
  }

  UINT rowPitch = ( twidth * bpp + 7 ) / 8;
  UINT imageSize = theight * rowPitch;
  _byteSize = imageSize;

  std::unique_ptr<BYTE[]> buffer(new BYTE[imageSize]);

  if(pf == convertPf && twidth == width && theight == height)
  {
    frame->CopyPixels(NULL, rowPitch, imageSize, buffer.get());
  }
  else if(twidth != width || theight != height)
  {
    ComPtr<IWICBitmapScaler> scaler;
    V_HR(_wic->CreateBitmapScaler(&scaler));
    V_HR_ASSERT(scaler->Initialize(frame, twidth, theight, WICBitmapInterpolationModeFant),
                "Unable to initialize WIC bitmap scaler");
    
    WICPixelFormatGUID scalerPf;
    V_HR(scaler->GetPixelFormat(&scalerPf));
    if(scalerPf == convertPf)
    {
      V_HR(scaler->CopyPixels(NULL, rowPitch, imageSize, buffer.get()));
    }
    else
    {
      ComPtr<IWICFormatConverter> fc;
      V_HR(_wic->CreateFormatConverter(&fc));
      V_HR_ASSERT(fc->Initialize(
                    scaler,
                    convertPf,
                    WICBitmapDitherTypeErrorDiffusion,
                    NULL,
                    0,
                    WICBitmapPaletteTypeCustom),
                  "Unable to initialize WIC format converter.");
      V_HR(fc->CopyPixels(NULL, rowPitch, imageSize, buffer.get()));
    }
  }
  else
  {
    ComPtr<IWICFormatConverter> fc;
    V_HR(_wic->CreateFormatConverter(&fc));
    V_HR_ASSERT(fc->Initialize(
                  frame,
                  convertPf,
                  WICBitmapDitherTypeErrorDiffusion,
                  NULL,
                  0,
                  WICBitmapPaletteTypeCustom),
                "Unable to initialize WIC format converter.");
    V_HR(fc->CopyPixels(NULL, rowPitch, imageSize, buffer.get()));
  }

  BOOL genMips = FALSE;
  if(!canDraw)
  {
    hr = _device->CheckFormatSupport(_format, &support);
    if(SUCCEEDED(hr) || (support & D3D11_FORMAT_SUPPORT_MIP_AUTOGEN))
      genMips = TRUE;
  }

  D3D11_TEXTURE2D_DESC td = {0};
  td.MipLevels = genMips ? 0 : 1;
  td.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
  td.ArraySize = 1;
  td.Format = _format;
  td.Width = twidth;
  td.Height = theight;
  td.Usage = D3D11_USAGE_DEFAULT;
  td.SampleDesc.Count = 1;
  td.SampleDesc.Quality = 0;
  td.MiscFlags = genMips ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;
  if(canDraw)
    td.MiscFlags = D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX;

  D3D11_SUBRESOURCE_DATA sd;
  sd.pSysMem = buffer.get();
  sd.SysMemPitch = rowPitch;
  sd.SysMemSlicePitch = imageSize;
  
  V_HR_ASSERT(_device->CreateTexture2D(&td, genMips ? NULL : &sd, &_tex), "Unable to create texture");

  D3D11_SHADER_RESOURCE_VIEW_DESC srvD;
  SecureZeroMemory(&srvD, sizeof(srvD));
  srvD.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
  srvD.Format = _format;
  srvD.Texture2D.MipLevels = genMips ? -1 : 1;

  V_HR_ASSERT(_device->CreateShaderResourceView(_tex, &srvD, &_srv), "Unable to create shader resource view");

  if(genMips)
  {
    _dc->UpdateSubresource(_tex, 0, NULL, sd.pSysMem, sd.SysMemPitch, sd.SysMemSlicePitch);
    _dc->GenerateMips(_srv);
  }

  _vp.MinDepth = 0;
  _vp.MaxDepth = 1;
  _vp.TopLeftX = 0;
  _vp.TopLeftY = 0;
  _vp.Width = (FLOAT)twidth;
  _vp.Height = (FLOAT)theight;

  if(canDraw)
  {
    V_HR(_tex.QueryInterface(&_3dKm));
    V_HR_ASSERT(_device->CreateRenderTargetView(_tex, NULL, &_rtv), "Unable to create texture RTV");
    td.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    td.Format = DXGI_FORMAT_D32_FLOAT;
    td.MipLevels = 1;
    td.MiscFlags = 0;
    ComPtr<ID3D11Texture2D> ds;
    V_HR_ASSERT(_device->CreateTexture2D(&td, NULL, &ds), "Unable to create texture depth stencil");
    V_HR_ASSERT(_device->CreateDepthStencilView(ds, NULL, &_dsv), "Unable to create texture dsv");
    V_HR_ASSERT(D3DWSurface::Create(_ctx, this, &_surface, &_2dKm), "Unable to create drawing surface");
  }

  return S_OK;
}

HRESULT WINAPI D3DWTexture::GetWicFormatBpp(IWICImagingFactory *factory, REFGUID wicFormat, UINT *oBpp)
{
  HRESULT hr;
  if(!factory || !oBpp)
    return E_POINTER;
  *oBpp = 0;

  ComPtr<IWICComponentInfo> ci;
  V_HR_ASSERT(factory->CreateComponentInfo(wicFormat, &ci), "Unable to create WIC component info");

  WICComponentType wicType;
  V_HR(ci->GetComponentType(&wicType));

  if(WICPixelFormat != wicType)
    return E_INVALIDARG;

  ComPtr<IWICPixelFormatInfo> pfi;
  V_HR(ci.QueryInterface(&pfi));

  UINT bpp;
  V_HR(pfi->GetBitsPerPixel(&bpp));
  if(!bpp)
    return E_FAIL;
  *oBpp = bpp;
  return S_OK;
}

HRESULT WINAPI D3DWTexture::TranslateWicPixelFormat(REFGUID wicFormat, DXGI_FORMAT *oFormat)
{
  if(!oFormat)
    return E_POINTER;
  DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
  *oFormat = format;
  for(int i = 0; i<ARRAYSIZE(_wicTranslate); ++i)
  {
    if(_wicTranslate[i].wic == wicFormat)
    {
      format = _wicTranslate[i].format;
      break;
    }
  }
  if(DXGI_FORMAT_UNKNOWN == format)
    return HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
  *oFormat = format;
  return S_OK;
}