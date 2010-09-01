/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  AIW Service Handler stub implementation.
*
*/


#include <aiwservicehandler.h>

#include "t_caiwservicehandlerstub.h"

CAiwServiceHandler* CAiwServiceHandler::NewL()
    {
    CAiwServiceHandler* self = new (ELeave) CAiwServiceHandler();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }


CAiwServiceHandler::CAiwServiceHandler()
    {
    
    }
    
CAiwServiceHandler::~CAiwServiceHandler()
    {
    delete iImpl;
    }
    
void CAiwServiceHandler::ConstructL()
    {
    iImpl = CAiwServiceHandlerImpl::NewL();
    }
    
CAiwGenericParamList& CAiwServiceHandler::InParamListL()
    {
    return iImpl->InParamListL();
    }

CAiwGenericParamList& CAiwServiceHandler::OutParamListL()
    {
    return iImpl->OutParamListL();
    }

void CAiwServiceHandler::InitializeMenuPaneL(
            CEikMenuPane& /* aMenuPane */,
            TInt /* aMenuResourceId */,
            TInt /* aBaseMenuCmdId */,
            const CAiwGenericParamList& /* aInParamList */)
    {
    iImpl->ResetInParams();
    }
    
void CAiwServiceHandler::AttachMenuL(TInt /* aMenuResourceId */, TInt /* aInterestResourceId */)
    {
    
    }

TInt CAiwServiceHandler::ServiceCmdByMenuCmd(TInt /* aMenuCmdId */) const
    {
    return 0;
    }
    
void CAiwServiceHandler::ExecuteMenuCmdL(
            TInt /* aMenuCmdId */,
            const CAiwGenericParamList& /* aInParamList */,
            CAiwGenericParamList& /* aOutParamList */,
            TUint /* aCmdOptions */,
            MAiwNotifyCallback* /* aCallback */)
    {
    
    }
    
TBool CAiwServiceHandler::HandleSubmenuL(CEikMenuPane& /* aPane */)
    {
    return ETrue;
    }
    
TBool CAiwServiceHandler::IsAiwMenu(TInt /* aMenuResourceId */)
    {
    return ETrue;
    }

///////////////////////////////////////////////////////////////////////////////

CAiwServiceHandlerImpl* CAiwServiceHandlerImpl::NewL()
    {
    CAiwServiceHandlerImpl* self = new (ELeave) CAiwServiceHandlerImpl();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }


CAiwServiceHandlerImpl::CAiwServiceHandlerImpl()
    {
    
    }
    
CAiwServiceHandlerImpl::~CAiwServiceHandlerImpl()
    {
    delete iInParams;
    delete iOutParams;
    }
    
void CAiwServiceHandlerImpl::ConstructL()
    {
    }

CAiwGenericParamList& CAiwServiceHandlerImpl::InParamListL()
    {
    if (!iInParams)
        {
        iInParams = CAiwGenericParamList::NewL();
        }
    return *iInParams;
    }

CAiwGenericParamList& CAiwServiceHandlerImpl::OutParamListL()
    {
    if (!iOutParams)
        {
        iOutParams = CAiwGenericParamList::NewL();
        }
    return *iOutParams;
    }

void CAiwServiceHandlerImpl::ResetInParams()
    {
    delete iInParams;
    iInParams = 0;
    }
