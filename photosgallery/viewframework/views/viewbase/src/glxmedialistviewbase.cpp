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
* Description:    Media List View Base
*
*/




#include "glxmedialistviewbase.h"

#include <mpxcollectionutility.h>
#include <glxcommandhandler.h>
#include <glxassert.h>
#include <glxgallery.hrh>
#include <glxattributecontext.h>
#include <glxuistd.h>
#include <glxfetchcontextremover.h>
#include <glxcommandhandlers.hrh>
#include <AknUtils.h>
#include <glxtracer.h>                         // For Logs
#include <glxattributeretriever.h>
#include "glxtitlefetcher.h"
#include "glxmedialistfactory.h"
#include "mglxmedialist.h"

 _LIT(KBlankTitle,"    ");

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//	
EXPORT_C CGlxMediaListViewBase::CGlxMediaListViewBase()
    {
    }

// -----------------------------------------------------------------------------
// MLViewBaseConstructL
// -----------------------------------------------------------------------------
//	
EXPORT_C void CGlxMediaListViewBase::MLViewBaseConstructL(
        MGlxMediaListFactory* aMediaListFactory, 
        const TDesC& aTitle,TBool aEnableMiddleSoftkey)
    {
    iMediaListFactory = aMediaListFactory;
    iCollectionUtility = MMPXCollectionUtility::NewL(NULL, KMcModeDefault);
    iEnableMidddleSoftkey = aEnableMiddleSoftkey; 

    iSelectionIterator.SetRange(KMaxTInt);
    iPreloadContextForCommandHandlers  = new (ELeave) CGlxAttributeContext(&iSelectionIterator);
    
    if(aTitle.Length() > 0)
        {
        iFixedTitle = aTitle.AllocL();
        }
    
    
    if(iUiUtility->IsPenSupported())
        {
        // Responsible for controlling the ui states of toolbar
        // Create ToolbarController only for touch supported devices.
           iToolbarControl = CGlxToolbarController::NewL();
        }
    else
        {
        // Responsible for controlling the middle softkey if enabled.
        // Create Middle Softkey Controller only for non-touch devices
            iCbaControl = CGlxMSKController::NewL();
        }   
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//	
EXPORT_C CGlxMediaListViewBase::~CGlxMediaListViewBase()
    {
    CloseMediaList();

    if ( iCollectionUtility )
        {
        iCollectionUtility->Close();
        }

    delete iFixedTitle;
    delete iTitleFetcher;
    delete iPreloadContextForCommandHandlers;
        
    if( iCbaControl )
        {
        delete iCbaControl;                                          
        }
    
    if( iToolbarControl )
        {
        delete iToolbarControl;        
        }    
    }

// -----------------------------------------------------------------------------
// MediaList
// -----------------------------------------------------------------------------
//	
EXPORT_C MGlxMediaList& CGlxMediaListViewBase::MediaList()
    {
    GLX_ASSERT_ALWAYS( iMediaList, Panic( EGlxPanicNullMediaList ),
                       "Media list has been closed" );
    return *iMediaList;
    }

// -----------------------------------------------------------------------------
// DoViewActivateL
// -----------------------------------------------------------------------------
//	
EXPORT_C void CGlxMediaListViewBase::DoViewActivateL(
        const TVwsViewId& aPrevViewId, TUid aCustomMessageId, 
        const TDesC8& aCustomMessage)
    {
    TRACER ("CGlxMediaListViewBase::DoViewActivateL()");
    iUiUtility->SetAppOrientationL( EGlxOrientationDefault );	
    if ( iMediaList )
        {
        // may need to refresh the media list if it has got out of sync
        // with the current navigational state, e.g. when jumping back two views
        // instead of just one, the intermediate view's media list will not have
        // been closed so if that view is subsequently re-opened its media list
        // could contain out of date items
        CMPXCollectionPath* path = iMediaList->PathLC();
        // current navigational state
        CMPXCollectionPath* navigationalState = 
            iCollectionUtility->Collection().PathL();
        CleanupStack::PushL( navigationalState );
        // current node id in UI Hierarchy
        TMPXItemId navStateNodeId = 
            navigationalState->Id( navigationalState->Levels() - 2 );
        // current media list's node id in UI hierarchy
        TMPXItemId mediaListNodeId = path->Id( path->Levels() - 2 );
        
        if ( mediaListNodeId != navStateNodeId )
            {
            // the node ids are out of synch so close the media list
            // for it to be recreated later on
            CloseMediaList();
            }
        CleanupStack::PopAndDestroy( navigationalState );
        CleanupStack::PopAndDestroy( path );        
        }
    
    if (!iMediaList && iMediaListFactory)
        {
        iMediaList = &iMediaListFactory->CreateMediaListL(*iCollectionUtility);
    	iMediaList->AddContextL(iPreloadContextForCommandHandlers, 0);
        }

    if(iFixedTitle)
        {
        // If there is a fixed title, set it
        SetTitleL(*iFixedTitle);
        }
    else
        {
        // else obtain a title from the media list
        // First set a blank title to claim the title space
        SetTitleL(KBlankTitle);

        CMPXCollectionPath* path = iMediaList->PathLC( NGlxListDefs::EPathParent );
        iTitleFetcher = CGlxTitleFetcher::NewL(*this, path);
        CleanupStack::PopAndDestroy(path);
        }
   
    //Allow the MskController to observe medialist everytime a view with a valid
    //medialist becomes active
    if( iCbaControl && iMediaList && Cba()&& iEnableMidddleSoftkey )
        {
        CMPXCollectionPath* navigationalState = iCollectionUtility->Collection().PathL();
        CleanupStack::PushL(navigationalState);
        iCbaControl->AddToObserverL(*iMediaList,Cba()); 
        if(!(1 == navigationalState->Levels())) // Checking for the main list view
          {
            iCbaControl->SetStatusOnViewActivationL(iMediaList);
            }
        else
            {
            iCbaControl->SetMainStatusL();
            }
        CleanupStack::PopAndDestroy(navigationalState);
        }
    
    //Allow the toolbarController to observe medialist everytime a view with a valid
    //medialist becomes active
    if( Toolbar() && iToolbarControl )
        {
        iToolbarControl->AddToObserverL(*iMediaList, Toolbar());
        iToolbarControl->SetStatusOnViewActivationL(iMediaList);
        }    
  
    DoMLViewActivateL(aPrevViewId, aCustomMessageId, aCustomMessage);
    }

// -----------------------------------------------------------------------------
// DoViewDeactivate
// -----------------------------------------------------------------------------
//	
EXPORT_C void CGlxMediaListViewBase::DoViewDeactivate()
    {
    DoMLViewDeactivate();
    if( iCbaControl && iMediaList && Cba() )
        {
        //Remove Mskcontroller from medialist observer
        iCbaControl->RemoveFromObserver(*iMediaList);
        }
    if( Toolbar() && iToolbarControl )
        {
        //Remove Toolbarcontroller from medialist observer
        iToolbarControl->RemoveFromObserver(*iMediaList);
        }    
    
    // Only close the medialist if navigating backwards
    if ( iUiUtility->ViewNavigationDirection() == EGlxNavigationBackwards )
        {
        CloseMediaList();
        }

    delete iTitleFetcher;
    iTitleFetcher = NULL;
    }

// -----------------------------------------------------------------------------
// CGlxMediaListViewBase::FetchAttributesL
// -----------------------------------------------------------------------------
//	
EXPORT_C void CGlxMediaListViewBase::FetchAttributesL()
	{
	FetchAttributesL(EFalse);
	}

// -----------------------------------------------------------------------------
// CGlxMediaListViewBase::FetchAttributesForCommandL
// -----------------------------------------------------------------------------
//	
EXPORT_C void CGlxMediaListViewBase::FetchAttributesForCommandL(TInt aCommand)
	{
	FetchAttributesL(ETrue, aCommand);
	}

// -----------------------------------------------------------------------------
// SetToolbarStateL
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxMediaListViewBase::SetToolbarStateL()
    {
    TRACER("CGlxMediaListViewBase::SetToolbarStateL");
    
    if( iToolbarControl && iMediaList )
        {
        iToolbarControl->SetStatusL(iMediaList);
        }
    }

// -----------------------------------------------------------------------------
// CGlxMediaListViewBase::FetchAttributesL
// -----------------------------------------------------------------------------
//	
void CGlxMediaListViewBase::FetchAttributesL(TBool aFilterUsingCommandId, 
		                                                   TInt aCommandId)
	{
	TRACER("CGlxMediaListViewBase::FetchAttributesL");

    TGlxSelectionIterator iterator;
    CGlxAttributeContext* attributeContext = new(ELeave) CGlxAttributeContext(&iterator);
    CleanupStack::PushL(attributeContext);
    
	TInt commandHandlerCount = iCommandHandlerList.Count();
	for (TInt i = 0; i < commandHandlerCount; i++)
		{
		AddAttributesToContextL(*attributeContext, iCommandHandlerList[i], ETrue, aFilterUsingCommandId, aCommandId);
		}
	
	if (attributeContext->AttributeCount())
		{
		// Check if media attributes are already fetched.
		// If media item is NULL, Cancel the previous pending request
		MediaList().CancelPreviousRequests();
				
	    MediaList().AddContextL(attributeContext, KGlxFetchContextPriorityCommandHandlerOpening );
	    
    	// TGlxContextRemover will remove the context when it goes out of scope
    	// Used here to avoid a trap and still have safe cleanup
	    	TGlxFetchContextRemover contextRemover(attributeContext, MediaList());
        // put to cleanupstack as cleanupstack is emptied before stack objects
        // are deleted
        CleanupClosePushL( contextRemover );
        // retrieve attributes, ignore return value
    	(void)GlxAttributeRetriever::RetrieveL(*attributeContext, MediaList(), ETrue);
        // context off the list
        CleanupStack::PopAndDestroy( &contextRemover );
		}

	CleanupStack::PopAndDestroy(attributeContext);
	}

// -----------------------------------------------------------------------------
// HandleTitleAvailableL
// -----------------------------------------------------------------------------
//	
EXPORT_C void CGlxMediaListViewBase::HandleTitleAvailableL(
        const TDesC& aTitle)
    {
	 // Convertion of unsigned short  to unsigned int  
    TUint16* tileConv = const_cast<TUint16*>(aTitle.Ptr());
	TInt titleLen = aTitle.Length();
	TPtr titlePtr(tileConv,titleLen,titleLen);
	
	//to convert between arabic-indic digits and european digits.
	//based on existing language setting.
    AknTextUtils::LanguageSpecificNumberConversion(titlePtr);
    
    // Set a title after it has been obtained by the title fetcher
    SetTitleL(aTitle);
    }

// -----------------------------------------------------------------------------
// CGlxMediaListViewBase::DoPrepareCommandHandlerL
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxMediaListViewBase::DoPrepareCommandHandlerL(
											CGlxCommandHandler* aCommandHandler)
	{
	AddAttributesToContextL(*iPreloadContextForCommandHandlers, 
			                                            aCommandHandler, EFalse, EFalse);
	}
	
// -----------------------------------------------------------------------------
// CGlxMediaListViewBase::CloseMediaList
// -----------------------------------------------------------------------------
//
void CGlxMediaListViewBase::CloseMediaList()
	{
    if (iMediaList)
        {
        iMediaList->RemoveContext(iPreloadContextForCommandHandlers);
        iMediaList->Close();
        iMediaList = NULL;
        }
	}

// -----------------------------------------------------------------------------
// CGlxMediaListViewBase::AddAttributesToContextL
// -----------------------------------------------------------------------------
//
void CGlxMediaListViewBase::AddAttributesToContextL(
								 CGlxAttributeContext& aAttributeContext, 
			                     CGlxCommandHandler* aCommandHandler, 
			                     TBool aFilterUsingSelection, 
			                     TBool aFilterUsingCommandId, 
			                     TInt aCommandId)
    {
	RArray<TMPXAttribute> requiredAttributes;
	CleanupClosePushL(requiredAttributes); 
	aCommandHandler->
	GetRequiredAttributesL(requiredAttributes, aFilterUsingSelection, aFilterUsingCommandId, aCommandId); 
	
	TInt attributeCount = requiredAttributes.Count();
	for (TInt i = 0; i < attributeCount; i++)
		{
		aAttributeContext.AddAttributeL(requiredAttributes[i]);
		}
	CleanupStack::PopAndDestroy(&requiredAttributes);
	}

// -----------------------------------------------------------------------------
// HandleViewCommandL
// -----------------------------------------------------------------------------
//	
EXPORT_C TBool CGlxMediaListViewBase::HandleViewCommandL(TInt /*aCommand*/)
	{
	return EFalse;
	}
 
// -----------------------------------------------------------------------------
// DoHandleCommandL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CGlxMediaListViewBase::DoHandleCommandL(TInt aCommand)
    {
    // Pass the command to the deriving class
    return HandleViewCommandL( aCommand );
    }

