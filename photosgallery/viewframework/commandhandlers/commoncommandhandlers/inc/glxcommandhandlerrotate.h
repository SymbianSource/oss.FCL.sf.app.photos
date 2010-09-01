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
* Description:    Rotate command handler
*
*/




#ifndef C_GLXCOMMANDHANDLERROTATE_H
#define C_GLXCOMMANDHANDLERROTATE_H

// INCLUDES

#include <glxmedialistcommandhandler.h>
#include <glxrotationlayout.h>
#include <glxlayoutsplitter.h>

#include <ExifModify.h>
#include <glxmedia.h>
// FORWARD DECLARATIONS

class CGlxDefaultAttributeContext;
class CGlxDefaultThumbnailContext;
class CGlxUiUtility;
class CGlxVisualListManager;
class MGlxLayoutOwner;

// CLASS DECLARATION

// CLASS DECLARATION

/**
 * Command to rotate the thumbnail in full screen view to better fit the aspect
 * ratio of the screen.
 *
 * @ingroup glx_group_command_handlers
 */
NONSHARABLE_CLASS (CGlxCommandHandlerRotate)
    : public CGlxMediaListCommandHandler, public MGlxMediaListObserver
    {
public:
    /**
     * Two-phased constructor.
     * @param aMediaListProvider Source of media list
     * @param aLayoutOwner Owner of Layout to append rotate layout to
     * @param aLayoutIndex Index of layout to append rotate layout to
     */
    IMPORT_C static CGlxCommandHandlerRotate* NewL(
                                        MGlxMediaListProvider* aMediaListProvider,
                                        MGlxLayoutOwner* aLayoutOwner = NULL,
                                        TInt aLayoutIndex = 0);

    /**
     * Destructor.
     */
    ~CGlxCommandHandlerRotate();

private:    // From CGlxMediaListCommandHandler
    TBool DoExecuteL(TInt aCommandId, MGlxMediaList& aList);
    void DoActivateL(TInt aViewId);
    void Deactivate();
    TBool DoIsDisabled(TInt aCommandId, MGlxMediaList& aList) const;

private:    // From MGlxMediaListObserver
    void HandleItemAddedL(TInt aStartIndex, TInt aEndIndex,
                MGlxMediaList* aList);
    void HandleMediaL(TInt aListIndex, MGlxMediaList* aList);
    void HandleItemRemovedL(TInt aStartIndex, TInt aEndIndex,
                MGlxMediaList* aList);
    void HandleItemModifiedL(const RArray<TInt>& aItemIndexes,
                MGlxMediaList* aList);
    void HandleAttributesAvailableL(TInt aItemIndex,
                const RArray<TMPXAttribute>& aAttributes, MGlxMediaList* aList);	
    void HandleFocusChangedL(NGlxListDefs::TFocusChangeType aType, TInt aNewIndex,
                TInt aOldIndex, MGlxMediaList* aList);
    void HandleItemSelectedL(TInt aIndex, TBool aSelected, MGlxMediaList* aList);
    void HandleMessageL(const CMPXMessage& aMessage, MGlxMediaList* aList);

private:
    /**
     * C++ default constructor.
     * @param aMediaListProvider Source of media list
     * @param aLayoutOwner Owner of Layout to append rotate layout to
     * @param aLayoutIndex Index of layout to append rotate layout to
     */
    CGlxCommandHandlerRotate(MGlxMediaListProvider* aMediaListProvider,
        MGlxLayoutOwner* aLayoutOwner, TInt aLayoutIndex);

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

    /**
     * Rotate the Image Using antriksh animation.
     */
    void DoRotateL(MGlxMediaList& aList, TInt aAngle);
    /**
     * Calculate and Update the image's Exif data with final orientation.
     */
    void DoPreserveOrientationL(); 
    /**
     * Calculate the final orientation from the previous orientation and the final rotation angle.
     */
    TInt DoCalculateOrientationL(TInt aInitialOrientation);
    /**
     * Initializes class variables required for rotation
     */
    void DoInitializeRotationParametersL();
    /**
     * Cleans up all class variables used for rotation
     */
    void DoCleanupRotationParametersL();
private:    // Data
    /** ID of owning view */
    TInt iViewId;
    /** Visual list manager */
    CGlxVisualListManager* iVisualListManager;
    /** HUI utility */
    CGlxUiUtility* iUiUtility;
    /** Attribute context */
    CGlxDefaultAttributeContext* iAttributeContext;
    /** Thumbnail context for portrait thumbnails */
    CGlxDefaultThumbnailContext* iPortraitThumbnailContext;
    /** Visual list containing the rotated visual */
    MGlxVisualList* iVisualList;
    /** Visual to which the rotation is applied (not owned) */
    CAlfVisual* iVisual;
	
    /** Layout to split layout chain to focused item */
    TGlxLayoutSplitter iSplitter;
    /** Layout to rotate the focused item */
    TGlxRotationLayout iLayout;
    /** Whether the command is shown in the active palette */
    TBool iShowInToolbar;
    /** Whether the command is available in the current view state */
    TBool iRotationAvailable;
    /** Whether we have rotated the visual */
    TBool iRotationApplied;
    /** Which tooltip to show */
    TBool iViewPortrait;
    
    /// The layout owner
    MGlxLayoutOwner* iLayoutOwner;
    /// The index of the layout to request from the owner
    TInt iLayoutIndex;
    /// The layout to append the rotate layout to
    MGlxLayout* iLayoutToFollow;
    MGlxMediaList* iMediaList; //not owned do not delete 
    TInt iRotationAngle;
    TFileName iFileToBeRotated;
    TFileName iTempFile;
    RFs iFs;
    TUint16 iInitialOrientation;
    RFile iFileHandle;
    HBufC8* iExifData;
    CExifModify* iExifWriter;
    TBool iDoAttemptToSaveFile;
    const TGlxMedia* iGlxItem;
    TBool iAddedObserver;
    
    };

#endif  // C_GLXCOMMANDHANDLERROTATE_H

// End of File
