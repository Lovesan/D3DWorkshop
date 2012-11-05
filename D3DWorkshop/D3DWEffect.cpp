#include "StdAfx.h"
#include "D3DWEffect.hpp"
#include "D3DWContext.hpp"
#include "D3DWConstantBuffer.hpp"

const IID IID_ID3D11ShaderReflection = { 0x0a233719, 0x3960, 0x4578, { 0x9d, 0x7c, 0x20, 0x3b, 0x8b, 0x1d, 0x9c, 0xc1 } };

D3DWEffect::D3DWEffect()
{
}

D3DWEffect::~D3DWEffect()
{
  std::map<std::string,D3DWConstantBuffer*>::iterator itCb = _allBuffers.begin();
  std::map<std::string,D3DWConstantBuffer*>::iterator endCb = _allBuffers.end();
  while(itCb != endCb)
  {
    SafeDelete(itCb->second);
    ++itCb;
  }
  std::map<std::string,ResourceVar*>::iterator itRes = _allRes.begin();
  std::map<std::string,ResourceVar*>::iterator endRes = _allRes.end();
  while(itRes != endRes)
  {
    SafeDelete(itRes->second);
    ++itRes;
  }
}

STDMETHODIMP D3DWEffect::GetContext(ID3DWContext **oCtx)
{
  if(!oCtx)
    return E_POINTER;
  *oCtx = NULL;
  _ctx->AddRef();
  *oCtx = _ctx;
  return S_OK;
}

HRESULT D3DWEffect::GetDevice(ID3D11Device **oDevice)
{
  return _ctx->GetDevice(oDevice);
}

HRESULT D3DWEffect::GetDc(ID3D11DeviceContext **oDc)
{
  return _ctx->GetDc(oDc);
}

STDMETHODIMP D3DWEffect::LockTextures(ID3DWTextureInternals *output)
{
  HRESULT hr = S_OK;
  std::map<std::string, ResourceVar*>::iterator it = _allRes.begin();
  std::map<std::string, ResourceVar*>::iterator end = _allRes.end();
  while(it != end)
  {
    hr = it->second->tex->Lock3D();
    if(SUCCEEDED(hr))
    {
      if(output && it->second->tex.IsEqualObject(output))
        hr = E_INVALIDARG;
    }
    ++it;
  }
  return hr;
}

STDMETHODIMP D3DWEffect::UnlockTextures()
{
  HRESULT hr = S_OK;
  std::map<std::string, ResourceVar*>::iterator it = _allRes.begin();
  std::map<std::string, ResourceVar*>::iterator end = _allRes.end();
  while(it != end)
  {
    hr = it->second->tex->Unlock3D();
    ++it;
  }
  return hr;
}

STDMETHODIMP D3DWEffect::Apply()
{
  HRESULT hr;
  int n;

  _dc->IASetInputLayout(_layout);
  
  _dc->VSSetShader(_vs, NULL, 0);
  n = _vsCbuffers.size();
  for(int i = 0; i<n; ++i)
  {
    ComPtr<ID3D11Buffer> buffer;
    hr = _vsCbuffers[i]->GetBuffer(&buffer);
    if(SUCCEEDED(hr))
    {
      UINT slot;
      hr = _vsCbuffers[i]->GetSlot(&slot);
      if(SUCCEEDED(hr))
      {
        _dc->VSSetConstantBuffers(slot, 1, buffer.AddressOf());
      }
    }
  }
  n = _vsRes.size();
  for(int i = 0; i<n; ++i)
  {
    ResourceVar *rv = _vsRes[i];
    if(!!rv->srv)
      _dc->VSSetShaderResources(rv->slot, 1, rv->srv.AddressOf());
  }

  if(_gs)
  {
    _dc->GSSetShader(_gs, NULL, 0);
    n = _gsCbuffers.size();
    for(int i = 0; i<n; ++i)
    {
      ComPtr<ID3D11Buffer> buffer;
      hr = _gsCbuffers[i]->GetBuffer(&buffer);
      if(SUCCEEDED(hr))
      {
        UINT slot;
        hr = _gsCbuffers[i]->GetSlot(&slot);
        if(SUCCEEDED(hr))
        {
          _dc->GSSetConstantBuffers(slot, 1, buffer.AddressOf());
        }
      }
    }
    n = _gsRes.size();
    for(int i = 0; i<n; ++i)
    {
      ResourceVar *rv = _gsRes[i];
      if(!!rv->srv)
        _dc->GSSetShaderResources(rv->slot, 1, rv->srv.AddressOf());
    }
  }

  _dc->PSSetShader(_ps, NULL, 0);
  n = _psCbuffers.size();
  for(int i = 0; i<n; ++i)
  {
    ComPtr<ID3D11Buffer> buffer;
    hr = _psCbuffers[i]->GetBuffer(&buffer);
    if(SUCCEEDED(hr))
    {
      UINT slot;
      hr = _psCbuffers[i]->GetSlot(&slot);
      if(SUCCEEDED(hr))
      {
        _dc->PSSetConstantBuffers(slot, 1, buffer.AddressOf());
      }
    }
  }
  n = _psRes.size();
  for(int i = 0; i<n; ++i)
  {
    ResourceVar *rv = _psRes[i];
    if(!!rv->srv)
      _dc->PSSetShaderResources(rv->slot, 1, rv->srv.AddressOf());
  }

  return S_OK;
}

STDMETHODIMP D3DWEffect::GetConstantBuffer(LPCSTR name, ID3DWConstantBuffer **oCb)
{
  if(!oCb || !name)
    return E_POINTER;
  *oCb = NULL;
  std::map<std::string,D3DWConstantBuffer*>::const_iterator it = _allBuffers.find(name);
  if(it == _allBuffers.end())
    return E_INVALIDARG;
  D3DWConstantBuffer *cb = it->second;
  cb->AddRef();
  *oCb = cb;
  return S_OK;
}

STDMETHODIMP D3DWEffect::SetTexture(LPCSTR name, ID3DWTexture *tex)
{
  HRESULT hr;
  if(!name)
    return E_POINTER;
  std::string resName(name);
  std::map<std::string,ResourceVar*>::iterator it = _allRes.find(resName);
  if(it == _allRes.end())
    return E_INVALIDARG;
  ResourceVar *rv = it->second;
  if(rv->dim != D3D11_SRV_DIMENSION_TEXTURE2D)
  {
    V_HR_BREAK("Resource variable is not a texture");
    return E_INVALIDARG;
  }
  if(!tex)
  {
    rv->tex = NULL;
    rv->srv = NULL;
    return S_OK;
  }
  
  ComPtr<ID3DWTextureInternals> texInternals;
  hr = tex->QueryInterface(IID_PPV_ARGS(&texInternals));
  if(FAILED(hr))
    return E_INVALIDARG;
  ComPtr<ID3D11ShaderResourceView> srv;
  V_HR(texInternals->GetSrv(&srv));
  rv->tex = texInternals;
  rv->srv = srv;
  return S_OK;
}

STDMETHODIMP D3DWEffect::SetCubeMap(LPCSTR name, ID3DWCubeMap *cubeMap)
{
  HRESULT hr;
  if(!name)
    return E_POINTER;
  std::string resName(name);
  std::map<std::string,ResourceVar*>::iterator it = _allRes.find(resName);
  if(it == _allRes.end())
    return E_INVALIDARG;
  ResourceVar *rv = it->second;
  if(rv->dim != D3D11_SRV_DIMENSION_TEXTURECUBE)
  {
    V_HR_BREAK("Resource variable is not a cube texture");
    return E_INVALIDARG;
  }
  if(!cubeMap)
  {
    rv->tex = NULL;
    rv->srv = NULL;
    return S_OK;
  }
  
  ComPtr<ID3DWTextureInternals> texInternals;
  hr = cubeMap->QueryInterface(IID_PPV_ARGS(&texInternals));
  if(FAILED(hr))
    return E_INVALIDARG;
  ComPtr<ID3D11ShaderResourceView> srv;
  V_HR(texInternals->GetSrv(&srv));
  rv->tex = texInternals;
  rv->srv = srv;
  return S_OK;
}

HRESULT WINAPI D3DWEffect::CreateFromMemory(
  LPCSTR data, SIZE_T size, LPCSTR vsEntry, LPCSTR gsEntry, LPCSTR psEntry, D3DWContext *ctx, D3DWEffect **oEffect)
{
  if(!oEffect)
    return E_POINTER;
  *oEffect = NULL;
  D3DWEffect *effect = new (std::nothrow) ComObject<D3DWEffect>();
  if(!effect)
    return E_OUTOFMEMORY;
  HRESULT hr = effect->InitFromMemory(data, size, vsEntry, gsEntry, psEntry, ctx);
  if(FAILED(hr))
  {
    SafeDelete(effect);
    return hr;
  }
  effect->AddRef();
  *oEffect = effect;  
  return S_OK;
}

HRESULT WINAPI D3DWEffect::CreateFromFile(
  LPCWSTR filename, LPCSTR vsEntry, LPCSTR gsEntry, LPCSTR psEntry, D3DWContext *ctx, D3DWEffect **oEffect)
{
  HRESULT hr;
  if(!oEffect)
    return E_POINTER;
  *oEffect = NULL;
  D3DWEffect *effect = new (std::nothrow) ComObject<D3DWEffect>();
  if(!effect)
    return E_OUTOFMEMORY;
  
  std::string code;
  V_HR(ReadFileIntoString(filename, code));
  hr = effect->InitFromMemory(code.c_str(), code.size(), vsEntry, gsEntry, psEntry, ctx);
  if(FAILED(hr))
  {
    SafeDelete(effect);
    return hr;
  }
  effect->AddRef();
  *oEffect = effect;  
  return S_OK;
}

HRESULT WINAPI D3DWEffect::CreateFromResource(
  HMODULE module, LPCWSTR resourceName, LPCWSTR resourceType, LPCSTR vsEntry, LPCSTR gsEntry, LPCSTR psEntry, D3DWContext *ctx, D3DWEffect **oEffect)
{
  if(!oEffect)
    return E_POINTER;
  *oEffect = NULL;
  D3DWEffect *effect = new (std::nothrow) ComObject<D3DWEffect>();
  if(!effect)
    return E_OUTOFMEMORY;
  HRSRC hrsrc = FindResource(module, resourceName, resourceType);
  if(!hrsrc)
  {
    SafeDelete(effect);
    return HRESULT_FROM_WIN32(GetLastError());
  }  
  HGLOBAL resource = LoadResource(module, hrsrc);
  if(!resource)
  {
    SafeDelete(effect);
    return HRESULT_FROM_WIN32(GetLastError());
  }
  LPCSTR data = (LPCSTR)LockResource(resource);
  if(!data)
  {
    SafeDelete(effect);
    return HRESULT_FROM_WIN32(GetLastError());
  }
  SIZE_T size = SizeofResource(module, hrsrc);

  HRESULT hr = effect->InitFromMemory(data, size, vsEntry, gsEntry, psEntry, ctx);
  if(FAILED(hr))
  {
    SafeDelete(effect);
    return hr;
  }
  effect->AddRef();
  *oEffect = effect;  
  return S_OK;
}

HRESULT D3DWEffect::ReadFileIntoString(LPCWSTR filename, std::string& str)
{
  BYTE buffer[4096];
  SIZE_T nIn = 0;
  HANDLE file = CreateFile(
    filename,
    GENERIC_READ,
    FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
    NULL,
    OPEN_EXISTING,
    0,
    NULL);
  if(INVALID_HANDLE_VALUE == file)
    return HRESULT_FROM_WIN32(GetLastError());
  do
  {
    if(!ReadFile(file, buffer, ARRAYSIZE(buffer), &nIn, NULL))
    {
      CloseHandle(file);
      return HRESULT_FROM_WIN32(GetLastError());
    }
    str.append((char*)buffer, nIn);
  } while(nIn > 0);
  CloseHandle(file);
  return S_OK;
}

HRESULT D3DWEffect::InitFromMemory
  (LPCSTR data, SIZE_T size, LPCSTR vsEntry, LPCSTR gsEntry, LPCSTR psEntry, D3DWContext *ctx)
{
  if(!data || !vsEntry || !psEntry || !ctx)
    return E_POINTER;

  _ctx = ctx;

  HRESULT hr;
  ComPtr<ID3DBlob> code;
  V_HR(ctx->GetDevice(&_device));
  V_HR(ctx->GetDc(&_dc));

  V_HR(CompileShader(data, size, vsEntry, "vs_4_0", &code));
  V_HR(_device->CreateVertexShader(code->GetBufferPointer(), code->GetBufferSize(), NULL, &_vs));
  V_HR(D3DReflect(code->GetBufferPointer(), code->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&_vsReflect));
  V_HR(InitLayout(code));
  code.Release();
  V_HR(InitCbuffers(_vsReflect, _vsCbuffers, _vsRes));

  if(gsEntry)
  {
    V_HR(CompileShader(data, size, vsEntry, "gs_4_0", &code));
    V_HR(_device->CreateGeometryShader(code->GetBufferPointer(), code->GetBufferSize(), NULL, &_gs));
    V_HR(D3DReflect(code->GetBufferPointer(), code->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&_gsReflect));
    code.Release();
    V_HR(InitCbuffers(_gsReflect, _gsCbuffers, _gsRes));
  }

  V_HR(CompileShader(data, size, psEntry, "ps_4_0", &code));
  V_HR(_device->CreatePixelShader(code->GetBufferPointer(), code->GetBufferSize(), NULL, &_ps));
  V_HR(D3DReflect(code->GetBufferPointer(), code->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&_psReflect));
  code.Release();
  V_HR(InitCbuffers(_psReflect, _psCbuffers, _psRes));

  return S_OK;
}

HRESULT D3DWEffect::InitCbuffers(ID3D11ShaderReflection *reflect, std::vector<D3DWConstantBuffer*>& cbs, std::vector<ResourceVar*>& res)
{
  HRESULT hr;
  D3D11_SHADER_DESC sd;
  V_HR(reflect->GetDesc(&sd));
  for(UINT i = 0; i<sd.BoundResources; ++i)
  {
    D3D11_SHADER_INPUT_BIND_DESC srd;
    V_HR(reflect->GetResourceBindingDesc(i, &srd));
    if(D3D_SIT_CBUFFER == srd.Type)
    {
      std::string cbName(srd.Name);
      D3DWConstantBuffer *cb = _allBuffers[cbName];
      if(!cb)
      {
        V_HR(D3DWConstantBuffer::Create(srd.BindPoint, reflect->GetConstantBufferByName(srd.Name), this, &cb));
        _allBuffers[cbName] = cb;
      }
      cbs.push_back(cb);
    }
    else if(D3D_SIT_TEXTURE == srd.Type)
    {
      std::string resName(srd.Name);
      std::map<std::string,ResourceVar*>::iterator it = _allRes.find(resName);
      ResourceVar *rv = NULL;
      if(it == _allRes.end())
      {
        rv = new (std::nothrow) ResourceVar();
        if(!rv)
          return E_OUTOFMEMORY;
        rv->slot = srd.BindPoint;
        rv->dim = srd.Dimension;
        UINT n = res.size();  
        _allRes[resName] = rv;
      }
      res.push_back(rv);
    }    
  }
  return S_OK;
}

HRESULT D3DWEffect::InitLayout(ID3DBlob *vsBytecode)
{
  if(!vsBytecode)
    return E_POINTER;
  HRESULT hr;
  D3D11_SHADER_DESC sd;
  V_HR(_vsReflect->GetDesc(&sd));
  std::vector<D3D11_INPUT_ELEMENT_DESC> layout;
  UINT offset = 0;
  for(UINT i = 0; i<sd.InputParameters; ++i)
  {
    D3D11_INPUT_ELEMENT_DESC elt;
    D3D11_SIGNATURE_PARAMETER_DESC pd;
    V_HR(_vsReflect->GetInputParameterDesc(i, &pd));
    if(D3D_NAME_POSITION != pd.SystemValueType &&
       D3D_NAME_UNDEFINED != pd.SystemValueType)
    {
      continue;
    }
    elt.SemanticName = pd.SemanticName;
    elt.SemanticIndex = pd.SemanticIndex;
    elt.InstanceDataStepRate = 0;
    elt.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    elt.InputSlot = 0;
    int nComponents =
      (0x1 == pd.Mask) ? 1 :
      (0x3 == pd.Mask) ? 2 :
      (0x7 == pd.Mask) ? 3 :
      (0xF == pd.Mask) ? 4 : -1;
    elt.AlignedByteOffset = offset;    
    switch(pd.ComponentType)   
    {
      case D3D_REGISTER_COMPONENT_FLOAT32:
        switch(nComponents)
        {
          case 1:
            elt.Format = DXGI_FORMAT_R32_FLOAT;
            break;
          case 2:
            elt.Format = DXGI_FORMAT_R32G32_FLOAT;
            break;
          case 3:
            elt.Format = DXGI_FORMAT_R32G32B32_FLOAT;
            break;
          case 4:
            elt.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
            break;
          default:
#if defined(DEBUG) || defined(_DEBUG)
            OutputDebugString(L"Strange input component count\n");
            DebugBreak();
#endif
            return E_FAIL;
        }
        break;
      case D3D_REGISTER_COMPONENT_UINT32:
        switch(nComponents)
        {
          case 1:
            elt.Format = DXGI_FORMAT_R32_UINT;
            break;
          case 2:
            elt.Format = DXGI_FORMAT_R32G32_UINT;
            break;
          case 3:
            elt.Format = DXGI_FORMAT_R32G32B32_UINT;
            break;
          case 4:
            elt.Format = DXGI_FORMAT_R32G32B32A32_UINT;
            break;
          default:
#if defined(DEBUG) || defined(_DEBUG)
            OutputDebugString(L"Strange input component count\n");
            DebugBreak();
#endif
            return E_FAIL;
        }
        break;
      case D3D_REGISTER_COMPONENT_SINT32:
        switch(nComponents)
        {
          case 1:
            elt.Format = DXGI_FORMAT_R32_SINT;
            break;
          case 2:
            elt.Format = DXGI_FORMAT_R32G32_SINT;
            break;
          case 3:
            elt.Format = DXGI_FORMAT_R32G32B32_SINT;
            break;
          case 4:
            elt.Format = DXGI_FORMAT_R32G32B32A32_SINT;
            break;
          default:
#if defined(DEBUG) || defined(_DEBUG)
            OutputDebugString(L"Strange input component count\n");
            DebugBreak();
#endif
            return E_FAIL;
        }
        break;
      default:
        return E_FAIL;
    }
    layout.push_back(elt);
    offset += 4*nComponents;
  }

  V_HR_ASSERT(_device->CreateInputLayout(
                layout.data(),
                layout.size(),
                vsBytecode->GetBufferPointer(),
                vsBytecode->GetBufferSize(),
                &_layout),
              "Unable to create input layout");
  return S_OK;
}

HRESULT D3DWEffect::CompileShader(LPCSTR data, SIZE_T size, LPCSTR entry, LPCSTR profile, ID3DBlob **oBytecode)
{
  if(!oBytecode || !data || !entry || !profile)
    return E_POINTER;

  *oBytecode = NULL;

  DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_OPTIMIZATION_LEVEL3;
#if defined(DEBUG) || defined(_DEBUG)
 // shaderFlags |= D3DCOMPILE_DEBUG;
#endif
  ComPtr<ID3DBlob> code;
  ComPtr<ID3DBlob> errors;

  HRESULT hr = D3DCompile(
    data,
    size,
    entry,
    NULL,
    NULL,
    entry,
    profile,
    shaderFlags,
    0,
    &code,
    &errors);

  if(FAILED(hr))
  {
#if defined(DEBUG) || defined(_DEBUG)
    if(errors)
    {
      OutputDebugStringA((LPCSTR)errors->GetBufferPointer());
      OutputDebugString(L"\n");
    }
    DebugBreak();
#endif    
  }
  code->AddRef();
  *oBytecode = code;
  return S_OK;
}