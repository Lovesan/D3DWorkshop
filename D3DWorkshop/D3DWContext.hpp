#ifndef __D3DWCONTEXT_HPP__
#define __D3DWCONTEXT_HPP__

#include <windows.h>
#include <wincodec.h>
#include <d3d11.h>
#include <d3d10_1.h>
#include <d3d10.h>
#include <d2d1.h>
#include "D3DW.h"
#include "D3DWInternals.hpp"

#ifndef __D3DWWINDOW_DEFINED__
class D3DWWindow;
#endif // __D3DWWINDOW_DEFINED__

#ifndef __D3DWEFFECT_DEFINED__
class D3DWEffect;
#endif // __D3DWEFFECT_DEFINED__

#ifndef __D3DWMESH_DEFINED__
class D3DWMesh;
#endif // __D3DWMESH_DEFINED__

#ifndef __D3DWTEXTURE_DEFINED__
class D3DWTexture;
#endif // __D3DWTexture_DEFINED__

#ifndef __D3DWCUBEMAP_DEFINED__
class D3DWCubeMap;
#endif // __D3DWCUBEMAP_DEFINED__

#ifndef __D3DWFLOAT_ANIMATION_DEFINED__
class D3DWFloatAnimation;
#endif // __D3DWFLOAT_ANIMATION_DEFINED__

class COM_NO_VTABLE D3DWContext :
  public ID3DWContext, 
  public ID3DWContextInternals
{
public:
  BEGIN_INTERFACE_MAP    
    INTERFACE_MAP_ENTRY(ID3DWContext)
    INTERFACE_MAP_ENTRY(ID3DWContextInternals)
  END_INTERFACE_MAP

  static HRESULT WINAPI Create(HWND hwnd, D3DWWindow *window, D3DWContext **oContext);
  virtual ~D3DWContext();
    
  STDMETHODIMP GetSwapChain(IDXGISwapChain **oSwc);
  STDMETHODIMP Present();
  STDMETHODIMP GetWindow(ID3DWWindow **oWindow);
  STDMETHODIMP GetSize(UINT *oWidth, UINT *oHeight);
  STDMETHODIMP SetSize(UINT width, UINT height);
  STDMETHODIMP GetAnisotropyLevel(UINT *oLevel);
  STDMETHODIMP SetAnisotropyLevel(UINT level);
  STDMETHODIMP SetAsTarget();
  STDMETHODIMP Clear(FLOAT clearColor[4]);
  STDMETHODIMP ClearDepthStencil();
  STDMETHODIMP CreateEffectFromMemory(
    LPCSTR data, SIZE_T size, LPCSTR vsEntry, LPCSTR gsEntry, LPCSTR psEntry, ID3DWEffect **oEffect);
  STDMETHODIMP CreateEffectFromFile(
    LPCWSTR filename, LPCSTR vsEntry, LPCSTR gsEntry, LPCSTR psEntry, ID3DWEffect **oEffect);
  STDMETHODIMP CreateEffectFromResource(
    HMODULE module, LPCWSTR resourceType, LPCWSTR resourceName, LPCSTR vsEntry, LPCSTR gsEntry, LPCSTR psEntry, ID3DWEffect **oEffect);
  STDMETHODIMP CreateTexture(
    UINT width, UINT height, ID3DWTexture **oTex);
  STDMETHODIMP CreateTextureFromMemory(
    LPVOID data, SIZE_T size, BOOL canDraw, ID3DWTexture **oTex);
  STDMETHODIMP CreateTextureFromResource(
    HMODULE module, LPCWSTR resourceName, LPCWSTR resourceType, BOOL canDraw, ID3DWTexture **oTex);
  STDMETHODIMP CreateTextureFromFile(
    LPCWSTR filename, BOOL canDraw, ID3DWTexture **oTex);
  STDMETHODIMP CreateCubeMap(
    ID3DWTexture *posX,
    ID3DWTexture *negX,
    ID3DWTexture *posY,
    ID3DWTexture *negY,
    ID3DWTexture *posZ,
    ID3DWTexture *negZ,
    ID3DWCubeMap **oCubeMap);
  STDMETHODIMP CreateMesh(
    LPVOID vertexData, SIZE_T vertexSize, UINT nVertices, UINT32 *indices, UINT nIndices, D3DW_TOPOLOGY topology, BOOL canMap, ID3DWMesh **oMesh);
  STDMETHODIMP CreateFloatAnimation(
    FLOAT begin, FLOAT end, FLOAT interval, BOOL repeat, BOOL autoReverse, ID3DWFloatAnimation **oAnimation);

  HRESULT GetDevice(ID3D11Device **oDevice);
  HRESULT GetDc(ID3D11DeviceContext **oDc);
  HRESULT GetDevice10(ID3D10Device1 **oDevice);
  HRESULT GetWic(IWICImagingFactory **oWic);
  HRESULT Get2DFactory(ID2D1Factory **oFactory);

protected:  
  D3DWContext();

private:
  HWND _hwnd;
  D3DWWindow *_wnd;
  ComPtr<ID2D1Factory> _2dFactory;
  ComPtr<IWICImagingFactory> _wic;
  ComPtr<ID3D11Device> _device;
  ComPtr<ID3D11DeviceContext> _dc;
  ComPtr<IDXGISwapChain> _swc;
  ComPtr<ID3D10Device1> _device10;
  ComPtr<ID3D11RenderTargetView> _rtv;
  ComPtr<ID3D11DepthStencilView> _dsv;
  D3D11_VIEWPORT _vp;
  ComPtr<ID3D11SamplerState> _sampler;
  UINT _afLevel;
    
  D3DWContext(const D3DWContext& copy);
  HRESULT Initialize(HWND hwnd, D3DWWindow *window);  
  HRESULT RecreateTargets(UINT width, UINT height);
};

#define __D3DWCONTEXT_DEFINED__

#endif // __D3DWCONTEXT_HPP__