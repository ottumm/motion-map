/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2012-2013 Intel Corporation. All Rights Reserved.

*******************************************************************************/
/** @file pxcprojection.h
    Defines the PXCProjection interface, which defines mappings between
    pixel, depth, and real world coordinates.
  */
#pragma once
#include "pxccapture.h"

/**
   This interface defines mappings between various coordinate systems
   used by modules of the PCSDK.

   The implementation is usually provided by an I/O module as a serializable
   implementation attached to the session metadata.  For example, the
   following statements create a PXCProjection instance from PXCCapture::Device:
    \code{.cpp}
     device->QueryPropertyAsUID(PXCCapture::Device::PROPERTY_PROJECTION_SERIALIZABLE,&id);
     session->DynamicCast<PXCMetadata>()->CreateSerializable<PXCProjection>(id,&projection);
    \endcode
 */
class PXCProjection: public PXCBase {
public:
    PXC_CUID_OVERWRITE(0x494A8537);

    virtual pxcStatus PXCAPI ProjectImageToRealWorld   (pxcU32 npoints, PXCPoint3DF32 *pos2d, PXCPoint3DF32 *pos3d)=0;
    virtual pxcStatus PXCAPI ProjectRealWorldToImage   (pxcU32 npoints, PXCPoint3DF32 *pos3d, PXCPointF32   *pos2d)=0;
    virtual pxcStatus PXCAPI MapDepthToColorCoordinates(pxcU32 npoints, PXCPoint3DF32 *pos2d, PXCPointF32   *posc)=0;
    virtual pxcStatus PXCAPI MapColorCoordinatesToDepth(pxcU32 npoints, PXCPointF32   *posc,  PXCPointF32   *posd)=0;
};

