#ifndef __COM_UTILS_HPP__
#define __COM_UTILS_HPP__

#include <windows.h>

template<class T>
class ComPtr
{
public:
  ComPtr()
  {
    _p = NULL;
  }

  ComPtr(T *p)
  {
    _p = p;
    if(NULL != _p)
      _p->AddRef();
  }

  ComPtr(const ComPtr<T>& copy)
  {
    T* p = copy._p;
    _p = p;
    if(NULL != _p)
      _p->AddRef();
  }

  ~ComPtr()
  {
    Release();
  }

  inline operator T*() const
  {
    return _p;
  }

  inline T* operator->() const
  {
#if defined(DEBUG) || defined(_DEBUG)
    if(NULL == _p)
    {
      OutputDebugString(TEXT("Access to NULL ComPtr\n"));
      DebugBreak();
    }
#endif // DEBUG || _DEBUG
    return _p;
  }

  inline T& operator*() const
  {
#if defined(DEBUG) || defined(_DEBUG)
    if(NULL == _p)
    {
      OutputDebugString(TEXT("Dereferencing NULL ComPtr\n"));
      DebugBreak();
    }
#endif // DEBUG || _DEBUG
    return *_p;
  }

  inline T** operator&()
  {
#if defined(DEBUG) || defined(_DEBUG)
    if(NULL != _p)
    {
      OutputDebugString(TEXT("Implicitly getting address of non-null ComPtr\n"));
      DebugBreak();
    }
#endif // DEBUG || _DEBUG
    return &_p;
  }

  inline bool operator!() const
  {
    return NULL == _p;
  }

  inline bool operator<(T* p) const
  {
    return _p < p;
  }

  inline bool operator==(T* p) const
  {
    return _p == p;
  }

  inline bool operator!=(T* p) const
  {
    return _p != p;
  }

  inline T* operator=(T* p)
  {
    if(_p == p)
      return _p;
    else
    {
      Release();
      if(NULL != p)
        p->AddRef();
      _p = p;
      return _p;
    }
  }

  inline T* operator=(ComPtr<T> p)
  {
    return operator=(p._p);
  }

  inline T** AddressOf()
  {
    return &_p;
  }

  inline void Attach(T* p)
  {
    if(_p)
      _p->Release();
    _p = p;
  }

  inline T* Detach()
  {
    T* p = _p;
    _p = NULL;
    return p;
  }

  inline void Release()
  {
    if(_p)
    {
      _p->Release();
      _p = NULL;
    }    
  }

  bool IsEqualObject(IUnknown* other)
  {
    if(NULL == _p && NULL == other)
      return true;
    if(NULL == _p || NULL == other)
      return false;
    ComPtr<T> unk1;
    ComPtr<T> unk2;
    _p->QueryInterface(__uuidof(IUnknown), (void**)&unk1);
    other->QueryInterface(__uuidof(IUnknown), (void**)&unk2);
    return unk1 == unk2;
  }

  HRESULT CoCreateInstance(REFCLSID clsid, IUnknown* outer = NULL, DWORD clsCtx = CLSCTX_ALL)
  {
#if defined(DEBUG) || defined(_DEBUG)
    if(NULL != _p)
    {
      OutputDebugString(TEXT("Calling CoCreateInstance for non-NULL ComPtr\n"));
      DebugBreak();
    }
#endif // DEBUG || _DEBUG
    return ::CoCreateInstance(clsid, outer, clsCtx, __uuidof(T), (void**)&_p);
  }

  HRESULT CoCreateInstance(LPCOLESTR progId, IUnknown* outer = NULL, DWORD clsCtx = CLSCTX_ALL)
  {
#if defined(DEBUG) || defined(_DEBUG)
    if(NULL != _p)
    {
      OutputDebugString(TEXT("Calling CoCreateInstance for non-NULL ComPtr\n"));
      DebugBreak();
    }
#endif // DEBUG || _DEBUG
    HRESULT hr;
    CLSID clsid;
    hr = ::CLSIDFromProgID(progId, &clsid);
    if(SUCCEEDED(hr))
      hr = ::CoCreateInstance(clsid, outer, clsCtx, __uuidof(T), (void**)&_p);
    return hr;
  }

  template<class Q>
  HRESULT QueryInterface(Q** ppOut)
  {
#if defined(DEBUG) || defined(_DEBUG)
    if(NULL == _p)
    {
      OutputDebugString(TEXT("Access to NULL ComPtr\n"));
      DebugBreak();
    }
#endif // DEBUG || _DEBUG
    return _p->QueryInterface(__uuidof(Q), (void**)ppOut);
  }
private:
  T *_p;
};


template<class Base>
class ComObject : public Base
{
public:
  ComObject() { _ref = 0; }
  virtual ~ComObject() { }
  STDMETHODIMP_(ULONG) AddRef()
  {
    InterlockedIncrement(&_ref);
    return _ref;
  }
  STDMETHODIMP_(ULONG) Release()
  {
    InterlockedDecrement(&_ref);
    if(0 == _ref)
      delete this;
    return _ref;
  }
private:
  ComObject(ComObject<Base>& rhs);
  ULONG _ref;
};

template<class Base>
class ComContainedObject : public Base
{
public:
  ComContainedObject(LPVOID outerUnk)
  {
#if defined(DEBUG) || defined(_DEBUG)
    if(!outerUnk)
    {
      OutputDebugString(TEXT("NULL outer for ComContainedObject"));
      DebugBreak();
    }
#endif // DEBUG || _DEBUG
    _outer = (IUnknown*)outerUnk;
  }
  STDMETHODIMP_(ULONG) AddRef()
  {
    return _outer->AddRef();
  }
  STDMETHODIMP_(ULONG) Release()
  {
    return _outer->Release();
  }
private:
  ComContainedObject(ComContainedObject<Base>& rhs);
  IUnknown *_outer;
};

#define BEGIN_INTERFACE_MAP STDMETHOD(QueryInterface)(REFIID riid, LPVOID *oObject){ \
                              if(NULL == oObject) return E_POINTER; \
                              if(__uuidof(IUnknown) == riid) {*oObject = (LPVOID)this; AddRef(); return S_OK;}
#define INTERFACE_MAP_ENTRY(name) if(__uuidof(name) == riid) {*oObject = (name*)this; AddRef(); return S_OK;}
#define END_INTERFACE_MAP *oObject = NULL; return E_NOINTERFACE; } \
        STDMETHOD_(ULONG, AddRef)() = 0; \
        STDMETHOD_(ULONG, Release)() = 0;

#define V_HR(x) do{ hr = x; if(FAILED(hr)) { return hr; } }while(0)

#if defined(DEBUG) || defined(_DEBUG)
#define V_HR_BREAK(msg) do{ OutputDebugString(TEXT(msg)TEXT("\n")); DebugBreak(); }while(0)
#else
#define V_HR_BREAK(msg)
#endif // DEBUG || _DEBUG

#define V_HR_ASSERT(x, msg) do{ hr = x; if(FAILED(hr)) { V_HR_BREAK(msg); return hr; } }while(0)

#define HRESULT_FROM_LAST_ERROR() (HRESULT_FROM_WIN32(GetLastError()))

#ifndef COM_NO_VTABLE
#define COM_NO_VTABLE __declspec(novtable)
#endif

template<class T>
void SafeDelete(T*& p)
{
  if(p)
  {
    delete p;
    p = NULL;
  }
}

#endif // __COM_UTILS_HPP__