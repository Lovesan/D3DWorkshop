#include "StdAfx.h"
#include "D3DWMesh.hpp"
#include "D3DWContext.hpp"

D3DWMesh::D3DWMesh()
{
  _canMap = FALSE;
  _vbMapped = FALSE;
  _ibMapped = FALSE;  
}

D3DWMesh::~D3DWMesh()
{
}

HRESULT D3DWMesh::GetContext(ID3DWContext **oCtx)
{
  if(!oCtx)
    return E_POINTER;
  *oCtx = NULL;
  _ctx->AddRef();
  *oCtx = _ctx;
  return S_OK;
}

STDMETHODIMP D3DWMesh::GetVertexSize(SIZE_T *oVertexSize)
{
  if(!oVertexSize)
    return E_POINTER;
  *oVertexSize = _vertexSize;
  return S_OK;
}

STDMETHODIMP D3DWMesh::GetVertexCount(UINT *oCount)
{
  if(!oCount)
    return E_POINTER;
  *oCount = _nVertices;
  return S_OK;
}

STDMETHODIMP D3DWMesh::GetIndexCount(UINT *oCount)
{
  if(!oCount)
    return E_POINTER;
  *oCount = _nIndices;
  return S_OK;
}

STDMETHODIMP D3DWMesh::Draw(ID3DWEffect *effect)
{
  HRESULT hr;
  if(_canMap && (_vbMapped || _ibMapped))
    return HRESULT_FROM_WIN32(ERROR_INVALID_OPERATION);

  if(!effect)
    return E_POINTER;

  ComPtr<ID3DWEffectInternals> effectInternals;
  hr = effect->QueryInterface(IID_PPV_ARGS(&effectInternals));
  if(FAILED(hr))
  {
    V_HR_BREAK("Invalid effect object passed to ID3DWMesh::Draw");
    return E_INVALIDARG;
  }
  
  _dc->IASetPrimitiveTopology(_topology);

  UINT stride = _vertexSize, offset = 0;
  _dc->IASetVertexBuffers(0, 1, _vb.AddressOf(), &stride, &offset);
  _dc->IASetIndexBuffer(_ib, DXGI_FORMAT_R32_UINT, 0);
    
  V_HR(_ctx->SetAsTarget());
  hr = effectInternals->LockTextures(NULL);
  if(FAILED(hr))
  {
    effectInternals->UnlockTextures();
    return hr;
  }
  hr = effectInternals->Apply();
  if(FAILED(hr))
  {
    effectInternals->UnlockTextures();
    return hr;
  }
  _dc->DrawIndexed(_nIndices, 0, 0);
  effectInternals->UnlockTextures();
  
  return S_OK;
}

STDMETHODIMP D3DWMesh::DrawToCubeMap(ID3DWEffect *effect, ID3DWCubeMap *cubeMap)
{
  HRESULT hr;
  if(!cubeMap)
    return E_POINTER;
  ComPtr<ID3DWTextureInternals> texInternals;
  hr = cubeMap->QueryInterface(IID_PPV_ARGS(&texInternals));
  if(FAILED(hr))
  {
    V_HR_BREAK("Invalid surface object passed to ID3DWMesh::Draw");
    return E_INVALIDARG;
  }
  return DrawToTextureInternals(effect, texInternals);
}

STDMETHODIMP D3DWMesh::DrawToSurface(ID3DWEffect *effect, ID3DWSurface *surface)
{
  HRESULT hr;
  if(!surface)
    return E_POINTER;
  ComPtr<ID3DWTexture> tex;
  ComPtr<ID3DWTextureInternals> texInternals;
  hr = surface->GetTexture(&tex);
  if(SUCCEEDED(hr))
  {
    hr = tex->QueryInterface(IID_PPV_ARGS(&texInternals));
    if(FAILED(hr))
    {
      V_HR_BREAK("Invalid surface object passed to ID3DWMesh::Draw");
      return E_INVALIDARG;
    }
  }
  return DrawToTextureInternals(effect, texInternals);
}

HRESULT D3DWMesh::DrawToTextureInternals(ID3DWEffect *effect, ID3DWTextureInternals *texInternals)
{
  HRESULT hr;
  if(_canMap && (_vbMapped || _ibMapped))
    return HRESULT_FROM_WIN32(ERROR_INVALID_OPERATION);

  if(!effect || !texInternals)
    return E_POINTER;

  ComPtr<ID3DWEffectInternals> effectInternals;
  
  hr = effect->QueryInterface(IID_PPV_ARGS(&effectInternals));
  if(FAILED(hr))
  {
    V_HR_BREAK("Invalid effect object passed to ID3DWMesh::Draw");
    return E_INVALIDARG;
  }
    
  _dc->IASetPrimitiveTopology(_topology);

  UINT stride = _vertexSize, offset = 0;
  _dc->IASetVertexBuffers(0, 1, _vb.AddressOf(), &stride, &offset);
  _dc->IASetIndexBuffer(_ib, DXGI_FORMAT_R32_UINT, 0);
  
  hr = effectInternals->LockTextures(texInternals);
  if(FAILED(hr))
  {
    if(E_INVALIDARG == hr)
    {
      V_HR_BREAK("Unable to set texture as an output - it is already bound in this effect");
    }
    effectInternals->UnlockTextures();
    return hr;
  }
  hr = texInternals->Lock3D();
  if(FAILED(hr))
  {
    effectInternals->UnlockTextures();
    return hr;
  }
  hr = texInternals->SetAsTarget();
  if(FAILED(hr))
  {
    texInternals->Unlock3D();
    effectInternals->UnlockTextures();
    return hr;
  }
  hr = effectInternals->Apply();
  if(FAILED(hr))
  {
    texInternals->Unlock3D();
    effectInternals->UnlockTextures();
    return hr;
  }
  _dc->DrawIndexed(_nIndices, 0, 0);
  texInternals->Unlock3D();
  effectInternals->UnlockTextures();
  return S_OK;
}

STDMETHODIMP D3DWMesh::MapVertices(LPVOID *oVertexData)
{
  HRESULT hr;
  if(!_canMap || _vbMapped)
    return HRESULT_FROM_WIN32(ERROR_INVALID_OPERATION);
  if(!oVertexData)
    return E_POINTER;
  *oVertexData = NULL;

  _dc->CopyResource(_mappedVb, _vb);
  D3D11_MAPPED_SUBRESOURCE src;  
  V_HR_ASSERT(_dc->Map(_mappedVb, 0, D3D11_MAP_READ_WRITE, 0, &src), "Unable to map vertex buffer");
  *oVertexData = src.pData;
  _vbMapped = TRUE;
  return S_OK;
}

STDMETHODIMP D3DWMesh::UnmapVertices()
{
  if(!_canMap || !_vbMapped)
    return HRESULT_FROM_WIN32(ERROR_INVALID_OPERATION);
  _dc->Unmap(_mappedVb, 0);
  _dc->CopyResource(_vb, _mappedVb);
  _vbMapped = FALSE;
  return S_OK;
}

STDMETHODIMP D3DWMesh::MapIndices(UINT32 **oIndices)
{
  HRESULT hr;
  if(!_canMap || _ibMapped)
    return HRESULT_FROM_WIN32(ERROR_INVALID_OPERATION);
  if(!oIndices)
    return E_POINTER;
  *oIndices = NULL;
  _dc->CopyResource(_mappedIb, _ib);
  D3D11_MAPPED_SUBRESOURCE src;
  V_HR_ASSERT(_dc->Map(_mappedIb, 0, D3D11_MAP_READ_WRITE, 0, &src), "Unable to map index buffer");
  *oIndices = (UINT32*)src.pData;
  _ibMapped = TRUE;
  return S_OK;
}

STDMETHODIMP D3DWMesh::UnmapIndices()
{
  if(!_canMap || !_ibMapped)
    return HRESULT_FROM_WIN32(ERROR_INVALID_OPERATION);
  _dc->Unmap(_mappedIb, 0);
  _dc->CopyResource(_ib, _mappedIb);
  _ibMapped = FALSE;
  return S_OK;
}

HRESULT D3DWMesh::Create(
    LPVOID vertexData, SIZE_T vertexSize, UINT nVertices, UINT32 *indices, UINT nIndices, D3DW_TOPOLOGY topology, BOOL canMap, D3DWContext *ctx, D3DWMesh **oMesh)
{
  HRESULT hr;
  if(!oMesh)
    return E_POINTER;
  *oMesh = NULL;
  D3DWMesh *mesh = new (std::nothrow) ComObject<D3DWMesh>();
  if(!mesh)
    return E_OUTOFMEMORY;
  hr = mesh->Initialize(vertexData, vertexSize, nVertices, indices, nIndices, topology, canMap, ctx);
  if(FAILED(hr))
  {
    SafeDelete(mesh);
    return hr;
  }
  mesh->AddRef();
  *oMesh = mesh;
  return S_OK;
}

HRESULT D3DWMesh::Initialize(
  LPVOID vertexData, SIZE_T vertexSize, UINT nVertices, UINT32 *indices, UINT nIndices, D3DW_TOPOLOGY topology, BOOL canMap, D3DWContext *ctx)
{
  HRESULT hr;
  if(!ctx || !vertexData || !indices)
    return E_POINTER;

  V_HR(ctx->GetDevice(&_device));
  V_HR(ctx->GetDc(&_dc));
  _ctx = ctx;
  _canMap = canMap;

  switch(topology)
  {
    case D3DW_TOPOLOGY_TRIANGLE:
      _topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
      break;
    case D3DW_TOPOLOGY_LINE:
      _topology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
      break;
    case D3DW_TOPOLOGY_POINT:
      _topology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
      break;
    default:
      {
        V_HR_BREAK("Invalid primitive topology");
        return E_INVALIDARG;
      }
  }

  D3D11_BUFFER_DESC bd = {0};  
  D3D11_SUBRESOURCE_DATA sd = {0};

  bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  bd.Usage = canMap ? D3D11_USAGE_DEFAULT : D3D11_USAGE_IMMUTABLE;
  bd.CPUAccessFlags = 0;
  bd.ByteWidth = vertexSize * nVertices;    
  sd.pSysMem = vertexData;
  _vertexSize = vertexSize;
  _nVertices = nVertices;
  V_HR_ASSERT(_device->CreateBuffer(&bd, &sd, &_vb), "Unable to create vertex buffer");
  
  bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
  bd.ByteWidth = nIndices*sizeof(UINT32);
  sd.pSysMem = indices;
  _nIndices = nIndices;
  V_HR_ASSERT(_device->CreateBuffer(&bd, &sd, &_ib), "Unable to create index buffer");

  if(_canMap)
  {
    bd.BindFlags = 0;
    bd.Usage = D3D11_USAGE_STAGING;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
    
    bd.ByteWidth = vertexSize * nVertices;
    V_HR_ASSERT(_device->CreateBuffer(&bd, NULL, &_mappedVb), "Unable to create staging vertex buffer");
    
    bd.ByteWidth = nIndices*sizeof(UINT32);
    V_HR_ASSERT(_device->CreateBuffer(&bd, NULL, &_mappedIb), "Unable to create staging index buffer");
  }
  return S_OK;
}