/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2012 Intel Corporation. All Rights Reserved.

*******************************************************************************/
#pragma once
#include "service/pxcschedulerservice.h"
#include "pxcsmartptr.h"

template <class T, class Ti1, class To1>
class PXCSmartAsyncImpl {
public:

    typedef pxcStatus (PXCAPI T::*TaskFunc)(Ti1 *i1, To1 *o1);
    typedef pxcStatus (PXCAPI T::*AbortFunc)(pxcStatus sts);

    static pxcStatus SubmitTask(Ti1 *i1, To1 *o1, PXCScheduler::SyncPoint **sp, T *instance, PXCScheduler *scheduler, TaskFunc tfunc, AbortFunc afunc=0, const pxcCHAR* tname=0) {
        PXCSchedulerService *scheduler2=(PXCSchedulerService*)scheduler->DynamicCast(PXCSchedulerService::CUID);

        PXCSmartSP sp2;
        pxcStatus sts=scheduler2->CreateSyncPoint(1, (void**)&o1,&sp2);
        if (sts<PXC_STATUS_NO_ERROR) return sts;

        PXCSmartPtr<CallbackImpl> ci(new CallbackImpl(i1,o1,sp2,instance,scheduler2,tfunc,afunc,tname));
        if (!ci) return PXC_STATUS_ALLOC_FAILED;

        sts=scheduler2->MarkOutputs(1,(void**)&o1,PXC_STATUS_EXEC_INPROGRESS);
        if (sts<PXC_STATUS_NO_ERROR) return sts;

        sts=scheduler2->RequestInputs(1,(void**)&i1,ci);
        if (sts<PXC_STATUS_NO_ERROR) return sts;

        ci.ReleasePtr();
        *sp=sp2.ReleasePtr();
        return sts;
    }

protected:

    class CallbackImpl:public PXCBaseImpl<PXCSchedulerService::Callback> {
    public:
        CallbackImpl(Ti1 *i1, To1 *o1, PXCScheduler::SyncPoint *sp, T *instance, PXCSchedulerService *scheduler, TaskFunc tfunc, AbortFunc afunc, const pxcCHAR* tname) {
            this->instance=instance;
            this->scheduler=scheduler;
            this->sp=(PXCSchedulerService::SyncPointService*)sp->DynamicCast(PXCSchedulerService::SyncPointService::CUID);
            this->sp->AddRef();
            this->tfunc=tfunc;    
            this->afunc=afunc;
            this->i1=i1;
            this->o1=o1;
            this->tname=tname;
        }

        virtual void PXCAPI Run(pxcStatus sts) {
            if (sts<PXC_STATUS_NO_ERROR) {
                sts=(afunc)?(instance->*afunc)(sts):PXC_STATUS_EXEC_ABORTED;
            } else {
                sts=(instance->*tfunc)(i1,o1);
            }
            scheduler->MarkOutputs(1,(void**)&o1,sts);
            sp->SignalSyncPoint(sts);
            sp->Release();
            Release();
        }

        virtual const pxcCHAR* PXCAPI QueryCallbackName() { return tname; }

    protected:

        T*              instance;
        TaskFunc        tfunc;
        AbortFunc        afunc;
        PXCSchedulerService *scheduler;
        PXCSchedulerService::SyncPointService *sp;
        Ti1*            i1;
        To1*            o1;
        const pxcCHAR*  tname;
    };
};

/* These are simplified asynchronous task handlers, for cases where I1 simutenously generates O1 & O2.
   If I1->O1 and I1->O2 are not synchrnous, use the original scheduler functions for efficient signalling.
*/
template <class T, class Ti1>
class PXCSmartAsyncImplI1 {
public:

    typedef pxcStatus (PXCAPI T::*TaskFunc)(Ti1 *i1);
    typedef pxcStatus (PXCAPI T::*AbortFunc)(pxcStatus sts);

    static pxcStatus SubmitTask(Ti1 *i1, PXCScheduler::SyncPoint **sp, T *instance, PXCScheduler *scheduler, TaskFunc tfunc, AbortFunc afunc=0, const pxcCHAR *tname=0) {
        PXCSchedulerService *scheduler2=(PXCSchedulerService*)scheduler->DynamicCast(PXCSchedulerService::CUID);

        PXCSmartSP sp2;
        pxcStatus sts=scheduler2->CreateSyncPoint(0,0,&sp2);
        if (sts<PXC_STATUS_NO_ERROR) return sts;

        PXCSmartPtr<CallbackImpl> ci(new CallbackImpl(i1,sp2,instance,scheduler2,tfunc,afunc,tname));
        if (!ci) return PXC_STATUS_ALLOC_FAILED;

        sts=scheduler2->RequestInputs(1,(void**)&i1,ci);
        if (sts<PXC_STATUS_NO_ERROR) return sts;

        ci.ReleasePtr();
        *sp=sp2.ReleasePtr();
        return sts;
    }

protected:

    class CallbackImpl:public PXCBaseImpl<PXCSchedulerService::Callback> {
    public:
        CallbackImpl(Ti1 *i1, PXCScheduler::SyncPoint *sp, T *instance, PXCSchedulerService *scheduler, TaskFunc tfunc, AbortFunc afunc, const pxcCHAR *tname) {
            this->instance=instance;
            this->scheduler=scheduler;
            this->sp=(PXCSchedulerService::SyncPointService*)sp->DynamicCast(PXCSchedulerService::SyncPointService::CUID);
            this->sp->AddRef();
            this->tfunc=tfunc;
            this->afunc=afunc;
            this->i1=i1;
            this->tname = tname;
        }

        virtual void PXCAPI Run(pxcStatus sts) {
            if (sts<PXC_STATUS_NO_ERROR) {
                sts=(afunc)?(instance->*afunc)(sts):PXC_STATUS_EXEC_ABORTED;
            } else {
                sts=(instance->*tfunc)(i1);
            }
            sp->SignalSyncPoint(sts);
            sp->Release();
            Release();
        }

        virtual const pxcCHAR* PXCAPI QueryCallbackName(void) { return tname; }

    protected:

        T*              instance;
        TaskFunc        tfunc;
        AbortFunc       afunc;
        PXCSchedulerService *scheduler;
        PXCSchedulerService::SyncPointService *sp;
        Ti1*            i1;
        const pxcCHAR*  tname;
    };
};

template <class T, class Ti1, class To1, class To2>
class PXCSmartAsyncImplI1O2 {
public:

    typedef pxcStatus (PXCAPI T::*TaskFunc)(Ti1 *i1, To1 *o1, To2 *o2);
    typedef pxcStatus (PXCAPI T::*AbortFunc)(pxcStatus sts);

    static pxcStatus SubmitTask(Ti1 *i1, To1 *o1, To2 *o2, PXCScheduler::SyncPoint **sp, T *instance, PXCScheduler *scheduler, TaskFunc tfunc, AbortFunc afunc=0, const pxcCHAR* tname=0) {
        PXCSchedulerService *scheduler2=(PXCSchedulerService*)scheduler->DynamicCast(PXCSchedulerService::CUID);

        PXCSmartSP sp2;
        void *outputs[2]={o1,o2};
        pxcStatus sts=scheduler2->CreateSyncPoint(2,outputs,&sp2);
        if (sts<PXC_STATUS_NO_ERROR) return sts;

        PXCSmartPtr<CallbackImpl> ci(new CallbackImpl(i1,o1,o2,sp2,instance,scheduler2,tfunc,afunc,tname));
        if (!ci) return PXC_STATUS_ALLOC_FAILED;

        sts=scheduler2->MarkOutputs(2,outputs,PXC_STATUS_EXEC_INPROGRESS);
        if (sts<PXC_STATUS_NO_ERROR) return sts;

        sts=scheduler2->RequestInputs(1,(void**)&i1,ci);
        if (sts<PXC_STATUS_NO_ERROR) return sts;

        ci.ReleasePtr();
        *sp=sp2.ReleasePtr();
        return sts;
    }

protected:

    class CallbackImpl:public PXCBaseImpl<PXCSchedulerService::Callback> {
    public:
        CallbackImpl(Ti1 *i1, To1 *o1, To2 *o2, PXCScheduler::SyncPoint *sp, T *instance, PXCSchedulerService *scheduler, TaskFunc tfunc, AbortFunc afunc, const pxcCHAR* tname) {
            this->instance=instance;
            this->scheduler=scheduler;
            this->sp=(PXCSchedulerService::SyncPointService*)sp->DynamicCast(PXCSchedulerService::SyncPointService::CUID);
            this->sp->AddRef();
            this->tfunc=tfunc;
            this->afunc=afunc;
            this->i1=i1;
            this->o1=o1;
            this->o2=o2;
            this->tname=tname;
        }

        virtual void PXCAPI Run(pxcStatus sts) {
            if (sts<PXC_STATUS_NO_ERROR) {
                sts=(afunc)?(instance->*afunc)(sts):PXC_STATUS_EXEC_ABORTED;
            } else {
                sts=(instance->*tfunc)(i1,o1,o2);
            }
            void *outputs[2]={o1,o2};
            scheduler->MarkOutputs(2,outputs,sts);
            sp->SignalSyncPoint(sts);
            sp->Release();
            Release();
        }

        virtual const pxcCHAR* PXCAPI QueryCallbackName() { return tname; }

    protected:

        T*              instance;
        TaskFunc        tfunc;
        AbortFunc       afunc;
        PXCSchedulerService *scheduler;
        PXCSchedulerService::SyncPointService *sp;
        Ti1*            i1;
        To1*            o1;
        To2*            o2;
        const pxcCHAR*  tname;
    };
};


template <class T, class Ti1, class Ti2, class To1>
class PXCSmartAsyncImplI2O1 {
public:

    typedef pxcStatus (PXCAPI T::*TaskFunc)(Ti1 *i1, Ti2 *i2, To1 *o1);
    typedef pxcStatus (PXCAPI T::*AbortFunc)(pxcStatus sts);

    static pxcStatus SubmitTask(Ti1 *i1, Ti2 *i2, To1 *o1, PXCScheduler::SyncPoint **sp, T *instance, PXCScheduler *scheduler, TaskFunc tfunc, AbortFunc afunc=0, const pxcCHAR* tname=0) {
        PXCSchedulerService *scheduler2=(PXCSchedulerService*)scheduler->DynamicCast(PXCSchedulerService::CUID);

        PXCSmartSP sp2;
        pxcStatus sts=scheduler2->CreateSyncPoint(1,(void**)&o1,&sp2);
        if (sts<PXC_STATUS_NO_ERROR) return sts;

        PXCSmartPtr<CallbackImpl> ci(new CallbackImpl(i1,i2,o1,sp2,instance,scheduler2,tfunc,afunc,tname));
        if (!ci) return PXC_STATUS_ALLOC_FAILED;

        sts=scheduler2->MarkOutputs(1,(void**)&o1,PXC_STATUS_EXEC_INPROGRESS);
        if (sts<PXC_STATUS_NO_ERROR) return sts;

        void *inputs[2]={ i1, i2 };
        sts=scheduler2->RequestInputs(2,inputs,ci);
        if (sts<PXC_STATUS_NO_ERROR) return sts;

        ci.ReleasePtr();
        *sp=sp2.ReleasePtr();
        return sts;
    }

protected:

    class CallbackImpl:public PXCBaseImpl<PXCSchedulerService::Callback> {
    public:
        CallbackImpl(Ti1 *i1, Ti2 *i2, To1 *o1, PXCScheduler::SyncPoint *sp, T *instance, PXCSchedulerService *scheduler, TaskFunc tfunc, AbortFunc afunc, const pxcCHAR* tname) {
            this->instance=instance;
            this->scheduler=scheduler;
            this->sp=(PXCSchedulerService::SyncPointService*)sp->DynamicCast(PXCSchedulerService::SyncPointService::CUID);
            this->sp->AddRef();
            this->tfunc=tfunc;
            this->afunc=afunc;
            this->i1=i1;
            this->i2=i2;
            this->o1=o1;
            this->tname=tname;
        }

        virtual void PXCAPI Run(pxcStatus sts) {
            if (sts<PXC_STATUS_NO_ERROR) {
                sts=(afunc)?(instance->*afunc)(sts):PXC_STATUS_EXEC_ABORTED;
            } else {
                sts=(instance->*tfunc)(i1,i2,o1);
            }
            scheduler->MarkOutputs(1,(void**)&o1,sts);
            sp->SignalSyncPoint(sts);
            sp->Release();
            Release();
        }

        virtual const pxcCHAR* PXCAPI QueryCallbackName(void) { return tname; }

    protected:

        T*              instance;
        TaskFunc        tfunc;
        AbortFunc       afunc;
        PXCSchedulerService *scheduler;
        PXCSchedulerService::SyncPointService *sp;
        Ti1*            i1;
        Ti2*            i2;
        To1*            o1;
        const pxcCHAR*  tname;
    };
};

template <class T, class Ti, int TiX, class To, int ToY>
class PXCSmartAsyncImplIxOy {
public:

    typedef pxcStatus (PXCAPI T::*TaskFunc)(Ti *inputs[], To *outputs[]);
    typedef pxcStatus (PXCAPI T::*AbortFunc)(pxcStatus sts);

    static pxcStatus SubmitTask(Ti *inputs[], pxcU32 ninputs, To *outputs[], pxcU32 noutputs, PXCScheduler::SyncPoint **sp, T *instance, PXCScheduler *scheduler, TaskFunc tfunc, AbortFunc afunc=0, const pxcCHAR* tname=0) {
        PXCSchedulerService *scheduler2=(PXCSchedulerService*)scheduler->DynamicCast(PXCSchedulerService::CUID);

        PXCSmartSP sp2;
        pxcStatus sts=scheduler2->CreateSyncPoint(noutputs,(void**)outputs,&sp2);
        if (sts<PXC_STATUS_NO_ERROR) return sts;

        PXCSmartPtr<CallbackImpl> ci(new CallbackImpl(inputs,ninputs,outputs,noutputs,sp2,instance,scheduler2,tfunc,afunc,tname));
        if (!ci) return PXC_STATUS_ALLOC_FAILED;

        sts=scheduler2->MarkOutputs(noutputs,(void**)outputs,PXC_STATUS_EXEC_INPROGRESS);
        if (sts<PXC_STATUS_NO_ERROR) return sts;

        sts=scheduler2->RequestInputs(ninputs,(void**)inputs,ci);
        if (sts<PXC_STATUS_NO_ERROR) return sts;

        ci.ReleasePtr();
        *sp=sp2.ReleasePtr();
        return sts;
    }

protected:

    class CallbackImpl:public PXCBaseImpl<PXCSchedulerService::Callback> {
    public:
        CallbackImpl(Ti *inputs[], pxcU32 ninputs, To *outputs[], pxcU32 noutputs, PXCScheduler::SyncPoint *sp, T *instance, PXCSchedulerService *scheduler, TaskFunc tfunc, AbortFunc afunc, const pxcCHAR* tname) {
            this->instance=instance;
            this->scheduler=scheduler;
            this->sp=(PXCSchedulerService::SyncPointService*)sp->DynamicCast(PXCSchedulerService::SyncPointService::CUID);
            this->sp->AddRef();
            this->tfunc=tfunc;
            this->afunc=afunc;
            this->noutputs=noutputs;
            this->tname=tname;
            for (int i=0;i<(int)ninputs;i++) this->inputs[i]=inputs[i];
            for (int j=0;j<(int)noutputs;j++) this->outputs[j]=outputs[j];
        }

        virtual void PXCAPI Run(pxcStatus sts) {
            if (sts<PXC_STATUS_NO_ERROR) {
                sts=(afunc)?(instance->*afunc)(sts):PXC_STATUS_EXEC_ABORTED;
            } else {
                sts=(instance->*tfunc)(inputs,outputs);
            }
            scheduler->MarkOutputs(noutputs,(void**)outputs,sts);
            sp->SignalSyncPoint(sts);
            sp->Release();
            Release();
        }

        virtual const pxcCHAR* PXCAPI QueryCallbackName() { return tname; }

    protected:

        T*              instance;
        TaskFunc        tfunc;
        AbortFunc       afunc;
        PXCSchedulerService *scheduler;
        PXCSchedulerService::SyncPointService *sp;
        Ti*             inputs[TiX];
        To*             outputs[ToY];
        pxcU32          noutputs;
        const pxcCHAR*  tname;
    };
};

