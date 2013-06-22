/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2013 Intel Corporation. All Rights Reserved.

*******************************************************************************/
/** @file pxcvoice.h
    Defines the PXCVoiceRecognition and PXCVoiceSynthesis interfaces for
    voice recognition and synthesis, respectively.
 */
#pragma once
#include "pxcsession.h"
#include "pxccapture.h"

/**
   This class defines a standard interface for performing voice recognition.
 */
class PXCVoiceRecognition:public PXCBase {
public:
    PXC_CUID_OVERWRITE(0x8013C527);

    struct Recognition {
        struct NBest {
            pxcEnum label; 
            pxcU32  confidence;
        };

        pxcU64  timeStamp; 
        union {
            struct {
                pxcEnum label; 
                pxcU32  confidence;
            };
            NBest nBest[4];
        }; 
        pxcU32  reserved[5];
        pxcU32  duration; 
        pxcCHAR dictation[1024]; // used in dictation mode

        class Handler:public PXCCallbackBase {
        public:
            virtual void PXCAPI OnRecognized(Recognition *data)=0;
        };
    };

    struct Alert {
        typedef pxcEnum Label;
        enum {
            LABEL_VOLUME_HIGH= 0x00001,
            LABEL_VOLUME_LOW = 0x00002,
            LABEL_SNR_LOW    = 0x00004,
            LABEL_SPEECH_UNRECOGNIZABLE = 0x00008,
        };
        pxcU64  timeStamp;
        Label   label;
        pxcU32  reserved[6];

        class Handler: public PXCCallbackBase {
        public:
            virtual void PXCAPI OnAlert(Alert *data)=0;
        }; 
    };

    struct ProfileInfo {
        enum Language {
            LANGUAGE_US_ENGLISH  = PXC_UID('e','n','U','S'),
        };

        PXCCapture::AudioStream::DataDesc inputs;
        pxcCHAR         speaker[128];       // speaker name
        Language        language;           // supported language
        Alert::Label    alerts;             // interested alerts
        pxcU32          endOfSentence;      // end of sentence in ms
        pxcU32          reserved[13];
    };

    virtual pxcStatus PXCAPI QueryProfile(pxcU32 pidx, ProfileInfo *pinfo)=0;
    pxcStatus __inline QueryProfile(ProfileInfo *pinfo) { return QueryProfile(WORKING_PROFILE,pinfo); }
    virtual pxcStatus PXCAPI SetProfile(ProfileInfo *pinfo)=0;

    virtual pxcStatus PXCAPI CreateGrammar(pxcUID *gid)=0; 
    virtual pxcStatus PXCAPI AddGrammar(pxcUID gid, pxcEnum label, pxcCHAR *cmd)=0;
    virtual pxcStatus PXCAPI SetGrammar(pxcUID gid)=0; 
    virtual pxcStatus PXCAPI DeleteGrammar(pxcUID gid)=0; 
    pxcStatus __inline SetDictation() { return SetGrammar(0); }

    virtual pxcStatus PXCAPI ProcessAudioAsync(PXCAudio *audio, PXCScheduler::SyncPoint **sp)=0;
    virtual pxcStatus PXCAPI SubscribeRecognition(pxcU32 threshold, Recognition::Handler *handler)=0;
    virtual pxcStatus PXCAPI SubscribeAlert(Alert::Handler *handler)=0;
};

/**
   This class defines a standard interface for performing voice synthesis.
 */
class PXCVoiceSynthesis: public PXCBase {
public:

    PXC_CUID_OVERWRITE(PXC_UID('V','I','T','S'));

    struct ProfileInfo {
        enum Language {
            LANGUAGE_US_ENGLISH  = PXC_UID('e','n','U','S'),
        };

        enum Voice {
            VOICE_FEMALE=1,
        };

        PXCCapture::AudioStream::DataDesc outputs;  // output format, need bufferSize to limit the latency.
        Language    language;
        Voice       voice;
        pxcF32      rate;    // normal rate is 1.0
        pxcU32      volume;  // range [0, 100]
        pxcU32      reserved[4];
    };

    virtual pxcStatus PXCAPI QueryProfile(pxcU32 pidx, ProfileInfo *pinfo)=0;
    pxcStatus __inline QueryProfile(ProfileInfo *pinfo) { return QueryProfile(WORKING_PROFILE,pinfo); }
    virtual pxcStatus PXCAPI SetProfile(ProfileInfo *pinfo)=0;

    virtual pxcStatus PXCAPI QueueSentence(pxcCHAR *sentence, pxcU32 nchars, pxcUID *id)=0;
	__declspec(deprecated("** Use QueueSentence() **")) pxcStatus __inline QueueSentense(pxcCHAR *sentence, pxcU32 nchars, pxcUID *id) { return QueueSentence(sentence, nchars, id); }

    virtual pxcStatus PXCAPI ProcessAudioAsync(pxcUID id, PXCAudio **audio, PXCScheduler::SyncPoint **sp)=0; // return PXC_STATUS_ITEM_UNAVAILABLE for end of sentense
};
