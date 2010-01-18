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
* Description:    Media List Command Handler
*
*/




#include <e32base.h>

#include "glxmpxcommandcommandhandler.h"

#include <AknProgressDialog.h> 
#include <bautils.h>
#include <data_caging_path_literals.hrh>
#include <eikprogi.h>
#include <mpxmediageneraldefs.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxmessageprogressdefs.h>
#include <mpxcommandgeneraldefs.h>
#include <StringLoader.h>

#include <glxgeneraluiutilities.h>
#include <mglxmedialist.h>
#include <glxfetchcontextremover.h>
#include <glxmedialistiterator.h>
#include <glxattributecontext.h>
#include <glxattributeretriever.h>
#include <glxuistd.h>
#include <glxuiutilities.rsg>
#include <glxcommandhandlers.hrh>
#include <glxtracer.h>
#include <glxresourceutilities.h>                // for CGlxResourceUtilities

/// @todo Move elsewhere
const TInt KGlxMaxNoteLength = 256;

// -----------------------------------------------------------------------------
// ConstructL
// -----------------------------------------------------------------------------
//	
EXPORT_C CGlxMpxCommandCommandHandler::
    CGlxMpxCommandCommandHandler(MGlxMediaListProvider* aMediaListProvider, TBool aHasToolbarItem)
        : CGlxMediaListCommandHandler(aMediaListProvider, aHasToolbarItem)
	{
    TRACER("CGlxMpxCommandCommandHandler::CGlxMpxCommandCommandHandler");
    iAppUi = static_cast< CAknAppUi* >( CCoeEnv::Static()->AppUi() );
	}
	
// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//	
EXPORT_C CGlxMpxCommandCommandHandler::~CGlxMpxCommandCommandHandler()
	{
    TRACER("CGlxMpxCommandCommandHandler::~CGlxMpxCommandCommandHandler");
	// cancel command if running
	if (iProgressComplete)
	    {
//    	MediaList().Cancel();
	    }
	    
    delete iProgressDialog;

    if ( iResourceOffset )
        {
        CCoeEnv::Static()->DeleteResourceFile(iResourceOffset);
        }
	}

// -----------------------------------------------------------------------------
// DoExecuteL
// -----------------------------------------------------------------------------
//	
EXPORT_C TBool CGlxMpxCommandCommandHandler::DoExecuteL(TInt aCommandId, 
        MGlxMediaList& aList)
    {
    TRACER("CGlxMpxCommandCommandHandler::DoExecuteL");
    // reset progress complete flag
    iProgressComplete = EFalse;
    
    // deriving class consumes command by default
    TBool consume = ETrue;
	TInt selectionCount = aList.SelectionCount();
	const TGlxMedia& focusedMedia = aList.Item(aList.FocusIndex());
    // show confirmation note
    consume = ConfirmationNoteL(aCommandId, aList);

    // Check if the selected / focused file(s)s have been deleted from
	// another application while the confirmation note is displayed
    if (((selectionCount > 0) && (aList.SelectionCount() == 0)) ||
        ((selectionCount == 0) &&
        (focusedMedia.Id() != aList.Item(aList.FocusIndex()).Id())))
		{
		// All the selected / focused item(s) have been deleted,
		// do not allow the command to execute.
		consume = EFalse;
		}

    if ( consume )
        {
        // get a command object from the deriving class.
        // Allow deriving class modify the consume value, even without 
        // creating a commmand (in case it wants to filter out a command)
        CMPXCommand* command = CreateCommandL(aCommandId, aList, consume);
        
        if (command)
            {
            CleanupStack::PushL(command);
            
            if ( CommandInfo(aCommandId).iStopAnimationForExecution )
                {
                // Stop GIF animation
                iAppUi->ProcessCommandL(EGlxCmdDisableAnimations);
                iAnimationStopped = ETrue;
                }
            
            // Add the pointer of this command handler as session id into the message
            // This can be used to ensure that this object is the intended recipient
            // of a message
            command->SetTObjectValueL<TAny*>(KMPXCommandGeneralSessionId,
            		static_cast<TAny*>(this));

       		aList.AddMediaListObserverL(this);
			
            aList.CommandL(*command);
            
            // raise progress note. Note will be closed when complete message received
            ProgressNoteL(aCommandId);
            
            CleanupStack::PopAndDestroy(command);
            }
        }
    
    return consume;
    }

// -----------------------------------------------------------------------------
// TryExitL
// -----------------------------------------------------------------------------
//	
EXPORT_C void CGlxMpxCommandCommandHandler::TryExitL(TInt aErrorCode)
	{
    TRACER("CGlxMpxCommandCommandHandler::TryExitL");
	// any error will abort the command execution
    if ( KErrNone != aErrorCode ) 
	    {
	    // error received. Close progress note
		DismissProgressNoteL();
		
	    // Remove medialist observer
	    RemoveMediaListObserver();
		
		// handle error
		HandleErrorL(aErrorCode);
	    }
	else if (iProgressComplete)
        {
    	// ask deriving class if it is ready to exit (i.e., to close progress note). 
    	// (It may be waiting for another message, etc.)
    	if (OkToExit()) 
    	    {
    	    // close progress note
    		DismissProgressNoteL();
    		
    	    // Remove medialist observer
    	    RemoveMediaListObserver();
    	    
    		// show completion note
    		CompletionNoteL();
    	    }
        }
	}	


// -----------------------------------------------------------------------------
// Default implementation shows an error note
// -----------------------------------------------------------------------------
//	
EXPORT_C void CGlxMpxCommandCommandHandler::HandleErrorL(TInt aErrorCode) 
    {
    TRACER("CGlxMpxCommandCommandHandler::HandleErrorL");
	// show error note
	GlxGeneralUiUtilities::ShowErrorNoteL(aErrorCode);
    }

// -----------------------------------------------------------------------------
// Update progress 
// -----------------------------------------------------------------------------
//	
TBool CGlxMpxCommandCommandHandler::UpdateProgressL(const CMPXMessage& aMessage,
        TInt& aError)
    {
    TRACER("CGlxMpxCommandCommandHandler::UpdateProgressL");
    TBool isComplete = EFalse;
    
    // should not receive any progress messages after progress is completed
    __ASSERT_DEBUG(!iProgressComplete, Panic(EGlxPanicIllegalState));
    
    ///    @todo remove progress update error handling
    TMPXAttribute currentCountAttr(KMPXMessageContentIdProgress, EMPXMessageProgressCurrentCount);
    TMPXAttribute totalCountAttr(KMPXMessageContentIdProgress, EMPXMessageProgressTotalCount);
    if ( ( KErrNone == aError ) && (aMessage.IsSupported(currentCountAttr))
    	&& (aMessage.IsSupported(totalCountAttr) ) )
        {
        // get current and total count from the message
	    TInt currentValue = aMessage.ValueTObjectL<TInt>(currentCountAttr);	
	    TInt total= aMessage.ValueTObjectL<TInt>(totalCountAttr);	

        // update progress bar if not complete yet, otherwise remember completion
		if ( currentValue < total )
			{
			// Progress info construction may have failed, but command was issued before
			if (iProgressInfo)
			    {
    			iProgressInfo->SetFinalValue(total);
    			iProgressInfo->SetAndDraw(currentValue);
			    }
            }
		else
			{
			isComplete = ETrue;

            if ( iAnimationStopped )
                {
                // Restart animation
                iAppUi->ProcessCommandL(EGlxCmdEnableAnimations);
                iAnimationStopped = EFalse;
                }
			}
		}
	
	return isComplete;    
    }

// -----------------------------------------------------------------------------
// Default implementation of advanced command handling does nothing
// -----------------------------------------------------------------------------
//	
EXPORT_C void CGlxMpxCommandCommandHandler::DoHandleCommandCompleteL(TAny* /*aSessionId*/, 
        CMPXCommand* /*aCommandResult*/, TInt /*aError*/, MGlxMediaList* /*aList*/)
    {
    TRACER("CGlxMpxCommandCommandHandler::DoHandleCommandCompleteL");
    }


// -----------------------------------------------------------------------------
// Default implementation of advanced message handling does nothing
// -----------------------------------------------------------------------------
//	
EXPORT_C TInt CGlxMpxCommandCommandHandler::DoHandleMessageL(const CMPXMessage& /*aMessage*/,
	    MGlxMediaList& /*aList*/)
    {
    TRACER("CGlxMpxCommandCommandHandler::DoHandleMessageL");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// Default implementation DoHandleItemAddedL does not do anything
// -----------------------------------------------------------------------------
//	
EXPORT_C void CGlxMpxCommandCommandHandler::DoHandleItemAddedL(TInt /*aStartIndex*/, 
		TInt /*aEndIndex*/, MGlxMediaList* /*aList*/)
	{
    TRACER("CGlxMpxCommandCommandHandler::DoHandleItemAddedL");
	}

// -----------------------------------------------------------------------------
// Default implementation is always ready to exit
// -----------------------------------------------------------------------------
//	
EXPORT_C TBool CGlxMpxCommandCommandHandler::OkToExit() const
    {
    TRACER("CGlxMpxCommandCommandHandler:::OkToExit()");
    return ETrue;
    }

// -----------------------------------------------------------------------------
// Show confirmation note
// -----------------------------------------------------------------------------
//	
EXPORT_C TBool CGlxMpxCommandCommandHandler::ConfirmationNoteL(TInt aCommandId,
        MGlxMediaList& aMediaList) const
	{
    TRACER("CGlxMpxCommandCommandHandler::ConfirmationNoteL");
	TInt selectionCount = aMediaList.SelectionCount();

    // If media list is not empty, treat focused item as selected
    // At this point can assume that the command was disabled 
    // if static items were not supported	
	if ( selectionCount == 0 && aMediaList.Count() > 0 )
		{
		selectionCount = 1;
		}
	
    // Show confirmation note
	TBool confirmed = EFalse;
	if ( selectionCount == 1 )
	    {
		confirmed = ConfirmationNoteSingleL(aCommandId, aMediaList);
	    }
	else
	    {
		confirmed = ConfirmationNoteMultipleL(aCommandId, aMediaList);
	    }
		
	return confirmed;
	}

// -----------------------------------------------------------------------------
// So confirmation note if a single item is selected
// -----------------------------------------------------------------------------
//	
EXPORT_C TBool CGlxMpxCommandCommandHandler::ConfirmationNoteSingleL(TInt aCommandId, 
        MGlxMediaList& aMediaList) const
    {
    TRACER("CGlxMpxCommandCommandHandler::ConfirmationNoteSingleL");
    // if no confirmation note shown, assume command is confirmed
    TBool confirmed = ETrue;

    // get confirmation note text for single selection
    HBufC* noteText = ConfirmationTextL(aCommandId, EFalse); // EFalse for single selection
    if ( noteText )
    {
        CleanupStack::PushL(noteText);
    
        // Retrieve title string for selected item from the collection plugin
            
        // create fetch context
        TGlxSelectionIterator iterator;
        iterator.SetRange(1);
        CGlxAttributeContext* attributeContext = new(ELeave) CGlxAttributeContext(&iterator);
        CleanupStack::PushL(attributeContext);
        attributeContext->AddAttributeL(KMPXMediaGeneralTitle);
                        
        // add context to media list
        aMediaList.AddContextL(attributeContext, KGlxFetchContextPriorityLow);
            
        // TGlxContextRemover will remove the context when it goes out of scope
        // Used here to avoid a trap and still have safe cleanup
        TGlxFetchContextRemover contextRemover(attributeContext, aMediaList);
        // put to cleanupstack as cleanupstack is emptied before stack objects
        // are deleted
        CleanupClosePushL( contextRemover);
        // retrieve title attribute
        TInt err = GlxAttributeRetriever::RetrieveL(*attributeContext, aMediaList, EFalse);
        // context off the list
        CleanupStack::PopAndDestroy( &contextRemover );
    
        if ( err == KErrNone )
            {
            // use iterator to get the index of the right item 
            iterator.SetToFirst(&aMediaList);
            const CGlxMedia* media = aMediaList.Item(iterator++).Properties();
        
            // noteText has a place for a title string in it
            const TDesC& itemName = media->ValueText(KMPXMediaGeneralTitle);
    
            TBuf<KGlxMaxNoteLength> text;
            StringLoader::Format(text, *noteText, -1, itemName);
    
            // show popup
            confirmed = GlxGeneralUiUtilities::ConfirmQueryL(R_GLX_QUERY_YES_NO, text);
            }
            // (else) If error, assume confirmed anyway
            	
            CleanupStack::PopAndDestroy(attributeContext);
            CleanupStack::PopAndDestroy(noteText);
     }
	 return confirmed;
	 }
	
// -----------------------------------------------------------------------------
// Confirmation note if multiple items are selected
// -----------------------------------------------------------------------------
//	
EXPORT_C TBool CGlxMpxCommandCommandHandler::ConfirmationNoteMultipleL(TInt aCommandId, 
        MGlxMediaList& aMediaList) const
	{
    TRACER("CGlxMpxCommandCommandHandler::ConfirmationNoteMultipleL");
	// if no confirmation note shown, assume command is confirmed
	TBool confirmed = ETrue;

	// get confirmation note text for multiselection
	HBufC* noteText = ConfirmationTextL(aCommandId, ETrue); // ETrue for Multiselection
	if ( noteText )
		{
		CleanupStack::PushL(noteText);

        // string has a place for number in it, and that number should be populated with selected
        // item count
		TInt count = aMediaList.SelectionCount();

        TBuf<KGlxMaxNoteLength> text;
		GlxGeneralUiUtilities::FormatString(text, *noteText, -1, count, ETrue);
		
        // show popup
		confirmed = GlxGeneralUiUtilities::ConfirmQueryL(R_GLX_QUERY_YES_NO, text);

        CleanupStack::PopAndDestroy(noteText);
		}
	return confirmed;
	}

// -----------------------------------------------------------------------------
// Default implementation returns null, and no confirmation note will be shown
// -----------------------------------------------------------------------------
//	
EXPORT_C HBufC* CGlxMpxCommandCommandHandler::ConfirmationTextL(TInt /*aCommandId*/, 
        TBool /*aMultiSelection*/) const
    {
    return NULL;
    }       

// -----------------------------------------------------------------------------
// Return default progress string
// -----------------------------------------------------------------------------
//	
EXPORT_C HBufC* CGlxMpxCommandCommandHandler::ProgressTextL(TInt /*aCommandId*/) const
    {
    TRACER("CGlxMpxCommandCommandHandler::ProgressTextL");
    // Lazy construction for resource file
    if (iResourceOffset == 0)
        {
        // Load resource file
    	TParse parse;
        parse.Set(KGlxUiUtilitiesResource, &KDC_APP_RESOURCE_DIR, NULL);
        TFileName resourceFile;
        resourceFile.Append(parse.FullName());
        CGlxResourceUtilities::GetResourceFilenameL(resourceFile);  

        // Store resource offset as member variable, so don't have to trap the
        // leaving calls here
       	iResourceOffset = CCoeEnv::Static()->AddResourceFileL(resourceFile);
        }

    // get progress note string
    return StringLoader::LoadL( R_GLX_PROGRESS_GENERAL );
    }

// -----------------------------------------------------------------------------
// Default implementation returns null, and no completion note will be shown
// -----------------------------------------------------------------------------
//	
EXPORT_C HBufC* CGlxMpxCommandCommandHandler::CompletionTextL() const
    {
    return NULL;
    }

// -----------------------------------------------------------------------------
// ProgressNoteL
// -----------------------------------------------------------------------------
//	
void CGlxMpxCommandCommandHandler::ProgressNoteL(TInt aCommandId)
	{
    TRACER("CGlxMpxCommandCommandHandler::ProgressNoteL(TInt aCommandId)");
	//MGlxMediaList& mediaList = MediaList();

    // get progress note 
    HBufC* progressText = ProgressTextL(aCommandId);
    __ASSERT_DEBUG(progressText, Panic(EGlxPanicNullDescriptor));
	CleanupStack::PushL(progressText);
    // construct progress dialog
	iProgressDialog = new(ELeave)CAknProgressDialog(
			(REINTERPRET_CAST(CEikDialog**,&iProgressDialog)));
	iProgressDialog->PrepareLC(R_GLX_PROGRESS_NOTE); 
	iProgressDialog->SetTextL(*progressText);
	iProgressDialog->SetCallback(this);
	
    // pick up progress info so that progress notification can be later updated
	iProgressInfo = iProgressDialog->GetProgressInfoL();
	
    // launch the note
	iProgressDialog->RunLD();
	CleanupStack::PopAndDestroy(progressText); 
	}

// -----------------------------------------------------------------------------
// DismissProgressNoteL
// -----------------------------------------------------------------------------
//	
void CGlxMpxCommandCommandHandler::DismissProgressNoteL()
	{
    TRACER("CGlxMpxCommandCommandHandler::DismissProgressNoteL");
    // Close the progress note, if displayed
    if (iProgressDialog) 
        {
    	iProgressDialog->ProcessFinishedL();
    	iProgressDialog = NULL;
    	iProgressInfo = NULL;
        }
	}

// -----------------------------------------------------------------------------
// Remove MediaList Observer
// -----------------------------------------------------------------------------
//
void CGlxMpxCommandCommandHandler::RemoveMediaListObserver()
	{
    TRACER("CGlxMpxCommandCommandHandler::RemoveMediaListObserver");
	MGlxMediaList& mediaList = MediaList();
	mediaList.RemoveMediaListObserver(this);
	}

// -----------------------------------------------------------------------------
// Show a completion note
// -----------------------------------------------------------------------------
//	
void CGlxMpxCommandCommandHandler::CompletionNoteL() const
	{
    TRACER("CGlxMpxCommandCommandHandler::CompletionNoteL");
	// Get completion note text 
	HBufC* noteText = CompletionTextL(); 
	if ( noteText )
	    {
		CleanupStack::PushL(noteText);
		GlxGeneralUiUtilities::ShowConfirmationNoteL(*noteText, ETrue);
        CleanupStack::PopAndDestroy(noteText);
		}
	}

// -----------------------------------------------------------------------------
// HandleItemAddedL
// -----------------------------------------------------------------------------
//  
EXPORT_C void CGlxMpxCommandCommandHandler::HandleItemAddedL(TInt /*aStartIndex*/, TInt /*aEndIndex*/, MGlxMediaList* /*aList*/)
    {
    // Do nothing
    }

// -----------------------------------------------------------------------------
// HandleMediaL
// -----------------------------------------------------------------------------
//	
EXPORT_C void CGlxMpxCommandCommandHandler::HandleMediaL(TInt /*aListIndex*/, MGlxMediaList* /*aList*/)
	{
	// Do nothing
	}
	
// -----------------------------------------------------------------------------
// HandleItemRemovedL
// -----------------------------------------------------------------------------
//	
EXPORT_C void CGlxMpxCommandCommandHandler::HandleItemRemovedL(TInt /*aStartIndex*/, TInt /*aEndIndex*/, MGlxMediaList* /*aList*/)
	{
	// Do nothing
	}
	
// -----------------------------------------------------------------------------
// HandleItemModifiedL
// -----------------------------------------------------------------------------
//	
EXPORT_C void CGlxMpxCommandCommandHandler::HandleItemModifiedL(const RArray<TInt>& /*aItemIndexes*/, 
        MGlxMediaList* /*aList*/)
	{
	// Do nothing
	}	

// -----------------------------------------------------------------------------
// HandleAttributesAvailableL
// -----------------------------------------------------------------------------
//	
EXPORT_C void CGlxMpxCommandCommandHandler::HandleAttributesAvailableL(TInt /*aItemIndex*/, 	
	const RArray<TMPXAttribute>& /*aAttributes*/, MGlxMediaList* /*aList*/)
	{
	// Do nothing
	}
		
// -----------------------------------------------------------------------------
// HandleFocusChangedL
// -----------------------------------------------------------------------------
//	
EXPORT_C void CGlxMpxCommandCommandHandler::HandleFocusChangedL(NGlxListDefs::TFocusChangeType /*aType*/, 
        TInt /*aNewIndex*/, TInt /*aOldIndex*/, MGlxMediaList* /*aList*/) 
	{
	// Do nothing
	}
	
// -----------------------------------------------------------------------------
// HandleItemSelectedL
// -----------------------------------------------------------------------------
//	
EXPORT_C void CGlxMpxCommandCommandHandler::HandleItemSelectedL(TInt /*aIndex*/, 
        TBool /*aSelected*/, MGlxMediaList* /*aList*/)
	{
	// Do nothing
	}

// -----------------------------------------------------------------------------
// HandleMessageL
// -----------------------------------------------------------------------------
//	
EXPORT_C void CGlxMpxCommandCommandHandler::HandleMessageL(const CMPXMessage& aMessage, 
        MGlxMediaList* aList)
	{
    TRACER("CGlxMpxCommandCommandHandler::HandleMessageL");
    if ((aMessage.IsSupported(KMPXMessageGeneralId) &&
    	(aMessage.IsSupported(KMPXCommandGeneralSessionId))))
        {
        TInt msgId = aMessage.ValueTObjectL<TInt>(KMPXMessageGeneralId);
        TAny* sessionId = aMessage.ValueTObjectL<TAny*>(KMPXCommandGeneralSessionId);

        TInt error = KErrNone; 
        // Check if this is a progress message and intended for this object
        if ( KMPXMessageContentIdProgress == msgId && static_cast<TAny*>(this) == sessionId )
            {
            // Update progress note
            iProgressComplete = UpdateProgressL(aMessage, error);
    		}
    	else 
    	    {
    	    // This is not a progress message - let deriving class handle
    	    error = DoHandleMessageL(aMessage, *aList);
    	    }
    	    
    	// Check if ready to exit, i.e., to close progress note, and allow user
    	// to select another menu option
        TryExitL(error);
        }
	}

// -----------------------------------------------------------------------------
// HandleMessageL
// -----------------------------------------------------------------------------
//	
EXPORT_C void CGlxMpxCommandCommandHandler::HandleCommandCompleteL(TAny* aSessionId,
	CMPXCommand* aCommandResult, TInt aError, MGlxMediaList* aList)
    {
    TRACER("CGlxMpxCommandCommandHandler::HandleCommandCompleteL");
    // Unmark all medialist items. Exit Multiple marking mode upon command completion
    iAppUi->ProcessCommandL(EGlxCmdEndMultipleMarking);

    ///@todo Enforce a single command request at a time on the media list.
    DoHandleCommandCompleteL(aSessionId, aCommandResult, aError, aList);
    iProgressComplete = ETrue;
    TryExitL(aError);
    }

// -----------------------------------------------------------------------------
// DialogDismissedL
// -----------------------------------------------------------------------------
//  
EXPORT_C void CGlxMpxCommandCommandHandler::DialogDismissedL(TInt /*aButtonId*/)
	{
    TRACER("CGlxMpxCommandCommandHandler::DialogDismissedL");
	if (!iProgressComplete)
		{
		// Remove as media list observer
		MGlxMediaList& mediaList = MediaList();
		mediaList.CancelCommand();
		mediaList.RemoveMediaListObserver(this);
		}
	}

//-----------------------------------------------------------------------------
// DoActivateL
//-----------------------------------------------------------------------------
//
EXPORT_C void CGlxMpxCommandCommandHandler::DoActivateL(TInt /*aViewId*/)
	{
	// Do nothing	
	}
	
// -----------------------------------------------------------------------------
// Deactivate - Deactivate this command handler
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxMpxCommandCommandHandler::Deactivate()
    {
    TRACER("CGlxMpxCommandCommandHandler::Deactivate");
    // Close the progress note, if displayed
	if (iProgressDialog)
        {
        TRAP_IGNORE(DismissProgressNoteL());
        }
    }
