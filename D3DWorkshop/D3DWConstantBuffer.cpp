#include "StdAfx.h"
#include "D3DWConstantBuffer.hpp"
#include "D3DWEffect.hpp"

D3DWConstantBuffer::D3DWConstantBuffer()
{
}

D3DWConstantBuffer::~D3DWConstantBuffer()
{
}

HRESULT D3DWConstantBuffer::GetBuffer(ID3D11Buffer **oBuffer)
{
  if(!oBuffer)
    return E_POINTER;
  *oBuffer = NULL;
  _buffer->AddRef();
  *oBuffer = _buffer;
  return S_OK;
}

STDMETHODIMP D3DWConstantBuffer::GetSize(SIZE_T *oSize)
{
  if(!oSize)
    return E_POINTER;
  *oSize = (SIZE_T)_size;
  return S_OK;
}

HRESULT D3DWConstantBuffer::GetSlot(UINT *oSlot)
{
  if(!oSlot)
    return E_POINTER;
  *oSlot = _slot;
  return S_OK;
}

HRESULT D3DWConstantBuffer::Create(UINT slot, ID3D11ShaderReflectionConstantBuffer *reflect, D3DWEffect *effect, D3DWConstantBuffer **oCb)
{
  if(!oCb || !effect)
    return E_POINTER;
  *oCb = NULL;
  ComContainedObject<D3DWConstantBuffer> *cb = new (std::nothrow) ComContainedObject<D3DWConstantBuffer>(effect);
  if(!cb)
    return E_POINTER;
  HRESULT hr = cb->Initialize(slot, reflect, effect);
  if(FAILED(hr))
  {
    SafeDelete(cb);
    return hr;
  }
  *oCb = cb;
  return S_OK;
}

HRESULT D3DWConstantBuffer::Initialize(UINT slot, ID3D11ShaderReflectionConstantBuffer *reflect, D3DWEffect *effect)
{
  HRESULT hr;
  if(!reflect)
    return E_POINTER;
  _effect = effect;
  _slot = slot;

  D3D11_SHADER_BUFFER_DESC sbd;
  reflect->GetDesc(&sbd);
  _size = sbd.Size; 
  _name = sbd.Name;

  ComPtr<ID3D11Device> device;
  V_HR(effect->GetDevice(&device));
  D3D11_BUFFER_DESC bd = {0};
  bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  bd.ByteWidth = _size;
  V_HR_ASSERT(device->CreateBuffer(&bd, NULL, &_buffer), "Unable to create constant buffer");
  return S_OK;
}

STDMETHODIMP D3DWConstantBuffer::GetEffect(ID3DWEffect **oEffect)
{
  if(!oEffect)
    return E_POINTER;
  *oEffect = NULL;
  _effect->AddRef();
  *oEffect = _effect;
  return S_OK;
}

STDMETHODIMP D3DWConstantBuffer::Update(LPVOID data, SIZE_T size)
{
  HRESULT hr;
  if(size < _size)
  {
    V_HR_BREAK("Supplied data is too small for constant buffer");
    return HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
  }
  ComPtr<ID3D11DeviceContext> dc;
  V_HR(_effect->GetDc(&dc));
  dc->UpdateSubresource(_buffer, 0, NULL, data, 0, 0);
  return S_OK;
}