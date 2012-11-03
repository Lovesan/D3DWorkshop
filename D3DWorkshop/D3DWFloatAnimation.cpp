#include "StdAfx.h"
#include "D3DWFloatAnimation.hpp"
#include "D3DWContext.hpp"

D3DWFloatAnimation::D3DWFloatAnimation()
{
  _started = FALSE;
  _reverse = FALSE;
  _repeat = FALSE;
  _back = FALSE;
  _begin = 0;
  _current = 0;
  _end = 0;
  _range = 0;
  _interval = 0;
  QueryPerformanceFrequency(&_freq);
}

D3DWFloatAnimation::~D3DWFloatAnimation()
{
}

STDMETHODIMP D3DWFloatAnimation::GetContext(ID3DWContext **oCtx)
{
  if(!oCtx)
    return E_POINTER;
  *oCtx = NULL;
  _ctx->AddRef();
  *oCtx = _ctx;
  return S_OK;
}

HRESULT D3DWFloatAnimation::Create(
    FLOAT begin, FLOAT end, FLOAT interval, BOOL repeat, BOOL autoReverse, D3DWContext *ctx, D3DWFloatAnimation **oAnimation)
{
  if(!oAnimation || !ctx)
    return E_POINTER;
  *oAnimation = NULL;
  D3DWFloatAnimation *animation = new (std::nothrow) ComObject<D3DWFloatAnimation>();
  if(!animation)
    return E_OUTOFMEMORY;
  animation->_ctx = ctx;
  animation->SetRange(begin, end);
  animation->SetInterval(interval);
  animation->SetRepeat(repeat);
  animation->SetAutoReverse(autoReverse);
  animation->AddRef();
  *oAnimation = animation;
  return S_OK;
}

STDMETHODIMP D3DWFloatAnimation::Start()
{
  if(!_started)
  {
    QueryPerformanceCounter(&_counter);
    _started = TRUE;
    Query(&_current);
    return S_OK;
  }
  return S_FALSE;
}

STDMETHODIMP D3DWFloatAnimation::Stop()
{
  if(_started)
  {
    Query(&_current);
    _started = FALSE;
    return S_OK;
  }
  return S_FALSE;
}

STDMETHODIMP D3DWFloatAnimation::GetStatus(/* [out] */ BOOL *oStarted)
{
  if(!oStarted)
    return E_POINTER;
  *oStarted = _started;
  return S_OK;
}

STDMETHODIMP D3DWFloatAnimation::Query(/* [out] */ FLOAT *oCurrent)
{
  if(!oCurrent)
    return E_POINTER;
  if(!_started)
  {
    *oCurrent = _current;
    return S_OK;
  }
  LARGE_INTEGER tmpCounter;
  QueryPerformanceCounter(&tmpCounter);
  FLOAT dt = (tmpCounter.QuadPart - _counter.QuadPart)
             / (FLOAT)_freq.QuadPart
             / _interval
             * _range;
  _counter = tmpCounter;
  if(_back)
  {
    _current -= dt;
    if(_current < _begin)
    {
      _current = _begin;
      if(!_repeat)
        _started = FALSE;
      if(_reverse)
        _back = FALSE;
      else
        _current = _end;
    }
  }
  else
  {
    _current += dt;
    if(_current > _end)
    {
      _current = _end;
      if(!_repeat)
        _started = FALSE;
      if(_reverse)
        _back = TRUE;
      else
        _current = _begin;
    }
  }
  *oCurrent = _current;
  return S_OK;
}

STDMETHODIMP D3DWFloatAnimation::GetRange(/* [out] */ FLOAT *oBegin, /* [out] */ FLOAT *oEnd)
{
  if(!oBegin || !oEnd)
    return E_POINTER;
  *oBegin = _begin;
  *oEnd = _end;
  return S_OK;
}

STDMETHODIMP D3DWFloatAnimation::SetRange(FLOAT begin, FLOAT end)
{
  _back = begin > end;  
  _begin = _back ? end : begin;
  _end = _back ? begin : end;
  _current = _back ? end : begin;
  _range = abs(end - begin);
  return S_OK;
}

STDMETHODIMP D3DWFloatAnimation::GetInterval(/* [out] */ FLOAT *oSeconds)
{
  if(!oSeconds)
    return E_POINTER;
  *oSeconds = _interval;
  return S_OK;
}

STDMETHODIMP D3DWFloatAnimation::SetInterval(FLOAT seconds)
{
  _interval = seconds;
  return S_OK;
}

STDMETHODIMP D3DWFloatAnimation::GetRepeat(/* [out] */ BOOL *oRepeat)
{
  if(!oRepeat)
    return E_POINTER;
  *oRepeat = _repeat;
  return S_OK;
}

STDMETHODIMP D3DWFloatAnimation::SetRepeat(BOOL repeat)
{
  _repeat = repeat;
  return S_OK;
}

STDMETHODIMP D3DWFloatAnimation::GetAutoReverse(/* [out] */ BOOL *oAutoReverse)
{
  if(!oAutoReverse)
    return E_POINTER;
  *oAutoReverse = _reverse;
  return S_OK;
}

STDMETHODIMP D3DWFloatAnimation::SetAutoReverse(BOOL autoReverse)
{
  _reverse = autoReverse;
  return S_OK;
}