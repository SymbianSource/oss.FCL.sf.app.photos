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
* Description:    Header for implementation of background processing indicator
*
*/

#ifndef GLXFULLSCREENBUSYICON_H_
#define GLXFULLSCREENBUSYICON_H_

#include <alf/alfControl.h>
#include <mglxmedialistobserver.h>
// Forward decleartion
class CAlfEnv;
class CAlfAnchorLayout;
class CAlfImageVisual;
class CGlxUiUtility;
class MGlxMediaList;

enum TGlxBusyIconControlGroupId
    {
    EBGPSOriPortrait = 10,
    EBGPsOriLandscape = 20
    };

class CGlxFullScreenBusyIcon: public CAlfControl,public MGlxMediaListObserver
    {
public:
    /*
     * NewL()
     */
    static CGlxFullScreenBusyIcon* NewL(MGlxMediaList& aMediaList,CGlxUiUtility& aUiUtility);
    /*
     * destructor
     */
    ~CGlxFullScreenBusyIcon();
    /*
     *  Shows the Busy Icon to indicate TN generation is going on.
     *  @param aShow ETrue the icon will be shown and EFlase the icon will be hidden  
     */    
    void ShowBusyIconL(TBool aShow);

public://frm CAlControl
    void VisualLayoutUpdated(CAlfVisual& aVisual);        
public: // from MGlxMediaListObserver
    void HandleItemAddedL( TInt /*aStartIndex*/, TInt /*aEndIndex*/, MGlxMediaList* /*aList*/ ){}
    void HandleItemRemovedL( TInt /*aStartIndex*/, TInt /*aEndIndex*/, MGlxMediaList* /*aList*/){}
    void HandleAttributesAvailableL( TInt aItemIndex, 
            const RArray<TMPXAttribute>& /*aAttributes*/, MGlxMediaList* aList );    
    void HandleFocusChangedL( NGlxListDefs::TFocusChangeType /*aType*/, 
            TInt aNewIndex, TInt aOldIndex, MGlxMediaList* aList );
    void HandleItemSelectedL(TInt /*aIndex*/, TBool /*aSelected*/, MGlxMediaList* /*aList*/ ){}
    void HandleMessageL( const CMPXMessage& /*aMessage*/, MGlxMediaList* /*aList*/ ){}
    void HandleError( TInt /*aError*/ );
    void HandleCommandCompleteL( CMPXCommand* /*aCommandResult*/, TInt /*aError*/, 
            MGlxMediaList* /*aList*/ ){}
    void HandleMediaL( TInt /*aListIndex*/, MGlxMediaList* /*aList*/ ){}
    void HandleItemModifiedL( const RArray<TInt>& /*aItemIndexes*/, MGlxMediaList* /*aList*/ ){}
private:
    /*
     * Constructor
     */
	CGlxFullScreenBusyIcon(MGlxMediaList& aMediaList,CGlxUiUtility& aUiUtility);    
    /*
     * ConstructL()
     */
    void ConstructL();
    /*
     *  Updates the position of the icon based on the screen size landscape/portrait 
     */
    void UpdatePosition();
    /*
     *  Gets the current screen size 
     */
    TRect GetScreenSize();
    /*
     * Callback for periodic timer
     */
    static TInt PeriodicCallbackL( TAny* aPtr );
    /*
     * nonstatic func called from periodic timer
     */
    void CallPeriodicCallbackL();
    /*
     *  Check the thumbnail generation for focus item is done
     *  @return ETrue if the generation is complete,EFalse if generation is going on.
     *   
     */        
    TBool IsReadyL();
    
private:
	MGlxMediaList&         iMediaList;
	CGlxUiUtility&         iUiUtility;                      // CGlxUiUtility passed from FS view
    CAlfEnv*               iEnv;                            // Alf Env    
    CAlfAnchorLayout*      iMainVisual;                     // Anchor Layout
    CAlfImageVisual*       iBackgroundBorderImageVisual;    // Background Image Visual
    TRect                  iScreenSize;                     // Screen size      
    CPeriodic*             iPeriodic;
    TInt                   iBusyIconIndex;
    
    };


#endif /* GLXFULLSCREENBUSYICON_H_ */
