#ifndef __D3DWTEXTURE_HPP__
#define __D3DWTEXTURE_HPP__

#include <windows.h>
#include <wincodec.h>
#include <d3d11.h>
#include "ComUtils.hpp"
#include "D3DW.h"
#include "D3DWInternals.hpp"

#ifndef __D3DWCONTEXT_DEFINED__
class D3DWContext;
#endif // __D3DWCONTEXT_DEFINED__

#ifndef __D3DWSURFACE_DEFINED
class D3DWSurface;
#endif // __D3DWSURFACE_DEFINED__

class COM_NO_VTABLE D3DWTexture :
  public ID3DWTexture,
  public ID3DWTextureInternals
{
public:
  BEGIN_INTERFACE_MAP
    INTERFACE_MAP_ENTRY(ID3DWContextChild)
    INTERFACE_MAP_ENTRY(ID3DWResource)
    INTERFACE_MAP_ENTRY(ID3DWTexture)
    INTERFACE_MAP_ENTRY(ID3DWResourceInternals)
    INTERFACE_MAP_ENTRY(ID3DWShaderResourceInternals)
    INTERFACE_MAP_ENTRY(ID3DWRenderTargetInternals)
    INTERFACE_MAP_ENTRY(ID3DWTextureInternals)
  END_INTERFACE_MAP

  static HRESULT Create(UINT width, UINT height, D3DWContext *ctx, D3DWTexture **oTex);
  static HRESULT CreateFromMemory(
    LPVOID data, SIZE_T size, BOOL canDraw, D3DWContext *ctx, D3DWTexture **oTex);
  static HRESULT CreateFromResource(
    HMODULE module, LPCWSTR resourceName, LPCWSTR resourceType, BOOL canDraw, D3DWContext *ctx, D3DWTexture **oTex);
  static HRESULT CreateFromFile(
    LPCWSTR filename, BOOL canDraw, D3DWContext *ctx, D3DWTexture **oTex);
  virtual ~D3DWTexture();
    
  static HRESULT WINAPI GetWicFormatBpp(IWICImagingFactory *factory, REFGUID wicFormat, UINT *oBpp);
  static HRESULT WINAPI TranslateWicPixelFormat(REFGUID wicFormat, DXGI_FORMAT *oFormat);

  STDMETHODIMP GetRgbaData(UINT width, UINT height, LPVOID oData);
  STDMETHODIMP Lock3D();
  STDMETHODIMP Unlock3D();
  STDMETHODIMP Lock2D();
  STDMETHODIMP Unlock2D();
  STDMETHODIMP GetFormat(DXGI_FORMAT *oFormat);
  STDMETHODIMP GetRes(ID3D11Resource **oRes);
  STDMETHODIMP GetSrv(ID3D11ShaderResourceView **oSrv);
  STDMETHODIMP GetRtv(ID3D11RenderTargetView **oRtv);
  STDMETHODIMP SetAsTarget();
  STDMETHODIMP Clear(const FLOAT clearColor[4]);
  STDMETHODIMP ClearDepthStencil();
  STDMETHODIMP GetContext(ID3DWContext **oCtx);
  STDMETHODIMP GetByteSize(SIZE_T *oSize);
  STDMETHODIMP GetSize(UINT *oWidth, UINT *oHeight);
  STDMETHODIMP GetSurface(ID3DWSurface **oSurface);

protected:
  D3DWTexture();

private:
  ComPtr<D3DWContext> _ctx;
  ComPtr<IWICImagingFactory> _wic;
  ComPtr<ID3D11Device> _device;
  ComPtr<ID3D11DeviceContext> _dc;
  ComPtr<ID3D11Texture2D> _tex;
  ComPtr<ID3D11ShaderResourceView> _srv;
  ComPtr<ID3D11RenderTargetView> _rtv;
  ComPtr<ID3D11DepthStencilView> _dsv;
  D3D11_VIEWPORT _vp;
  DXGI_FORMAT _format;
  BOOL _canDraw;
  ComPtr<IDXGIKeyedMutex> _3dKm;
  ComPtr<IDXGIKeyedMutex> _2dKm;
  D3DWSurface *_surface;
  BOOL _3dLock, _2dLock;
  SIZE_T _byteSize;

  HRESULT Init(UINT width, UINT height, D3DWContext *ctx);
  HRESULT InitFromMemory(LPVOID data, SIZE_T size, BOOL canDraw, D3DWContext *ctx);
  HRESULT InitFromResource(HMODULE module, LPCWSTR resourceName, LPCWSTR resourceType, BOOL canDraw, D3DWContext *ctx);
  HRESULT InitFromFile(LPCWSTR filename, BOOL canDraw, D3DWContext *ctx);
  HRESULT InitFromWicDecoder(BOOL canDraw, IWICBitmapDecoder *decoder, D3DWContext *ctx);

  struct WicTranslate
  {
    GUID                wic;
    DXGI_FORMAT         format;
  };
  struct WicConvert
  {
    GUID        source;
    GUID        target;
  };
  static const WicTranslate _wicTranslate[];
  static const WicConvert _wicConvert[];  
};

#define __D3DWTEXTURE_DEFINED__

#endif // __D3DWTEXTURE_HPP__