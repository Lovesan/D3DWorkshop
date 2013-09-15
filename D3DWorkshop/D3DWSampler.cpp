#include "StdAfx.h"
#include "D3DWSampler.hpp"
#include "D3DWContext.hpp"

D3DWSampler::D3DWSampler()
{
}

D3DWSampler::~D3DWSampler()
{
}

STDMETHODIMP D3DWSampler::GetContext(ID3DWContext **oCtx)
{
  if(!oCtx)
    return E_POINTER;
  *oCtx = NULL;
  _ctx->AddRef();
  *oCtx = _ctx;
  return S_OK;
}

STDMETHODIMP D3DWSampler::GetSamplerState(ID3D11SamplerState **oSamState)
{
  if(!oSamState)
    return E_POINTER;
  *oSamState = NULL;
  _samState->AddRef();
  *oSamState = _samState;
  return S_OK;
}

STDMETHODIMP D3DWSampler::GetAnisotropyLevel(UINT *oLevel)
{
  if(!oLevel)
    return E_POINTER;
  *oLevel = _anisotropyLevel;
  return S_OK;
}

STDMETHODIMP D3DWSampler::SetAnisotropyLevel(UINT level)
{
  HRESULT hr;
  ComPtr<ID3D11SamplerState> state;
  V_HR(RecreateSamplerState(level, _addressMode, _borderColor, &state));
  _samState = state;
  _anisotropyLevel = level;
  return S_OK;
}

STDMETHODIMP D3DWSampler::GetAddressMode(D3DW_TEXTURE_MODE *oMode)
{
  if(!oMode)
    return E_POINTER;
  *oMode = _addressMode;
  return S_OK;
}

STDMETHODIMP D3DWSampler::SetAddressMode(D3DW_TEXTURE_MODE mode)
{
  HRESULT hr;
  ComPtr<ID3D11SamplerState> state;
  V_HR(RecreateSamplerState(_anisotropyLevel, mode, _borderColor, &state));
  _samState = state;
  _addressMode = mode;
  return S_OK;
}

STDMETHODIMP D3DWSampler::GetBorderColor(FLOAT oColor[4])
{
  if(!oColor)
    return E_POINTER;
  oColor[0] = _borderColor[0];
  oColor[1] = _borderColor[1];
  oColor[2] = _borderColor[2];
  oColor[3] = _borderColor[3];
  return S_OK;
}

STDMETHODIMP D3DWSampler::SetBorderColor(const FLOAT color[4])
{
  HRESULT hr;
  ComPtr<ID3D11SamplerState> state;
  V_HR(RecreateSamplerState(_anisotropyLevel, _addressMode, color, &state));
  _samState = state;
  if(color)
  {
    _borderColor[0] = color[0];
    _borderColor[1] = color[1];
    _borderColor[2] = color[2];
    _borderColor[3] = color[3];
  }
  else
  {
    _borderColor[0] = 
    _borderColor[1] =
    _borderColor[2] = 
    _borderColor[3] = 0;
  }
  return S_OK;
}

HRESULT D3DWSampler::Create(
  UINT anisotropyLevel, D3DW_TEXTURE_MODE addressMode, const FLOAT borderColor[4], D3DWContext *ctx, D3DWSampler **oSampler)
{
  if(!oSampler)
    return E_POINTER;
  *oSampler = NULL;
  ComObject<D3DWSampler> *sampler = new (std::nothrow) ComObject<D3DWSampler>();
  if(!sampler)
    return E_OUTOFMEMORY;
  HRESULT hr = sampler->Initialize(anisotropyLevel, addressMode, borderColor, ctx);
  if(FAILED(hr))
  {
    SafeDelete(sampler);
    return hr;
  }
  sampler->AddRef();
  *oSampler = sampler;
  return S_OK;
}

HRESULT D3DWSampler::Initialize(UINT anisotropyLevel, D3DW_TEXTURE_MODE addressMode, const FLOAT borderColor[4], D3DWContext *ctx)
{
  HRESULT hr;
  if(!ctx)
    return E_POINTER;
  _ctx = ctx;
  V_HR(_ctx->GetDevice(&_device));
  V_HR(_ctx->GetDc(&_dc));
  ComPtr<ID3D11SamplerState> state;
  V_HR(RecreateSamplerState(anisotropyLevel, addressMode, borderColor, &state));
  _samState = state;
  return S_OK;
}

HRESULT D3DWSampler::RecreateSamplerState(
  UINT anisotropyLevel, D3DW_TEXTURE_MODE addressMode, const FLOAT borderColor[4], ID3D11SamplerState **oSamState)
{
  HRESULT hr;
  if(!oSamState)
    return E_POINTER;
  *oSamState = NULL;

  D3D11_SAMPLER_DESC sd;
  SecureZeroMemory(&sd, sizeof(sd));
  switch(addressMode)
  {
  case D3DW_TEXTURE_MODE_WRAP:
    sd.AddressU = sd.AddressV = sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    break;
  case D3DW_TEXTURE_MODE_MIRROR:
    sd.AddressU = sd.AddressV = sd.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
    break;
  case D3DW_TEXTURE_MODE_CLAMP:
    sd.AddressU = sd.AddressV = sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    break;
  case D3DW_TEXTURE_MODE_BORDER:
    sd.AddressU = sd.AddressV = sd.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
    break;
  case D3DW_TEXTURE_MODE_MIRROR_ONCE:
    sd.AddressU = sd.AddressV = sd.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR_ONCE;
    break;
  default:
    return E_INVALIDARG;
  }
  if(anisotropyLevel > 0)
  {
    sd.Filter = D3D11_FILTER_ANISOTROPIC;
    sd.MaxAnisotropy = anisotropyLevel;    
  }
  else
  {
    sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    if(borderColor)
    {
      sd.BorderColor[0] = borderColor[0];
      sd.BorderColor[1] = borderColor[1];
      sd.BorderColor[2] = borderColor[2];
      sd.BorderColor[3] = borderColor[3];
    }
  }
  sd.ComparisonFunc = D3D11_COMPARISON_LESS;
  sd.MipLODBias = 0;
  sd.MinLOD = 0;
  sd.MaxLOD = D3D11_FLOAT32_MAX;
  V_HR_ASSERT(_device->CreateSamplerState(&sd, oSamState), "Unable to create sampler state");

  return S_OK;
}