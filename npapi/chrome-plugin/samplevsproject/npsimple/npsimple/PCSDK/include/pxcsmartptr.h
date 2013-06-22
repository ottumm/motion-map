/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2011-2013 Intel Corporation. All Rights Reserved.

*******************************************************************************/
/** @file pxcsmartptr.h
    Defines useful smart pointer classes for PCSDK applications.
  */
#pragma once
#include "pxcscheduler.h"
#include "pxccapture.h"

/**
   This template class wraps a pointer type with an object that
   automatically releases it's reference upon destruction.  It does this
   by calling a Release() method on the pointer upon destruction of this
   object. Operations on the wrapped type can directly be called on the
   this object and are automatically delegated to the wrapped pointer.
 */
template <class T>
class PXCSmartPtr {
public: 
    PXCSmartPtr(T* t=0) {
        ptr=t; 
    }

    ~PXCSmartPtr(void) { 
        ReleaseRef(); 
    }

    PXCSmartPtr& operator = (T* t) {
        ReleaseRef();
        ptr=t;
        return *this;
    }

    T** operator & (void) { 
        return &ptr; 
    }

    T*  operator -> (void) { 
        return ptr; 
    }

    operator T* (void) { 
        return ptr; 
    }

    T*  ReleasePtr(void) { 
        T *t=ptr; 
        ptr=0; 
        return t; 
    }

    T** ReleaseRef(void) { 
        if (ptr) {
            ptr->Release(); 
            ptr=0; 
        } 
        return &ptr; 
    }

    bool IsValid(void) {
        return (0 != ptr);
    }

protected:

    PXCSmartPtr(PXCSmartPtr &s) { ptr=s.ptr; s.ptr=0; }

    T   *ptr;
};

/**
    The PXCSmartSP class extends the PXCSmartPtr template to help the
    application manage the SP instances. The PXCSmartSP is provided
    for convenience. It is equivalent to PXCSmartPtr<PXCScheduler::SyncPoint>.
 */
class PXCSmartSP:public PXCSmartPtr<PXCScheduler::SyncPoint> {
};

/**
   This template class defines an array of smart pointers.
 */
template <class T> 
class PXCSmartArray {
public:

    PXCSmartArray(int size=PXCCapture::VideoStream::STREAM_LIMIT) {
        this->size=size;
        objects=new T*[size];
        if (objects) for (int i=0;i<size;i++) objects[i]=0;
        objects2=0;
    }

    ~PXCSmartArray(void) {
        ReleaseRefs();
        if (objects) delete [] objects;
        if (objects2) delete [] objects2;
    }

    operator T** (void) {
        return objects;
    }

    T** ReleaseRefs(void) { 
        if (objects) for (int i=0;i<size;i++) if (objects[i]) objects[i]->Release(), objects[i]=0;
        return objects;
    }

    T** ReleasePtrs(void) {
        if (!objects) return 0;
        if (!objects2) {
            objects2=new T*[size];
            if (!objects2) return 0;
        }
        for (int i=0;i<size;i++) objects2[i]=objects[i], objects[i]=0;
        return objects2;
    }

    T*  ReleasePtr(int idx) {
        if (!objects) return 0;
        T* t=objects[idx];
        objects[idx]=0;
        return t;
    }

    T** ReleaseRef(int idx) {
        if (!objects) return 0;
        if (objects[idx]) objects[idx]->Release(), objects[idx]=0;
        return &objects[idx];
    }

    int QuerySize(void) { 
        return size; 
    }

protected:

    T   **objects;
    T   **objects2;
    int size;
};

/**
   The PXCSmartSPArray class extends the PXCSmartArray template to help
   applications manage arrays of SP instances.
 */
class PXCSmartSPArray:public PXCSmartArray<PXCScheduler::SyncPoint> {
public:

    PXCSmartSPArray(int size):PXCSmartArray<PXCScheduler::SyncPoint>::PXCSmartArray(size) {}

    pxcStatus SynchronizeEx(pxcU32 *idx=0, pxcU32 timeout=PXCScheduler::SyncPoint::TIMEOUT_INFINITE) {
        if (!objects) return PXC_STATUS_HANDLE_INVALID;
        for (int i=0;i<(int)size;i++) {
            if (!objects[i]) continue;
            return objects[i]->SynchronizeEx(size,objects,idx,timeout);
        }
        return PXC_STATUS_HANDLE_INVALID;
    }
};
