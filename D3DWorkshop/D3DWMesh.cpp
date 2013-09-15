#include "StdAfx.h"
#include "D3DWMesh.hpp"
#include "D3DWContext.hpp"

D3DWMesh::D3DWMesh()
{
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
  V_HR(DrawToRenderTargetInternals(effect, _ctx));
  return S_OK;
}

STDMETHODIMP D3DWMesh::DrawToBuffer(ID3DWEffect *effect, ID3DWBuffer *buffer)
{
  HRESULT hr;
  if(!buffer)
    return E_POINTER;
  ComPtr<ID3DWBufferInternals> internals;
  hr = buffer->QueryInterface(IID_PPV_ARGS(&internals));
  if(FAILED(hr))
  {
    V_HR_BREAK("Invalid buffer object passed to ID3DWMesh::DrawToBuffer");
    return E_INVALIDARG;
  }
  return DrawToRenderTargetInternals(effect, internals);
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
  }
  if(FAILED(hr))
  {
    V_HR_BREAK("Invalid surface object passed to ID3DWMesh::DrawToSurface");
    return E_INVALIDARG;
  }
  V_HR(texInternals->Lock3D());
  hr = DrawToRenderTargetInternals(effect, texInternals);
  texInternals->Unlock3D();
  return hr;
}

STDMETHODIMP D3DWMesh::DrawToCubeMap(ID3DWEffect *effect, ID3DWCubeMap *cubeMap)
{
  HRESULT hr;
  if(!cubeMap)
    return E_POINTER;
  ComPtr<ID3DWCubeMapInternals> internals;
  hr = cubeMap->QueryInterface(IID_PPV_ARGS(&internals));
  if(FAILED(hr))
  {
    V_HR_BREAK("Invalid cube map object passed to ID3DWMesh::DrawToCubeMap");
    return E_INVALIDARG;
  }
  return DrawToRenderTargetInternals(effect, internals);
}

HRESULT D3DWMesh::DrawToRenderTargetInternals(ID3DWEffect *effect, ID3DWRenderTargetInternals *rtInternals)
{
  HRESULT hr;
  if(_vbMapped || _ibMapped)
  {
    V_HR_BREAK("Mesh is mapped. Unable to draw.");
    return HRESULT_FROM_WIN32(ERROR_INVALID_OPERATION);
  }

  if(!effect || !rtInternals)
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

  hr = effectInternals->LockTargets(rtInternals);
  if(FAILED(hr))
  {
    V_HR_BREAK("Unable draw to object because it is already bound as shader resource");
    return hr;
  }

  hr = rtInternals->SetAsTarget();
  if(FAILED(hr))
  {
    effectInternals->UnlockTargets();
    return hr;
  }

  hr = effectInternals->Apply();
  if(FAILED(hr))
  {
    effectInternals->UnlockTargets();
    return hr;
  }

  _dc->DrawIndexed(_nIndices, 0, 0);

  effectInternals->UnlockTargets();

  return S_OK;
}

STDMETHODIMP D3DWMesh::MapVertices(LPVOID *oVertexData)
{
  HRESULT hr;
  if(!_vbMapped)
    return HRESULT_FROM_WIN32(ERROR_INVALID_OPERATION);
  if(!oVertexData)
    return E_POINTER;
  *oVertexData = NULL;
  D3D11_BUFFER_DESC bd = {0};
  bd.ByteWidth = _nVertices * _vertexSize;
  bd.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
  bd.Usage = D3D11_USAGE_STAGING;
  V_HR(_device->CreateBuffer(&bd, NULL, &_mappedVb));
  _dc->CopyResource(_mappedVb, _vb);
  D3D11_MAPPED_SUBRESOURCE src;
  hr = _dc->Map(_mappedVb, 0, D3D11_MAP_READ_WRITE, 0, &src);
  if(FAILED(hr))
  {
    _mappedVb.Release();
    V_HR_BREAK("Unable to map vertex buffer");
    return hr;
  }
  *oVertexData = (UINT32*)src.pData;
  _vbMapped = TRUE;
  return S_OK;
}

STDMETHODIMP D3DWMesh::UnmapVertices()
{
  if(!_vbMapped)
    return HRESULT_FROM_WIN32(ERROR_INVALID_OPERATION);
  _dc->Unmap(_mappedVb, 0);
  _dc->CopyResource(_vb, _mappedVb);
  _mappedVb.Release();
  _vbMapped = FALSE;
  return S_OK;
}

STDMETHODIMP D3DWMesh::MapIndices(UINT32 **oIndices)
{
  HRESULT hr;
  if(!_ibMapped)
    return HRESULT_FROM_WIN32(ERROR_INVALID_OPERATION);
  if(!oIndices)
    return E_POINTER;
  *oIndices = NULL;
  D3D11_BUFFER_DESC bd = {0};
  bd.ByteWidth = _nIndices * sizeof(UINT32);
  bd.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
  bd.Usage = D3D11_USAGE_STAGING;
  V_HR(_device->CreateBuffer(&bd, NULL, &_mappedIb));
  _dc->CopyResource(_mappedIb, _ib);
  D3D11_MAPPED_SUBRESOURCE src;
  hr = _dc->Map(_mappedIb, 0, D3D11_MAP_READ_WRITE, 0, &src);
  if(FAILED(hr))
  {
    _mappedIb.Release();
    V_HR_BREAK("Unable to map index buffer");
    return hr;
  }
  *oIndices = (UINT32*)src.pData;
  _ibMapped = TRUE;
  return S_OK;
}

STDMETHODIMP D3DWMesh::UnmapIndices()
{
  if(!_ibMapped)
    return HRESULT_FROM_WIN32(ERROR_INVALID_OPERATION);
  _dc->Unmap(_mappedIb, 0);
  _dc->CopyResource(_ib, _mappedIb);
  _mappedIb.Release();
  _ibMapped = FALSE;
  return S_OK;
}

HRESULT D3DWMesh::Create(
    LPVOID vertexData, SIZE_T vertexSize, UINT nVertices, UINT32 *indices, UINT nIndices, D3DW_TOPOLOGY topology, D3DWContext *ctx, D3DWMesh **oMesh)
{
  HRESULT hr;
  if(!oMesh)
    return E_POINTER;
  *oMesh = NULL;
  D3DWMesh *mesh = new (std::nothrow) ComObject<D3DWMesh>();
  if(!mesh)
    return E_OUTOFMEMORY;
  hr = mesh->Initialize(vertexData, vertexSize, nVertices, indices, nIndices, topology, ctx);
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
  LPVOID vertexData, SIZE_T vertexSize, UINT nVertices, UINT32 *indices, UINT nIndices, D3DW_TOPOLOGY topology, D3DWContext *ctx)
{
  HRESULT hr;
  if(!ctx || !vertexData || !indices)
    return E_POINTER;

  V_HR(ctx->GetDevice(&_device));
  V_HR(ctx->GetDc(&_dc));
  _ctx = ctx;

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
  bd.Usage = D3D11_USAGE_DEFAULT;
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

  return S_OK;
}