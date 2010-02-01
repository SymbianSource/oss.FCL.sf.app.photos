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

const TInt KGlxAiwEditCommandSpace = 0x00000200;

// -----------------------------------------------------------------------------
// NewL
// -----------------------------------------------------------------------------
//	
EXPORT_C CGlxCommandHandlerAiwEdit* CGlxCommandHandlerAiwEdit::NewL(MGlxMediaListProvider* aMediaListProvider, TBool aCommandSingleClick)
    {
    CGlxCommandHandlerAiwEdit* self = new (ELeave) CGlxCommandHandlerAiwEdit(aMediaListProvider, aCommandSingleClick);
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
    }

// -----------------------------------------------------------------------------
// ConstructL
// -----------------------------------------------------------------------------
//
void CGlxCommandHandlerAiwEdit::ConstructL()
    {
    TRAP_IGNORE(
            {
            iServiceHandler = CAiwServiceHandler::NewL();
            iServiceHandler->AttachL( R_GLX_AIW_EDIT_INTEREST );
            iEditSupported = ETrue;
            }  );
    
    if(iCommandSingleClick)
        {
        TCommandInfo info(EGlxCmdAiwSingleClickEdit);
        AddCommandL(info);
        }
    else
        {
        TCommandInfo info(EGlxCmdAiwEdit);
        AddCommandL(info);
        }
    }

// -----------------------------------------------------------------------------
// CGlxCommandHandlerAiwEdit
// -----------------------------------------------------------------------------
//
CGlxCommandHandlerAiwEdit::CGlxCommandHandlerAiwEdit( MGlxMediaListProvider* aMediaListProvider, TBool aCommandSingleClick )
                                :CGlxMediaListCommandHandler(aMediaListProvider),
                                 iCommandSingleClick(aCommandSingleClick)
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
void CGlxCommandHandlerAiwEdit::DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane)
    {
    MGlxMediaList& mediaList = MediaList();
    TInt pos;
    
    if (aMenuPane && iCommandSingleClick && aMenuPane->MenuItemExists(EGlxCmdAiwSingleClickEdit, pos)
            && (mediaList.SelectionCount() > 1))
        {
        aMenuPane->SetItemDimmed(EGlxCmdAiwSingleClickEdit, ETrue);
        }
    else if (aMenuPane && aMenuPane->MenuItemExists(EGlxCmdAiwEdit, pos) 
            && (mediaList.SelectionCount() != 1) && !IsInFullScreenViewingModeL())
        {
        aMenuPane->SetItemDimmed(EGlxCmdAiwEdit, ETrue);
        }

    }

// -----------------------------------------------------------------------------
// DoExecuteL
// -----------------------------------------------------------------------------
//
TBool CGlxCommandHandlerAiwEdit::DoExecuteL( TInt aCommandId , MGlxMediaList& aList)
    {
    TBool handled = EFalse;
    if (iEditSupported && (EGlxCmdAiwEdit == aCommandId || EGlxCmdAiwSingleClickEdit == aCommandId))
        { 
        CAiwGenericParamList& inputParams = iServiceHandler->InParamListL();
        
        TGlxSelectionIterator iterator;
        iterator.SetToFirst(&aList);
        TInt index = iterator++;
        const TGlxMedia& mediaItem = aList.Item(index);
        
        TAiwGenericParam param( EGenericParamFile, TAiwVariant(mediaItem.Uri()));
        inputParams.AppendL( param );
        
        TAiwGenericParam param2( EGenericParamMIMEType, TAiwVariant(mediaItem.MimeType()));
        inputParams.AppendL( param2 );

        // Execute the KAiwCmdUpload command  EGlxCmdAiwEdit
        iServiceHandler->ExecuteServiceCmdL(KAiwCmdEdit, inputParams, iServiceHandler->OutParamListL());       

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
    CGlxNavigationalState* aNavigationalState = CGlxNavigationalState::InstanceL();
    CMPXCollectionPath* naviState = aNavigationalState->StateLC();
    
    if ( naviState->Levels() >= 1)
        {
        if (aNavigationalState->ViewingMode() == NGlxNavigationalState::EBrowse) 
            {
            // For image viewer collection, goto view mode
            if (naviState->Id() == TMPXItemId(KGlxCollectionPluginImageViewerImplementationUid))
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
    CleanupStack::PopAndDestroy( naviState );
    aNavigationalState->Close();
    return fullscreenViewingMode;
    }
