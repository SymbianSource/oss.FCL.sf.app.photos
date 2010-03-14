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
* Description:    Temporary rotate command implementation
*
*/




// INCLUDE FILES

#include "glxcommandhandlerrotate.h"

#include <data_caging_path_literals.hrh>
#include <alf/alfdisplay.h>

#include <glxattributecontext.h>
#include <glxuiutility.h>
#include <glxmediageneraldefs.h>
#include <glxsettingsmodel.h>
#include <glxthumbnailcontext.h>
#include <glxuistd.h>
#include <glxvisuallistmanager.h>
#include <mglxmedialist.h>
#include <mglxvisuallist.h>
#include <glxlog.h>
#include <glxtracer.h>
#include <mglxlayoutowner.h>

#include <glxcommandhandlers.hrh>
#include <glxuiutilities.rsg>
#include <glxicons.mbg>

#include    <ExifRead.h>
#include    <glxcommandfactory.h>
#include    <mpxcommandgeneraldefs.h>                   // Content ID identifying general category of content provided
#include    <glxtexturemanager.h>

#include<bautils.h>

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxCommandHandlerRotate*
            CGlxCommandHandlerRotate::NewL(
                                        MGlxMediaListProvider* aMediaListProvider,
                                        MGlxLayoutOwner* aLayoutOwner,
                                        TInt aLayoutIndex)
    {
    TRACER("CGlxCommandHandlerRotate* CGlxCommandHandlerRotate::NewL()");
    CGlxCommandHandlerRotate* self
            = new (ELeave) CGlxCommandHandlerRotate(aMediaListProvider, 
                aLayoutOwner, aLayoutIndex);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CGlxCommandHandlerRotate::~CGlxCommandHandlerRotate()
    {
    TRACER("CGlxCommandHandlerRotate::~CGlxCommandHandlerRotate");
    if ( iVisualListManager )
        {
        iVisualListManager->Close();
        }

    if ( iUiUtility )
        {
        iUiUtility->Close();
        }
    iFs.Close();
       
    if ( iExifData )
        {
        delete iExifData;
        iExifData = NULL;
        }
    if ( iExifWriter )
        {
        delete iExifWriter;
        iExifWriter = NULL;
        }
    }

// ---------------------------------------------------------------------------
// DoExecuteL
// ---------------------------------------------------------------------------
//
TBool CGlxCommandHandlerRotate::DoExecuteL(TInt aCommandId,
                                                    MGlxMediaList& aList)
    {
    TRACER("CGlxCommandHandlerRotate::DoExecuteL");
    TBool handled = EFalse;
	TInt rotationAngle ;
	iMediaList = &aList;
	switch ( aCommandId )
        {
		case EGlxCmdRotateLeft:
			rotationAngle = -90;
			break;
		case EGlxCmdRotateRight:
			rotationAngle = 90;
			break;
		default:
            return handled;
		}
	DoRotateL(aList, rotationAngle);
    
    return handled;
    }

// ---------------------------------------------------------------------------
// DoActivateL
// ---------------------------------------------------------------------------
//
void CGlxCommandHandlerRotate::DoActivateL(TInt aViewId)
    {
    TRACER("CGlxCommandHandlerRotate::DoActivateL");
    iViewId = aViewId;
    }

// ---------------------------------------------------------------------------
// Deactivate
// ---------------------------------------------------------------------------
//
void CGlxCommandHandlerRotate::Deactivate()
    {
    TRACER("CGlxCommandHandlerRotate::Deactivate");
    
    iViewId = 0;
    }




// ---------------------------------------------------------------------------
// HandleItemAddedL
// ---------------------------------------------------------------------------
//
void CGlxCommandHandlerRotate::HandleItemAddedL(TInt /*aStartIndex*/,
            TInt /*aEndIndex*/, MGlxMediaList* /*aList*/)
    {
    }

// ---------------------------------------------------------------------------
// HandleMediaL
// ---------------------------------------------------------------------------
//
void CGlxCommandHandlerRotate::HandleMediaL(TInt /*aListIndex*/,
            MGlxMediaList* /*aList*/)
    {
    }

// ---------------------------------------------------------------------------
// HandleItemRemovedL
// ---------------------------------------------------------------------------
//
void CGlxCommandHandlerRotate::HandleItemRemovedL(TInt /*aStartIndex*/,
            TInt /*aEndIndex*/, MGlxMediaList* /*aList*/)
    {
    }

// ---------------------------------------------------------------------------
// HandleItemModifiedL
// ---------------------------------------------------------------------------
//
void CGlxCommandHandlerRotate::HandleItemModifiedL(
            const RArray<TInt>& /*aItemIndexes*/, MGlxMediaList* /*aList*/)
    {
    }

// ---------------------------------------------------------------------------
// HandleAttributesAvailableL
// ---------------------------------------------------------------------------
//
void CGlxCommandHandlerRotate::HandleAttributesAvailableL(
            TInt /*aItemIndex*/, const RArray<TMPXAttribute>& /*aAttributes*/,
            MGlxMediaList* /*aList*/)
    {
    TRACER("CGlxCommandHandlerRotate::HandleAttributesAvailableL");
        
    }

// ---------------------------------------------------------------------------
// HandleFocusChangedL
// ---------------------------------------------------------------------------
//
void CGlxCommandHandlerRotate::HandleFocusChangedL(
            NGlxListDefs::TFocusChangeType /*aType*/, TInt /*aNewIndex*/, 
            TInt /*aOldIndex*/, MGlxMediaList* /*aList*/)
    {
    TRACER("CGlxCommandHandlerRotate::HandleFocusChangedL");
    //Not required as we are doing this after each rotation DoPreserveOrientationL();
    DoCleanupRotationParametersL();
    
    iMediaList = NULL;
    }

// ---------------------------------------------------------------------------
// HandleItemSelectedL
// ---------------------------------------------------------------------------
//
void CGlxCommandHandlerRotate::HandleItemSelectedL(TInt /*aIndex*/,
            TBool /*aSelected*/, MGlxMediaList* /*aList*/)
    {
    }

// ---------------------------------------------------------------------------
// HandleMessageL
// ---------------------------------------------------------------------------
//
void CGlxCommandHandlerRotate::HandleMessageL(
            const CMPXMessage& /*aMessage*/, MGlxMediaList* /*aList*/)
    {
    }

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code that might leave.
// ---------------------------------------------------------------------------
//
CGlxCommandHandlerRotate::CGlxCommandHandlerRotate(
                                        MGlxMediaListProvider* aMediaListProvider,
                                        MGlxLayoutOwner* aLayoutOwner,
                                        TInt aLayoutIndex) :
        CGlxMediaListCommandHandler(aMediaListProvider),
        iLayoutOwner(aLayoutOwner),
        iLayoutIndex(aLayoutIndex)
    {
    TRACER("CGlxCommandHandlerRotate::CGlxCommandHandlerRotate");
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
// 
void CGlxCommandHandlerRotate::ConstructL()
    {
    TRACER("CGlxCommandHandlerRotate::ConstructL");
    iUiUtility = CGlxUiUtility::UtilityL();
      
    // Add rotate command for images only
   	TCommandInfo rotate(EGlxCmdRotate);
   	rotate.iCategoryFilter = EMPXImage;
   	rotate.iCategoryRule = TCommandInfo::ERequireAll;
    AddCommandL(rotate);
	TCommandInfo rotateLeft(EGlxCmdRotateLeft);
	rotateLeft.iCategoryFilter = EMPXImage;
   	rotateLeft.iCategoryRule = TCommandInfo::ERequireAll;
    AddCommandL(rotateLeft);
	TCommandInfo rotateRight(EGlxCmdRotateRight);
	rotateLeft.iCategoryFilter = EMPXImage;
   	rotateLeft.iCategoryRule = TCommandInfo::ERequireAll;
    AddCommandL(rotateRight);

    // Add view state dummy commands
   	TCommandInfo view(EGlxCmdStateView);
    AddCommandL(view);
   	TCommandInfo browse(EGlxCmdStateBrowse);
    AddCommandL(browse);
	
    //File system 
    User::LeaveIfError(iFs.Connect());
    iAddedObserver=EFalse;
    }


// ---------------------------------------------------------------------------
// DoIsDisabled
// ---------------------------------------------------------------------------
//
TBool CGlxCommandHandlerRotate::DoIsDisabled(TInt aCommandId, 
                                                MGlxMediaList& aList) const
    {
    TRACER("CGlxCommandHandlerRotate::DoIsDisabled");
    TBool disabled = EFalse;
    /**/
    if( ViewingState() == TCommandInfo::EViewingStateView )
        {
        return ETrue;
        }
    switch (aCommandId)
        {
        case EGlxCmdRotate:
		case EGlxCmdRotateLeft:
		case EGlxCmdRotateRight:
            {
            if((aList.Count()<=0) || aList.Item(aList.FocusIndex()).IsStatic())
                {
                disabled = ETrue;
                }
            break;
            }
      
        default:
            break;
        }
    
    return disabled;
    }
// ---------------------------------------------------------------------------
// DoRotateL
// ---------------------------------------------------------------------------
//
void CGlxCommandHandlerRotate::DoRotateL(MGlxMediaList& /*aList*/, TInt aAngle)
    {
    TRACER("CGlxCommandHandlerRotate::DoRotateL");
    const TGlxMedia& item = MediaList().Item( MediaList().FocusIndex() );	
    iGlxItem =  &item;
    GLX_LOG_INFO("CGlxCommandHandlerRotate::DoRotateL1");
    TSize imageSize(0,0);
    item.GetDimensions(imageSize);
    iRotationAngle += aAngle;
    if(aAngle<0)
        {
        if (iRotationAngle < 0)
            {
            iRotationAngle = 270;
            }
        }
    else
        {
        if (iRotationAngle > 270)
            {
            iRotationAngle = 0;
            }
        }
    iFileToBeRotated.Copy(item.Uri());
    DoInitializeRotationParametersL();
    if(iInitialOrientation > 4)
        {
        imageSize.SetSize(imageSize.iHeight,imageSize.iWidth);
        }
    iUiUtility->SetRotatedImageSize(imageSize);
    //Perform Rotation Here itself rather than Calling it each time 
    DoPreserveOrientationL();
    } 

// ---------------------------------------------------------------------------
// DoCalculateOrientationL
// ---------------------------------------------------------------------------
//
TInt CGlxCommandHandlerRotate::DoCalculateOrientationL(TInt aInitialOrientation)
    {
    TRACER("CGlxCommandHandlerRotate::DoCalculateOrientationL");
    /*
     * possible orientation state with angles for rotation
     * Possible Angles 0 - 90 - 180 - 270
     * Possible states 1 - 6 - 3 - 8 without a Flip
     * Possible states 2 - 5 - 4 - 7 when Flip is on
     */
    GLX_LOG_INFO("CGlxCommandHandlerRotate::DoCalculateOrientationL");
    TInt finalOrientation = aInitialOrientation;
    if(aInitialOrientation >8 || aInitialOrientation <0 )
        {
        return finalOrientation;
        }
    
    //Nitz subst code
    TInt rotOffset = iRotationAngle/90;
    TInt isOrientationOdd = aInitialOrientation % 2;
    TInt initStateIndex = 0;
    TInt finalStateIndex = 0;
    //Setting the orientation states for the initial unflipped orientation combinations
    TInt orientationStateArray[] = {1,6,3,8};
    //Seting the index for current orientation
    if(aInitialOrientation < 3)
        {
        initStateIndex = 0;
        }
    else if(aInitialOrientation >= 3 && aInitialOrientation < 5)
        {
        initStateIndex = 2;
        }
    else if(aInitialOrientation >= 5 && aInitialOrientation < 7)
        {
        initStateIndex = 1;
        }
    else if(aInitialOrientation >= 7 && aInitialOrientation <= 8)
        {
        initStateIndex = 3;
        }
    //Calculating the final orientation using the cyclic orientationStateArray. 
    //folding final index so that it behaves like a cyclic machine 
    finalStateIndex = (initStateIndex+rotOffset)%4;
    finalOrientation = orientationStateArray[finalStateIndex];
    //Checking if a Flip was present 
    if(aInitialOrientation>4 && isOrientationOdd )
        {
        finalOrientation -= 1;
        }
    if(aInitialOrientation<5 && !isOrientationOdd)
        {
        finalOrientation += 1;
        }
    GLX_LOG_INFO1("CGlxCommandHandlerRotate::DoCalculateOrientationL finalOrientation=%d",finalOrientation);
    return finalOrientation;
    }


// ---------------------------------------------------------------------------
// DoPreserveOrientationL
// ---------------------------------------------------------------------------
//
void CGlxCommandHandlerRotate::DoPreserveOrientationL()
    {
    TRACER("CGlxCommandHandlerRotate::DoPreserveOrientationL");
    TUint16 finalOrientation = DoCalculateOrientationL(iInitialOrientation);
    if(finalOrientation != iInitialOrientation && iDoAttemptToSaveFile)
        {
        iExifWriter->SetOrientationL(finalOrientation);
        HBufC8* ModifiedexifData=NULL;
        //need only first KGlxMaxExifSize bytes of data as exif cannot bypass this size 
        TRAPD(err,ModifiedexifData = iExifWriter->WriteDataL(iExifData->Des()));  
        //Nitz To-do Write only first KGlxMaxExifSize bytes to the file
        //It currently fails in cases of JPEG files without Exif so removing it. 
        //TPtrC8 exifPtr =  ModifiedexifData->Mid(0,KGlxMaxExifSize);
        if(err == KErrNone)
            {
            iFileHandle.Write(0,ModifiedexifData->Des());
            delete ModifiedexifData;
            }
        else
            {
            iDoAttemptToSaveFile = EFalse;
            }

        //Reinitializing some class variables
        iInitialOrientation = finalOrientation;
        iRotationAngle = 0;

        }
    iFileHandle.Close();
    iInitialOrientation = 9;
    iRotationAngle = 0;
    iRotationApplied = EFalse;
    if ( iExifData )
        {
        delete iExifData;
        iExifData = NULL;
        }
    if ( iExifWriter )
        {
        delete iExifWriter;
        iExifWriter = NULL;
        } 
    //This will be called when there would be a change in Media list focus.
    //So removing the call from here
    //DoCleanupRotationParameters();
    }
// ---------------------------------------------------------------------------
// DoInitializeRotationParameters
// ---------------------------------------------------------------------------
//
void CGlxCommandHandlerRotate::DoInitializeRotationParametersL()
    {
    TRACER("CGlxCommandHandlerRotate::DoInitializeRotationParametersL");
    if((!iDoAttemptToSaveFile)&& (!iAddedObserver))
        {
        iAddedObserver=ETrue;
        iMediaList->AddMediaListObserverL(this);
        }
    if(!iRotationApplied)
            {
            
            iTempFile.Copy(_L("c:\\data\\temp.ext"));
            User::LeaveIfError(iFileHandle.Open(iFs,
                    iFileToBeRotated, EFileWrite));
            iRotationApplied = ETrue;
            TInt filesize;
            User::LeaveIfError(iFileHandle.Size(filesize));
            iExifData = HBufC8::NewL(filesize);
            TPtr8 ptr(iExifData->Des());
            User::LeaveIfError(iFileHandle.Read(ptr));
            const CExifRead* exifReader = NULL;
            TRAPD(Exiferr,iExifWriter = CExifModify::NewL(*iExifData,CExifModify::EModify,CExifModify::ENoJpegParsing));
            if(Exiferr == KErrNone)
                {
                iDoAttemptToSaveFile = ETrue;
                exifReader = iExifWriter->Reader();
                TInt err = exifReader->GetOrientation(iInitialOrientation);
                if(err != KErrNone)
                    {
                    iInitialOrientation = 9;
                    }
                }
            else
                {
                iDoAttemptToSaveFile = EFalse;
                }
            }
    }
// ---------------------------------------------------------------------------
// DoCleanupRotationParameters
// ---------------------------------------------------------------------------
//
void CGlxCommandHandlerRotate::DoCleanupRotationParametersL()
    {
    TRACER("CGlxCommandHandlerRotate::DoCleanupRotationParametersL()");
    if(iDoAttemptToSaveFile)
        {
        //This is done just to get an MDS callback
        BaflUtils::CopyFile(iFs,iFileToBeRotated,iTempFile);
        BaflUtils::CopyFile(iFs,iTempFile,iFileToBeRotated);   
        BaflUtils::DeleteFile(iFs,iTempFile);
        iDoAttemptToSaveFile = EFalse;
        }  

    iTempFile.Zero();
    iInitialOrientation = 9;
    iRotationAngle = 0;
    //Cleanup the thumbnail DB using Cleanup command as MDS takes a Long time for a callback.
    CMPXCommand* command = TGlxCommandFactory::ThumbnailCleanupCommandLC();
    command->SetTObjectValueL<TAny*>(KMPXCommandGeneralSessionId, static_cast<TAny*>(this));
    
    //Nitz To Do Initialize cleanup command Just for the respective Media Items
  //TGlxMediaId itemId= iGlxItem->Id();
  //  command->SetTObjectValueL<TMPXItemId>(KMPXMediaGeneralId,itemId.Value()); 
    iMediaList->CommandL(*command);
    CleanupStack::PopAndDestroy(command);
    iMediaList->RemoveMediaListObserver(this);
    iAddedObserver=EFalse;
    //CGlxTextureManager& tm = iUiUtility->GlxTextureManager();
    //tm.UpdateThumbnailTexture(*iGlxItem);
    iMediaList = NULL;
    iGlxItem = NULL;

    iRotationApplied = EFalse;
    if ( iExifData )
        {
        delete iExifData;
        iExifData = NULL;
        }
    if ( iExifWriter )
        {
        delete iExifWriter;
        iExifWriter = NULL;
        } 
    }
//  End of File
