#ifndef __D3DWCONSTANT_BUFFER_HPP__
#define __D3DWCONSTANT_BUFFER_HPP__

#include <windows.h>
#include <d3d11.h>
#include "ComUtils.hpp"
#include "D3DW.h"
#include "D3DWInternals.hpp"

#ifndef __D3DWCONTEXT_DEFINED__
class D3DWContext;
#endif // __D3DWCONTEXT_DEFINED__

class COM_NO_VTABLE D3DWConstantBuffer :
  public ID3DWConstantBuffer,
  public ID3DWConstantBufferInternals
{
public:
  BEGIN_INTERFACE_MAP
    INTERFACE_MAP_ENTRY(ID3DWConstantBuffer)
    INTERFACE_MAP_ENTRY(ID3DWResourceInternals)
    INTERFACE_MAP_ENTRY(ID3DWConstantBufferInternals)
  END_INTERFACE_MAP

  static HRESULT Create(SIZE_T size, D3DWContext *ctx, D3DWConstantBuffer **oCb);
  virtual ~D3DWConstantBuffer();
  
  STDMETHODIMP GetRes(ID3D11Resource **oRes);
  STDMETHODIMP GetCb(ID3D11Buffer **oCb);
  STDMETHODIMP GetContext(ID3DWContext **oCtx);
  STDMETHODIMP GetByteSize(SIZE_T *oSize);
  STDMETHODIMP Update(LPVOID data, SIZE_T size);

protected:  
  D3DWConstantBuffer();

private:
  ComPtr<D3DWContext> _ctx;
  ComPtr<ID3D11Device> _device;
  ComPtr<ID3D11DeviceContext> _dc;
  ComPtr<ID3D11Buffer> _buffer;
  SIZE_T _size;
  
  HRESULT Initialize(SIZE_T size, D3DWContext *ctx);
};

#define __D3DWCONSTANT_BUFFER_DEFINED__

#endif //__D3DWCONSTANT_BUFFER_HPP__