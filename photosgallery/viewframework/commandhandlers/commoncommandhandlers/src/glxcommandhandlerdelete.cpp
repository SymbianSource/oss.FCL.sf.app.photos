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
* Description:    Delete command handler
*
*/




#include "glxcommandhandlerdelete.h"

#include <AknUtils.h>
#include <bautils.h>
#include <data_caging_path_literals.hrh>
#include <glxcommandhandlers.hrh>
#include <glxuiutility.h>
#include <glxuistd.h>
#include <glxuiutilities.rsg>
#include <mglxmedialist.h>
#include <glxresourceutilities.h>                // for CGlxResourceUtilities
#include <glxscreenfurniture.h>
#include <mpxcollectionpath.h>
#include <StringLoader.h>

#include <glxicons.mbg>
#include <glxtracer.h>
#include <glxnavigationalstate.h>
#include <mpxcollectionpath.h>
#include <glxcollectionpluginimageviewer.hrh>
#include "glxcommandfactory.h"

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxCommandHandlerDelete* CGlxCommandHandlerDelete::NewL(
		MGlxMediaListProvider* aMediaListProvider, TBool aContainerList,
		TBool aHasToolbarItem, const TDesC& aFileName)
	{
	/// @todo Minor: Rowland Cook 06/06/07 no use of glx logging
	CGlxCommandHandlerDelete* self = new (ELeave) CGlxCommandHandlerDelete(
			aMediaListProvider, aHasToolbarItem);
	CleanupStack::PushL(self);
	self->ConstructL(aContainerList, aFileName);
	CleanupStack::Pop(self);
	return self;
	}

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CGlxCommandHandlerDelete::CGlxCommandHandlerDelete(MGlxMediaListProvider* aMediaListProvider, TBool aHasToolbarItem)
        : CGlxMpxCommandCommandHandler( aMediaListProvider, aHasToolbarItem )
    {
    // Don't do anything.
    }
 


// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CGlxCommandHandlerDelete::ConstructL(TBool aIsContainerList,
		const TDesC& aFileName)
	{
	iIsContainerList = aIsContainerList;
	iUiUtility = CGlxUiUtility::UtilityL();

	// Load resource file
	iResourceOffset = CCoeEnv::Static()->AddResourceFileL(aFileName);

	// Add supported command
	TCommandInfo info(EGlxCmdDelete);
	// Filter out static items
	info.iMinSelectionLength = 1;
	info.iDisallowSystemItems = aIsContainerList;
	AddCommandL(info);
	}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxCommandHandlerDelete::~CGlxCommandHandlerDelete()
    {
    if (iResourceOffset)
        {
        CCoeEnv::Static()->DeleteResourceFile(iResourceOffset);
        }
    if(iUiUtility)
        {
        iUiUtility->Close();
        }
    }

// ---------------------------------------------------------------------------
// Create an add to container command
// ---------------------------------------------------------------------------
//
CMPXCommand* CGlxCommandHandlerDelete::CreateCommandL(TInt /*aCommandId*/, 
        MGlxMediaList& aMediaList, TBool& /*aConsume*/) const
    {
    CMPXCollectionPath* path = aMediaList.PathLC( NGlxListDefs::EPathFocusOrSelection );
    CMPXCommand* command = TGlxCommandFactory::DeleteCommandLC(*path);
    CleanupStack::Pop(command);
    CleanupStack::PopAndDestroy(path);
    return command;
    } 

// -----------------------------------------------------------------------------
// Return default progress string
// -----------------------------------------------------------------------------
//	
HBufC* CGlxCommandHandlerDelete::ProgressTextL(TInt /*aCommandId*/) const
    {
    return StringLoader::LoadL(R_QTN_FLDR_DELETING_WAIT_NOTE);
    }
    
// -----------------------------------------------------------------------------
// Return confirmation string
// -----------------------------------------------------------------------------
//	
HBufC* CGlxCommandHandlerDelete::ConfirmationTextL(TInt /*aCommandId*/, 
        TBool aMultiSelection) const
    {
    // Return appropriate confirmation note
    HBufC* text = NULL;
    if (aMultiSelection)    
        {
        if (iIsContainerList) 
            {
            // Delete multiple containers string
            text = StringLoader::LoadL(R_GLX_CONFIRMATION_DELETE_CONTAINER_MULTIPLE);
            }
        else 
            {
            // Delete multiple items string
            text = StringLoader::LoadL(R_GLX_CONFIRMATION_DELETE_MULTIPLE);
            }
        }
    else 
    	{
	    if (iIsContainerList) 
	          {
	          // Delete one container string
	          text = StringLoader::LoadL(R_GLX_CONFIRMATION_DELETE_CONTAINER_SINGLE);
	          }
	        else 
	          {
	          // Delete one item string
	          text = StringLoader::LoadL(R_GLX_CONFIRMATION_DELETE_SINGLE);
	          }
    	}
    return text; 
    }
    
// ----------------------------------------------------------------------------
// DoActivateL
// ----------------------------------------------------------------------------
void CGlxCommandHandlerDelete::DoActivateL(TInt /*aViewId*/)
	{

	}

// ----------------------------------------------------------------------------
// OfferKeyEventL
// ----------------------------------------------------------------------------
TKeyResponse CGlxCommandHandlerDelete::OfferKeyEventL(const TKeyEvent& aKeyEvent, 
															TEventCode aType)
	{
    TKeyResponse response = EKeyWasNotConsumed;
    // Is the key event from the Cancel (Backspace) key
	if (aKeyEvent.iCode == EKeyBackspace && aType == EEventKey)
		{
		// try to execute the delete command
		if(ExecuteL( EGlxCmdDelete ))
		    {
    		response = EKeyWasConsumed;
		    }
		}
	return response;	
	}

// ----------------------------------------------------------------------------
// PopulateToolbar
// ----------------------------------------------------------------------------
//
void CGlxCommandHandlerDelete::PopulateToolbarL()
	{
	TRACER( "CGlxCommandHandlerRename::PopulateToolbar" );
	
	iUiUtility->ScreenFurniture()->SetTooltipL( EGlxCmdDelete, CAknButton::EPositionLeft );
	}

// -----------------------------------------------------------------------------
// DoIsDisabled
// -----------------------------------------------------------------------------
//
TBool CGlxCommandHandlerDelete::DoIsDisabled(TInt aCommandId, 
        MGlxMediaList& aList) const 
        {
        TRACER( "CGlxCommandHandlerDelete::DoIsDisabled" );
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
                } 
            else 
                {
                //it means we are in Fullscreen.
                fullscreenViewingMode = ETrue;
                }                
            }
        CleanupStack::PopAndDestroy( naviState );
        aNavigationalState->Close();
        if (EGlxCmdDelete==aCommandId && 0 == aList.Count() &&
		                                 !fullscreenViewingMode)
            {   
            return ETrue;
            }     
        return EFalse;
        }
//end of file
		
