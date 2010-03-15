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
* Description:    Details commmand handler
*
*/




#include "glxcommandhandlerdetails.h"

#include <AknUtils.h>
#include <bautils.h>

#include <mpxviewutility.h>

#include <data_caging_path_literals.hrh> 	//for gallery related constatns
#include <glxcommandhandlers.hrh> 			//for command id

#include <glxuiutility.h>

#include <glxuistd.h>
#include <glxuiutility.h>
#include <glxuiutilities.rsg>
#include <glxpanic.h>
#include <StringLoader.h>
#include <glxtracer.h>
#include <glxlog.h>
#include <glxicons.mbg>

#include <glxmedia.h> 
#include <glxmedialist.h> 
#include <mpxmediageneraldefs.h>            // for TMPXAttribute
#include <glxattributeretriever.h>          // for CGlxAttributeRetriever 
#include <glxattributecontext.h>            // for CGlxDefaultAttributeContext
#include <glxresourceutilities.h>           // for CGlxResourceUtilities
#include <glxfetchcontextremover.h>         // for TGlxFetchContextRemover

#include <glxviewpluginuids.hrh> //for view plugin implmentation id
#include <glxcollectionpluginimageviewer.hrh>
#include <mpxcollectionutility.h>
#include <glxmetadatadialog.h>              //for metadata dialog
#include <glximgvwrmetadatadialog.h>
#include <glxnavigationalstate.h>
#include <mpxcollectionpath.h>
// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxCommandHandlerDetails* 
			CGlxCommandHandlerDetails::NewL( 
			MGlxMediaListProvider* aMediaListProvider )
	{
	CGlxCommandHandlerDetails* self = new (ELeave) CGlxCommandHandlerDetails(aMediaListProvider);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//	
CGlxCommandHandlerDetails::CGlxCommandHandlerDetails(MGlxMediaListProvider* aMediaListProvider )
														:CGlxMediaListCommandHandler(aMediaListProvider)
	{
	//Do nothing
	}			

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//	
void CGlxCommandHandlerDetails::ConstructL()
	{
    // Load resource file
    GLX_FUNC("GLX_Property::ConstructL");
	
	// register property command in command handler
	TParse parse;
    parse.Set(KGlxUiUtilitiesResource, &KDC_APP_RESOURCE_DIR, NULL);
    TFileName resourceFile;
    resourceFile.Append(parse.FullName());

    CGlxResourceUtilities::GetResourceFilenameL(resourceFile); 
   	iResourceOffset = CCoeEnv::Static()->AddResourceFileL(resourceFile);
				
	// register property command in command handler
   	TCommandInfo infoProp(EGlxCmdDetails);
   	// Filter out static items and only allows one item to select
    infoProp.iMinSelectionLength = 1;
    infoProp.iMaxSelectionLength = 1;
    //Allow DRM
    infoProp.iDisallowDRM = EFalse;
    //Allow system items
    infoProp.iDisallowSystemItems= EFalse;
    //Allow animation
    infoProp.iStopAnimationForExecution = EFalse;  
    //no category filter
    infoProp.iCategoryFilter = EMPXNoCategory;
   	AddCommandL(infoProp);
		   			   	
   	TCommandInfo infoSubMenu(EGlxCmdDetailsOption);
    infoSubMenu.iViewingState = TCommandInfo::EViewingStateBrowse;
   	
   	// register for the reset view command so that we can 
   	// dismiss the dialog if it is being shown when we are activated
   	// from SpaceUI 
    TCommandInfo resetViewCmd( EGlxCmdResetView );
    AddCommandL( resetViewCmd );
    
   	// get pointer to HUI utility
    iUiUtility = CGlxUiUtility::UtilityL();
	}	
	
// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxCommandHandlerDetails::~CGlxCommandHandlerDetails()
	{
	TRACER("GLX_Property::~CGlxCommandHandlerDetails");
	if ( iResourceOffset )
        {
        CCoeEnv::Static()->DeleteResourceFile(iResourceOffset);
        }
        
    if ( iUiUtility )
        {
        iUiUtility->Close();
        }
	}

// -----------------------------------------------------------------------------
// DoExecuteL
// -----------------------------------------------------------------------------
//	
TBool CGlxCommandHandlerDetails::DoExecuteL( TInt aCommandId, MGlxMediaList& aList )
	{
	TBool consume(EFalse);
	TRACER("GLX_Property::DoExecuteL");
	GLX_LOG_INFO1("GLX_Property::DoExecuteL::DoExecuteL %d",aCommandId);
	switch(aCommandId)
		{
		case EGlxCmdDetails:
			{
			GLX_LOG_INFO("GLX_Property::DoExecuteL::Launch UMP");

			CGlxDefaultAttributeContext* attrContext = CGlxDefaultAttributeContext::NewL();
			CleanupStack::PushL(attrContext);

			attrContext->AddAttributeL(KMPXMediaGeneralUri);

			aList.AddContextL(attrContext, KGlxFetchContextPriorityBlocking);	

			// TGlxContextRemover will remove the context when it goes out of scope
			// Used here to avoid a trap and still have safe cleanup
			TGlxFetchContextRemover contextRemover (attrContext, aList);

			CleanupClosePushL( contextRemover);

			TInt err = GlxAttributeRetriever::RetrieveL(*attrContext,aList,ETrue);

			// context off the list
			CleanupStack::PopAndDestroy( &contextRemover );

			if ( err == KErrNone )
				{
				TInt focusIndex = aList.FocusIndex();
				TGlxMedia item = aList.Item(focusIndex);
				const CGlxMedia* media = item.Properties();

				TBool isDrm = aList.Item(focusIndex).IsDrmProtected();
				TBool isVideo = EFalse;

				TMPXGeneralCategory type = MediaList().Item( focusIndex ).Category();
				if(type == EMPXVideo)   
					{
					isVideo = ETrue;
					}


				//check media support uri attribute
				if(media && media->IsSupported(KMPXMediaGeneralUri))
					{
					const TDesC& uri = media->ValueText(KMPXMediaGeneralUri);

					HBufC* uriBuf = HBufC::NewLC(uri.Length()+2);

					TPtr uriPtr (uriBuf->Des());
					uriPtr = uri;
					uriPtr.Append( isDrm );
					uriPtr.Append( isVideo );

					GLX_DEBUG2("GLX_Property::DoExecuteL::URI:%S:",&uri);

                    iUiUtility->SetViewNavigationDirection(EGlxNavigationForwards); 
                    if (aList.Collection().UidL().iUid == KGlxCollectionPluginImageViewerImplementationUid)
                        {
                        CGlxImgVwrMetadataDialog* dialog  = CGlxImgVwrMetadataDialog::NewL( uri);
                        dialog->ExecuteLD();
                        }
                    else
                        {
                        CGlxMetadataDialog* dialog  = CGlxMetadataDialog::NewL( uri );    
                        dialog->ExecuteLD();
                        }

                    
                    CleanupStack::PopAndDestroy(uriBuf);
                    }
                }
            CleanupStack::PopAndDestroy( attrContext );
            consume = ETrue;
            break;
            }// contextRemover goes out of scope and removes the context from media list
        default:
            {
            break;		
            }
        }

	return consume;
	}
// -----------------------------------------------------------------------------
// LaunchViewL
// -----------------------------------------------------------------------------
//		
void CGlxCommandHandlerDetails::LaunchViewL(TUid aViewPluginId,const TDesC& aURI)
	{
	TRACER("GLX_UMP::CGlxCommandHandlerDetails:LaunchViewL");
	
	MMPXViewUtility* viewUtility = MMPXViewUtility::UtilityL();
	CleanupClosePushL(*viewUtility);
	
	if( aURI.Length() > 0 )
		{
		//following is to preserve unicode format of filename
		//Convert aURI 16 bit descriptor to 8 bit descriptor
		TPtrC8 ptr8((TUint8*)aURI.Ptr(),aURI.Length());
		GLX_LOG_INFO1("GLX_UMP::CGlxCommandHandlerDetails:ActivateViewL::ptr8:%S:",&ptr8);
		
		//convert back to 16 bit descriptor
		TPtrC16 ptr16((TUint16*)ptr8.Ptr(),ptr8.Length());
		GLX_LOG_INFO1("GLX_UMP::CGlxCommandHandlerDetails:ActivateViewL::ptr16:%S:",&ptr16);	
		
		viewUtility->ActivateViewL(aViewPluginId,&ptr16);
		}
	else
		{
		viewUtility->ActivateViewL(aViewPluginId,&KNullDesC);
		}

	CleanupStack::PopAndDestroy(viewUtility); 
	}
// -----------------------------------------------------------------------------
// DoActivateL
// -----------------------------------------------------------------------------
//		
void CGlxCommandHandlerDetails::DoActivateL( TInt /*aViewId*/ )
	{
	
	}

// -----------------------------------------------------------------------------
// DoIsDisabled
// -----------------------------------------------------------------------------
//
TBool CGlxCommandHandlerDetails::DoIsDisabled(TInt aCommandId, 
                                        MGlxMediaList& aList) const 
    {
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
                //it means we are in img viewer.so details will be available
                fullscreenViewingMode = ETrue;
                }
            else
                {
                //it means we are in grid view.so details will be not beavailable
                fullscreenViewingMode = EFalse;
                }
            } 
        else 
            {
            //it means we are in Fullscreen.so details will be available
            fullscreenViewingMode = ETrue;
            }                
        }
    CleanupStack::PopAndDestroy( naviState );
    aNavigationalState->Close();
    
    if(EGlxCmdDetails==aCommandId && (0 == aList.Count() || 1 != aList.SelectionCount())&& !fullscreenViewingMode )
        {   
        return ETrue;
        }
    return EFalse;
    }

	
//End of file
