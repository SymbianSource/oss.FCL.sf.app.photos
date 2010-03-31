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
* Description:    AIW print preview command handler
*
*/




#include "glxcommandhandleraiwprintpreview.h"
#include "glxaiwservicehandler.h"

#include <glxattributecontext.h>
#include <glxcommandhandlers.hrh>
#include <glxmedia.h>
#include <glxuiutilities.rsg>
#include <mglxmedialist.h>
#include <StringLoader.h>
#include <glxnavigationalstate.h>
#include <mpxcollectionpath.h>
#include <glxcollectionpluginimageviewer.hrh>

const TInt KGlxAiwPrintPreviewCommandSpace = 0x00000300;

// -----------------------------------------------------------------------------
// NewL
// -----------------------------------------------------------------------------
//	
EXPORT_C CGlxCommandHandlerAiwPrintPreview* 
        CGlxCommandHandlerAiwPrintPreview::NewL(
        MGlxMediaListProvider* aMediaListProvider, TInt aMenuResource)
    {
    CGlxCommandHandlerAiwPrintPreview* self = new ( ELeave ) 
        CGlxCommandHandlerAiwPrintPreview(aMediaListProvider, aMenuResource);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// -----------------------------------------------------------------------------
// CGlxCommandHandlerAiwPrintPreview::DoGetRequiredAttributesL
// -----------------------------------------------------------------------------
//	
void CGlxCommandHandlerAiwPrintPreview::DoGetRequiredAttributesL
									(RArray<TMPXAttribute>& aAttributes, TBool aFilterUsingSelection) const
	{
	if (!aFilterUsingSelection || SelectionLength() <= MaxSelectedItems())
		{
	    // Add MIME type and fileaname attributes
		aAttributes.AppendL(KMPXMediaGeneralMimeType);
		aAttributes.AppendL(KMPXMediaGeneralUri);
		}
	}

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//	
CGlxCommandHandlerAiwPrintPreview::CGlxCommandHandlerAiwPrintPreview(
        MGlxMediaListProvider* aMediaListProvider, TInt aMenuResource)
:   CGlxCommandHandlerAiwBase(aMediaListProvider, aMenuResource)
    {
    }
    
// -----------------------------------------------------------------------------
// CommandId
// -----------------------------------------------------------------------------
//	
TInt CGlxCommandHandlerAiwPrintPreview::CommandId() const
    {
    return EGlxCmdAiwPrint;
    }
    
// -----------------------------------------------------------------------------
// AiwCommandId
// -----------------------------------------------------------------------------
//	
TInt CGlxCommandHandlerAiwPrintPreview::AiwCommandId() const
    {
    return KAiwCmdPrint;
    }
    
// -----------------------------------------------------------------------------
// AiwInterestResource
// -----------------------------------------------------------------------------
//	
TInt CGlxCommandHandlerAiwPrintPreview::AiwInterestResource() const
    {
    return R_GLX_AIW_PRINT_INTEREST;
    }
        
// -----------------------------------------------------------------------------
// AppendAiwParameterL
// -----------------------------------------------------------------------------
//	
TBool CGlxCommandHandlerAiwPrintPreview::AppendAiwParameterL(const TGlxMedia& aItem, 
                                     CGlxAiwServiceHandler& aAiwServiceHandler)
    {
    return AppendDefaultAiwParameterL(  aItem, 
                                        aAiwServiceHandler, 
                                        ETrue,                  // Add the Uri
                                        ETrue);                 // Add the MineType
    }
    

// -----------------------------------------------------------------------------
// CommandSpace
// -----------------------------------------------------------------------------
//	
TInt CGlxCommandHandlerAiwPrintPreview::CommandSpace() const
    {
    return KGlxAiwPrintPreviewCommandSpace;
    }
    
// -----------------------------------------------------------------------------
// AiwDoDynInitMenuPaneL for PrintPreview
// -----------------------------------------------------------------------------
//
void CGlxCommandHandlerAiwPrintPreview::PreDynInitMenuPaneL(TInt aResourceId)
    {
    CGlxCommandHandlerAiwBase::PreDynInitMenuPaneL(aResourceId);
    }
// -----------------------------------------------------------------------------
// AiwDoDynInitMenuPaneL
// -----------------------------------------------------------------------------
//  
void CGlxCommandHandlerAiwPrintPreview::AiwDoDynInitMenuPaneL(TInt /*aResourceId*/, 
        CEikMenuPane* aMenuPane)
    {
    TBool fullscreenViewingMode = EFalse;
    CGlxNavigationalState* aNavigationalState = CGlxNavigationalState::InstanceL();
    CMPXCollectionPath* naviState = aNavigationalState->StateLC();
       
    if ( naviState->Levels() >= 1)
        {
        if (aNavigationalState->ViewingMode() == NGlxNavigationalState::EBrowse) 
            {
            if (naviState->Id() == TMPXItemId(KGlxCollectionPluginImageViewerImplementationUid))
                {
                fullscreenViewingMode = ETrue;
                }
            else
                {
                fullscreenViewingMode = EFalse;
                }
            } 
        else 
            {
            fullscreenViewingMode = ETrue;
            }                
        }
    CleanupStack::PopAndDestroy( naviState );
    aNavigationalState->Close();
      
    HBufC* currentTitle = StringLoader::LoadLC( R_QTN_LGAL_OPTIONS_PRINT_MENU );
    TInt cmdId = AiwMenuCmdIdL( *currentTitle,aMenuPane ) ;
    CleanupStack::PopAndDestroy(currentTitle);
    if( KErrNotFound != cmdId )
        {
        if ((MediaList().SelectionCount() == 0) && !fullscreenViewingMode)
            {
            aMenuPane->SetItemDimmed(cmdId,ETrue);    
            }
        }
    }   
    
                                        
