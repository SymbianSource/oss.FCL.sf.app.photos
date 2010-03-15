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
* Description:    AIW command handler base class
*
*/




#include "glxcommandhandleraiwbase.h"

#include <glxattributecontext.h>
#include <glxattributeretriever.h>
#include <mglxmedialist.h>
#include <glxuiutilities.rsg>
#include <glxgeneraluiutilities.h>
#include <glxcommandhandlers.hrh>
#include <glxtracer.h>
#include <glxassert.h>
#include <glxuistd.h>

#include "glxaiwservicehandler.h"

#include <glxresourceutilities.h>                // for CGlxResourceUtilities
#include <data_caging_path_literals.hrh>

const TInt KGlxAiwDefaultMaxSelectedItems = 200;

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//	
CGlxCommandHandlerAiwBase::CGlxCommandHandlerAiwBase(
    MGlxMediaListProvider* aMediaListProvider, TInt aMenuResource )
        : CGlxMediaListCommandHandler( aMediaListProvider ),
            iMenuResource( aMenuResource )
    {
    iAppUi = static_cast< CAknAppUi* >( CCoeEnv::Static()->AppUi() );
    }

// -----------------------------------------------------------------------------
// ConstructL
// -----------------------------------------------------------------------------
//	
void CGlxCommandHandlerAiwBase::ConstructL()
    {

    TParse parse;
    parse.Set(KGlxUiUtilitiesResource, &KDC_APP_RESOURCE_DIR, NULL);
    TFileName resourceFile;
    resourceFile.Append(parse.FullName());
    CGlxResourceUtilities::GetResourceFilenameL(resourceFile);  

    iResourceOffset = CCoeEnv::Static()->AddResourceFileL(resourceFile);

    iAiwServiceHandler = CGlxAiwServiceHandler::InstanceL();
    
    iAiwServiceHandler->AttachMenuL( iMenuResource, AiwInterestResource() ); 

   	// Add supported command
   	TCommandInfo info(CommandId());
   	// Filter out static items
    info.iMinSelectionLength = 1;
    info.iMaxSelectionLength = MaxSelectedItems();
    info.iStopAnimationForExecution = ETrue;
    info.iCategoryFilter = EMPXCommand;
    info.iCategoryRule = TCommandInfo::EForbidAll; 
   	AddCommandL(info);
   	

    }
    
// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//	
CGlxCommandHandlerAiwBase::~CGlxCommandHandlerAiwBase()
    {
    if (iAiwServiceHandler)
        {
        iAiwServiceHandler->Close();
        }
    if (iResourceOffset)
        {
        CCoeEnv::Static()->DeleteResourceFile(iResourceOffset);
        }   

    }

// -----------------------------------------------------------------------------
// DoActivateL
// -----------------------------------------------------------------------------
//	
void CGlxCommandHandlerAiwBase::DoActivateL(TInt /*aViewId*/)
    {
    }
    
// -----------------------------------------------------------------------------
// Deactivate
// -----------------------------------------------------------------------------
//	
void CGlxCommandHandlerAiwBase::Deactivate()
    {
    }


// -----------------------------------------------------------------------------
// DoExecuteL
// -----------------------------------------------------------------------------
//
	

TBool CGlxCommandHandlerAiwBase::DoExecuteL(TInt aCommandId, 
        MGlxMediaList& /* aList */)
    {
    TBool consumed = EFalse;
    
    // Check whether we can handle this command.
    // Because BypassFiltersForExecute is enabled, we receive all command IDs.
    if (iAiwServiceHandler->IsSameCommand( aCommandId, AiwCommandId() )
            && AiwItemEnabledL() ) 
        {
        // Stop animations
        iAppUi->ProcessCommandL( EGlxCmdDisableAnimations );
        
        // Now gather the AIW parameters only for this service
        // The InParam list at this point is empty (and must be empty).
        GatherAiwParametersL(*iAiwServiceHandler);
        iAiwServiceHandler->AppendInParamsL();

        TRAPD( err, iAiwServiceHandler->ExecuteMenuCmdL( aCommandId, 0, this ) ); 

        // If there is an error we must restart animations. then we can leave
        if (KErrNone != err)
            {
            // Restart animations 
            iAppUi->ProcessCommandL( EGlxCmdEnableAnimations ); 
        
            User::Leave( err );
            }
        // After executing aiw command reset params list and menuInitialized flag
        // so that next iptions menu initialization takes new params list    
        iAiwServiceHandler->ResetMenuInitialisedFlag();
        consumed = ETrue;
        }

    return consumed;
    }

// -----------------------------------------------------------------------------
// DoDynInitMenuPaneL
// -----------------------------------------------------------------------------
//	
void CGlxCommandHandlerAiwBase::DoDynInitMenuPaneL(TInt aResourceId, 
        CEikMenuPane* aMenuPane)
    {
    if (aMenuPane)
        {
        if (iAiwServiceHandler->HandleSubmenuL( *aMenuPane ))
            {
            // AIW handles AIW sub-menus so do nothing else here
            return;
            }
         
        TInt err = KErrNone;
        TInt itemPos = 0;
        if (aMenuPane->MenuItemExists(CommandId(), itemPos))
            {
            if (!AiwItemEnabledL())
                {
                err = KErrGeneral;
                }
            }
        if (err == KErrNone && iAiwServiceHandler->IsAiwMenu(aResourceId) )
            {    
            AiwDoDynInitMenuPaneL(aResourceId, aMenuPane);

            // Initialize menu pane
            TRAP(err, iAiwServiceHandler->InitializeMenuPaneL(
                     *aMenuPane, aResourceId, EGlxCmdAiwBase));
            }
                
        if (err != KErrNone)
            {
            // If there's been any error (e.g. incomplete attributes due to 
            // user cancellation), then remove command from menu
            TInt pos;
            if (aMenuPane->MenuItemExists(CommandId(), pos))
                {
                aMenuPane->SetItemDimmed(CommandId(), ETrue);
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// AppendDefaultAiwParameterL
// -----------------------------------------------------------------------------
//	
/// parameter should also be of format aParameter, so glxAiwServiceHandler => aAiwServiceHandler
TBool CGlxCommandHandlerAiwBase::AppendDefaultAiwParameterL(const TGlxMedia& aItem, 
                                                            CGlxAiwServiceHandler& aAiwServiceHandler,
                                                            TBool aAddUri,
                                                            TBool aAddMimeType)
    {
    TBool successUri = ETrue;
    TBool successMimeType = ETrue;

    // First check to see if we need to add the Uri
    if (aAddUri)
        {
        successUri = AppendTextParameterL(aItem, aAiwServiceHandler, aItem.Uri(), EGenericParamFile);
        }
    
    // Then check to see if we need the mime type  
    if (aAddMimeType)
        {
        successMimeType = AppendTextParameterL(aItem, aAiwServiceHandler, aItem.MimeType(), EGenericParamMIMEType);
        }
    
    return successUri && successMimeType;
    }
    
// -----------------------------------------------------------------------------
// AppendTextParameterL
// -----------------------------------------------------------------------------
//	
TBool CGlxCommandHandlerAiwBase::AppendTextParameterL(const TGlxMedia& aItem,
                                                    CGlxAiwServiceHandler& aAiwServiceHandler,
                                                    const TDesC& aText,
                                                    TGenericParamIdValue aGenericParamIdValue)
    {
    TBool success = EFalse;
    
    if (aText != KNullDesC)
        {
        aAiwServiceHandler.AddParamL(aItem.Id(), TAiwGenericParam( aGenericParamIdValue, TAiwVariant( aText ) ) );

        success = ETrue;
        }
        
    return success;
    }

// -----------------------------------------------------------------------------
// BypassFiltersForExecute
// -----------------------------------------------------------------------------
//	
TBool CGlxCommandHandlerAiwBase::BypassFiltersForExecute() const
    {
    // We need to bypass the MediaListCommandHandler filters for execution.
    // Because Aiw assigns its own IDs for items added to the menu, we don't 
    // know which command IDs we actually support.
    // By returning true, we receive all commands in DoExecuteL.
    return ETrue;
    }
    
// -----------------------------------------------------------------------------
// BypassFiltersForExecute
// -----------------------------------------------------------------------------
//	
TBool CGlxCommandHandlerAiwBase::BypassFiltersForMenu() const
    {
    // We need to bypass the MediaListCommandHandler filters for menu init.
    // Due to the AIW commands appearing in a submenu, we need to make sure
    // they are not automatically filtered out because they aren't
    // listed in our "Supported" command set
    // (We don't know which command values will be assigned by AIW)
    return ETrue;
    }

// -----------------------------------------------------------------------------
// GatherAiwParametersL
// -----------------------------------------------------------------------------
//
TBool CGlxCommandHandlerAiwBase::GatherAiwParametersL(CGlxAiwServiceHandler& aAiwServiceHandler)
    {

    TRACER("CGlxCommandHandlerAiwBase::GatherAiwParametersL()");
    TInt allFound = ETrue;
    
    MGlxMediaList& mediaList = MediaList();
    
    // Iterate through selected items to gather the parameters for AIW
    
	TGlxSelectionIterator iterator;
	iterator.SetToFirst(&mediaList);
	TInt index = KErrNotFound;
    
	while (KErrNotFound != (index = iterator++) && allFound)
	    {
    	allFound = AppendAiwParameterL(mediaList.Item(index), aAiwServiceHandler);
	    }
        
    return allFound;        
    }


// -----------------------------------------------------------------------------
// PreDynInitMenuPaneL
// -----------------------------------------------------------------------------
//	
void CGlxCommandHandlerAiwBase::PreDynInitMenuPaneL( TInt aResourceId )
    {
    if( AiwItemEnabledL() && iAiwServiceHandler->IsAiwMenu( aResourceId ) )
        { 
        GatherAiwParametersL(*iAiwServiceHandler);
        }
    }

// -----------------------------------------------------------------------------
// MaxSelectedItems
// -----------------------------------------------------------------------------
//	
TInt CGlxCommandHandlerAiwBase::MaxSelectedItems() const
    {
    return KGlxAiwDefaultMaxSelectedItems;
    }
    
// -----------------------------------------------------------------------------
// HandleNotifyL
// -----------------------------------------------------------------------------
//	
TInt CGlxCommandHandlerAiwBase::HandleNotifyL(
        TInt /*aCmdId*/,
        TInt aEventId,
        CAiwGenericParamList& /*aEventParamList*/,
        const CAiwGenericParamList& /*aInParamList*/)
    {
    switch ( aEventId )
        {
        case KAiwEventCompleted:
        case KAiwEventCanceled:
        case KAiwEventError:
        case KAiwEventStopped:
            {
            // Restart animations
            iAppUi->ProcessCommandL(EGlxCmdEnableAnimations);
            }
            break;
        }
        
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// AiwDoDynInitMenuPaneL
// -----------------------------------------------------------------------------
//	
void CGlxCommandHandlerAiwBase::AiwDoDynInitMenuPaneL(TInt /*aResourceId*/, 
        CEikMenuPane* /*aMenuPane*/)
    {
    // Do nothing by default, may be overridden
    }

    
// -----------------------------------------------------------------------------
// AiwItemEnabledL
// -----------------------------------------------------------------------------
//	
TBool CGlxCommandHandlerAiwBase::AiwItemEnabledL()
    {
    TBool enabled = EFalse;
    
    MGlxMediaList& mediaList = MediaList();
    if (mediaList.Count() == 0)
        {
        return EFalse;	
        }
    if ( mediaList.SelectionCount() == 0 )
        {
        // No items in selection list. Check if focus item is static
        // When image opened from image viewer, metadata will not
        // available immediately and Uri for that item will be null,
        // so need to disable aiw commands at that time. Once metadata 
        // been filled, aiw commands should available.
        TInt focusIndex = mediaList.FocusIndex();
        if((mediaList.FocusIndex()!=KErrNotFound)&&
           (mediaList.Item(focusIndex).Uri()!=KNullDesC)&&
           (!mediaList.Item(focusIndex).IsStatic()))
                {
                enabled = ETrue;
            }
        }
    else if ( mediaList.SelectionCount() <= MaxSelectedItems() )
        {
        // Count of selected items within range, so enable
        enabled = ETrue;
        }
        
    return enabled;
    }

// -----------------------------------------------------------------------------
// AiwMenuCmdIdL
// -----------------------------------------------------------------------------
//	
TInt CGlxCommandHandlerAiwBase::AiwMenuCmdIdL( const TDesC& aMenuItemText, 
                                CEikMenuPane* aMenuPane )
    {
    TInt index = 0;
    TInt cmdId = KErrNotFound;
    TInt count = aMenuPane->NumberOfItemsInPane();
    
    // Traverse through menupane until menu text required is found 
    // or there are no more items in menupane 
    while( index < count && aMenuItemText.CompareC( aMenuPane->
            ItemDataByIndexL( index ).iText ) )
        {
        index++;
        }
    // if count less than count then required item text is found, so get commandId for it    
    if( index < count )
        {
        cmdId = aMenuPane->ItemDataByIndexL( index ).iCommandId ;
        }
        
    return cmdId;
    }
    
