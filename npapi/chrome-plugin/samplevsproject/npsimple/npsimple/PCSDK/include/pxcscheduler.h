/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2011-2013 Intel Corporation. All Rights Reserved.

*******************************************************************************/
/** @file pxcscheduler.h
    Defines the PXCScheduler interface, which is used for controlling
    asynchronous pipeline execution.
  */
#pragma once
#include "pxcbase.h"

/**
   This interface manages asynchronous pipeline execution.  Programs use the
   CreateScheduler function to create an instance of the PXCSceduler interface.
   The application can create multiple schedulers with different priorities.
   Internally, the SDK coordinates all execution requests according to the
   specified priorities.  If not specified, the SDK session uses its default
   scheduler, which executes at normal priority.
 */
class PXCScheduler:public PXCBase {
public:
    PXC_CUID_OVERWRITE(PXC_UID('S','H','E','D'));

    enum Priority {
        PRIORITY_NORMAL = 0,
        PRIORITY_LOW    = 1,
        PRIORITY_HIGH   = 2
    };

    class SyncPoint:public PXCBase {
    public:

        PXC_CUID_OVERWRITE(PXC_UID('S','H','S','P'));
        PXC_DEFINE_CONST(TIMEOUT_INFINITE,0xffffffff);
        PXC_DEFINE_CONST(SYNCEX_LIMIT,64);

        virtual pxcStatus PXCAPI QueryPtr(void *ptr)=0;
        virtual pxcStatus PXCAPI Synchronize(pxcU32 timeout)=0;
        pxcStatus __inline Synchronize(void) { return Synchronize((pxcU32)TIMEOUT_INFINITE); }

        /* if idx!=0, the function waits for any sync point */
        virtual pxcStatus PXCAPI SynchronizeEx(pxcU32 n1, SyncPoint **sps, pxcU32 n2, pxcHDL *events, pxcU32 *idx, pxcU32 timeout)=0;
        pxcStatus __inline SynchronizeEx(pxcU32 n, SyncPoint **sps, pxcU32 *idx, pxcU32 timeout) { return SynchronizeEx(n,sps,0,0,idx, timeout); }
        pxcStatus __inline SynchronizeEx(pxcU32 n, SyncPoint **sps, pxcU32 *idx) { return SynchronizeEx(n,sps,0,0,idx,(pxcU32)TIMEOUT_INFINITE); }
        pxcStatus __inline SynchronizeEx(pxcU32 n, SyncPoint **sps) { return SynchronizeEx(n,sps,0,0,0,(pxcU32)TIMEOUT_INFINITE); }
    };

    virtual pxcStatus PXCAPI SetPriority(Priority priority)=0;
    virtual pxcStatus PXCAPI QueryPriority(Priority* priority)=0;
};
