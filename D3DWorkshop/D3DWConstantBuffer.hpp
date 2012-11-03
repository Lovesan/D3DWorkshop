#ifndef __D3DWCONSTANT_BUFFER_HPP__
#define __D3DWCONSTANT_BUFFER_HPP__

#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include "D3DW.h"

#ifndef __D3DWEFFECT_DEFINED__
class D3DWEffect;
#endif // __D3DWEFFECT_DEFINED__

class COM_NO_VTABLE D3DWConstantBuffer : public ID3DWConstantBuffer
{
public:
  BEGIN_INTERFACE_MAP
    INTERFACE_MAP_ENTRY(ID3DWConstantBuffer)
  END_INTERFACE_MAP

  static HRESULT Create(UINT slot, ID3D11ShaderReflectionConstantBuffer *reflect, D3DWEffect *effect, D3DWConstantBuffer **oCb);
  virtual ~D3DWConstantBuffer();

  HRESULT GetBuffer(ID3D11Buffer **oBuffer);
  HRESULT GetSlot(UINT *oSlot);

  STDMETHODIMP GetEffect(ID3DWEffect **oEffect);
  STDMETHODIMP GetSize(SIZE_T *oSize);
  STDMETHODIMP Update(LPVOID data, SIZE_T size);

protected:  
  D3DWConstantBuffer();

private:
  D3DWEffect *_effect;
  UINT _size;
  LPCSTR _name;
  UINT _slot;
  ComPtr<ID3D11Buffer> _buffer;
  
  HRESULT Initialize(UINT reg, ID3D11ShaderReflectionConstantBuffer *reflect, D3DWEffect *effect);
};

#define __D3DWCONSTANT_BUFFER_DEFINED__

#endif //__D3DWCONSTANT_BUFFER_HPP__