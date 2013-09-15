#ifndef __D3DWCUBEMAP_HPP__
#define __D3DWCUBEMAP_HPP__

#include <windows.h>
#include "ComUtils.hpp"
#include "D3DW.h"
#include "D3DWInternals.hpp"

#ifndef __D3DWCONTEXT_DEFINED__
class D3DWContext;
#endif // __D3DWCONTEXT_DEFINED__

#ifndef __D3DWTEXTURE_DEFINED__
class D3DWTexture;
#endif // __D3DWTEXTURE_DEFINED__

class COM_NO_VTABLE D3DWCubeMap :
  public ID3DWCubeMap,
  public ID3DWCubeMapInternals
{
public:
  BEGIN_INTERFACE_MAP    
    INTERFACE_MAP_ENTRY(ID3DWContextChild)
    INTERFACE_MAP_ENTRY(ID3DWResource)
    INTERFACE_MAP_ENTRY(ID3DWCubeMap)
    INTERFACE_MAP_ENTRY(ID3DWResourceInternals)
    INTERFACE_MAP_ENTRY(ID3DWShaderResourceInternals)
    INTERFACE_MAP_ENTRY(ID3DWRenderTargetInternals)
    INTERFACE_MAP_ENTRY(ID3DWCubeMapInternals)
  END_INTERFACE_MAP
  
  static HRESULT Create(
    ID3DWTexture *posX,
    ID3DWTexture *negX,
    ID3DWTexture *posY,
    ID3DWTexture *negY,
    ID3DWTexture *posZ,
    ID3DWTexture *negZ,
    D3DWContext *ctx,
    D3DWCubeMap **oCubeMap);
  virtual ~D3DWCubeMap();

  STDMETHODIMP GetRes(ID3D11Resource **oRes);
  STDMETHODIMP GetSrv(ID3D11ShaderResourceView **oSrv);
  STDMETHODIMP GetRtv(ID3D11RenderTargetView **oRtv);
  STDMETHODIMP SetAsTarget();
  STDMETHODIMP Clear(const FLOAT clearColor[4]);
  STDMETHODIMP ClearDepthStencil();
  STDMETHODIMP GetContext(ID3DWContext **oCtx);
  STDMETHODIMP GetByteSize(SIZE_T *oSize);
  STDMETHODIMP GetSideSize(UINT *oSize);

protected:
  D3DWCubeMap();

private:
  ComPtr<D3DWContext> _ctx;
  ComPtr<ID3D11Device> _device;
  ComPtr<ID3D11DeviceContext> _dc;
  ComPtr<ID3D11Texture2D> _tex;
  ComPtr<ID3D11ShaderResourceView> _srv;
  ComPtr<ID3D11RenderTargetView> _rtv;
  ComPtr<ID3D11DepthStencilView> _dsv;
  D3D11_VIEWPORT _vp;
  SIZE_T _byteSize;

  HRESULT Initialize(
    ID3DWTexture *posX,
    ID3DWTexture *negX,
    ID3DWTexture *posY,
    ID3DWTexture *negY,
    ID3DWTexture *posZ,
    ID3DWTexture *negZ,
    D3DWContext *ctx);
};

#define __D3DWCUBEMAP_DEFINED__

#endif // __D3DWCUBEMAP_HPP__