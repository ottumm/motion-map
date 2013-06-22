/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2011-2013 Intel Corporation. All Rights Reserved.

*******************************************************************************/
/** @file pxcmetadata.h
    Defines the PXCMetadata interface, which is used for managing
    metadata storage.
 */
#pragma once
#include "pxcbase.h"

/**
    This interface manages metadata storage.  The PXCSession, PXCImage
    and PXCAudio implementations expose the PXCMetadata interface.
 */
class PXCMetadata:public PXCBase {
public:
    PXC_CUID_OVERWRITE(0x62398423);

    virtual pxcUID    PXCAPI QueryUID(void)=0;
    virtual pxcStatus PXCAPI QueryMetadata(pxcU32 idx, pxcUID *id)=0;
    virtual pxcStatus PXCAPI DetachMetadata(pxcUID id)=0;

    virtual pxcStatus PXCAPI AttachBuffer(pxcUID id, pxcBYTE *buffer, pxcU32 size)=0;
    virtual pxcStatus PXCAPI QueryBuffer(pxcUID id, pxcBYTE *buffer, pxcU32 *size)=0;

    virtual pxcStatus PXCAPI AttachSerializable(pxcUID id, PXCBase *instance)=0;
    virtual pxcStatus PXCAPI CreateSerializable(pxcUID id, pxcUID cuid, void **instance)=0;
    template <class T> pxcStatus __inline CreateSerializable(pxcUID id, T **instance) { return CreateSerializable(id, T::CUID, (void**)instance); }
};
