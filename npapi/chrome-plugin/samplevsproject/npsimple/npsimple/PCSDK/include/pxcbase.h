/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2011-2013 Intel Corporation. All Rights Reserved.

*******************************************************************************/
/** @file pxcbase.h
    Defines PXCBase, the base interface for all PCSDK interfaces
 */
#pragma once
#include "pxcdefs.h"
#include "pxcstatus.h"

#define PXCAPI __stdcall

///////////////////////////////////////////////////////////////////////////////////////
/**
   This interface forms the base of all SDK interface definitions.
   The interface overrides the class delete operator to work with the SDK
   dispatching mechanism; and exposes a custom (RTTI-free) dynaimc casting
   mechanism with the DynamicCast method.  The application that implements
   any PXCBase derived interface must derive from one of the PXCBaseImpl
   class templates.
 */
class PXCBase {
public:
    enum { CUID=0 };
    enum { WORKING_PROFILE=0xffffffff };

    virtual void* PXCAPI DynamicCast(pxcUID cuid) = 0;
    template <class T> T* DynamicCast(void) { return (T*)this->DynamicCast(T::CUID); }

    virtual void  PXCAPI Release(void) = 0;

    __declspec(deprecated("** Use Release() instead for portability **")) void operator delete(void* pthis) {
        ((PXCBase*)pthis)->Release();
    }

    PXCBase(void) {}

private:
    /* prohibit using copy & assignment constructors */
    PXCBase(PXCBase const &);
    PXCBase& operator = (PXCBase const &);
};

class PXCCallbackBase {
public:
    enum { CUID=0 };
    virtual void* PXCAPI DynamicCast(pxcUID cuid) { return 0; }
    virtual void  PXCAPI Release(void) { delete this; }
};

#define PXC_CUID_OVERWRITE(X) enum { CUID=X }

///////////////////////////////////////////////////////////////////////////////////////
/**
   This template class contains a default implementation of the PXCBase
   interface.  The application that implements any PXCBase derived interface,
   such as a callback handler, should derive the implementation from this
   template class.  See the SDK Interface section (in the sdkcore manual)
   for details.  The PXCBaseImpl template assumes single inheritance.
   The variations, PXCBaseImpl2 and PXCBaseImpl3, implement two or three
   inheritances, respectively.
*/
template <class T>
class PXCBaseImpl:public T {
public:

    virtual ~PXCBaseImpl(void) {}
    enum { CUID=T::CUID+1 };
    virtual void* PXCAPI DynamicCast(pxcUID cuid) {
        if (cuid==CUID) return this;
        if (cuid==T::CUID) return (T*)this;
        return 0; 
    }
    virtual void  PXCAPI Release(void) { ::delete this; }
    void operator delete(void* pthis) { ::delete(pthis); }
};

/** See PXCBaseImpl */
template <class T1, class T2>
class PXCBaseImpl2:public T1, public T2 {
public:

    virtual ~PXCBaseImpl2(void) {}
    enum { CUID = T1::CUID ^ T2::CUID };
    virtual void* PXCAPI DynamicCast(pxcUID cuid) {
        if (cuid==CUID) return this;
        if (cuid==T1::CUID) return (T1*)this;
        if (cuid==T2::CUID) return (T2*)this;
        return 0;
    }
    virtual void  PXCAPI Release(void) { ::delete this; }
    void operator delete(void* pthis) { ::delete(pthis); }
};

/** See PXCBaseImpl */
template <class T1, class T2, class T3>
class PXCBaseImpl3:public T1, public T2, public T3 {
public:

    virtual ~PXCBaseImpl3(void) {}
    enum { CUID = T1::CUID ^ T2::CUID ^ T3::CUID };
    virtual void* PXCAPI DynamicCast(pxcUID cuid) {
        if (cuid==CUID) return this;
        if (cuid==T1::CUID) return (T1*)this;
        if (cuid==T2::CUID) return (T2*)this;
        if (cuid==T3::CUID) return (T3*)this;
        return 0;
    }
    virtual void  PXCAPI Release(void) { ::delete this; }
    void operator delete(void* pthis) { ::delete(pthis); }
};

/** See PXCBaseImpl */
template <class T1, class T2, class T3, class T4>
class PXCBaseImpl4:public T1, public T2, public T3, public T4 {
public:

    virtual ~PXCBaseImpl4(void) {}
    enum { CUID = T1::CUID ^ T2::CUID ^ T3::CUID ^ T4::CUID };
    virtual void* PXCAPI DynamicCast(pxcUID cuid) {
        if (cuid==CUID) return this;
        if (cuid==T1::CUID) return (T1*)this;
        if (cuid==T2::CUID) return (T2*)this;
        if (cuid==T3::CUID) return (T3*)this;
        if (cuid==T4::CUID) return (T4*)this;
        return 0;
    }
    virtual void  PXCAPI Release(void) { ::delete this; }
    void operator delete(void* pthis) { ::delete(pthis); }
};

/** See PXCBaseImpl */
template <class T1, class T2, class T3, class T4, class T5>
class PXCBaseImpl5:public T1, public T2, public T3, public T4, public T5 {
public:

    virtual ~PXCBaseImpl5(void) {}
    enum { CUID = T1::CUID ^ T2::CUID ^ T3::CUID ^ T4::CUID ^ T5::CUID };
    virtual void* PXCAPI DynamicCast(pxcUID cuid) {
        if (cuid==CUID) return this;
        if (cuid==T1::CUID) return (T1*)this;
        if (cuid==T2::CUID) return (T2*)this;
        if (cuid==T3::CUID) return (T3*)this;
        if (cuid==T4::CUID) return (T4*)this;
        if (cuid==T5::CUID) return (T5*)this;
        return 0;
    }
    virtual void  PXCAPI Release(void) { ::delete this; }
    void operator delete(void* pthis) { ::delete(pthis); }
};
