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




#include "glxcommandhandleraiwedit.h"

#include <glxattributecontext.h>

#include <glxcommandhandlers.hrh>
#include <glxmedia.h>
#include <glxuiutilities.rsg>
#include <glximageviewermanager.h>
#include <glxnavigationalstate.h>
#include <mpxcollectionpath.h>
#include <glxcollectionpluginimageviewer.hrh>

#include "glxaiwservicehandler.h"
#include <StringLoader.h>
#include <glxmedialist.h> 
#include <glxtracer.h>
#include <glxlog.h>
#include <AiwGenericParam.h>                // for passing data between applications
#include "AiwServiceHandler.h"                  // AIW service handler
#include "glxmedia.h"
#include <featdiscovery.h>
// -----------------------------------------------------------------------------
// NewL
// -----------------------------------------------------------------------------
//	
EXPORT_C CGlxCommandHandlerAiwEdit* CGlxCommandHandlerAiwEdit::NewL(
        MGlxMediaListProvider* aMediaListProvider)
    {
    CGlxCommandHandlerAiwEdit* self = new (ELeave) CGlxCommandHandlerAiwEdit(
            aMediaListProvider);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// ~CGlxCommandHandlerAiwEdit
// -----------------------------------------------------------------------------
//
EXPORT_C CGlxCommandHandlerAiwEdit::~CGlxCommandHandlerAiwEdit()
    {
    delete iServiceHandler;

    if (NULL != iImageViewerInstance)
        {
        iImageViewerInstance->DeleteInstance();
        }
    }

// -----------------------------------------------------------------------------
// ConstructL
// -----------------------------------------------------------------------------
//
void CGlxCommandHandlerAiwEdit::ConstructL()
    {
    CFeatureDiscovery* featManager = CFeatureDiscovery::NewL();
    CleanupStack::PushL(featManager);
    
    if(featManager->IsFeatureSupportedL(KFeatureIdFfImageEditor))
        {
        iIsImageEditSupported = ETrue;
        }
    
    if(featManager->IsFeatureSupportedL(KFeatureIdFfVideoEditor))
        {
        iIsVideoEditSupported = ETrue;
        }
      
    if(iIsVideoEditSupported || iIsImageEditSupported)
        {
        TRAP_IGNORE(
                    {
                    iServiceHandler = CAiwServiceHandler::NewL();
                    iServiceHandler->AttachL( R_GLX_AIW_EDIT_INTEREST );
                    });
        }
    CleanupStack::PopAndDestroy(featManager);

    TCommandInfo info(EGlxCmdAiwEdit);
    AddCommandL(info);

    iImageViewerInstance = CGlxImageViewerManager::InstanceL();   
    
    iIsFullScreenMode = IsInFullScreenViewingModeL();
    }

// -----------------------------------------------------------------------------
// CGlxCommandHandlerAiwEdit
// -----------------------------------------------------------------------------
//
CGlxCommandHandlerAiwEdit::CGlxCommandHandlerAiwEdit(
        MGlxMediaListProvider* aMediaListProvider) :
    CGlxMediaListCommandHandler(aMediaListProvider)
    {
    // Do Nothing
    }

// -----------------------------------------------------------------------------
// DoActivateL
// -----------------------------------------------------------------------------
//
void CGlxCommandHandlerAiwEdit::DoActivateL(TInt /*aViewId*/)
    {
    // DO Nothing
    }

// -----------------------------------------------------------------------------
// DynInitMenuPaneL
// -----------------------------------------------------------------------------
//
void CGlxCommandHandlerAiwEdit::DynInitMenuPaneL(TInt /*aResourceId*/,
        CEikMenuPane* aMenuPane, TBool /*aIsBrowseMode*/)
    {

    if (aMenuPane)
        {
        MGlxMediaList& mediaList = MediaList();
        TInt pos = 0;
        
        TBool editSupported = EFalse;
        TGlxSelectionIterator iterator;
        iterator.SetToFirst(&mediaList);
        TInt index = iterator++;
        if ( index != KErrNotFound )
            {
            TMPXGeneralCategory cat = mediaList.Item(index).Category();
            
            if ( cat == EMPXImage && iIsImageEditSupported)
                {
                editSupported = ETrue;
                }
            else if ( cat == EMPXVideo && iIsVideoEditSupported)
                {
                editSupported = ETrue;
                }
            }
        if (aMenuPane->MenuItemExists(EGlxCmdAiwEdit, pos))
            {
            // If the image path is private or view is in grid & 
            // selection is greater than 1, we should hide Edit menu item 
            if (iImageViewerInstance->IsPrivate()
                    || ((!mediaList.Count() || (mediaList.SelectionCount() > 1))
                    || !editSupported))
                {
                aMenuPane->SetItemDimmed(EGlxCmdAiwEdit, ETrue);
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// DoExecuteL
// -----------------------------------------------------------------------------
//
TBool CGlxCommandHandlerAiwEdit::DoExecuteL(TInt aCommandId,
        MGlxMediaList& aList)
    {
    TBool handled = EFalse;
    if ((iIsImageEditSupported || iIsVideoEditSupported)
                    && (EGlxCmdAiwEdit == aCommandId))
        {
        CAiwGenericParamList& inputParams = iServiceHandler->InParamListL();

        TGlxSelectionIterator iterator;
        iterator.SetToFirst(&aList);
        TInt index = iterator++;
        const TGlxMedia& mediaItem = aList.Item(index);

        TAiwGenericParam param(EGenericParamFile,
                TAiwVariant(mediaItem.Uri()));
        inputParams.AppendL(param);

        TAiwGenericParam param2(EGenericParamMIMEType, TAiwVariant(
                mediaItem.MimeType()));
        inputParams.AppendL(param2);

        // Execute the KAiwCmdUpload command  EGlxCmdAiwEdit
        iServiceHandler->ExecuteServiceCmdL(KAiwCmdEdit, inputParams,
                iServiceHandler->OutParamListL());

        handled = ETrue;
        }
    return handled;
    }

// -----------------------------------------------------------------------------
// IsInFullScreenViewingModeL
// -----------------------------------------------------------------------------
//
TBool CGlxCommandHandlerAiwEdit::IsInFullScreenViewingModeL()
    {
    TRACER("CGlxCommandHandlerAiwEdit::IsInFullScreenViewingModeL()");
    TBool fullscreenViewingMode = EFalse;
    CGlxNavigationalState* aNavigationalState =
            CGlxNavigationalState::InstanceL();
    CMPXCollectionPath* naviState = aNavigationalState->StateLC();

    if (naviState->Levels() >= 1)
        {
        if (aNavigationalState->ViewingMode()
                == NGlxNavigationalState::EBrowse)
            {
            // For image viewer collection, goto view mode
            if (naviState->Id() == TMPXItemId(
                    KGlxCollectionPluginImageViewerImplementationUid))
                {
                //it means we are in img viewer
                fullscreenViewingMode = ETrue;
                }
            }
        else
            {
            //it means we are in Fullscreen
            fullscreenViewingMode = ETrue;
            }
        }
    CleanupStack::PopAndDestroy(naviState);
    aNavigationalState->Close();
    return fullscreenViewingMode;
    }
