/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    Applications interface class to map and navigation use cases
*
*/




// INCLUDE FILES
#include "glxcommandhandleraiwshowonmaphardkey.h"

#include <data_caging_path_literals.hrh>
#include <epos_poslandmarkserialization.h>
#include <EPos_CPosLandmark.h>
#include <StringLoader.h>
#include <glxuiutilities.rsg>
#include <glxgeneraluiutilities.h>
#include <glxcommandhandlers.hrh>           
#include <mnaiwservices.h>
#include <mnmapview.h>
#include <glxtracer.h>
#include <mglxmedialist.h>                  // for accessing the media items



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CGlxCommandHandlerAiwShowMapHardKey()
// -----------------------------------------------------------------------------
CGlxCommandHandlerAiwShowMapHardKey::CGlxCommandHandlerAiwShowMapHardKey(MGlxMediaListProvider* 
        aMediaListProvider, TBool aHasToolbarItem)
        :CGlxMediaListCommandHandler(aMediaListProvider,aHasToolbarItem)
	{	
	TRACER("CGlxCommandHandlerAiwShowMapHardKey::CGlxCommandHandlerAiwShowMapHardKey()");
	// No Implementation
	}

// -----------------------------------------------------------------------------
// CGlxCommandHandlerAiwShowMapHardKey::ConstructL
// -----------------------------------------------------------------------------
void CGlxCommandHandlerAiwShowMapHardKey::ConstructL()
	{
	TRACER("CGlxCommandHandlerAiwShowMapHardKey::ConstructL()");
	iAiwServiceHandler = CAiwServiceHandler::NewL();
    iAiwServiceHandler->AttachL( R_GLX_AIW_SHOWMAP_INTEREST_HARDKEY );
    
	iInList = CAiwGenericParamList::NewL();
    iOutList = CAiwGenericParamList::NewL();  
    
    // Add the upload command
    TCommandInfo info(KGlxCmdMnShowMap);
    AddCommandL(info);  
	}

// -----------------------------------------------------------------------------
// CGlxCommandHandlerAiwShowMapHardKey::NewL
// -----------------------------------------------------------------------------
EXPORT_C CGlxCommandHandlerAiwShowMapHardKey* CGlxCommandHandlerAiwShowMapHardKey::
    NewL(MGlxMediaListProvider* aMediaListProvider, TBool aHasToolbarItem)
	{	
	TRACER("CGlxCommandHandlerAiwShowMapHardKey::NewL()");
	CGlxCommandHandlerAiwShowMapHardKey* self = new (ELeave) CGlxCommandHandlerAiwShowMapHardKey(
                    aMediaListProvider,aHasToolbarItem);
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
// -----------------------------------------------------------------------------
// ~CGlxCommandHandlerAiwShowMapHardKey
// C++ Destructor
// -----------------------------------------------------------------------------
 CGlxCommandHandlerAiwShowMapHardKey::~CGlxCommandHandlerAiwShowMapHardKey()
	{	
	TRACER("CGlxCommandHandlerAiwShowMapHardKey::~CGlxCommandHandlerAiwShowMapHardKey");
	if (iAiwServiceHandler)
        {
        delete iAiwServiceHandler;    
        iAiwServiceHandler = NULL;
        }
    if (iInList)
        {
        delete iInList;
        iInList = NULL;
        }
    if (iOutList)
        {
        delete iOutList;   
        iOutList = NULL;  
        }
	}

// ----------------------------------------------------------------------------
// DoExecuteL
// ----------------------------------------------------------------------------
TBool CGlxCommandHandlerAiwShowMapHardKey::DoExecuteL (TInt /*aCommandId*/, MGlxMediaList& aList)
    {
    TRACER("CGlxCommandHandlerAiwShowMapHardKey::DoExecuteL");
    TBool handled = EFalse;
    TCoordinate coordinate; 
    TInt focusIndex = aList.FocusIndex();
    const TGlxMedia& item = aList.Item(focusIndex);

    if(item.GetCoordinate(coordinate))
        {
        CPosLandmark* lmk = CPosLandmark::NewLC();
        TLocality locality;
        HBufC8* lmBuf;
        
        //set location
        locality.SetCoordinate( coordinate.Latitude(), coordinate.Longitude(), 
                coordinate.Altitude() );
     
        lmk->SetPositionL(locality);
        
        //set  in parameters
        iInList->Reset();
        lmBuf =PosLandmarkSerialization::PackL( *lmk );
        CleanupStack::PushL( lmBuf );
        TAiwGenericParam param( EGenericParamLandmark, TAiwVariant( *lmBuf ) );     
        iInList->AppendL( param );       
        TMnAiwCommonOptionsParam options;
        options.iRunChained = EFalse;
        TPckg<TMnAiwCommonOptionsParam> optionsPack( options );
        TAiwGenericParam optParam( EGenericParamMnCommonOptions, TAiwVariant( optionsPack ) );
        iInList->AppendL( optParam );
                
        iAiwServiceHandler->ExecuteServiceCmdL( KAiwCmdMnShowMap, *iInList, *iOutList, 0); 
        
        CleanupStack::PopAndDestroy( lmBuf);
        CleanupStack::PopAndDestroy( lmk); 
        handled = ETrue;
        }
    else
        {
        // if location info not present then display note to put on camera settings
        HBufC* popupText = StringLoader::LoadLC(
                R_GLX_POPUP_NO_LOCATION_INFO);                  
        GlxGeneralUiUtilities::ShowInfoNoteL( popupText->Des(), EFalse ); 
        CleanupStack::PopAndDestroy( popupText ); 
        }
    return handled;
    }

// ----------------------------------------------------------------------------
// DoActivateL
// ----------------------------------------------------------------------------
void CGlxCommandHandlerAiwShowMapHardKey::DoActivateL(TInt aViewId)
    {
    TRACER("CGlxCommandHandlerAiwShowMapHardKey::DoActivateL");
    iViewId = aViewId;   
    
    }

// ----------------------------------------------------------------------------
// Deactivate
// ----------------------------------------------------------------------------
void CGlxCommandHandlerAiwShowMapHardKey::Deactivate()
    {
    TRACER("CGlxCommandHandlerAiwShowMapHardKey::Deactivate");
    
    }   
 
// End of file

