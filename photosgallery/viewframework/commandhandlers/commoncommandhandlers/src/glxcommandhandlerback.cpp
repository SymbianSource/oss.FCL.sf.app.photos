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
* Description:    Back commmand handler
*
*/




#include "glxcommandhandlerback.h"

#include <avkon.hrh>
#include <data_caging_path_literals.hrh>

   
#include <apgtask.h>


#include <apgcli.h>
#include <mpxcollectionutility.h>
#include <mpxviewutility.h>			// For MMPXViewUtility

#include <glxcommandhandlers.hrh>
#include <glxpanic.h>
#include <glxtracer.h>
#include <glxlog.h>
#include <glxnavigationalstate.h>
#include <glxcollectionplugincamera.hrh>
#include <glxcollectionplugindownloads.hrh>
#include <glxcollectionpluginalbums.hrh>
#include <glxcollectionpluginmonths.hrh>
#include <glxcollectionplugintags.hrh>
#include <glxcollectionpluginimageviewer.hrh>
#include <glxgallery.hrh>

#include "glxmediaselectionpopup.h"
//constants


// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxCommandHandlerBack* 
			CGlxCommandHandlerBack::NewBackCommandHandlerL()
	{
	return CGlxCommandHandlerBack::NewL(EGlxCmdBack,NULL,0);
	}																	

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxCommandHandlerBack* 
			CGlxCommandHandlerBack::NewPreviousViewCommandHandlerL( 	
			MGlxMediaListProvider* aMediaListProvider )
	{
	return CGlxCommandHandlerBack::NewL(EGlxCmdPreviousView,aMediaListProvider,0);
	}

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxCommandHandlerBack* 
			CGlxCommandHandlerBack::NewContainerPreviousViewCommandHandlerL( 	
			MGlxMediaListProvider* aMediaListProvider, TInt aPrevViewId )
	{
	return CGlxCommandHandlerBack::NewL(EGlxCmdContainerPreviousView,
			aMediaListProvider,aPrevViewId);
	}																	

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CGlxCommandHandlerBack* 
			CGlxCommandHandlerBack::NewL( 
			TInt aCommandId, MGlxMediaListProvider* aMediaListProvider,
			TInt aPrevViewId )
	{
	CGlxCommandHandlerBack* self = new (ELeave)
	    CGlxCommandHandlerBack(aMediaListProvider,aPrevViewId);
	CleanupStack::PushL(self);
	self->ConstructL(aCommandId);
	CleanupStack::Pop(self);
	return self;
	}

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//	
CGlxCommandHandlerBack::CGlxCommandHandlerBack( MGlxMediaListProvider*
    aMediaListProvider, TInt aPrevViewId )
	{
	iMediaListProvider = aMediaListProvider;
	iPrevViewId = aPrevViewId;
	}			

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//	
void CGlxCommandHandlerBack::ConstructL( TInt aCommandId )
	{
    TRACER( "CGlxCommandHandlerBack::ConstructL" );
    
    GLX_LOG_INFO1( "CGlxCommandHandlerBack::ConstructL::Command Id (%d)",
        aCommandId );
    iCurrentCommandId = aCommandId;
    iNavigationalState =  CGlxNavigationalState::InstanceL();
    iIsViewActivated = EFalse;
  
    iAppUi = static_cast< CAknAppUi* >( CCoeEnv::Static()->AppUi() );
	}	
	
// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxCommandHandlerBack::~CGlxCommandHandlerBack()
	{
	TRACER( "CGlxCommandHandlerBack::~CGlxCommandHandlerBack" );
	
	iNavigationalState->Close();
	}

// -----------------------------------------------------------------------------
// ExecuteL
// -----------------------------------------------------------------------------
//	
TBool CGlxCommandHandlerBack::ExecuteL( TInt aCommandId )
	{
    TRACER("CGlxCommandHandlerBack::ExecuteL");
	TBool consume(EFalse);

	GLX_LOG_INFO1( "CGlxCommandHandlerBack::ExecuteL::Command Id (%d)",
	    aCommandId );
	if(EAknSoftkeyClose == aCommandId)
		{
        GLX_LOG_INFO("CGlxCommandHandlerBack::ExecuteL::go to \
	                    root in UI hierarchy");
        CMPXCollectionPath* newState = CMPXCollectionPath::NewL();
        CleanupStack::PushL( newState );
        iNavigationalState->NavigateToL( *newState );
        CleanupStack::PopAndDestroy( newState );
		consume = ETrue;
		}   
	else if ( aCommandId == EAknSoftkeyBack )
    	{
    	switch( iCurrentCommandId )
    		{
    		case EGlxCmdBack:
    			{
    			GLX_LOG_INFO("CGlxCommandHandlerBack::ExecuteL::One step \
    			    back in UI hierarchy");
                MMPXCollectionUtility* collectionUtility = MMPXCollectionUtility::NewL(NULL, KMcModeDefault);
                CleanupClosePushL(*collectionUtility);
                CMPXCollectionPath* navigationalState = collectionUtility->Collection().PathL();
                CleanupStack::PopAndDestroy(collectionUtility);
                CleanupStack::PushL(navigationalState);

                // let Back Stepping Service handle the event
                if((2 == navigationalState->Levels()||
                		iNavigationalState->StartingLevel() == navigationalState->Levels()-1)
                		&&  iNavigationalState->BackExitStatus())
                    {
                    if(iAppUi)
                        {
                        iAppUi->ProcessCommandL(EAknSoftkeyExit);    
                        }
                    }
                else if( iIsViewActivated )
                    {
                    iIsViewActivated = EFalse;
                    iNavigationalState->NavigateToParentL();  
                    }
                CleanupStack::PopAndDestroy(navigationalState); 
    			consume = ETrue;
    			break;
    			}
    		case EGlxCmdPreviousView:
    		    {
    		    GLX_LOG_INFO("CGlxCommandHandlerBack::ExecuteL::Go back \
    		    to the previous view");
    		    CMPXCollectionPath* path = iNavigationalState->StateLC();
    		    if (path->Id() == TMPXItemId(KGlxCollectionPluginImageViewerImplementationUid))
    		        {
    		        if(iAppUi)
    		            {
    		            iAppUi->ProcessCommandL(EAknSoftkeyExit);    
    		            }
    		        }
    		    else
    		        {
    		        if( iIsViewActivated )
    		            {
    		            iIsViewActivated = EFalse;
    		            iNavigationalState->ActivatePreviousViewL();
    		            }
    		        }
    		    CleanupStack::PopAndDestroy( path );
    		    consume = ETrue;
    		    break;
    		    }
    			
    		case EGlxCmdContainerPreviousView:
    			{
    			GLX_LOG_INFO("CGlxCommandHandlerBack::ExecuteL::Go back \
    			    to the previous view when continer is still there else go two views back");
                // The code commented is to be removed after testing with migrated code is over
                // below code is no longer  expected to be required  as the 
                // "deletion of tags" from tag manager test case dont exist in rel 8.0
                // the private API CheckContainerDeletion()can also to be removed 
                
    		/*	// check if special case of container deletion need to be checked
    			// this must be done before the previous view is activated as the check
    			// is based on the view id
    			TBool containerDeleted = CheckContainerDeletion();
 
    			//go back one view

                 if ( iViewUtility->ViewHistoryDepth() > 1 )
                     {
                     iViewUtility->ActivatePreviousViewL();
                     }

    			if( containerDeleted )
    				{
    				//path to get list of containers
    				CMPXCollectionPath* containerPath = iMediaListProvider->MediaList().PathLC();
                    //current navigational state
                    CMPXCollectionPath* navigationalState = iCollectionUtility->Collection().PathL();
                    CleanupStack::PushL(navigationalState);
   				
    				//current node id in UI Hierarchy
    				TMPXItemId currentNodeId = navigationalState->Id(navigationalState->Levels() - 2);

    				//check if current node is present in containers list
    				if(KErrNotFound == containerPath->IndexOfId(currentNodeId))
    					{
    					iCollectionUtility->Collection().BackL();
    					}
                    CleanupStack::PopAndDestroy(navigationalState);
    				CleanupStack::PopAndDestroy(containerPath);
    				}*/
    			consume = ETrue;
    			break;
    			}
    		default:
    		    User::Leave( KErrNotFound );
    		    break;
    		}
    	}
		
	return consume;
	}

//The below commented code to be removed 
// -----------------------------------------------------------------------------
// CheckContainerDeletion
// -----------------------------------------------------------------------------
//
/*TBool CGlxCommandHandlerBack::CheckContainerDeletion() const
	{
	return((iViewUtility->PreviousViewType().operator != (TUid::Uid(0))) && 
		(iViewUtility->PreviousViewType().operator ==(TUid::Uid(iPrevViewId))));
	}*/
// -----------------------------------------------------------------------------
// DynInitMenuPaneL
// -----------------------------------------------------------------------------
//
void CGlxCommandHandlerBack::DynInitMenuPaneL(TInt /*aResourceId*/, 
												CEikMenuPane* /*aMenuPane*/)
	{
	// Do Nothing		
	}

// -----------------------------------------------------------------------------
// DoActivateL
// -----------------------------------------------------------------------------
//
void CGlxCommandHandlerBack::DoActivateL(TInt /*aViewId*/)
	{
	iIsViewActivated = ETrue;
	}

// -----------------------------------------------------------------------------
// Deactivate
// -----------------------------------------------------------------------------
//
void CGlxCommandHandlerBack::Deactivate()
	{
	iIsViewActivated = EFalse;
	}

// -----------------------------------------------------------------------------
// OfferKeyEventL
// -----------------------------------------------------------------------------
//
TKeyResponse CGlxCommandHandlerBack::OfferKeyEventL(const TKeyEvent& /*aKeyEvent*/, 
														TEventCode /*aType*/)
	{
	return EKeyWasNotConsumed;		
	}

// -----------------------------------------------------------------------------
// PreDynInitMenuPaneL
// -----------------------------------------------------------------------------
//
void CGlxCommandHandlerBack::PreDynInitMenuPaneL( TInt /*aResourceId*/ )
	{
	// Do Nothing		
	}
	
	
//End of file
