/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2011-2013 Intel Corporation. All Rights Reserved.

*******************************************************************************/
/** @file pxcpowerstate.h
    Defines the PXCPowerState interface,  which exposes controls for
    detecting and changing the power state of the runtimes. 
  */
#pragma once
#include "pxcsession.h"

/**
   This interface manages the SDK implementation power state.  Any SDK I/O
   or algorithm module implementation that are power aware exposes this
   interface.  Programs may use the DynamicCast function to bind to this
   interface from any module instance.
 */
class PXCPowerState: public PXCBase {
public:
    PXC_CUID_OVERWRITE(PXC_UID('P','W','M','G'));

    enum State {
        STATE_ADAPTIVE=0,
        STATE_C0,             /* full feature set/best algorithm */
        STATE_C1,             /* reduced feature set/simplied algorithm */
        STATE_C2,             /* additional reduced feature set/simplied algorithm */
        STATE_C3,             /* idle */
    };

    virtual pxcStatus PXCAPI QueryState(State *state, State *istate)=0;
    virtual pxcStatus PXCAPI SetState(State state)=0;
};
