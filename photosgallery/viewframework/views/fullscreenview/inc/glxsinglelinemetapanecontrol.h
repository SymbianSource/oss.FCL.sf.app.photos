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
* Description:    Header for class to implement Single line metapane
*
*/

#ifndef GLXMETADATACONTROL_H_
#define GLXMETADATACONTROL_H_

#include <alf/alfControl.h>
#include <mglxmedialistobserver.h>

#include "glxfullscreenviewimp.h"

enum TGlxControlGroupId
    {
    //List View
    EMetaOrinentaionPortrait = 10,
    EMetaOrinentaionLandscape = 20
    };
    
// Forward decleartion
class CAlfEnv;
class CAlfAnchorLayout;
class CAlfGridLayout;
class CAlfDeckLayout;
class CAlfImageVisual;
class CAlfTextVisual;
class CAlfImageBrush;
class CAlfBorderBrush;
class MGlxMediaList;
class CGlxDefaultAttributeContext;
class CGlxUiUtility;
class TGlxMedia;
class CGlxTextureManager;
class CGlxSLMPFavMLObserver;

/*
 * This is an observer class to observe the favourites attribute change and pass it back to the metapane
 * for updating its favourite icon 
 */
class MGlxSLMPObserver
    {
public:
    virtual void HandleUpdateIconL(TBool aModify) = 0;
    
    };


/*
 * For craeting and maintaining the Single Line metapane 
 */
class CGlxSinleLineMetaPane: public CAlfControl,
                            public MGlxMediaListObserver,
                            public MGlxSLMPObserver
     {
public:
    /*
     * NewL()
     */
    static CGlxSinleLineMetaPane* NewL( CGlxFullScreenViewImp& aFullscreenView,
            MGlxMediaList& aMediaList, CGlxUiUtility& aUiUtility);
    
    /*
     * destructor
     */
    ~CGlxSinleLineMetaPane();
    
    /*
     * Show/Hide the Metapane
     */
    void ShowMetaPane(TBool aShow);
    
public://frm CAlControl
    TBool OfferEventL(const TAlfEvent &aEvent);
    
    void VisualLayoutUpdated(CAlfVisual& aVisual);
    
public: // from MGlxMediaListObserver
    void HandleItemAddedL( TInt /*aStartIndex*/, TInt /*aEndIndex*/, MGlxMediaList* /*aList*/ );
    void HandleItemRemovedL( TInt /*aStartIndex*/, TInt /*aEndIndex*/, MGlxMediaList* /*aList*/);
    void HandleAttributesAvailableL( TInt aItemIndex, 
        const RArray<TMPXAttribute>& /*aAttributes*/, MGlxMediaList* aList );    
    void HandleFocusChangedL( NGlxListDefs::TFocusChangeType /*aType*/, 
        TInt aNewIndex, TInt aOldIndex, MGlxMediaList* aList );
    void HandleItemSelectedL(TInt /*aIndex*/, TBool /*aSelected*/, MGlxMediaList* /*aList*/ );
    void HandleMessageL( const CMPXMessage& /*aMessage*/, MGlxMediaList* /*aList*/ );
    void HandleError( TInt /*aError*/ );
    void HandleCommandCompleteL( CMPXCommand* /*aCommandResult*/, TInt /*aError*/, 
        MGlxMediaList* /*aList*/ );
    void HandleMediaL( TInt /*aListIndex*/, MGlxMediaList* /*aList*/ );
    void HandleItemModifiedL( const RArray<TInt>& /*aItemIndexes*/, MGlxMediaList* /*aList*/ );

public:// MGlxSLMPObserver
    virtual void HandleUpdateIconL(TBool aModify);
    
private:
    /*
     * Constructor
     */
    CGlxSinleLineMetaPane(CGlxFullScreenViewImp& aFullscreenView,
            MGlxMediaList& aMediaList,CGlxUiUtility& aUiUtility);
    
    /*
     * ConstructL()
     */
    void ConstructL();
    
    /*
     * Creating the grid
     */
    void CreateGridL();
    
    /*
     * Update position
     */
    void UpdatePosition();
    
    /*
     * Determines the orientation of the view
     */
    TInt DetermineOrientation(TRect aRect);
    
    /*
     * This is called from handleattributes available, it updtates the 
     * metapane values for each item 
     */
    void UpdateAttributesInMetaPane(TMPXAttribute aAtrribute, 
            TInt aFocusIndex, const TGlxMedia& item);
    
    /*
     * This is called from handleattributes available and focus changed, it updtates the 
     * metapane values for focus item
     */
    void UpdateMetaPaneL(TInt aFocusIndex, MGlxMediaList* aList, 
            TBool aUpdateFavIcon = EFalse);
    
    /*
     * This creates the icons for Favourites and Location Icon and then 
     * updates the respective image visuals
     */
    void CreateIconTextureAndUpdateVisualsL();
    
    TRect GetScreenSize();
    
private:
    CGlxFullScreenViewImp&          iFullscreenView;        // Fullscreenview
    MGlxMediaList&                  iMediaList;             // MGlxMedialist passed from FS view
    CGlxUiUtility&                  iUiUtility;             // CGlxUiUtility passed from FS view
    CGlxDefaultAttributeContext*    iAttribContext;         // CGlxDefaultAttributeContext 

    CAlfEnv*               iEnv;                            // Alf Env
    CAlfDisplay*           iDisplay;                        // Alf Display  
    CAlfAnchorLayout*      iMainVisual;                     // Anchor Layout
    CAlfGridLayout*        iGridVisual;                     // Grid layout
    CAlfGridLayout*        iLocationGridVisual;             // Grid layout
    
    CGlxSLMPFavMLObserver* iSLMPFavMLObserver;              // Single line metapane medialist observer
    
    CAlfImageVisual*    iBackgroundBorderImageVisual;       // Background Image Visual
    
    CAlfTextVisual*     iDateTextVisual;                    // Date Text
    CAlfImageVisual*    iFavIconImageVisual;                // Location Icon
    CAlfImageVisual*    iLocIconImageVisual;                // Size Text
    CAlfTextVisual*     iLocationTextVisual;                // Location Text
    
    CAlfTexture*        iFavAddedIconTexture;               // Fav Added Icon Texture ( Yellow Icon ) 
    CAlfTexture*        iFavNotAddedIconTexture;            // Fav Not Added Icon Texture ( Black Icon )
    CAlfBorderBrush*    iBorderBrush;                       // Border brush for background image visual
    
    TRect               iScreenSize;                        // To store the screen size
    TBool               iFavIconStatus;                     // ETrue when the item is in favourites else EFalse
    };


#endif /* GLXMETADATACONTROL_H_ */
