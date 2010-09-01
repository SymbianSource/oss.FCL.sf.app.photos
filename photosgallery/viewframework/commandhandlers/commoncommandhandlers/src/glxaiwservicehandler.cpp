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
* Description:    Photos command handler base class
*
*/




#include <glxsingletonstore.h>

#include "glxaiwservicehandler.h"
#include "glxaiwmedia.h"
#include "AiwServiceHandler.h"

// -----------------------------------------------------------------------------
// InstanceL
// -----------------------------------------------------------------------------
//	
CGlxAiwServiceHandler* CGlxAiwServiceHandler::InstanceL()
    {
	return CGlxSingletonStore::InstanceL(&NewL);
    }
    
// -----------------------------------------------------------------------------
// Close
// -----------------------------------------------------------------------------
//
void CGlxAiwServiceHandler::Close()
	{
    CGlxSingletonStore::Close( this );
	}

// -----------------------------------------------------------------------------
// NewL
// -----------------------------------------------------------------------------
//	
CGlxAiwServiceHandler* CGlxAiwServiceHandler::NewL()
    {
    CGlxAiwServiceHandler* self = new (ELeave) CGlxAiwServiceHandler();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }
    
// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//	
CGlxAiwServiceHandler::CGlxAiwServiceHandler()
    {
    }
    
// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//	
CGlxAiwServiceHandler::~CGlxAiwServiceHandler()
    {
    iAiwMediaArray.ResetAndDestroy();
    iAiwMediaArray.Close();
    delete iAiwServiceHandler;
    }
    
// -----------------------------------------------------------------------------
// 2nd phase constructor
// -----------------------------------------------------------------------------
//	
void CGlxAiwServiceHandler::ConstructL()
    {
    iAiwServiceHandler = CAiwServiceHandler::NewL();
    }
    
// -----------------------------------------------------------------------------
// AddParamL
// -----------------------------------------------------------------------------
//	
void CGlxAiwServiceHandler::AddParamL(const TGlxMediaId& aId, const TAiwGenericParam& aParam )
    {
    // This method is called to add parameters to the iInParams.
    // This should be called by each command handler before InitializeMenuPaneL is called at all.
    // This ensures that the entire list on InParams is built for all command handlers before
    // initializing the menu.
    // When InitializeMenuPaneL is called it destroys the inParams making it necessary to 
    // aquire and append to the iInParams again should this be necessary.
    
    if ( ENotInitialised != iInitialisedMenu )
        {
        // This is either the very first time or the first time since calling InitializeMenuPaneL
        iInitialisedMenu = ENotInitialised;
        // Set iInParams to Null. This will force a call to the service Handler to aquire
        // the pointer to the new set of InParams (yet to be appended too).
        iInParams = NULL;
        }

    if (!iInParams)
        {
        // Must aquire a pointer to the new set of InParams
        iInParams = &iAiwServiceHandler->InParamListL();
        }

    // Get the Index of the TGlxMediaId in the array
    TInt index = FindId(aId);
    CGlxAiwMedia* media = NULL;
    
    if (KErrNotFound == index)
        {
        // The TGlxMediaId was not found. so create a new entry
        media = new (ELeave) CGlxAiwMedia(aId);
        CleanupStack::PushL(media);
        TLinearOrder<CGlxAiwMedia> orderer (&AiwMediaOrderById);
        iAiwMediaArray.InsertInOrderL(media, orderer);
        CleanupStack::Pop(media);
        }
    else
        {
        // The TGlxMediaId was found so use the current entry
        media = iAiwMediaArray[index];
        }
        
    // Add the parameter (method checks for duplicates.
    media->AddParamL(aParam);
    
    }

// -----------------------------------------------------------------------------
// FindId
// -----------------------------------------------------------------------------
//	
TInt CGlxAiwServiceHandler::FindId(const TGlxMediaId& aId)
    {
    // Now we can find the CGlxAiwMedia in the list with an Id equal to aId
    TInt index = iAiwMediaArray.FindInOrder(aId, (&AiwMediaOrderByMediaId));
    
    return index;

    }

// -----------------------------------------------------------------------------
// AiwMediaOrderByMediaId (static method)
// -----------------------------------------------------------------------------
//	
TInt CGlxAiwServiceHandler::AiwMediaOrderByMediaId( const TGlxMediaId* aMediaId, const CGlxAiwMedia& aAiwMedia )
    {
    const TGlxMediaId& Id = aAiwMedia.Id();
    
    if (*aMediaId < Id)
        {
        return -1;
        }
    if (*aMediaId > Id)
        {
        return 1;
        }

    return 0;
    }
    
// -----------------------------------------------------------------------------
// AiwMediaOrderById (static method)
// -----------------------------------------------------------------------------
//	
TInt CGlxAiwServiceHandler::AiwMediaOrderById( const CGlxAiwMedia& aAiwMedia1, const CGlxAiwMedia& aAiwMedia2 )
    {
    const TGlxMediaId& mediaId = aAiwMedia1.Id();
    
    return AiwMediaOrderByMediaId(&mediaId, aAiwMedia2);
    }
    
// -----------------------------------------------------------------------------
// AppendInParamsL
// -----------------------------------------------------------------------------
//	
void CGlxAiwServiceHandler::AppendInParamsL()
    {
    // Iterate through array backwards because removing the last tem causes
    // little effort.
    
    TInt count = 0;
    
    while ((count = iAiwMediaArray.Count()) > 0)
        {
        // get the last element
        CGlxAiwMedia* media = iAiwMediaArray[count - 1];
        
        // Now get all the parameters associated with the element
        media->AppendToInParamsL(*iInParams);
            
        // Remove it from array
        iAiwMediaArray.Remove(count - 1);
        // Destroy the CGlxAiwMedia
        delete media;
        }
    // Tidy up the array
    iAiwMediaArray.ResetAndDestroy();
    }
    
// -----------------------------------------------------------------------------
// ResetMenuInitialisedFlag
// -----------------------------------------------------------------------------
//
void CGlxAiwServiceHandler::ResetMenuInitialisedFlag()
    {
    iInitialisedMenu = ENotInitialised;
    // Also set params list to NULL before creating new params list
    iInParams = NULL;    
    }    

// -----------------------------------------------------------------------------
// InitializeMenuPaneL
// -----------------------------------------------------------------------------
//	
void CGlxAiwServiceHandler::InitializeMenuPaneL(CEikMenuPane& aMenuPane, TInt aResourceId, TInt aBaseMenuCmdId)
    {
    // All the parameters for all command handlers should now be appended in iInParams
    // Calling InitializeMenuPaneL will destroy the values in iInParams. Hence the need to 
    // record when this is done by using iIsMenuInitialized.

    if ( ENotInitialised == iInitialisedMenu )
        {
        AppendInParamsL();
       
        if(iInParams)
            {
            iAiwServiceHandler->InitializeMenuPaneL(aMenuPane, aResourceId, aBaseMenuCmdId, *iInParams);
            }
        
        iInitialisedMenu = EMainMenuInitialised;
        }
    }
    
// -----------------------------------------------------------------------------
// AttachMenuL
// -----------------------------------------------------------------------------
//	
void CGlxAiwServiceHandler::AttachMenuL(TInt aMenuResource, TInt aAiwInterestResource)
    {
    iAiwServiceHandler->AttachMenuL( aMenuResource, aAiwInterestResource ); 
    }
    
// -----------------------------------------------------------------------------
// IsSameCommand
// -----------------------------------------------------------------------------
//	
TBool CGlxAiwServiceHandler::IsSameCommand(TInt aCommandId, TInt aAiwCommandId)
    {
    return ( iAiwServiceHandler->ServiceCmdByMenuCmd( aCommandId ) == aAiwCommandId );
    }
    
// -----------------------------------------------------------------------------
// ExecuteMenuCmdL
// -----------------------------------------------------------------------------
//	
void CGlxAiwServiceHandler::ExecuteMenuCmdL(TInt aMenuCmdId, TUint aCmdOptions, MAiwNotifyCallback* aCallback)
    {
    CAiwGenericParamList& outParams = iAiwServiceHandler->OutParamListL();

    iAiwServiceHandler->ExecuteMenuCmdL(aMenuCmdId, *iInParams, outParams, aCmdOptions, aCallback);
    }
    
// -----------------------------------------------------------------------------
// HandleSubmenuL
// -----------------------------------------------------------------------------
//
	
TBool CGlxAiwServiceHandler::HandleSubmenuL(CEikMenuPane& aPane)
    {
    // If main menu is initialize then only check for submenu
    if ( EMainMenuInitialised == iInitialisedMenu )
        {
        if ( iAiwServiceHandler->HandleSubmenuL(aPane) )
            {
            iInitialisedMenu = ESubMenuInitialised;
            }
        }    
    return ( ESubMenuInitialised == iInitialisedMenu );
    }
    
// -----------------------------------------------------------------------------
// IsAiwMenu
// -----------------------------------------------------------------------------
//	
TBool CGlxAiwServiceHandler::IsAiwMenu(TInt aMenuResourceId)
    {
    return iAiwServiceHandler->IsAiwMenu(aMenuResourceId);
    }
    
// -----------------------------------------------------------------------------
// GetInParams (for testing only)
// -----------------------------------------------------------------------------
//	
CAiwGenericParamList& CGlxAiwServiceHandler::GetInParams()
    {
    return *iInParams;
    }

    

