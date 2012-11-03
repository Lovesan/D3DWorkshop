#ifndef __D3DWORKSHOP_INTERNALS_HPP__
#define __D3DWORKSHOP_INTERNALS_HPP__

#include <windows.h>
#include <d3d11.h>
#include "D3DW.h"

typedef interface ID3DWContextInternals ID3DWContextInternals;
typedef interface ID3DWEffectInternals ID3DWEffectInternals;
typedef interface ID3DWTextureInternals ID3DWTextureInternals;

MIDL_INTERFACE("{957439E0-1BFF-4B2D-8191-7AD58B8951BE}")
ID3DWContextInternals : public IUnknown
{
public:  
  STDMETHOD(GetSwapChain)(IDXGISwapChain **oSwc) = 0;
  STDMETHOD(SetAsTarget)() = 0;
  STDMETHOD(Present)() = 0;
};

MIDL_INTERFACE("{2F5BE71D-C108-47C6-BBBC-C12489E74162}")
ID3DWEffectInternals : public IUnknown
{
public:
  STDMETHOD(Apply)() = 0;
  STDMETHOD(LockTextures)(ID3DWTextureInternals *output) = 0;
  STDMETHOD(UnlockTextures)() = 0;
};

MIDL_INTERFACE("{246D8AB9-EC22-4A5C-9C39-A8E968EB47DF}")
ID3DWTextureInternals : public IUnknown
{
public:  
  STDMETHOD(GetRgbaData)(UINT width, UINT height, LPVOID oData) = 0;
  STDMETHOD(Lock3D)() = 0;
  STDMETHOD(Unlock3D)() = 0;
  STDMETHOD(Lock2D)() = 0;
  STDMETHOD(Unlock2D)() = 0;
  STDMETHOD(GetFormat)(DXGI_FORMAT *oFormat) = 0;
  STDMETHOD(GetRes)(ID3D11Resource **oRes) = 0;
  STDMETHOD(GetSrv)(ID3D11ShaderResourceView **oSrv) = 0;
  STDMETHOD(SetAsTarget)() = 0;
};

#endif // __D3DWORKSHOP_INTERNALS_HPP__