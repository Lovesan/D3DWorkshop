#ifndef __D3DWSURFACE_HPP__
#define __D3DWSURFACE_HPP__

#include <windows.h>
#include <d3d11.h>
#include <d2d1.h>
#include <d2derr.h>
#include "ComUtils.hpp"
#include "D3DW.h"

#ifndef __D3DWCONTEXT_DEFINED__
class D3DWContext;
#endif // __D3DWCONTEXT_DEFINED__

#ifndef __D3DWTEXTURE_DEFINED__
class D3DWTexture;
#endif // __D3DWTEXTURE_DEFINED__

class D3DWSurface : public ID3DWSurface
{
public:
  BEGIN_INTERFACE_MAP
    INTERFACE_MAP_ENTRY(ID3DWSurface)
  END_INTERFACE_MAP

  static HRESULT Create(D3DWContext *ctx, D3DWTexture *tex, D3DWSurface **oSurface, IDXGIKeyedMutex **oKm);
  virtual ~D3DWSurface();

  STDMETHODIMP GetTexture(ID3DWTexture **oTex);
  STDMETHODIMP GetSize(UINT *oWIdth, UINT *oHeight);
  STDMETHODIMP BeginDraw();
  STDMETHODIMP EndDraw();
  STDMETHODIMP Clear(const FLOAT clearColor[4]);
  STDMETHODIMP ClearDepthStencil();

protected:
  D3DWSurface();

private:
  ComPtr<D3DWContext> _ctx;
  D3DWTexture *_tex;
  ComPtr<ID2D1Factory> _factory;
  ComPtr<ID2D1RenderTarget> _rt;
  ComPtr<ID2D1SolidColorBrush> _clearBrush;
  BOOL _drawing;
  UINT _width, _height;

  HRESULT Initialize(D3DWContext *ctx, D3DWTexture *tex, IDXGIKeyedMutex **oKm);
};

#define __D3DWSURFACE_DEFINED__

#endif // __D3DWSURFACE_HPP__