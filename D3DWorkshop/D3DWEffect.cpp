#include "StdAfx.h"
#include "D3DWEffect.hpp"
#include "D3DWContext.hpp"
#include "D3DWConstantBuffer.hpp"

const IID IID_ID3D11ShaderReflection = {0x8d536ca1, 0x0cca, 0x4956, {0xa8, 0x37, 0x78, 0x69, 0x63, 0x75, 0x55, 0x84}};

D3DWEffect::D3DWEffect()
{
}

D3DWEffect::~D3DWEffect()
{
  for(int i = 0, size = _allRes.size(); i<size; ++i)
    SafeDelete(_allRes[i]);

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


STDMETHODIMP D3DWEffect::Apply()
{
  HRESULT hr = S_OK;

  _dc->IASetInputLayout(_layout);

  _dc->VSSetShader(_vs, NULL, NULL);
  for(int i = 0, n = _vsRes.size(); i<n; ++i)
  {
    ResourceVar *rv = _vsRes[i];
    UINT slot = rv->vsSlot;
    IUnknown **pResource = rv->resource.AddressOf();
    switch(rv->rvType)
    {
    case RVT_CONSTANT_BUFFER:
      {
        _dc->VSSetConstantBuffers(slot, 1, (ID3D11Buffer**)pResource);
      }
      break;
    case RVT_TEXTURE:
    case RVT_CUBE_MAP:
    case RVT_BUFFER:
      {
        _dc->VSSetShaderResources(slot, 1, (ID3D11ShaderResourceView**)pResource);
      }
      break;
    case RVT_SAMPLER:
      {
        _dc->VSSetSamplers(slot, 1, (ID3D11SamplerState**)pResource);
      }
      break;
    }
  }

  if(_gs)
  {
    _dc->GSSetShader(_gs, NULL, NULL);
    for(int i = 0, n = _gsRes.size(); i<n; ++i)
    {
      ResourceVar *rv = _gsRes[i];
      UINT slot = rv->gsSlot;
      IUnknown **pResource = rv->resource.AddressOf();
      switch(rv->rvType)
      {
      case RVT_CONSTANT_BUFFER:
        {
          _dc->GSSetConstantBuffers(slot, 1, (ID3D11Buffer**)pResource);
        }
        break;
      case RVT_TEXTURE:
      case RVT_CUBE_MAP:
      case RVT_BUFFER:
        {
          _dc->GSSetShaderResources(slot, 1, (ID3D11ShaderResourceView**)pResource);
        }
        break;
      case RVT_SAMPLER:
        {
          _dc->GSSetSamplers(slot, 1, (ID3D11SamplerState**)pResource);
        }
        break;
      }
    }
  }

  _dc->PSSetShader(_ps, NULL, NULL);
  for(int i = 0, n = _psRes.size(); i<n; ++i)
  {
    ResourceVar *rv = _psRes[i];
    UINT slot = rv->psSlot;
    IUnknown **pResource = rv->resource.AddressOf();
    switch(rv->rvType)
    {
    case RVT_CONSTANT_BUFFER:
      {
        _dc->PSSetConstantBuffers(slot, 1, (ID3D11Buffer**)pResource);
      }
      break;
    case RVT_TEXTURE:
    case RVT_CUBE_MAP:
    case RVT_BUFFER:
      {
        _dc->PSSetShaderResources(slot, 1, (ID3D11ShaderResourceView**)pResource);
      }
      break;
    case RVT_SAMPLER:
      {
        _dc->PSSetSamplers(slot, 1, (ID3D11SamplerState**)pResource);
      }
      break;
    }
  }

  return hr;
}

STDMETHODIMP D3DWEffect::LockTargets(ID3DWRenderTargetInternals *output)
{
  HRESULT hr = S_OK;
  for(int i = 0, size = _renderTargets.size(); i<size; ++i)
  {
    ResourceVar *rv = _renderTargets[i];
    if(!rv->internals)
      continue;
    if(RVT_TEXTURE == rv->rvType)
    {
      HRESULT hr2 = ((ID3DWTextureInternals*)(IUnknown*)rv->internals)->Lock3D();
      if(FAILED(hr2) && SUCCEEDED(hr))
        hr = hr2;
    }
    if(rv->internals.IsEqualObject(output))
      hr = E_INVALIDARG;
  }
  return hr;
}

STDMETHODIMP D3DWEffect::UnlockTargets()
{
  HRESULT hr = S_OK;
  for(int i = 0, size = _renderTargets.size(); i<size; ++i)
  {
    ResourceVar *rv = _renderTargets[i];
    if(!rv->internals)
      continue;
    if(RVT_TEXTURE == rv->rvType)
    {
      HRESULT hr2 = ((ID3DWTextureInternals*)(IUnknown*)rv->internals)->Unlock3D();
      if(FAILED(hr2) && SUCCEEDED(hr))
        hr = hr2;
    }
  }
  return hr;
}

STDMETHODIMP D3DWEffect::SetConstantBuffer(LPCSTR name, ID3DWConstantBuffer *constantBuffer)
{
  HRESULT hr;
  if(!name)
    return E_POINTER;
  
  std::string rn(name);
  ResourceMap::iterator it = _constantBuffers.find(rn);
  if(it == _constantBuffers.end())
    return E_INVALIDARG;
  ResourceVar *rv = it->second;
  
  if(!constantBuffer)
  {
    rv->internals = NULL;
    rv->resource = NULL;
    return S_OK;
  }

  ComPtr<ID3DWConstantBufferInternals> internals;
  ComPtr<ID3D11Buffer> buffer;
  SIZE_T size;
  hr = constantBuffer->QueryInterface(IID_PPV_ARGS(&internals));
  if(SUCCEEDED(hr))
  {
    hr = internals->GetCb(&buffer);
    if(SUCCEEDED(hr))
      hr = constantBuffer->GetByteSize(&size);
  }
  if(FAILED(hr))
  {
    V_HR_BREAK("Invalid constant buffer object passed to ID3DWEffect::SetConstantBuffer");
    return E_INVALIDARG;
  }
  if(size < rv->size)
  {
    V_HR_BREAK("Constant buffer is smaller then shader resource variable");
    return HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
  }
  rv->internals = internals;
  rv->resource = buffer;
  return S_OK;
}

STDMETHODIMP D3DWEffect::SetBuffer(LPCSTR name, ID3DWBuffer *buffer)
{
  HRESULT hr;
  if(!name)
    return E_POINTER;
  
  std::string rn(name);
  ResourceMap::iterator it = _buffers.find(rn);
  if(it == _buffers.end())
    return E_INVALIDARG;
  ResourceVar *rv = it->second;
  
  if(!buffer)
  {
    rv->internals = NULL;
    rv->resource = NULL;
    return S_OK;
  }

  ComPtr<ID3DWBufferInternals> internals;
  ComPtr<ID3D11ShaderResourceView> srv;
  SIZE_T size;
  hr = buffer->QueryInterface(IID_PPV_ARGS(&internals));
  if(SUCCEEDED(hr))
  {
    hr = internals->GetSrv(&srv);
    if(SUCCEEDED(hr))
      hr = buffer->GetByteSize(&size);
  }
  if(FAILED(hr))
  {
    V_HR_BREAK("Invalid buffer object passed to ID3DWEffect::SetBuffer");
    return E_INVALIDARG;
  }
  if(size < rv->size)
  {
    V_HR_BREAK("Buffer is smaller then shader resource variable");
    return HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
  }
  rv->internals = internals;
  rv->resource = buffer;
  return S_OK;
}

STDMETHODIMP D3DWEffect::SetTexture(LPCSTR name, ID3DWTexture *texture)
{
  HRESULT hr;
  if(!name)
    return E_POINTER;
  
  std::string rn(name);
  ResourceMap::iterator it = _textures.find(rn);
  if(it == _textures.end())
    return E_INVALIDARG;
  ResourceVar *rv = it->second;
  
  if(!texture)
  {
    rv->internals = NULL;
    rv->resource = NULL;
    return S_OK;
  }

  ComPtr<ID3DWTextureInternals> internals;
  ComPtr<ID3D11ShaderResourceView> srv;
  hr = texture->QueryInterface(IID_PPV_ARGS(&internals));
  if(SUCCEEDED(hr))
  {
    hr = internals->GetSrv(&srv);
  }
  if(FAILED(hr))
  {
    V_HR_BREAK("Invalid texture object passed to ID3DWEffect::SetTexture");
    return E_INVALIDARG;
  }
  rv->internals = internals;
  rv->resource = srv;
  return S_OK;
}

STDMETHODIMP D3DWEffect::SetCubeMap(LPCSTR name, ID3DWCubeMap *cubeMap)
{
  HRESULT hr;
  if(!name)
    return E_POINTER;
  
  std::string rn(name);
  ResourceMap::iterator it = _cubeMaps.find(rn);
  if(it == _cubeMaps.end())
    return E_INVALIDARG;
  ResourceVar *rv = it->second;
  
  if(!cubeMap)
  {
    rv->internals = NULL;
    rv->resource = NULL;
    return S_OK;
  }

  ComPtr<ID3DWCubeMapInternals> internals;
  ComPtr<ID3D11ShaderResourceView> srv;
  hr = cubeMap->QueryInterface(IID_PPV_ARGS(&internals));
  if(SUCCEEDED(hr))
  {
    hr = internals->GetSrv(&srv);
  }
  if(FAILED(hr))
  {
    V_HR_BREAK("Invalid cube map object passed to ID3DWEffect::SetCubeMap");
    return E_INVALIDARG;
  }
  rv->internals = internals;
  rv->resource = srv;
  return S_OK;
}

STDMETHODIMP D3DWEffect::SetSampler(LPCSTR name, ID3DWSampler *sampler)
{
  HRESULT hr;
  if(!name)
    return E_POINTER;
  
  std::string rn(name);
  ResourceMap::iterator it = _samplers.find(rn);
  if(it == _samplers.end())
    return E_INVALIDARG;
  ResourceVar *rv = it->second;
  
  if(!sampler)
  {
    rv->internals = NULL;
    rv->resource = NULL;
    return S_OK;
  }

  ComPtr<ID3DWSamplerInternals> internals;
  ComPtr<ID3D11SamplerState> samState;
  hr = sampler->QueryInterface(IID_PPV_ARGS(&internals));
  if(SUCCEEDED(hr))
  {
    hr = internals->GetSamplerState(&samState);
  }
  if(FAILED(hr))
  {
    V_HR_BREAK("Invalid sampler object passed to ID3DWEffect::SetSampler");
    return E_INVALIDARG;
  }
  rv->internals = internals;
  rv->resource = samState;
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
  HRESULT hr;
  if(!data || !vsEntry || !psEntry || !ctx)
    return E_POINTER;

  _ctx = ctx;

  V_HR(_ctx->GetDevice(&_device));
  V_HR(_ctx->GetDc(&_dc));

  ComPtr<ID3DBlob> code;

  V_HR(CompileShader(data, size, vsEntry, "vs_4_0", &code));
  V_HR(_device->CreateVertexShader(code->GetBufferPointer(), code->GetBufferSize(), NULL, &_vs));
  V_HR(InitResources(code, ST_VS));
  V_HR(InitLayout(code));
  code.Release();

  if(gsEntry)
  {
    V_HR(CompileShader(data, size, vsEntry, "gs_4_0", &code));
    V_HR(_device->CreateGeometryShader(code->GetBufferPointer(), code->GetBufferSize(), NULL, &_gs));
    V_HR(InitResources(code, ST_GS));;
    code.Release();
  }

  V_HR(CompileShader(data, size, psEntry, "ps_4_0", &code));
  V_HR(_device->CreatePixelShader(code->GetBufferPointer(), code->GetBufferSize(), NULL, &_ps));
  V_HR(InitResources(code, ST_PS));
  code.Release();

  return S_OK;
}

HRESULT D3DWEffect::InitResources(ID3DBlob *bytecode, S_TYPE sType)
{
  HRESULT hr;
  if(!bytecode)
    return E_POINTER;

  ResourceVector *res = NULL;
  switch(sType)
  {
  case ST_VS:
    res = &_vsRes;
    break;
  case ST_GS:
    res = &_gsRes;
    break;
  case ST_PS:
    res = &_psRes;
    break;
  default:
    return E_INVALIDARG;
  }

  ComPtr<ID3D11ShaderReflection> reflect;
  V_HR(D3DReflect(
        bytecode->GetBufferPointer(),
        bytecode->GetBufferSize(),
        IID_ID3D11ShaderReflection,
        (LPVOID*)&reflect));

  D3D11_SHADER_DESC sd;
  V_HR(reflect->GetDesc(&sd));

  for(UINT i = 0; i<sd.BoundResources; ++i)
  {
    D3D11_SHADER_INPUT_BIND_DESC bd;
    V_HR(reflect->GetResourceBindingDesc(i, &bd));

    ResourceMap *rm = NULL;
    RV_TYPE rvType;
    ID3D11ShaderReflectionConstantBuffer *reflectCb = NULL;
    BOOL rt = FALSE;
    switch(bd.Type)
    {
    case D3D_SIT_CBUFFER:
      rm = &_constantBuffers;
      rvType = RVT_CONSTANT_BUFFER;
      reflectCb = reflect->GetConstantBufferByName(bd.Name);
      break;
    case D3D_SIT_TBUFFER:
      rm = &_buffers;
      rvType = RVT_BUFFER;
      rt = TRUE;
      reflectCb = reflect->GetConstantBufferByName(bd.Name);
      break;
    case D3D_SIT_TEXTURE:
      if(D3D_SRV_DIMENSION_TEXTURE2D == bd.Dimension)
      {
        rm = &_textures;
        rvType = RVT_TEXTURE;        
        rt = TRUE;
        break;
      }
      else if(D3D_SRV_DIMENSION_TEXTURECUBE == bd.Dimension)
      {
        rm = &_cubeMaps;
        rvType = RVT_CUBE_MAP;
        rt = TRUE;
      }
      else
        continue;
      break;
    case D3D_SIT_SAMPLER:
      rm = &_samplers;
      rvType = RVT_SAMPLER;
      break;
    default:
      continue;
    }

    std::string rn(bd.Name);
    ResourceVar* rv = NULL;
    if(rm->find(rn) != rm->end())
      rv = rm->at(rn);
    if(!rv)
    {
      rv = new (std::nothrow) ResourceVar();
      if(!rv)
        return E_OUTOFMEMORY;
      if(reflectCb)
      {
        D3D11_SHADER_BUFFER_DESC cbd;
        hr = reflectCb->GetDesc(&cbd);
        if(FAILED(hr))
        {
          SafeDelete(rv);
          continue;
        }
        if(D3D_CT_CBUFFER == cbd.Type || D3D_CT_TBUFFER == cbd.Type)
          rv->size = cbd.Size;
        else
        {
          SafeDelete(rv);
          continue;
        }
      }
      rv->rvType = rvType;
      _allRes.push_back(rv);
      if(rt)
        _renderTargets.push_back(rv);
      (*rm)[rn] = rv;
    }

    switch(sType)
    {
    case ST_VS:
      rv->vsSlot = bd.BindPoint;
      break;
    case ST_GS:
      rv->gsSlot = bd.BindPoint;
      break;
    case ST_PS:
      rv->psSlot = bd.BindPoint;
      break;
    default:
      return E_INVALIDARG;
    }

    res->push_back(rv);
  }

  return S_OK;
}

HRESULT D3DWEffect::InitLayout(ID3DBlob *vsBytecode)
{
  HRESULT hr;
  if(!vsBytecode)
    return E_POINTER;
  
  ComPtr<ID3D11ShaderReflection> vsReflect;
  V_HR(D3DReflect(
        vsBytecode->GetBufferPointer(),
        vsBytecode->GetBufferSize(),
        IID_ID3D11ShaderReflection,
        (LPVOID*)&vsReflect));

  D3D11_SHADER_DESC sd;
  V_HR(vsReflect->GetDesc(&sd));

  std::vector<D3D11_INPUT_ELEMENT_DESC> layout;
  UINT offset = 0;
  for(UINT i = 0; i<sd.InputParameters; ++i)
  {
    D3D11_INPUT_ELEMENT_DESC elt;
    D3D11_SIGNATURE_PARAMETER_DESC pd;
    V_HR(vsReflect->GetInputParameterDesc(i, &pd));
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
  shaderFlags |= D3DCOMPILE_DEBUG;
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