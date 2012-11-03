#ifndef __D3DWFLOAT_ANIMATION_HPP__
#define __D3DWFLOAT_ANIMATION_HPP__

#include <windows.h>
#include "ComUtils.hpp"
#include "D3DW.h"

#ifndef __D3DWCONTEXT_DEFINED__
class D3DWContext;
#endif // __D3DWCONTEXT_DEFINED__

class COM_NO_VTABLE D3DWFloatAnimation : public ID3DWFloatAnimation
{
public:
  BEGIN_INTERFACE_MAP
    INTERFACE_MAP_ENTRY(ID3DWContextChild)
    INTERFACE_MAP_ENTRY(ID3DWFloatAnimation)
  END_INTERFACE_MAP

  static HRESULT Create(
    FLOAT begin, FLOAT end, FLOAT interval, BOOL repeat, BOOL autoReverse, D3DWContext *ctx, D3DWFloatAnimation **oAnimation);
  virtual ~D3DWFloatAnimation();

  STDMETHODIMP GetContext(ID3DWContext **oCtx);
  STDMETHODIMP Start();
  STDMETHODIMP Stop();
  STDMETHODIMP GetStatus(/* [out] */ BOOL *oStarted);
  STDMETHODIMP Query(/* [out] */ FLOAT *oCurrent);
  STDMETHODIMP GetRange(/* [out] */ FLOAT *oBegin, /* [out] */ FLOAT *oEnd);
  STDMETHODIMP SetRange(FLOAT begin, FLOAT end);
  STDMETHODIMP GetInterval(/* [out] */ FLOAT *oSeconds);
  STDMETHODIMP SetInterval(FLOAT seconds);
  STDMETHODIMP GetRepeat(/* [out] */ BOOL *oRepeat);
  STDMETHODIMP SetRepeat(BOOL repeat);
  STDMETHODIMP GetAutoReverse(/* [out] */ BOOL *oAutoReverse);
  STDMETHODIMP SetAutoReverse(BOOL autoReverse);

protected:
  D3DWFloatAnimation();

private:
  ComPtr<D3DWContext> _ctx;
  BOOL _started;
  BOOL _reverse;
  BOOL _repeat;
  BOOL _back;
  FLOAT _begin;
  FLOAT _end;
  FLOAT _range;
  FLOAT _current;
  FLOAT _interval;
  LARGE_INTEGER _freq;
  LARGE_INTEGER _counter;
};

#define __D3DWFLOAT_ANIMATION_DEFINED__

#endif // __D3DWFLOAT_ANIMATION_HPP__