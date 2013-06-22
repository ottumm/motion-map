/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2011 Intel Corporation. All Rights Reserved.

*******************************************************************************/
#pragma once
#include "pxcsession.h"

class PXCSessionService:public PXCBase {
public:
    PXC_CUID_OVERWRITE(PXC_UID('S','E','S','2'));

    /* algorithms in this group are core services */
    PXC_DEFINE_CONST(IMPL_SUBGROUP_ACCELERATOR,0x80000000);
    PXC_DEFINE_CONST(IMPL_SUBGROUP_SCHEDULER,0x40000000);

    PXC_DEFINE_UID(SUID_DLL_EXPORT_TABLE,'D','L','E',2);
    struct DLLExportTable {
        DLLExportTable  *next;
        pxcStatus       (PXCAPI *createInstance)(PXCSession *session, PXCScheduler *scheduler, PXCAccelerator *accel, DLLExportTable *table, pxcUID cuid, PXCBase **instance);
        pxcUID          suid;
        PXCSession::ImplDesc  desc;
    };

    virtual pxcStatus PXCAPI QueryImplEx(PXCSession::ImplDesc *templat, pxcU32 idx, DLLExportTable **table, void ***instance)=0;

    virtual pxcStatus PXCAPI LoadImpl(DLLExportTable *table)=0;
    virtual pxcStatus PXCAPI UnloadImpl(DLLExportTable *table)=0;

    virtual void   PXCAPI TraceEvent(const pxcCHAR *event_name)=0;
    virtual void   PXCAPI TraceBegin(const pxcCHAR *task_name)=0;
    virtual void   PXCAPI TraceEnd()=0;
    virtual void   PXCAPI TraceParam(const pxcCHAR *param_name, const pxcCHAR *param_value)=0;
};

