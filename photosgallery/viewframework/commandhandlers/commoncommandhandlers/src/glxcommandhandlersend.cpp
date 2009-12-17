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
* Description:    Send commmand handler
*
*/




//  CLASS HEADER
#include "glxcommandhandlersend.h"


//  EXTERNAL INCLUDES
#include <utf.h>							// for CnvUtfConverter
#include <sendui.h>							// for CSendui
#include <Sendnorm.rsg>						// for CSendui resources
#include <SendUiConsts.h>					// for CSendui plugin type constants
#include <CMessageData.h>					// for CMessageData
#include <coemain.h>						// for CCoeEnv
#include <StringLoader.h>					// for CStringLoader
#include <bautils.h>						// for BaflUtils
#include <data_caging_path_literals.hrh> 	// for directory and file names

//  INTERNAL INCLUDES
#include <glxcommandhandlers.hrh>			// for EGlxCmdUpload
#include <mglxmedialistprovider.h>
#include <mglxmedialist.h>
#include <glxscreenfurniture.h>             // for ScreenFurniture toolbar
#include <glxuistd.h>						// for KGlxUiUtilitiesResource
#include <glxuiutility.h>					// for HuiUtility
#include <glxlog.h>							// for debug logging
#include <glxattributecontext.h>			// for CGlxDefaultAttributeContext
#include <glxattributeretriever.h>			// for GlxAttributeRetriever
#include <glxuiutilities.rsg>				// for R_GLX_TOOLTIP_SEND
#include <glxfetchcontextremover.h>         // for TGlxFetchContextRemover 
#include <glxresourceutilities.h>           // for CGlxResourceUtilities
#include <glxicons.mbg>

// LOCAL FUNCTIONS AND CONSTANTS
namespace
	{
	const TInt KSendMsgBodySz = 0;
	const TInt KSendMsgOverallSz = 1024; // KMaxStringLength from SendUi.h
	// defined locally in the original code... @todo find a definition
	const TUid KSendAsPostcardUID = { 0x101FF961 };	
	}



// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
EXPORT_C CGlxCommandHandlerSend* CGlxCommandHandlerSend::NewL(
										MGlxMediaListProvider* aMediaListProvider, TBool aHasToolbarItem)
	{
	GLX_FUNC("CGlxCommandHandlerSend::NewL");
	CGlxCommandHandlerSend* self = 
			new (ELeave) CGlxCommandHandlerSend(aMediaListProvider, aHasToolbarItem);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}


// ----------------------------------------------------------------------------
// C++ default constructor 
// ----------------------------------------------------------------------------
CGlxCommandHandlerSend::CGlxCommandHandlerSend(MGlxMediaListProvider* aMediaListProvider, TBool aHasToolbarItem )
					   :CGlxMediaListCommandHandler(aMediaListProvider, aHasToolbarItem)
	{
	// nothing to do
	}			

// ----------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
void CGlxCommandHandlerSend::ConstructL()
	{
	GLX_FUNC("CGlxCommandHandlerSend::ConstructL");
	
	iUiUtility = CGlxUiUtility::UtilityL();
	iSendUi = CSendUi::NewL();
	
	LoadRscFileL();
	
	// add the Send command
   	TCommandInfo info(EGlxCmdSend);
    info.iMinSelectionLength = 1;
    info.iMaxSelectionLength = KMaxTInt;
   	AddCommandL(info);
	}	
	

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
EXPORT_C CGlxCommandHandlerSend::~CGlxCommandHandlerSend()
	{
	GLX_FUNC("CGlxCommandHandlerSend::~CGlxCommandHandlerSend");
	if (iResourceOffset)
        {
        CCoeEnv::Static()->DeleteResourceFile(iResourceOffset);
        }
	
	if (iUiUtility)
        {
        iUiUtility->Close();
        }
    delete iSendUi;    
	}
	
	

// ----------------------------------------------------------------------------
// ExecuteL
// ----------------------------------------------------------------------------
TBool CGlxCommandHandlerSend::DoExecuteL(TInt aCommandId, 
													MGlxMediaList& /*aList*/)
	{
    GLX_FUNC("CGlxCommandHandlerSend::DoExecuteL");
	
    if (aCommandId == EGlxCmdSend)
        {
        SendSelectedItemsL();
        return ETrue;
        }
    return EFalse;
	}



// ----------------------------------------------------------------------------
// DoActivateL
// ----------------------------------------------------------------------------
void CGlxCommandHandlerSend::DoActivateL(TInt aViewId)
	{
    GLX_FUNC("CGlxCommandHandlerSend::DoActivateL");

    iViewId = aViewId;
	MGlxMediaList& mediaList = MediaList();
                  
                     // TAP2GenericPluginParams::EGenericStdIconSend,
   //                 TAP2GenericPluginParams::EGenericStdIconSendMask);

	// need to check the category attribute for image or video
    iAttributeContext = CGlxDefaultAttributeContext::NewL();
    iAttributeContext->AddAttributeL(TMPXAttribute(KMPXMediaGeneralCategory));
    iAttributeContext->AddAttributeL(TMPXAttribute(KMPXMediaGeneralUri));
    mediaList.AddContextL( iAttributeContext, KGlxFetchContextPriorityLow );
    
    mediaList.AddMediaListObserverL(this);
	}




// ----------------------------------------------------------------------------
// Deactivate
// ----------------------------------------------------------------------------
void CGlxCommandHandlerSend::Deactivate()
    {
    GLX_FUNC("CGlxCommandHandlerSend::Deactivate");
    if (iAttributeContext)
        {
        MediaList().RemoveContext(iAttributeContext);
        delete iAttributeContext;
        iAttributeContext = NULL;
        }
        
    MediaList().RemoveMediaListObserver(this);    
    }	
	


// ----------------------------------------------------------------------------
// OfferKeyEventL
// ----------------------------------------------------------------------------
TKeyResponse CGlxCommandHandlerSend::OfferKeyEventL(const TKeyEvent& aKeyEvent, 
															TEventCode aType)
	{
    // Handle the send key
	if (aKeyEvent.iCode == EKeyYes && aType == EEventKey)
		{
		SendSelectedItemsL();
		return EKeyWasConsumed;
		}
	return EKeyWasNotConsumed;		
	}


// ----------------------------------------------------------------------------
// HandleFocusChangedL
// ----------------------------------------------------------------------------
void CGlxCommandHandlerSend::HandleFocusChangedL(
                    NGlxListDefs::TFocusChangeType /*aType*/, 
					TInt aNewIndex, TInt /*aOldIndex*/, MGlxMediaList* aList)
	{
	GLX_FUNC("CGlxCommandHandlerSend::HandleFocusChangedL");
	if ( aList && aNewIndex >= 0 )
		{
		const CGlxMedia* media = aList->Item(aNewIndex).Properties();  
		if (media)
			{// show the active palette send item
		//    iUiUtility->ScreenFurniture().ShowActivePaletteItemL(iViewId, 
		//				EGlxCmdSend, IsValidTypeL(*media));	
			}
		}	
	}

            


// ----------------------------------------------------------------------------
// SelectedFilesLC
// ----------------------------------------------------------------------------
CMessageData* CGlxCommandHandlerSend::SelectedFilesLC()
	{
	GLX_FUNC("CGlxCommandHandlerSend::SelectedFilesLC");

	// get the focused item from the media list
	MGlxMediaList& mediaList = MediaList();
	
	CMessageData* messageData = CMessageData::NewLC();    
	
	// retrieve the file name and path
	CGlxDefaultAttributeContext* attributeContext = 
										CGlxDefaultAttributeContext::NewL();
    CleanupStack::PushL(attributeContext);
    attributeContext->AddAttributeL(TMPXAttribute(KMPXMediaGeneralUri));
    mediaList.AddContextL(attributeContext, KGlxFetchContextPriorityBlocking);
    
    // TGlxContextRemover will remove the context when it goes out of scope
    // Used here to avoid a trap and still have safe cleanup   
    TGlxFetchContextRemover contextRemover (attributeContext, mediaList);
    CleanupClosePushL( contextRemover );
    
	User::LeaveIfError(GlxAttributeRetriever::RetrieveL(*attributeContext, 
															mediaList, ETrue));															
	// context off the list
    CleanupStack::PopAndDestroy( &contextRemover );														

	// if there are multiple media items, filter the postcard plugin
	iFilterOutPlugin = (mediaList.SelectionCount() > 1);
	
	// extract the filename from selected items in the media list and add it 
	// to the messageData to be returned
    TGlxSelectionIterator iterator;
    iterator.SetToFirst(&mediaList);
	TInt index = KErrNotFound;

	while ((index = iterator++) != KErrNotFound)
		{
		// does not return ownership.
	   	const CGlxMedia* media = mediaList.Item(index).Properties();
	   	if(media)
	   		{
	   		if(media->IsSupported(KMPXMediaGeneralUri) && IsValidTypeL(*media))
	    		{
	    		messageData->AppendAttachmentL(
	    								media->ValueText(KMPXMediaGeneralUri));
	   			}
	   		}
	   	else
	   		{
		    User::Leave(KErrNotReady);
		    }	
		}

	CleanupStack::PopAndDestroy(attributeContext);
	return messageData;
	}// contextRemover goes out of scope and removes the context from media list


// ----------------------------------------------------------------------------
// IsValidTypeL
// ----------------------------------------------------------------------------
TBool CGlxCommandHandlerSend::IsValidTypeL(const CGlxMedia& aMedia) 
	{
	GLX_FUNC("CGlxCommandHandlerSend::IsValidTypeL");
	// ensure the category attribute is supported by the current media item
    if (aMedia.IsSupported(KMPXMediaGeneralCategory))
        {
        switch(aMedia.ValueTObject<TInt>(KMPXMediaGeneralCategory))
        	{
    		case EMPXVideo:
    			{
    			// don't use the postcard plugin to send video
    			iFilterOutPlugin = ETrue;
    			return ETrue;
    			}
    		case EMPXImage:
    			{
	            return ETrue;
    	        }
    	    default:    
    	    	{
            	return EFalse;
            	}
        	}
        }
	return EFalse;
	}
	

// ----------------------------------------------------------------------------
// SendSelectedItemsL
// ----------------------------------------------------------------------------
void CGlxCommandHandlerSend::SendSelectedItemsL()
	{
	GLX_FUNC("CGlxCommandHandlerSend::SendSelectedItemsL");
    // retrieve the current files name and location
    CMessageData* msgData = SelectedFilesLC();
    HBufC* title = StringLoader::LoadLC(R_QTN_LGAL_POPUP_SEND);
	
	// filterOutPlugins is a list of component uids that should NOT be used to 
	// send the media file. if a video file was selected or multiple files 
	// were selected, filter out the option to send as a postcard
    CArrayFix<TUid>* filterOutPlugins =new (ELeave) CArrayFixFlat<TUid>(1);
    CleanupStack::PushL(filterOutPlugins);

	if(iFilterOutPlugin)
		{
		filterOutPlugins->AppendL(KSendAsPostcardUID);
		}
		
	// If Device supports text-entry mode only in portrait,
	// then the orientation has to be forced to EGlxOrientationTextEntry 
	// before launching the Send pop-up.   
	
 	if(iUiUtility->IsPenSupported())
		{
		iUiUtility->SetAppOrientationL( EGlxOrientationDefault );
		}
	// Trap the send call to ensure the orientation is reverted
	TRAP_IGNORE( iSendUi->ShowQueryAndSendL( msgData, Capabilities(),
	                filterOutPlugins, KNullUid, ETrue, *title ) );
		
	CleanupStack::PopAndDestroy(filterOutPlugins);
	CleanupStack::PopAndDestroy(title);
	CleanupStack::PopAndDestroy(msgData);
	}
	


// ----------------------------------------------------------------------------
// LoadRscFileL
// ----------------------------------------------------------------------------
void CGlxCommandHandlerSend::LoadRscFileL()
	{
	GLX_FUNC("CGlxCommandHandlerSend::LoadRscFileL");

	TParse parse;
    parse.Set(KGlxUiUtilitiesResource, &KDC_APP_RESOURCE_DIR, NULL);
    TFileName resourceFile;
    resourceFile.Append(parse.FullName());
    CGlxResourceUtilities::GetResourceFilenameL(resourceFile);  
    CCoeEnv* env = CCoeEnv::Static(); 
   	iResourceOffset = env->AddResourceFileL(resourceFile);
	}


// ----------------------------------------------------------------------------
// Capabilities
// ----------------------------------------------------------------------------
TSendingCapabilities CGlxCommandHandlerSend::Capabilities()
	{
	GLX_FUNC("CGlxCommandHandlerSend::Capabilities");
	return TSendingCapabilities (KSendMsgBodySz, KSendMsgOverallSz,
        						TSendingCapabilities::ESupportsAttachments);
	}

	
// ----------------------------------------------------------------------------
// PopulateToolbar
// ----------------------------------------------------------------------------
//
void CGlxCommandHandlerSend::PopulateToolbarL()
	{
	GLX_FUNC("CGlxCommandHandlerSend::PopulateToolbar");
	
	iUiUtility->ScreenFurniture()->SetTooltipL( EGlxCmdSend, CAknButton::EPositionLeft );
	}
// End of file		
