#ifndef __D3DWORKSHOP_H__
#define __D3DWORKSHOP_H__

#include <windows.h>

#define D3DW_MAJOR_VERSION 0
#define D3DW_MINOR_VERSION 2
#define D3DW_BUILD 0
#define D3DW_REVISION 1
#define D3DW_VERSION_STRING "0.2.0.1"

#ifdef __cplusplus
#define D3DW_EXTERN_C extern "C" extern
#else
#define D3DW_EXTERN_C extern
#endif

typedef interface ID3DWWindow ID3DWWindow;
typedef interface ID3DWWindowEvents ID3DWWindowEvents;
typedef interface ID3DWContext ID3DWContext;
typedef interface ID3DWContextChild ID3DWContextChild;
typedef interface ID3DWEffect ID3DWEffect;
typedef interface ID3DWSampler ID3DWSampler;
typedef interface ID3DWConstantBuffer ID3DWConstantBuffer;
typedef interface ID3DWMesh ID3DWMesh;
typedef interface ID3DWResource ID3DWResource;
typedef interface ID3DWBuffer ID3DWBuffer;
typedef interface ID3DWTexture ID3DWTexture;
typedef interface ID3DWSurface ID3DWSurface;
typedef interface ID3DWCubeMap ID3DWCubeMap;
typedef interface ID3DWFloatAnimation ID3DWFloatAnimation;

typedef enum _D3DW_MOUSE_BUTTON
{
  D3DW_MOUSE_BUTTON_LEFT,
  D3DW_MOUSE_BUTTON_MIDDLE,
  D3DW_MOUSE_BUTTON_RIGHT,
  D3DW_MOUSE_BUTTON_X1,
  D3DW_MOUSE_BUTTON_X2
} D3DW_MOUSE_BUTTON;

typedef enum _D3DW_TOPOLOGY
{
  D3DW_TOPOLOGY_TRIANGLE,
  D3DW_TOPOLOGY_LINE,
  D3DW_TOPOLOGY_POINT
} D3DW_TOPOLOGY;

typedef enum _D3DW_TEXTURE_MODE
{
  D3DW_TEXTURE_MODE_WRAP,
  D3DW_TEXTURE_MODE_MIRROR,
  D3DW_TEXTURE_MODE_CLAMP,
  D3DW_TEXTURE_MODE_BORDER,
  D3DW_TEXTURE_MODE_MIRROR_ONCE
} D3DW_TEXTURE_MODE;

D3DW_EXTERN_C HRESULT WINAPI D3DWCreateWindow(
  UINT width, UINT height, INT x, INT y, HWND parent, ID3DWWindow **oWindow);

MIDL_INTERFACE("{AD5995AD-771B-4C29-86F8-237D6780BCC1}")
ID3DWWindow : public IUnknown
{
public:
  STDMETHOD(Render)() = 0;
  STDMETHOD(AddEventSink)(ID3DWWindowEvents *events) = 0;
  STDMETHOD(RemoveEventSink)(ID3DWWindowEvents *events) = 0;
  STDMETHOD(GetRenderMode)(BOOL *oRenderAlways, BOOL *oPauseOnResize) = 0;
  STDMETHOD(SetRenderMode)(BOOL renderAlways, BOOL pauseOnResize) = 0;
  STDMETHOD(GetContext)(ID3DWContext **oCtx) = 0;
  STDMETHOD(GetFullscreenMode)(BOOL *oFullscreen) = 0;
  STDMETHOD(SetFullscreenMode)(BOOL fullscreen) = 0;
  STDMETHOD(GetSize)(UINT *oWidth, UINT *oHeight) = 0;
  STDMETHOD(SetSize)(UINT width, UINT height) = 0;
  STDMETHOD(GetPosition)(INT *oX, INT *oY) = 0;
  STDMETHOD(SetPosition)(INT x, INT y) = 0;
  STDMETHOD(Show)() = 0;
  STDMETHOD(ShowDialog)() = 0;
  STDMETHOD(Hide)() = 0;
  STDMETHOD(Close)() = 0;
  STDMETHOD(GetTitle)(LPCWSTR *oTitle) = 0;
  STDMETHOD(SetTitle)(LPCWSTR title) = 0;
  STDMETHOD(GetIcon)(HICON *oIcon) = 0;
  STDMETHOD(SetIcon)(HICON icon) = 0;
  STDMETHOD(GetMouseCapture)(BOOL *oCaptured) = 0;
  STDMETHOD(SetMouseCapture)(BOOL captured) = 0;
  STDMETHOD(GetKeyboardFocus)(BOOL *oFocused) = 0;
  STDMETHOD(SetKeyboardFocus)(BOOL focused) = 0;
};

MIDL_INTERFACE("{B353B9E9-B90B-4054-A06D-49F006C1F213}")
ID3DWWindowEvents : public IUnknown
{
public:
  STDMETHOD(OnLoaded)() = 0;
  STDMETHOD(OnClosed)() = 0;
  STDMETHOD(OnResize)(UINT width, UINT height) = 0;
  STDMETHOD(OnRender)() = 0;
  STDMETHOD(OnKeyDown)(DWORD vkKey) = 0;
  STDMETHOD(OnKeyUp)(DWORD vkKey) = 0;
  STDMETHOD(OnMouseDown)(INT x, INT y, D3DW_MOUSE_BUTTON button) = 0;
  STDMETHOD(OnMouseUp)(INT x, INT y, D3DW_MOUSE_BUTTON button) = 0;
  STDMETHOD(OnMouseDoubleClick)(INT x, INT y, D3DW_MOUSE_BUTTON button) = 0;
  STDMETHOD(OnMouseEnter)(INT x, INT y) = 0;
  STDMETHOD(OnMouseMove)(INT x, INT y) = 0;
  STDMETHOD(OnMouseLeave)(INT x, INT y) = 0;
  STDMETHOD(OnMouseWheel)(INT x, INT y, INT delta) = 0;
  STDMETHOD(OnGotCapture)(INT x, INT y) = 0;
  STDMETHOD(OnLostCapture)(INT x, INT y) = 0;
  STDMETHOD(OnGotFocus)() = 0;
  STDMETHOD(OnLostFocus)() = 0;
};

#ifdef __cplusplus
class __declspec(novtable) ID3DWWindowEventsImpl : public ID3DWWindowEvents
{
public:
	STDMETHODIMP OnLoaded() { return S_OK; }
	STDMETHODIMP OnClosed() { return S_OK; }
	STDMETHODIMP OnResize(UINT, UINT) { return S_OK; }
	STDMETHODIMP OnRender() { return S_OK; }
	STDMETHODIMP OnKeyDown(DWORD) { return S_OK; }
	STDMETHODIMP OnKeyUp(DWORD) { return S_OK; }
	STDMETHODIMP OnMouseDown(INT, INT, D3DW_MOUSE_BUTTON) { return S_OK; }
	STDMETHODIMP OnMouseUp(INT, INT, D3DW_MOUSE_BUTTON) { return S_OK; }
	STDMETHODIMP OnMouseDoubleClick(INT, INT, D3DW_MOUSE_BUTTON) { return S_OK; }
	STDMETHODIMP OnMouseEnter(INT, INT) { return S_OK; }
	STDMETHODIMP OnMouseMove(INT, INT) { return S_OK; }
	STDMETHODIMP OnMouseLeave(INT, INT) { return S_OK; }
	STDMETHODIMP OnMouseWheel(INT, INT, INT) { return S_OK; }
	STDMETHODIMP OnGotCapture(INT, INT) { return S_OK; }
	STDMETHODIMP OnLostCapture(INT, INT) { return S_OK; }
	STDMETHODIMP OnGotFocus() { return S_OK; }
	STDMETHODIMP OnLostFocus() { return S_OK; }
};
#endif

MIDL_INTERFACE("{06FD722D-60BF-4F54-9593-9D9ECF2E2360}")
ID3DWContext : public IUnknown
{
public:
  STDMETHOD(GetWindow)(ID3DWWindow **oWindow) = 0;
  STDMETHOD(GetSize)(UINT *oWidth, UINT *oHeight) = 0;
  STDMETHOD(SetSize)(UINT width, UINT height) = 0;
  STDMETHOD(Clear)(const FLOAT clearColor[4]) = 0;
  STDMETHOD(ClearDepthStencil)() = 0;
  STDMETHOD(CreateEffectFromMemory)(
    LPCSTR data, SIZE_T size, LPCSTR vsEntry, LPCSTR gsEntry, LPCSTR psEntry, ID3DWEffect **oEffect) = 0;
  STDMETHOD(CreateEffectFromFile)(
    LPCWSTR filename, LPCSTR vsEntry, LPCSTR gsEntry, LPCSTR psEntry, ID3DWEffect **oEffect) = 0;
  STDMETHOD(CreateEffectFromResource)(
    HMODULE module, LPCWSTR resourceName, LPCWSTR resourceType, LPCSTR vsEntry, LPCSTR gsEntry, LPCSTR psEntry, ID3DWEffect **oEffect) = 0;
  STDMETHOD(CreateSampler)(
    UINT anisotropyLevel, D3DW_TEXTURE_MODE addressMode, const FLOAT borderColor[4], ID3DWSampler **oSampler) = 0;
  STDMETHOD(CreateConstantBuffer)(SIZE_T size, ID3DWConstantBuffer **oConstantBuffer) = 0;
  STDMETHOD(CreateTexture)(
    UINT width, UINT height, ID3DWTexture **oTex) = 0;
  STDMETHOD(CreateTextureFromMemory)(
    LPVOID data, SIZE_T size, BOOL canDraw, ID3DWTexture **oTex) = 0;
  STDMETHOD(CreateTextureFromResource)(
    HMODULE module, LPCWSTR resourceName, LPCWSTR resourceType, BOOL canDraw, ID3DWTexture **oTex) = 0;
  STDMETHOD(CreateTextureFromFile)(
    LPCWSTR filename, BOOL canDraw, ID3DWTexture **oTex) = 0;
  STDMETHOD(CreateCubeMap)(
    ID3DWTexture *posX,
    ID3DWTexture *negX,
    ID3DWTexture *posY,
    ID3DWTexture *negY,
    ID3DWTexture *posZ,
    ID3DWTexture *negZ,
    ID3DWCubeMap **oCubeMap) = 0;
  STDMETHOD(CreateMesh)(
    LPVOID vertexData, SIZE_T vertexSize, UINT nVertices, UINT32 *indices, UINT nIndices, D3DW_TOPOLOGY topology, ID3DWMesh **oMesh) = 0;
  STDMETHOD(CreateFloatAnimation)(
    FLOAT begin, FLOAT end, FLOAT interval, BOOL repeat, BOOL autoReverse, ID3DWFloatAnimation **oAnimation) = 0;
};

MIDL_INTERFACE("{2F75FC4A-C526-46F6-9667-2AF5EA573724}")
ID3DWContextChild : public IUnknown
{
public:
  STDMETHOD(GetContext)(ID3DWContext **oCtx) = 0;
};

MIDL_INTERFACE("{5C5058C2-045A-4410-9B54-9E7862F38EB7}")
ID3DWEffect : public ID3DWContextChild
{
public:
  STDMETHOD(SetConstantBuffer)(LPCSTR name, ID3DWConstantBuffer *constantBuffer) = 0;
  STDMETHOD(SetBuffer)(LPCSTR name, ID3DWBuffer *buffer) = 0;
  STDMETHOD(SetTexture)(LPCSTR name, ID3DWTexture *texture) = 0;
  STDMETHOD(SetCubeMap)(LPCSTR name, ID3DWCubeMap *cubeMap) = 0;
  STDMETHOD(SetSampler)(LPCSTR name, ID3DWSampler *sampler) = 0;
};

MIDL_INTERFACE("{A2F7DB23-F10C-49D3-9041-A190CA9D1E6A}")
ID3DWMesh : public ID3DWContextChild
{
public:
  STDMETHOD(GetVertexSize)(SIZE_T *oVertexSize) = 0;
  STDMETHOD(GetVertexCount)(UINT *oCount) = 0;
  STDMETHOD(GetIndexCount)(UINT *oCount) = 0;
  STDMETHOD(Draw)(ID3DWEffect *effect) = 0;
  STDMETHOD(DrawToBuffer)(ID3DWEffect *effect, ID3DWBuffer *buffer) = 0;
  STDMETHOD(DrawToSurface)(ID3DWEffect *effect, ID3DWSurface *surface) = 0;
  STDMETHOD(DrawToCubeMap)(ID3DWEffect *effect, ID3DWCubeMap *cubeMap) = 0;
  STDMETHOD(MapVertices)(LPVOID *oVertexData) = 0;
  STDMETHOD(UnmapVertices)() = 0;
  STDMETHOD(MapIndices)(UINT32 **oIndices) = 0;
  STDMETHOD(UnmapIndices)() = 0;
};

MIDL_INTERFACE("{9CB5922A-DD2C-430F-AAA2-B4B153298346}")
ID3DWSampler : public ID3DWContextChild
{
public:
  STDMETHOD(GetAnisotropyLevel)(UINT *oLevel) = 0;
  STDMETHOD(SetAnisotropyLevel)(UINT level) = 0;
  STDMETHOD(GetAddressMode)(D3DW_TEXTURE_MODE *oMode) = 0;
  STDMETHOD(SetAddressMode)(D3DW_TEXTURE_MODE mode) = 0;
  STDMETHOD(GetBorderColor)(FLOAT oColor[4]) = 0;
  STDMETHOD(SetBorderColor)(const FLOAT color[4]) = 0;
};

MIDL_INTERFACE("{A21D8127-AAB7-428F-93F0-0D001D640594}")
ID3DWResource : public ID3DWContextChild
{
public:
  STDMETHOD(GetByteSize)(SIZE_T *oSize) = 0;
};

MIDL_INTERFACE("{B51300F1-11A5-400C-ADC9-170AA15DCC26}")
ID3DWConstantBuffer : public ID3DWResource
{
public:
  STDMETHOD(Update)(LPVOID data, SIZE_T size) = 0;
};

MIDL_INTERFACE("{30783E9F-5F05-4544-82F1-866A872D9F63}")
ID3DWBuffer : public ID3DWResource
{
public:
};

MIDL_INTERFACE("{A73D00D0-AD75-455A-BFCC-C8D437D9FADE}")
ID3DWTexture : public ID3DWResource
{
public:
  STDMETHOD(GetSize)(UINT *oWidth, UINT *oHeight) = 0;
  STDMETHOD(GetSurface)(ID3DWSurface **oSurface) = 0;
};

MIDL_INTERFACE("{07E09E1C-7283-43DC-99D8-32C0CC672218}")
ID3DWSurface : public IUnknown
{
public:
  STDMETHOD(GetTexture)(ID3DWTexture **oTex) = 0;
  STDMETHOD(GetSize)(UINT *oWidth, UINT *oHeight) = 0;
  STDMETHOD(BeginDraw)() = 0;
  STDMETHOD(EndDraw)() = 0;
  STDMETHOD(Clear)(const FLOAT clearColor[4]) = 0;
  STDMETHOD(ClearDepthStencil)() = 0;
};

MIDL_INTERFACE("{C3391F42-14C0-4C4C-9C54-A97A0413E972}")
ID3DWCubeMap : public ID3DWResource
{
public:  
  STDMETHOD(GetSideSize)(UINT *oSize) = 0;
  STDMETHOD(Clear)(const FLOAT clearColor[4]) = 0;
  STDMETHOD(ClearDepthStencil)() = 0;
};

MIDL_INTERFACE("{9D1DA4B4-1DDE-479C-BE3C-A652CAA71540}")
ID3DWFloatAnimation : public ID3DWContextChild
{
public:
  STDMETHOD(Start)() = 0;
  STDMETHOD(Stop)() = 0;
  STDMETHOD(GetStatus)(/* [out] */ BOOL *oStarted) = 0;
  STDMETHOD(Query)(/* [out] */ FLOAT *oCurrent) = 0;
  STDMETHOD(GetRange)(/* [out] */ FLOAT *oBegin, /* [out] */ FLOAT *oEnd) = 0;
  STDMETHOD(SetRange)(FLOAT begin, FLOAT end) = 0;
  STDMETHOD(GetInterval)(/* [out] */ FLOAT *oSeconds) = 0;
  STDMETHOD(SetInterval)(FLOAT seconds) = 0;
  STDMETHOD(GetRepeat)(/* [out] */ BOOL *oRepeat) = 0;
  STDMETHOD(SetRepeat)(BOOL repeat) = 0;
  STDMETHOD(GetAutoReverse)(/* [out] */ BOOL *oAutoReverse) = 0;
  STDMETHOD(SetAutoReverse)(BOOL autoReverse) = 0;
};

#endif // __D3DWORKSHOP_H__
