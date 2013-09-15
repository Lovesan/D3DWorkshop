#ifndef __D3DWEFFECT_HPP__
#define __D3DWEFFECT_HPP__

#include <map>
#include <vector>
#include <string>
#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include "ComUtils.hpp"
#include "D3DW.h"
#include "D3DWInternals.hpp"

#ifndef __D3DWCONTEXT_DEFINED__
class D3DWContext;
#endif // __D3DWCONTEXT_DEFINED__

#ifndef __D3DWCONSTANT_BUFFER_DEFINED__
class D3DWConstantBuffer;
#endif // __D3DWCONSTANT_BUFFER_DEFINED__

class COM_NO_VTABLE D3DWEffect :
  public ID3DWEffect,
  public ID3DWEffectInternals
{
public:
  BEGIN_INTERFACE_MAP
    INTERFACE_MAP_ENTRY(ID3DWContextChild)
    INTERFACE_MAP_ENTRY(ID3DWEffect)
    INTERFACE_MAP_ENTRY(ID3DWEffectInternals)
  END_INTERFACE_MAP

  static HRESULT WINAPI CreateFromMemory(
    LPCSTR data, SIZE_T size, LPCSTR vsEntry, LPCSTR gsEntry, LPCSTR psEntry, D3DWContext *ctx, D3DWEffect **oEffect);
  static HRESULT WINAPI CreateFromFile(
    LPCWSTR filename, LPCSTR vsEntry, LPCSTR gsEntry, LPCSTR psEntry, D3DWContext *ctx, D3DWEffect **oEffect);  
  static HRESULT WINAPI CreateFromResource(
    HMODULE module, LPCWSTR resourceName, LPCWSTR resourceType, LPCSTR vsEntry, LPCSTR gsEntry, LPCSTR psEntry,
    D3DWContext *ctx, D3DWEffect **oEffect);
  virtual ~D3DWEffect();
  
  STDMETHODIMP Apply();
  STDMETHODIMP LockTargets(ID3DWRenderTargetInternals *output);
  STDMETHODIMP UnlockTargets();
  STDMETHODIMP GetContext(ID3DWContext **oCtx);
  STDMETHODIMP SetConstantBuffer(LPCSTR name, ID3DWConstantBuffer *constantBuffer);
  STDMETHODIMP SetBuffer(LPCSTR name, ID3DWBuffer *buffer);
  STDMETHODIMP SetTexture(LPCSTR name, ID3DWTexture *texture);
  STDMETHODIMP SetCubeMap(LPCSTR name, ID3DWCubeMap *cubeMap);
  STDMETHODIMP SetSampler(LPCSTR name, ID3DWSampler *sampler);

protected:  
  D3DWEffect();

private:
  typedef enum _RV_TYPE
  {
    RVT_CONSTANT_BUFFER,
    RVT_TEXTURE,
    RVT_CUBE_MAP,
    RVT_BUFFER,
    RVT_SAMPLER
  } RV_TYPE;
  struct ResourceVar
  {
    RV_TYPE rvType;
    UINT vsSlot;
    UINT gsSlot;
    UINT psSlot;
    SIZE_T size;
    ComPtr<IUnknown> internals;
    ComPtr<IUnknown> resource;
  };
  typedef std::map<std::string,ResourceVar*> ResourceMap;
  typedef std::vector<ResourceVar*> ResourceVector;
  typedef enum _S_TYPE
  {
    ST_VS,
    ST_GS,
    ST_PS
  } S_TYPE;

  ComPtr<D3DWContext> _ctx;
  ComPtr<ID3D11Device> _device;
  ComPtr<ID3D11DeviceContext> _dc;
  ComPtr<ID3D11InputLayout> _layout;
  ComPtr<ID3D11VertexShader> _vs;
  ComPtr<ID3D11GeometryShader> _gs;
  ComPtr<ID3D11PixelShader> _ps;
  ResourceVector _vsRes;
  ResourceVector _gsRes;
  ResourceVector _psRes;
  ResourceVector _allRes;
  ResourceVector _renderTargets;
  ResourceMap _constantBuffers;
  ResourceMap _textures;
  ResourceMap _cubeMaps;
  ResourceMap _buffers;
  ResourceMap _samplers;

  HRESULT InitFromMemory(LPCSTR data, SIZE_T size, LPCSTR vsEntry, LPCSTR gsEntry, LPCSTR psEntry, D3DWContext *ctx);
  HRESULT InitLayout(ID3DBlob *vsBytecode);
  HRESULT InitResources(ID3DBlob *bytecode, S_TYPE sType);
  HRESULT CompileShader(LPCSTR data, SIZE_T size, LPCSTR entry, LPCSTR profile, ID3DBlob **oBytecode);
  static HRESULT ReadFileIntoString(LPCWSTR filename, std::string& string);
};

#define __D3DWEFFECT_DEFINED__


#endif // __D3DWEFFECT_HPP__