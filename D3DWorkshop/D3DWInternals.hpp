#ifndef __D3DWORKSHOP_INTERNALS_HPP__
#define __D3DWORKSHOP_INTERNALS_HPP__

#include <windows.h>
#include <d3d11.h>
#include "D3DW.h"

typedef interface ID3DWContextInternals ID3DWContextInternals;
typedef interface ID3DWEffectInternals ID3DWEffectInternals;
typedef interface ID3DWSamplerInternals ID3DWSamplerInternals;
typedef interface ID3DWResourceInternals ID3DWResourceInternals;
typedef interface ID3DWConstantBufferInternals ID3DWConstantBufferInternals;
typedef interface ID3DWShaderResourceInternals ID3DWShaderResourceInternals;
typedef interface ID3DWRenderTargetInternals ID3DWRenderTargetInternals;
typedef interface ID3DWBufferInternals ID3DWBufferInternals;
typedef interface ID3DWTextureInternals ID3DWTextureInternals;
typedef interface ID3DWCubeMapInternals ID3DWCubeMapInternals;

MIDL_INTERFACE("{2F5BE71D-C108-47C6-BBBC-C12489E74162}")
ID3DWEffectInternals : public IUnknown
{
public:
  STDMETHOD(Apply)() = 0;
  STDMETHOD(LockTargets)(ID3DWRenderTargetInternals *output) = 0;
  STDMETHOD(UnlockTargets)() = 0;
};

MIDL_INTERFACE("{9A0AE7E7-0958-4E6D-BFDD-090F9B53EB24}")
ID3DWSamplerInternals : public IUnknown
{
public:
  STDMETHOD(GetSamplerState)(ID3D11SamplerState **oState) = 0;
};

MIDL_INTERFACE("{9B1F4DAF-22EA-464D-A9E1-11FEFDF4A64C}")
ID3DWResourceInternals : public IUnknown
{
public:
  STDMETHOD(GetRes)(ID3D11Resource **oRes) = 0;
};

MIDL_INTERFACE("{9ABC14BE-F35B-4326-A766-497B798105CC}")
ID3DWConstantBufferInternals : public ID3DWResourceInternals
{
public:
  STDMETHOD(GetCb)(ID3D11Buffer **oCb) = 0;
};

MIDL_INTERFACE("{E605CC33-ABD4-4E30-977C-5A5749DF6855}")
ID3DWShaderResourceInternals : public ID3DWResourceInternals
{
public:  
  STDMETHOD(GetSrv)(ID3D11ShaderResourceView **oSrv) = 0;
};

MIDL_INTERFACE("{89CC18BC-5717-4B8C-AEC2-A19275874FC8}")
ID3DWRenderTargetInternals : public ID3DWShaderResourceInternals
{
public:
  STDMETHOD(GetRtv)(ID3D11RenderTargetView **oRtv) = 0;
  STDMETHOD(SetAsTarget)() = 0;
};

MIDL_INTERFACE("{957439E0-1BFF-4B2D-8191-7AD58B8951BE}")
ID3DWContextInternals : public ID3DWRenderTargetInternals
{
public:  
  STDMETHOD(GetSwapChain)(IDXGISwapChain **oSwc) = 0;
  STDMETHOD(Present)() = 0;
};

MIDL_INTERFACE("{3AC36A52-EEA0-450A-A523-9B330AC5F8C0}")
ID3DWBufferInternals : public ID3DWRenderTargetInternals
{
public:
};

MIDL_INTERFACE("{246D8AB9-EC22-4A5C-9C39-A8E968EB47DF}")
ID3DWTextureInternals : public ID3DWRenderTargetInternals
{
public:  
  STDMETHOD(GetRgbaData)(UINT width, UINT height, LPVOID oData) = 0;
  STDMETHOD(Lock3D)() = 0;
  STDMETHOD(Unlock3D)() = 0;
  STDMETHOD(Lock2D)() = 0;
  STDMETHOD(Unlock2D)() = 0;
};

MIDL_INTERFACE("{4827D7ED-3159-459F-B149-997148C5CA43}")
ID3DWCubeMapInternals : public ID3DWRenderTargetInternals
{
public:
};

#endif // __D3DWORKSHOP_INTERNALS_HPP__