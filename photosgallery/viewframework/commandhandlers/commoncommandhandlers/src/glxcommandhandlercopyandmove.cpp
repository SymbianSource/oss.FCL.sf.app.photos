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
* Description:    Add to container commmand handler
*
*/




#include <AknCommonDialogsDynMem.h>
#include <CAknCommonDialogsBase.h>
#include <CAknMemorySelectionDialogMultiDrive.h>
#include <glxattributecontext.h>
#include <glxattributeretriever.h>
#include <glxcollectiongeneraldefs.h>
#include <glxcommandfactory.h>
#include <glxcommandhandlers.hrh>
#include <glxresourceutilities.h>                // for CGlxResourceUtilities
#include <glxtextentrypopup.h>
#include <glxuistd.h>
#include <glxuiutilities.rsg>
#include <mglxmedialist.h>
#include <mglxmedialist.h>
#include <mpxcollectionpath.h>
#include <mpxcommonframeworkdefs.h>

#include <data_caging_path_literals.hrh>

#include "glxcommandhandlercopyandmove.h"

_LIT(KColonBackslash, ":\\");

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxCommandHandlerCopyAndMove* CGlxCommandHandlerCopyAndMove::NewL(
		MGlxMediaListProvider* aMediaListProvider, TInt aMenuResource,
		const TDesC& aFileName)
	{
	CGlxCommandHandlerCopyAndMove* self =
			new (ELeave) CGlxCommandHandlerCopyAndMove(aMediaListProvider,
					aMenuResource);
	CleanupStack::PushL(self);
	self->ConstructL(aFileName);
	CleanupStack::Pop(self);
	return self;
	}

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxCommandHandlerCopyAndMove::~CGlxCommandHandlerCopyAndMove()
    {
    if (iResourceOffset)
        {
        CCoeEnv::Static()->DeleteResourceFile(iResourceOffset);
        }
    }

// ---------------------------------------------------------------------------
// CGlxCommandHandlerCopyAndMove::CreateCommandL()
// ---------------------------------------------------------------------------
//
CMPXCommand* CGlxCommandHandlerCopyAndMove::CreateCommandL(TInt aCommandId, MGlxMediaList& aList,
        TBool& aConsume) const
    {
    CMPXCommand* command = NULL;
    switch (aCommandId)
    {
    case EGlxCmdCopy:
    case EGlxCmdMove:
        {
        TCommonDialogType type = 
            aCommandId == EGlxCmdMove ? ECFDDialogTypeMove : ECFDDialogTypeCopy;
       
        CAknMemorySelectionDialogMultiDrive* dialog = 
                        CAknMemorySelectionDialogMultiDrive::NewL
                        ( type, NULL, EFalse, AknCommonDialogsDynMem::EMemoryTypePhone|
                            AknCommonDialogsDynMem::EMemoryTypeMMC|
                            AknCommonDialogsDynMem::EMemoryTypeInternalMassStorage|
                            AknCommonDialogsDynMem::EMemoryTypeMMCExternal );
        
        CleanupStack::PushL(dialog);
        TDriveNumber driveNumber  = EDriveC;
      
        if (dialog->ExecuteL(driveNumber))
            {
            TBuf<KMaxFileName> rootPath;
            DriveLetterFromNumber(driveNumber, rootPath);

            CMPXCollectionPath* path = aList.PathLC( NGlxListDefs::EPathFocusOrSelection );
            if (aCommandId == EGlxCmdMove)
                  {
                  command = TGlxCommandFactory::MoveCommandLC(rootPath, *path);
                  }   
              else
                  {
                  command = TGlxCommandFactory::CopyCommandLC(rootPath, *path);
                  }
                
            CleanupStack::Pop(command);
            CleanupStack::PopAndDestroy(path);
            }
        CleanupStack::PopAndDestroy(dialog);
        }
    break;
    case EGlxCmdStateBrowse:
    case EGlxCmdStateView:
        {
        iVisible = (aCommandId == EGlxCmdStateBrowse);
        aConsume = EFalse;
        }
        break;
    case EGlxCmdCopyMoveSubmenu:
        // nothing to do here
    break;
    default:
        break;
    }
    
    return command;
    
    }
    
// ---------------------------------------------------------------------------
// CGlxCommandHandlerCopyAndMove::DoDynInitMenuPaneL()
// ---------------------------------------------------------------------------
//
void CGlxCommandHandlerCopyAndMove::DoDynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane)
    {
    if (aResourceId == iMenuResource && aMenuPane)
        {
        aMenuPane->SetItemDimmed(EGlxCmdCopyMoveSubmenu, !iVisible);
        }
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CGlxCommandHandlerCopyAndMove::ConstructL(const TDesC& aFileName)
	{
	// Load resource file

	iResourceOffset = CCoeEnv::Static()->AddResourceFileL(aFileName);

	// Add supported command
	TCommandInfo copyInfo(EGlxCmdCopy);
	copyInfo.iMinSelectionLength = 1;
	copyInfo.iMaxSelectionLength = KMaxTInt;
	// Filter out static items
	AddCommandL(copyInfo);

	TCommandInfo moveInfo(EGlxCmdMove);
	moveInfo.iMinSelectionLength = 1;
	moveInfo.iMaxSelectionLength = KMaxTInt;
	// Filter out static items
	AddCommandL(moveInfo);

	TCommandInfo copyAndMoveSubmenuInfo(EGlxCmdCopyMoveSubmenu);
	copyAndMoveSubmenuInfo.iMinSelectionLength = 1;
	copyAndMoveSubmenuInfo.iMaxSelectionLength = KMaxTInt;
	// Show in Grid View
	copyAndMoveSubmenuInfo.iViewingState = TCommandInfo::EViewingStateBrowse;
	// Filter out static items 
	// (The whole sub menu should be filtered out if a static item is selected)
	AddCommandL(copyAndMoveSubmenuInfo);

	TCommandInfo browseInfo(EGlxCmdStateBrowse);
	browseInfo.iMinSelectionLength = 0;
	browseInfo.iMaxSelectionLength = KMaxTInt;
	// Filter out static items
	AddCommandL(browseInfo);

	TCommandInfo viewInfo(EGlxCmdStateView);
	viewInfo.iMinSelectionLength = 0;
	viewInfo.iMaxSelectionLength = KMaxTInt;
	// Filter out static items
	AddCommandL(viewInfo);
	}

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CGlxCommandHandlerCopyAndMove::CGlxCommandHandlerCopyAndMove(MGlxMediaListProvider* aMediaListProvider, TInt aMenuResource)
: CGlxMpxCommandCommandHandler(aMediaListProvider), iVisible(ETrue), iMenuResource(aMenuResource)
    {
    }

// ---------------------------------------------------------------------------
// CGlxCommandHandlerCopyAndMove::DriveLetterFromNumber()
// ---------------------------------------------------------------------------
//
void CGlxCommandHandlerCopyAndMove::DriveLetterFromNumber(TDriveNumber aDriveNumber, TDes& aDriveLetter)
	{
    aDriveLetter.SetLength(aDriveLetter.Length() + 1);
    aDriveLetter[aDriveLetter.Length() - 1] = 'A' + aDriveNumber;
    aDriveLetter.Append(KColonBackslash);
	}
