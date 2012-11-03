#ifndef __D3DWMESH_HPP__
#define __D3DWMESH_HPP__

#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include "D3DW.h"
#include "D3DWInternals.hpp"

#ifndef __D3DWCONTEXT_DEFINED__
class D3DWContext;
#endif // __D3DWCONTEXT_DEFINED__

#ifndef __D3DWEFFECT_DEFINED__
class D3DWEffect;
#endif // __D3DWEFFECT_DEFINED__

class COM_NO_VTABLE D3DWMesh : public ID3DWMesh
{
public:
  BEGIN_INTERFACE_MAP
    INTERFACE_MAP_ENTRY(ID3DWContextChild)
    INTERFACE_MAP_ENTRY(ID3DWMesh)
  END_INTERFACE_MAP

  static HRESULT Create(
    LPVOID vertexData, SIZE_T vertexSize, UINT nVertices, UINT32 *indices, UINT nIndices, D3DW_TOPOLOGY topology, BOOL canMap, D3DWContext *ctx, D3DWMesh **oMesh);
  virtual ~D3DWMesh();

  STDMETHODIMP GetContext(ID3DWContext **oCtx);
  STDMETHODIMP GetVertexSize(SIZE_T *oVertexSize);
  STDMETHODIMP GetVertexCount(UINT *oCount);
  STDMETHODIMP GetIndexCount(UINT *oCount);
  STDMETHODIMP Draw(ID3DWEffect *effect);
  STDMETHODIMP DrawToSurface(ID3DWEffect *effect, ID3DWSurface *surface);
  STDMETHODIMP DrawToCubeMap(ID3DWEffect *effect, ID3DWCubeMap *cubeMap);
  STDMETHODIMP MapVertices(LPVOID *oVertexData);
  STDMETHODIMP UnmapVertices();
  STDMETHODIMP MapIndices(UINT32 **oIndices);
  STDMETHODIMP UnmapIndices();

protected:
  D3DWMesh();

private:
  ComPtr<D3DWContext> _ctx;
  ComPtr<ID3D11Device> _device;
  ComPtr<ID3D11DeviceContext> _dc;
  ComPtr<ID3D11Buffer> _vb;  
  SIZE_T _vertexSize;
  UINT _nVertices;
  ComPtr<ID3D11Buffer> _ib;
  UINT _nIndices;
  D3D11_PRIMITIVE_TOPOLOGY _topology;
  BOOL _canMap;
  BOOL _vbMapped;
  ComPtr<ID3D11Buffer> _mappedVb;
  BOOL _ibMapped;
  ComPtr<ID3D11Buffer> _mappedIb;

  HRESULT Initialize(
    LPVOID vertexData, SIZE_T vertexSize, UINT nVertices, UINT32 *indices, UINT nIndices, D3DW_TOPOLOGY topology, BOOL canMap, D3DWContext *ctx);
  HRESULT DrawToTextureInternals(ID3DWEffect *effect, ID3DWTextureInternals *texInternals);
};

#define __D3DWMESH_DEFINED__

#endif // __D3DWMESH_HPP__