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
* Description:    AIW assign command handler
*
*/




#include "glxcommandhandleraiwassign.h"

#include <eikmenup.h> 
#include <StringLoader.h>
#include <glxattributecontext.h>
#include <glxcommandhandlers.hrh>
#include <glxmedia.h>
#include <glxtracer.h>
#include <mglxmedialist.h>
#include <glxuiutilities.rsg>
#include <glxnavigationalstate.h>
#include <mpxcollectionpath.h>
#include <glxcollectionpluginimageviewer.hrh>
#include "glxaiwservicehandler.h"

const TInt KGlxAiwAssignCommandSpace = 0x00000100;

// -----------------------------------------------------------------------------
// NewL
// -----------------------------------------------------------------------------
//	
EXPORT_C CGlxCommandHandlerAiwAssign* CGlxCommandHandlerAiwAssign::NewL(
        MGlxMediaListProvider* aMediaListProvider, TInt aMenuResource)
    {
    TRACER("CGlxCommandHandlerAiwAssign* CGlxCommandHandlerAiwAssign::NewL");
    CGlxCommandHandlerAiwAssign* self = new ( ELeave ) 
        CGlxCommandHandlerAiwAssign(aMediaListProvider, aMenuResource);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//	
CGlxCommandHandlerAiwAssign::CGlxCommandHandlerAiwAssign(
        MGlxMediaListProvider* aMediaListProvider, TInt aMenuResource)
:   CGlxCommandHandlerAiwBase(aMediaListProvider, aMenuResource)
    {
    TRACER("CGlxCommandHandlerAiwAssign::CGlxCommandHandlerAiwAssign");
    }

// -----------------------------------------------------------------------------
// CGlxCommandHandlerAiwAssign::DoGetRequiredAttributesL
// -----------------------------------------------------------------------------
//	
void CGlxCommandHandlerAiwAssign::DoGetRequiredAttributesL
									(RArray<TMPXAttribute>& aAttributes, TBool aFilterUsingSelection) const
	{
	TRACER("void CGlxCommandHandlerAiwAssign::DoGetRequiredAttributesL");
  // Add MIME type and filename attributes
	if (!aFilterUsingSelection || SelectionLength() <= MaxSelectedItems())
		{
	  	aAttributes.AppendL(KMPXMediaGeneralMimeType);
	  	aAttributes.AppendL(KMPXMediaGeneralUri);
	  	aAttributes.AppendL(KMPXMediaGeneralCategory);
		} 
	}

// -----------------------------------------------------------------------------
// CommandId
// -----------------------------------------------------------------------------
//	
TInt CGlxCommandHandlerAiwAssign::CommandId() const
    {
    TRACER("TInt CGlxCommandHandlerAiwAssign::CommandId() const");
    return EGlxCmdAiwAssign;
    }
    
// -----------------------------------------------------------------------------
// AiwCommandId
// -----------------------------------------------------------------------------
//	
TInt CGlxCommandHandlerAiwAssign::AiwCommandId() const
    {
    TRACER("TInt CGlxCommandHandlerAiwAssign::AiwCommandId() const");
    return KAiwCmdAssign;
    }
    
// -----------------------------------------------------------------------------
// AiwInterestResource
// -----------------------------------------------------------------------------
//	
TInt CGlxCommandHandlerAiwAssign::AiwInterestResource() const
    {
    return R_GLX_AIW_ASSIGN_TO_CONTACT_INTEREST;
    }
        
// -----------------------------------------------------------------------------
// AppendAiwParameterL
// -----------------------------------------------------------------------------
//
TBool CGlxCommandHandlerAiwAssign::AppendAiwParameterL(const TGlxMedia& aItem, 
                                     CGlxAiwServiceHandler& aAiwServiceHandler)
    {
    return AppendDefaultAiwParameterL(  aItem, 
                                        aAiwServiceHandler, 
                                        ETrue,                  // Add the Uri
                                        ETrue);                 // Add the Mime Type
    }

// -----------------------------------------------------------------------------
// AiwDoDynInitMenuPaneL
// -----------------------------------------------------------------------------
//	
void CGlxCommandHandlerAiwAssign::AiwDoDynInitMenuPaneL(TInt /*aResourceId*/, 
        CEikMenuPane* aMenuPane)
    {
    TRACER("void CGlxCommandHandlerAiwAssign::AiwDoDynInitMenuPaneL");
    TBool fullscreenViewingMode = EFalse;
    CGlxNavigationalState* aNavigationalState = CGlxNavigationalState::InstanceL();
    CMPXCollectionPath* naviState = aNavigationalState->StateLC();
    
    if ( naviState->Levels() >= 1)
        {
        if (aNavigationalState->ViewingMode() == NGlxNavigationalState::EBrowse) 
            {
            // For image viewer collection, goto view mode
            if (naviState->Id() == TMPXItemId(KGlxCollectionPluginImageViewerImplementationUid))
                {
                //it means we are in img viewer.
                fullscreenViewingMode = ETrue;
                }
            else
                {
                 //it means we are in grid view.
                fullscreenViewingMode = EFalse;
                }
            } 
        else 
            {
            //it means we are in Fullscreen.
            fullscreenViewingMode = ETrue;
            }                
        }
    CleanupStack::PopAndDestroy( naviState );
    aNavigationalState->Close();
    
    HBufC* currentTitle = StringLoader::LoadLC( R_QTN_LGAL_OPTIONS_USE_ITEMS );
    // get commandId for the current menupane item title
    TInt cmdId = AiwMenuCmdIdL( *currentTitle,aMenuPane ) ;

    CleanupStack::PopAndDestroy(currentTitle);
    
    if( KErrNotFound != cmdId )
        {
        MGlxMediaList& mediaList = MediaList();

        if (mediaList.SelectionCount() == 1 || fullscreenViewingMode )
            {
    		TGlxSelectionIterator iterator;
            
            iterator.SetToFirst(&mediaList);

        	TInt index = iterator++;
        	if ( index != KErrNotFound )
        	    {
        		TMPXGeneralCategory cat = mediaList.Item(index).Category();
    		    TInt resId(0);
    		    
    		    if ( cat == EMPXImage )
    		        {
    		        resId = R_QTN_LGAL_OPTIONS_USE_IMAGE_AS;
    		        }
    		    else if ( cat == EMPXVideo )
    		        {
    		        resId = R_QTN_LGAL_OPTIONS_USE_VIDEO_AS;
    		        }

    		    if ( resId )
    		        {
                	HBufC* menuTitle = StringLoader::LoadLC( resId );
                	if ( menuTitle )
                	    {
                	    aMenuPane->SetItemTextL(cmdId, 
                	        *menuTitle);
                	    }
                    CleanupStack::PopAndDestroy(menuTitle);
    		        }
        	    }
            }
		// In case there are Multiple Media Items Marked
        else
           	{
           	// Setting the " Use Items " Main Menu Option to Invisible
           	aMenuPane->SetItemDimmed(cmdId,ETrue);
           	}
        }
    }

// -----------------------------------------------------------------------------
// CommandSpace
// -----------------------------------------------------------------------------
//	
TInt CGlxCommandHandlerAiwAssign::CommandSpace() const
    {
    return KGlxAiwAssignCommandSpace;
    }
