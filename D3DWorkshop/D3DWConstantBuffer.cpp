#include "StdAfx.h"
#include <new>
#include "D3DWConstantBuffer.hpp"
#include "D3DWContext.hpp"

D3DWConstantBuffer::D3DWConstantBuffer()
{
}

D3DWConstantBuffer::~D3DWConstantBuffer()
{
}

STDMETHODIMP D3DWConstantBuffer::GetRes(ID3D11Resource **oRes)
{
  if(!oRes)
    return E_POINTER;
  *oRes = NULL;
  _buffer->AddRef();
  *oRes = _buffer;
  return S_OK;
}

STDMETHODIMP D3DWConstantBuffer::GetCb(ID3D11Buffer **oCb)
{
  if(!oCb)
    return E_POINTER;
  *oCb = NULL;
  _buffer->AddRef();
  *oCb = _buffer;
  return S_OK;
}

STDMETHODIMP D3DWConstantBuffer::GetContext(ID3DWContext **oCtx)
{
  if(!oCtx)
    return E_POINTER;
  *oCtx = NULL;
  _ctx->AddRef();
  *oCtx = _ctx;
  return S_OK;
}

STDMETHODIMP D3DWConstantBuffer::GetByteSize(SIZE_T *oSize)
{
  if(!oSize)
    return E_POINTER;
  *oSize = _size;
  return S_OK;
}

STDMETHODIMP D3DWConstantBuffer::Update(LPVOID data, SIZE_T size)
{
  if(size < _size)
  {
    V_HR_BREAK("Supplied buffer size is too small for constant buffer");
    return HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
  }
  if(data)
  {
    _dc->UpdateSubresource(_buffer, 0, NULL, data, 0, 0);
  }
  return S_OK;
}

HRESULT D3DWConstantBuffer::Create(SIZE_T size, D3DWContext *ctx, D3DWConstantBuffer **oCb)
{
  if(!oCb)
    return E_POINTER;
  *oCb = NULL;
  ComObject<D3DWConstantBuffer> *cb = new (std::nothrow) ComObject<D3DWConstantBuffer>();
  if(!cb)
    return E_OUTOFMEMORY;
  HRESULT hr = cb->Initialize(size, ctx);
  if(FAILED(hr))
  {
    SafeDelete(cb);
    return hr;
  }
  cb->AddRef();
  *oCb = cb;
  return S_OK;
}

HRESULT D3DWConstantBuffer::Initialize(SIZE_T size, D3DWContext *ctx)
{
  HRESULT hr;
  if(!ctx)
    return E_POINTER;

  _ctx = ctx;
  _size = size;

  V_HR(_ctx->GetDevice(&_device));
  V_HR(_ctx->GetDc(&_dc));

  D3D11_BUFFER_DESC bd = {0};
  bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  bd.ByteWidth = (UINT)size;
  
  V_HR_ASSERT(_device->CreateBuffer(&bd, NULL, &_buffer), "Unable to create constant buffer");

  return S_OK;
}
