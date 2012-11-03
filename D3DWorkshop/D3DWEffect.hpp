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

  HRESULT GetDevice(ID3D11Device **oDevice);
  HRESULT GetDc(ID3D11DeviceContext **oDc);

  STDMETHODIMP GetContext(ID3DWContext **oCtx);
  STDMETHODIMP Apply();
  STDMETHODIMP LockTextures(ID3DWTextureInternals *output);
  STDMETHODIMP UnlockTextures();
  STDMETHODIMP GetConstantBuffer(LPCSTR name, ID3DWConstantBuffer **oCb);
  STDMETHODIMP SetTexture(LPCSTR name, ID3DWTexture *tex);
  STDMETHODIMP SetCubeMap(LPCSTR name, ID3DWCubeMap *cubeMap);

protected:  
  D3DWEffect();

private:  
  struct ResourceVar
  {
    UINT slot;
    ComPtr<ID3DWTextureInternals> tex;
    ComPtr<ID3D11ShaderResourceView> srv;
    D3D11_SRV_DIMENSION dim;
  };
  ComPtr<D3DWContext> _ctx;
  ComPtr<ID3D11Device> _device;
  ComPtr<ID3D11DeviceContext> _dc;
  ComPtr<ID3D11InputLayout> _layout;
  ComPtr<ID3D11VertexShader> _vs;
  ComPtr<ID3D11ShaderReflection> _vsReflect;
  std::vector<D3DWConstantBuffer*> _vsCbuffers;
  std::vector<ResourceVar*> _vsRes;
  ComPtr<ID3D11GeometryShader> _gs;
  ComPtr<ID3D11ShaderReflection> _gsReflect;
  std::vector<D3DWConstantBuffer*> _gsCbuffers;
  std::vector<ResourceVar*> _gsRes;
  ComPtr<ID3D11PixelShader> _ps;
  ComPtr<ID3D11ShaderReflection> _psReflect;
  std::vector<D3DWConstantBuffer*> _psCbuffers;
  std::vector<ResourceVar*> _psRes;
  std::map<std::string,D3DWConstantBuffer*> _allBuffers;
  std::map<std::string,ResourceVar*> _allRes;

  HRESULT InitFromMemory(LPCSTR data, SIZE_T size, LPCSTR vsEntry, LPCSTR gsEntry, LPCSTR psEntry, D3DWContext *ctx);
  HRESULT InitLayout(ID3DBlob *vsBytecode);
  HRESULT CompileShader(LPCSTR data, SIZE_T size, LPCSTR entry, LPCSTR profile, ID3DBlob **oBytecode);
  HRESULT InitCbuffers(
    ID3D11ShaderReflection *reflect, std::vector<D3DWConstantBuffer*>& cbs, std::vector<ResourceVar*>& res);
  static HRESULT ReadFileIntoString(LPCWSTR filename, std::string& string);
};

#define __D3DWEFFECT_DEFINED__


#endif // __D3DWEFFECT_HPP__