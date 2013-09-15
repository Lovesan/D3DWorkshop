#ifndef __D3DWSAMPLER_HPP__
#define __D3DWSAMPLER_HPP__

#include <windows.h>
#include <d3d11.h>
#include "ComUtils.hpp"
#include "D3DW.h"
#include "D3DWInternals.hpp"

#ifndef __D3DWCONTEXT_DEFINED__
class D3DWContext;
#endif // __D3DWCONTEXT_DEFINED__

class COM_NO_VTABLE D3DWSampler :
  public ID3DWSampler,
  public ID3DWSamplerInternals
{
public:
  BEGIN_INTERFACE_MAP
    INTERFACE_MAP_ENTRY(ID3DWContextChild)
    INTERFACE_MAP_ENTRY(ID3DWSampler)
    INTERFACE_MAP_ENTRY(ID3DWSamplerInternals)
  END_INTERFACE_MAP

  static HRESULT Create(
    UINT anisotropyLevel, D3DW_TEXTURE_MODE addressMode, const FLOAT borderColor[4], D3DWContext *ctx, D3DWSampler **oSampler);
  virtual ~D3DWSampler();

  STDMETHODIMP GetContext(ID3DWContext **oCtx);
  STDMETHODIMP GetSamplerState(ID3D11SamplerState **oSamState);
  STDMETHODIMP GetAnisotropyLevel(UINT *oLevel);
  STDMETHODIMP SetAnisotropyLevel(UINT level);
  STDMETHODIMP GetAddressMode(D3DW_TEXTURE_MODE *oMode);
  STDMETHODIMP SetAddressMode(D3DW_TEXTURE_MODE mode);
  STDMETHODIMP GetBorderColor(FLOAT oColor[4]);
  STDMETHODIMP SetBorderColor(const FLOAT color[4]);

protected:
  D3DWSampler();

private:
  ComPtr<D3DWContext> _ctx;
  ComPtr<ID3D11Device> _device;
  ComPtr<ID3D11DeviceContext> _dc;
  ComPtr<ID3D11SamplerState> _samState;  
  UINT _anisotropyLevel;
  D3DW_TEXTURE_MODE _addressMode;
  FLOAT _borderColor[4];

  HRESULT Initialize(UINT anisotropyLevel, D3DW_TEXTURE_MODE addressMode, const FLOAT borderColor[4], D3DWContext *ctx);
  HRESULT RecreateSamplerState(UINT anisotropyLevel, D3DW_TEXTURE_MODE addressMode, const FLOAT borderColor[4], ID3D11SamplerState **oSamState);
};

#define __D3DWSAMPLER_DEFINED__

#endif // __D3DWSAMPLER_HPP__