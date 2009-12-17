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
* Description:    commandhandler more info online
*
*/




/**
 * @internal reviewed 03/07/2007 by Rowland Cook
 */

#include "glxcommandhandlermoreinfo.h"

#include <mpxmediadrmdefs.h>
#include <mpxmediageneraldefs.h>

#include <glxattributecontext.h>
#include <glxattributeretriever.h>
#include <glxcommandhandlers.hrh>
#include <glxuistd.h>
#include <glxdrmutility.h>
#include <mglxmedialist.h>

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxCommandHandlerMoreInfo* CGlxCommandHandlerMoreInfo::NewL(
        MGlxMediaListProvider* aMediaListProvider,
        TBool aContainerList)
    {
    CGlxCommandHandlerMoreInfo* self = new ( ELeave )
        CGlxCommandHandlerMoreInfo(aMediaListProvider);
    CleanupStack::PushL( self );
    self->ConstructL(aContainerList);
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CGlxCommandHandlerMoreInfo::CGlxCommandHandlerMoreInfo(MGlxMediaListProvider*
                                                            aMediaListProvider)
        : CGlxMediaListCommandHandler(aMediaListProvider)
    {
    // Do nothing
    }
 
// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CGlxCommandHandlerMoreInfo::ConstructL( TBool aIsContainerList )
    {
    iIsContainerList = aIsContainerList;


    iDrmUtility = CGlxDRMUtility::InstanceL();
    // Add supported commands
    
    TCommandInfo info( EGlxCmdDrmMoreInfoOnline );
    // Filter out static items
    info.iMinSelectionLength = 1;
    info.iDisallowSystemItems = aIsContainerList;
    info.iCategoryFilter = EMPXCommand;
    info.iCategoryRule = TCommandInfo::EForbidAll; 
    AddCommandL( info );
    
    // create fetch context 
    iFetchContext = CGlxDefaultAttributeContext::NewL();
    iFetchContext->AddAttributeL( KMPXMediaDrmProtected );
    iFetchContext->AddAttributeL( KMPXMediaGeneralCategory );
    iFetchContext->AddAttributeL( KMPXMediaGeneralUri );
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxCommandHandlerMoreInfo::~CGlxCommandHandlerMoreInfo()
    {  
    if(iDrmUtility)
        {
        iDrmUtility->Close();
        }
    
    delete iFetchContext;
    delete iUrl;
    }

// ---------------------------------------------------------------------------
// DoActivateL
// ---------------------------------------------------------------------------
//    
void CGlxCommandHandlerMoreInfo::DoActivateL(TInt /*aViewId*/)
    {
    MGlxMediaList& mlist = MediaList();
    mlist.AddContextL(iFetchContext, KGlxFetchContextPriorityNormal);
    }

// ---------------------------------------------------------------------------
// Deactivate
// ---------------------------------------------------------------------------
//  
void CGlxCommandHandlerMoreInfo::Deactivate()
    {
    MGlxMediaList& mlist = MediaList();
    mlist.RemoveContext(iFetchContext);
    }

// -----------------------------------------------------------------------------
// DoExecute - the relevant action for the command id
// -----------------------------------------------------------------------------
//
TBool CGlxCommandHandlerMoreInfo::DoExecuteL(TInt aCommandId,
    MGlxMediaList& /*aList*/)
    {
    TBool handledCommand = ETrue;
    
    switch (aCommandId)
        {
        case EGlxCmdDrmMoreInfoOnline:
            {
            // assume we have a valid URI otherwise
            // this command will be disabled
            if(iUrl && iDrmUtility)
                {
                iDrmUtility->ShowInfoOnlineL(*iUrl);
                }
            break;
            }
        default:
            {
            handledCommand = EFalse;
            break;
            }
        }
        
    return handledCommand;
    }


// -----------------------------------------------------------------------------
// DoIsDisabled
// -----------------------------------------------------------------------------
//
TBool CGlxCommandHandlerMoreInfo::DoIsDisabled(TInt aCommandId, 
                                        MGlxMediaList& aList) const 
    {
    TBool hasInfoURL = EFalse;
    
    if(EGlxCmdDrmMoreInfoOnline==aCommandId && 0 != aList.Count() )
        {
        TRAP_IGNORE(hasInfoURL = CanGetInfoURLL(aList));
        }
    
    return !hasInfoURL;
    }

// -----------------------------------------------------------------------------
// CanGetInfoURLL - Is info URL available
// -----------------------------------------------------------------------------
//    
TBool CGlxCommandHandlerMoreInfo::CanGetInfoURLL(MGlxMediaList& aList) const 
    {
    TBool hasInfoURL = EFalse;

    TInt index = aList.FocusIndex();
    if ( index > KErrNotFound )
        {
        const TGlxMedia& item = aList.Item( index );
        TBool isDRMProtected = EFalse;
        if( item.GetDrmProtected( isDRMProtected ) && isDRMProtected )            
            {
            TMPXGeneralCategory  cat = item.Category();
            const TDesC& uri = item.Uri();
            if( EMPXImage == cat && uri.Length() > 0 ) 
                {           
                delete iUrl;
                iUrl = NULL;
                iUrl = uri.AllocL();
            
                // pass URI to DRM utility
                hasInfoURL = iDrmUtility->CanShowInfoOnlineL( *iUrl );
                }
            }
        }
    return hasInfoURL;
    }
