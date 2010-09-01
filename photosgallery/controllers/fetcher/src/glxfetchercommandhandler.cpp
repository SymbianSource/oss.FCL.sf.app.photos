/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    CCommandhandler that requests the marking commandhandler to 
*                mark items onto the medialist during multiple selection
*
*/




#include <glxfetcherdialog.rsg>           // FOR GETTING VIEW ID AND RESOURCE ID        
#include <eikappui.h>                     // To get AppUi instance
#include <eikenv.h>                       // To get AppUi instance  
#include <glxtracer.h>                    // For Tracer Logs
#include "glxfetchercommandhandler.h"


//-----------------------------------------------------------------------------
// C++ default constructor.
//-----------------------------------------------------------------------------
CGlxFetcherCommandHandler::CGlxFetcherCommandHandler( 
        MGlxMediaListProvider* aMediaListProvider,
        MGlxTitleObserver* aTitleObserver )
    : CGlxMediaListCommandHandler( aMediaListProvider ),
    iTitleObserver( aTitleObserver )
    {
    TRACER("CGlxFetcherCommandHandler::CGlxFetcherCommandHandler()");
    
    iMediaListProvider = aMediaListProvider;
    }

//-----------------------------------------------------------------------------
// Two-phased constructor.
//-----------------------------------------------------------------------------
CGlxFetcherCommandHandler* CGlxFetcherCommandHandler::NewL(
        MGlxMediaListProvider* aMediaListProvider,
        MGlxTitleObserver* aTitleObserver ) 
    {
    TRACER("CGlxFetcherCommandHandler::NewL()");
    
    CGlxFetcherCommandHandler* self = new( ELeave ) 
        CGlxFetcherCommandHandler( aMediaListProvider, aTitleObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// ConstructL()
// ---------------------------------------------------------------------------
void CGlxFetcherCommandHandler::ConstructL()
    {
    TRACER("CGlxFetcherCommandHandler::ConstructL()");
    
    iCommandhandlerMarking = CGlxCommandHandlerMarking::NewL(iMediaListProvider,EFalse);
    
    // Get AppUi instance
    // This class does not have access to a CEikonEnv and hence 
    // pls ignore the code scanner warning - Using CEikonEnv::Static
    CEikAppUi* appUi = CEikonEnv::Static()->EikAppUi();

    // Make sure the active view is our view, in case the view 
    // activation failed
    TVwsViewId activeViewId;
    appUi->GetActiveViewId( activeViewId );

    // Specify to the commandhandler about the active viewid
    iCommandhandlerMarking->ActivateL( activeViewId.iViewUid.iUid );
     
    }

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
CGlxFetcherCommandHandler::~CGlxFetcherCommandHandler()
    {   
    TRACER("CGlxFetcherCommandHandler::~CGlxFetcherCommandHandler()");
    
    if (iCommandhandlerMarking)
        {
        // Deactivate the commandhandler before deleting it.
        iCommandhandlerMarking->Deactivate();
        delete iCommandhandlerMarking;  
        }
    }

//-----------------------------------------------------------------------------
// DoExecuteL
//-----------------------------------------------------------------------------
TBool CGlxFetcherCommandHandler::DoExecuteL(TInt aCommandId, MGlxMediaList& /*aList*/)
   {
   TRACER("CGlxFetcherCommandHandler::DoExecuteL()");
   
   return iCommandhandlerMarking->ExecuteL( aCommandId );
   }

// ---------------------------------------------------------------------------
// DoIsDisabled
// ---------------------------------------------------------------------------
TBool CGlxFetcherCommandHandler::DoIsDisabled(TInt /*aCommandId*/, 
                                                MGlxMediaList& /*aList*/) const
    {    
    TRACER("CGlxFetcherCommandHandler::DoIsDisabled()");
    
    return EFalse;
    }  
    
// END OF FILE

