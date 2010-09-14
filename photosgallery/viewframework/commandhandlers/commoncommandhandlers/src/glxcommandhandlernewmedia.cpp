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
* Description:    New media command handler
*
*/




#include "glxcommandhandlernewmedia.h"

#include <AknUtils.h>
#include <badesca.h>
#include <bautils.h>
#include <glxattributecontext.h>
#include <glxattributeretriever.h>
#include <glxcollectiongeneraldefs.h>
#include <glxcommandfactory.h>
#include <glxcommandhandlers.hrh>
#include <glxfetchcontextremover.h>
#include <glxgeneraluiutilities.h>
#include <glxpanic.h>
#include <glxtracer.h>
#include <glxresourceutilities.h>                // for CGlxResourceUtilities
#include <glxsetappstate.h>
#include <glxtextentrypopup.h>
#include <glxuistd.h>
#include <glxuiutilities.rsg>
#include <mglxmedialist.h>
#include <mpxcollectionpath.h>
#include <mpxcommonframeworkdefs.h>
#include <StringLoader.h>
#include <glxmediageneraldefs.h>    // for KMaxMediaPopupTextLength
#include <data_caging_path_literals.hrh>
#include <glxuiutilities.rsg>
#include <e32const.h> //For TDigitType

const TInt KMaxNumberLength = 10;

_LIT(KOpenBracket, "(");
_LIT(KCloseBracket, ")");
_LIT(KFileNameFormatString, "(%+02u)");

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxCommandHandlerNewMedia* CGlxCommandHandlerNewMedia::NewL(
		MGlxMediaListProvider* aMediaListProvider, const TDesC& aFileName)
	{
	TRACER("CGlxCommandHandlerNewMedia* CGlxCommandHandlerNewMedia::NewL");
	CGlxCommandHandlerNewMedia* self = new (ELeave) CGlxCommandHandlerNewMedia(
			aMediaListProvider);
	CleanupStack::PushL(self);
	self->ConstructL(aFileName);
	CleanupStack::Pop(self);
	return self;
	}

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CGlxCommandHandlerNewMedia::CGlxCommandHandlerNewMedia(MGlxMediaListProvider* aMediaListProvider)
        : CGlxMpxCommandCommandHandler(aMediaListProvider)
    {
    // Don't do anything.
    }

// -----------------------------------------------------------------------------
// CGlxCommandHandlerNewMedia::DoHandleCommandCompleteL
// -----------------------------------------------------------------------------
//	
void CGlxCommandHandlerNewMedia::DoHandleCommandCompleteL(TAny* /*aSessionId*/, CMPXCommand* aCommandResult, 
            							TInt aError, MGlxMediaList* /*aList*/)
	{
    TRACER("CGlxCommandHandlerNewMedia::DoHandleCommandCompleteL");
	if (aError == KErrNone && aCommandResult && aCommandResult->IsSupported(KMPXMediaGeneralId))
    	{	
    	iNewMediaId = TGlxMediaId(aCommandResult->ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId));
    	iOkToExit = EFalse; // wait for media id to be added to the list 
                                // (unless we are using iSchedulerWait)
    	}	

	if (iSchedulerWait && aError != KErrAlreadyExists )
		{
	    // if iSchedulerWait exists then we know the command is being executed from
	    // the ExecuteLD() method.
		iNewMediaCreationError = aError;
		iSchedulerWait->AsyncStop();
		}
	}

// -----------------------------------------------------------------------------
// CGlxCommandHandlerNewMedia::OkToExit
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CGlxCommandHandlerNewMedia::OkToExit() const
    {
    return iOkToExit;
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CGlxCommandHandlerNewMedia::ConstructL(const TDesC& aFileName)
	{
	TRACER("CGlxCommandHandlerNewMedia::ConstructL()");
	iFileNameAlreadyExists = EFalse;

	iResourceOffset = CCoeEnv::Static()->AddResourceFileL(aFileName);

	iAsyncFocuser = new (ELeave) CGlxAsyncFocuser(this);
	// Add supported command
	TCommandInfo info(EGlxCmdAddMedia);
	// Filter out static items
	AddCommandL(info);
	}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxCommandHandlerNewMedia::~CGlxCommandHandlerNewMedia()
    {
    TRACER("CGlxCommandHandlerNewMedia::~CGlxCommandHandlerNewMedia()");
    if ( iResourceOffset )
        {
        CCoeEnv::Static()->DeleteResourceFile(iResourceOffset);
        }
    
    delete iSchedulerWait;
    delete iAsyncFocuser;
    delete iNewMediaItemTitle;
    }

// ---------------------------------------------------------------------------
// CGlxCommandHandlerNewMedia::ExecuteLD()
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CGlxCommandHandlerNewMedia::ExecuteLD(TGlxMediaId& aNewMediaId)
	{
    TRACER("CGlxCommandHandlerNewMedia::ExecuteLD");
	CleanupStack::PushL(this);
	iSchedulerWait = new (ELeave) CActiveSchedulerWait();
	
	CGlxMpxCommandCommandHandler::ExecuteL( EGlxCmdAddMedia );
	if (iNewMediaCreationError == KErrNone)
		{
		// The user pressed OK on the dialog. We need to wait for DoHandleCommandComplete()	
		iSchedulerWait->Start();
		
		if (iNewMediaCreationError == KErrNone)
			{
			aNewMediaId = iNewMediaId;
			}
		}
	
	TInt error = iNewMediaCreationError;
	CleanupStack::PopAndDestroy(this);
	return error;
	}

// ---------------------------------------------------------------------------
// Create an add to container command
// ---------------------------------------------------------------------------
//
CMPXCommand* CGlxCommandHandlerNewMedia::CreateCommandL(TInt /*aCommandId*/, 
        MGlxMediaList& aMediaList, TBool& /*aConsume*/) const
    {
    TRACER("CGlxCommandHandlerNewMedia::CreateCommandL");
    iOkToExit = ETrue;
    
    CMPXCollectionPath* path = aMediaList.PathLC( NGlxListDefs::EPathParent );
    CMPXCommand* command = NULL;

    TBuf <KMaxMediaPopupTextLength> defaultNewMediaItemTitle;
    
    TRAPD(error, TitlesL(TGlxMediaId(path->Id(0)), defaultNewMediaItemTitle));
    if(error != KErrNone)
        {
        iNewMediaCreationError = KErrCancel;
        CleanupStack::PopAndDestroy(path);
        return command;
        }
    
    HBufC* mediaPopupTitle = StringLoader::LoadLC(R_GLX_PROMPT_NAME);
    if(iFileNameAlreadyExists)
	    {
	    iFileNameAlreadyExists = EFalse ;
	    }
	else
	    {
   	    delete iNewMediaItemTitle;
    	iNewMediaItemTitle = NULL;
    	iNewMediaItemTitle = GenerateNewMediaItemTitleL(defaultNewMediaItemTitle, aMediaList);
	    }
    TPtr newMediaItemTitleDes = iNewMediaItemTitle->Des();
    
    CGlxTextEntryPopup* dialog = CGlxTextEntryPopup::NewL(*mediaPopupTitle, newMediaItemTitleDes);
    if(dialog->ExecuteLD() == EEikBidOk)
    	{
        command = TGlxCommandFactory::AddContainerCommandLC(*iNewMediaItemTitle, path->Id(0));
        CleanupStack::Pop(command);
    	}
    else
    	{
    	iNewMediaCreationError = KErrCancel;
    	}
    	
    CleanupStack::PopAndDestroy(mediaPopupTitle);
    CleanupStack::PopAndDestroy(path);
    return command;
    }


// ---------------------------------------------------------------------------
// CGlxCommandHandlerNewMedia::HandleErrorL
// ---------------------------------------------------------------------------
// 
void CGlxCommandHandlerNewMedia::HandleErrorL(TInt aError) 
	{
    TRACER("CGlxCommandHandlerNewMedia::HandleErrorL");
	if (aError == KErrAlreadyExists && iNewMediaItemTitle)
		{
		HBufC* info = StringLoader::LoadLC(R_GLX_NAME_ALREADY_USED, *iNewMediaItemTitle);
		GlxGeneralUiUtilities::ShowInfoNoteL(*info, ETrue);
		CleanupStack::PopAndDestroy(info);
		iFileNameAlreadyExists = ETrue ;
		// As the new Item name already existing , re-execute the command to show entry pop-up
		CGlxMpxCommandCommandHandler::ExecuteL( EGlxCmdAddMedia );
		if (iSchedulerWait && iNewMediaCreationError == KErrCancel )
			{
		    // if iSchedulerWait exists then we know the command is being executed from
		    // the ExecuteLD() method.
			// Stop the schedulerwait loop if dialog is cancelled after starting the schedulerwait loop
			iSchedulerWait->AsyncStop();
			}
		}
	else
		{
	    // Use default error handler
	    CGlxMpxCommandCommandHandler::HandleErrorL(aError);
		}
    }

// -----------------------------------------------------------------------------
// BypassFiltersForExecute
// -----------------------------------------------------------------------------
//	
EXPORT_C TBool CGlxCommandHandlerNewMedia::BypassFiltersForExecute() const
    {
    TRACER("CGlxCommandHandlerNewMedia::BypassFiltersForExecute()");
    // if iSchedulerWait exists then we know the command is being executed from
    // the ExecuteLD() method and filtering is not required.
    return iSchedulerWait != NULL;
    }
    

// ---------------------------------------------------------------------------
// TitlesL fetches the 'media popup title' and 'default new media item title'
// from the collection.
// ---------------------------------------------------------------------------
//

void CGlxCommandHandlerNewMedia::TitlesL(const TGlxMediaId aCollectionId,
        TDes& aDefaultNewMediaItemTitle) const
    {
    TRACER("CGlxCommandHandlerNewMedia::TitlesL");
    CMPXCollectionPath* path = CMPXCollectionPath::NewL();
    CleanupStack::PushL(path);
    MGlxMediaList* rootList = MGlxMediaList::InstanceL(*path);
    CleanupClosePushL(*rootList);

    TGlxSpecificIdIterator iter(KGlxIdSpaceIdRoot, aCollectionId);
    CGlxAttributeContext* attributeContext =
            new (ELeave) CGlxAttributeContext(&iter);
    CleanupStack::PushL(attributeContext);
    attributeContext->AddAttributeL(
            KGlxMediaCollectionPluginSpecificDefaultMediaTitle);
    rootList->AddContextL(attributeContext, KGlxFetchContextPriorityBlocking);

    // Media list must not have been deleted when the destructor of 
    // TGlxContextRemover is called while going out-of-scope.
        {
        TGlxFetchContextRemover contextRemover(attributeContext, *rootList);
        // put to cleanupstack as cleanupstack is emptied before stack objects
        // are deleted
        CleanupClosePushL(contextRemover);
        User::LeaveIfError(GlxAttributeRetriever::RetrieveL(
                *attributeContext, *rootList, ETrue));
        // context off the list
        CleanupStack::PopAndDestroy(&contextRemover);
        } // Limiting scope of contextRemover

    TInt index = rootList->Index(KGlxIdSpaceIdRoot, aCollectionId);

    __ASSERT_DEBUG(index != KErrNotFound, Panic(EGlxPanicRequiredItemNotFound));

    TGlxMedia item = rootList->Item(index);

    const CGlxMedia* media = item.Properties();
    if (media)
        {
        aDefaultNewMediaItemTitle.Copy(media->ValueText(
                KGlxMediaCollectionPluginSpecificDefaultMediaTitle).Left(
                        KMaxMediaPopupTextLength));
        }

    CleanupStack::PopAndDestroy(attributeContext);
    CleanupStack::PopAndDestroy(rootList);
    CleanupStack::PopAndDestroy(path);
    }

// ---------------------------------------------------------------------------
// TitlesL fetches the 'media popup title' and 'default new media item title'
// from the collection.
// ---------------------------------------------------------------------------
//
HBufC* CGlxCommandHandlerNewMedia::GenerateNewMediaItemTitleL(
        const TDesC& aDefaultNewMediaItemTitle, MGlxMediaList& aList) const
    {
    TRACER("CGlxCommandHandlerNewMedia::GenerateNewMediaItemTitleL");
    TGlxSequentialIterator iter;
    CGlxAttributeContext* attributeContext =
            new (ELeave) CGlxAttributeContext(&iter);
    CleanupStack::PushL(attributeContext);
    attributeContext->AddAttributeL(KMPXMediaGeneralTitle);
    aList.AddContextL(attributeContext, KGlxFetchContextPriorityBlocking);
    TGlxFetchContextRemover contextRemover(attributeContext, aList);
    // put to cleanupstack as cleanupstack is emptied before stack objects
    // are deleted
    CleanupClosePushL(contextRemover);
    User::LeaveIfError(GlxAttributeRetriever::RetrieveL(*attributeContext,
            aList, EFalse));
    // context off the list
    CleanupStack::PopAndDestroy(&contextRemover);
    CleanupStack::PopAndDestroy(attributeContext);

    RArray<TInt> numbers;
    CleanupClosePushL(numbers);

    TInt count = aList.Count();
    for (TInt i = 0; i < count; i++)
        {
        TGlxMedia item = aList.Item(i);
        const CGlxMedia* media = item.Properties();
        if (media)
            {
            const TDesC& title = media->ValueText(KMPXMediaGeneralTitle);

            TInt length = aDefaultNewMediaItemTitle.Length();
            if (title.Left(length).Compare(aDefaultNewMediaItemTitle) == 0)
                {
                if (length == title.Length())
                    {
                    numbers.InsertInOrderL(0); // special case
                    }
                else if (title.Length() > length + KOpenBracket().Length()
                        + KCloseBracket().Length())
                    {
                    TInt pos = length;
                    length = KOpenBracket().Length();

                    if (title.Mid(pos, length).Compare(KOpenBracket) == 0
                            && title.Right(KCloseBracket().Length()).Compare(
                                    KCloseBracket) == 0)
                        {
                        pos += length;
                        length = title.Length() - pos
                                - KCloseBracket().Length();
                        if (length > 0)
                            {
							HBufC* num = title.Mid(pos, length).AllocLC();
							TPtr numPtr = num->Des();
							//Change to Western numeric for determining next numeral
							AknTextUtils::ConvertDigitsTo(numPtr,EDigitTypeWestern);
                            TLex lex(numPtr);
                            TInt val = 0;
                            if (lex.Val(val) == KErrNone)
                                {
                                numbers.InsertInOrderL(val);
                                }
                            CleanupStack::PopAndDestroy(num);
                            }
                        }
                    }

                }
            }
        }

    TInt nextNumber = 0;
    count = numbers.Count();
    for (TInt i = 0; i < count; i++)
        {
        if (numbers[i] == nextNumber)
            {
            nextNumber++;
            }
        else
            {
            break;
            }
        }

    CleanupStack::PopAndDestroy(&numbers);

    TInt defaultTitleLength = aDefaultNewMediaItemTitle.Length()
            + KFileNameFormatString().Length() + KCloseBracket().Length()
            + KMaxNumberLength;
    // If the default title length is bigger than KMaxMediaPopupTextLength, 
    // make sure we allocate enough space for it.
    TInt titleLength = defaultTitleLength > KMaxMediaPopupTextLength ? 
                                            defaultTitleLength
                                            : KMaxMediaPopupTextLength;
    HBufC* newMediaItemTitle = HBufC::NewL(titleLength);
    TPtr newMediaItemTitleDes = newMediaItemTitle->Des();
    newMediaItemTitleDes.Append(aDefaultNewMediaItemTitle);

    if (nextNumber > 0)
        {
		newMediaItemTitleDes.AppendFormat(KFileNameFormatString, nextNumber);
		//Change numeric according to current input language here
		AknTextUtils::ConvertDigitsTo(newMediaItemTitleDes,
				AknTextUtils::TextEditorDigitType());
        }
    else
        {
        // 0 is a special case, return "New Media", not "New Media (0)"
        }

    return newMediaItemTitle;
    }

// -----------------------------------------------------------------------------
// SetFocusL()
// -----------------------------------------------------------------------------
// 
void CGlxCommandHandlerNewMedia::SetFocusL(TInt aIndex)
    {
    TRACER("CGlxCommandHandlerNewMedia::SetFocusL");
    iOkToExit = ETrue;
    MediaList().SetFocusL(NGlxListDefs::EAbsolute, aIndex);
    TryExitL(KErrNone);
    }

// -----------------------------------------------------------------------------
// HandleItemAddedL
// -----------------------------------------------------------------------------
//  
EXPORT_C void CGlxCommandHandlerNewMedia::HandleItemAddedL(TInt aStartIndex,
        TInt aEndIndex, MGlxMediaList* aList)
    {
    TRACER("CGlxCommandHandlerNewMedia::HandleItemAddedL");
    if (aList == &MediaList() && iNewMediaId != KGlxCollectionRootId)
        {
        for (TInt i = aStartIndex; i <= aEndIndex; i++)
            {
            if (aList->Item(i).Id() == iNewMediaId)
                {
                // calls CGlxCommandHandlerNewMedia::SetFocusL asynchronously
                iAsyncFocuser->SetFocus(i);
                break;
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//  
CGlxCommandHandlerNewMedia::CGlxAsyncFocuser::
    CGlxAsyncFocuser(CGlxCommandHandlerNewMedia* aGlxCommandHandlerNewMedia)
    : CActive(KMaxTInt), iGlxCommandHandlerNewMedia(aGlxCommandHandlerNewMedia)
    // The active object has the maximum possible priority to prevent 
    // other active objects running before it. (Unless they too are 
    // scheduled to run and have the maximum possible priority
    {
    TRACER("CGlxCommandHandlerNewMedia::CGlxAsyncFocuser::CGlxAsyncFocuser");
    __ASSERT_DEBUG(aGlxCommandHandlerNewMedia, Panic(EGlxPanicNullPointer));
    CActiveScheduler::Add(this);
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//  
CGlxCommandHandlerNewMedia::CGlxAsyncFocuser::
    ~CGlxAsyncFocuser()
    {
    TRACER("CGlxCommandHandlerNewMedia::CGlxAsyncFocuser::~CGlxAsyncFocuser()");
    Cancel();
    }

// -----------------------------------------------------------------------------
// RunL
// -----------------------------------------------------------------------------
//  
void CGlxCommandHandlerNewMedia::CGlxAsyncFocuser::RunL()
    {
    TRACER("CGlxCommandHandlerNewMedia::CGlxAsyncFocuser::RunL()");
    iGlxCommandHandlerNewMedia->SetFocusL(iFocusIndex);
    }

// -----------------------------------------------------------------------------
// DoCancel
// -----------------------------------------------------------------------------
//  
void CGlxCommandHandlerNewMedia::CGlxAsyncFocuser::DoCancel()
    {
    // No need to do anything 
    // CActive::Cancel() will wait for the request to complete
    }

// -----------------------------------------------------------------------------
// SetFocusL
// -----------------------------------------------------------------------------
//  
void CGlxCommandHandlerNewMedia::CGlxAsyncFocuser::SetFocus(TInt aIndex)
    {
    TRACER("CGlxCommandHandlerNewMedia::CGlxAsyncFocuser::SetFocus()");
    iFocusIndex = aIndex;
    TRequestStatus* requestStatus = &iStatus;
    User::RequestComplete(requestStatus,KErrNone);
    SetActive();
    }
