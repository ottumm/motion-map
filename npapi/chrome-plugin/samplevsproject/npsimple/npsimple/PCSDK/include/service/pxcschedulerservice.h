/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2011 Intel Corporation. All Rights Reserved.

*******************************************************************************/
#pragma once
#include "pxcsessionservice.h"
#include "pxcscheduler.h"

/* service for PXCScheduler interface */
class PXCSchedulerService:public PXCBase {
public:
    PXC_CUID_OVERWRITE(PXC_UID('S','C','H','2'));

    /* service for PXCScheduler::PXCSyncPoint interface */
    class SyncPointService:public PXCBase {
    public:
        PXC_CUID_OVERWRITE(PXC_UID('S','Y','N','1'));

        virtual pxcStatus PXCAPI SignalSyncPoint(pxcStatus status)=0;
        virtual void      PXCAPI AddRef(void)=0;
    };

    virtual pxcStatus PXCAPI CreateSyncPoint(pxcU32 noutput, void** outputs, PXCScheduler::SyncPoint **sp)=0;

    /* scoreboarding services */
    class Callback:public PXCBase {
    public:
        PXC_CUID_OVERWRITE(PXC_UID('C','L','L','B'));
        virtual void  PXCAPI   Run(pxcStatus sts)=0;
        virtual const pxcCHAR* PXCAPI QueryCallbackName() { return 0; }
    };
    virtual pxcStatus PXCAPI RequestInputs(pxcU32 ninput, void** inputs, Callback *cb)=0;
    virtual pxcStatus PXCAPI MarkOutputs(pxcU32 noutput, void** outputs, pxcStatus sts)=0;
};
