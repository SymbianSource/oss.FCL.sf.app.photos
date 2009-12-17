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
* Description:    ALF TK utilities
*
*/




#ifndef C_GLXUIUTILITY_H
#define C_GLXUIUTILITY_H

// external incudes
#include <e32base.h>
#include <gdi.h>

#include <alf/alfenv.h>  //for action observers

// Internal includes
#include <mglxtvobserver.h>     // for inteface MGlxTvObserver

// constants
const TUint KGlxDefaultVisibleItemsGranularity = 15;
const TUint KGlxQHDVisibleItemsGranularity = 20;
const TUint KGlxVGAVisibleItemsGranularity = 21;

class MGlxSkinChangeObserver;
class CGlxSkinChangeMonitor;
class CCoeAppUi;
class CGlxSettingsModel;
class CGlxTextureManager;
class CAlfEnv;
class CAlfDisplay;
class CAlfTexture;
class CAlfControl;
class CGlxTv;
class CGlxResolutionManager;
class CGlxScreenFurniture;
//class CHgContextUtility;
/**
 * Enumeration for different types of navigations that occur from one view to the next
 */
enum TGlxNavigationDirection 
    {
    EGlxNavigationForwards, 
    EGlxNavigationBackwards, 
    EGlxNavigationNext, 
    EGlxNavigationPrevious
    };

/**
 * Enumeration for screen orientation of the application.
 */
enum TGlxOrientation
    {
    EGlxOrientationUninitialised,   /**< For internal use only */
    EGlxOrientationDefault,         /**< Default orientation for the device */
    EGlxOrientationTextEntry,       /**< Text entry orientation for the device */
    EGlxOrientationLandscape        /**< Landscape orientation */
    };

/**
 *  CGlxUiUtility
 *
 *  Alf utils
 *
 *  @lib glxalfutils.lib
 *
 *  @internal reviewed 12/06/2007 by Dave Schofield
 */
class CGlxUiUtility : public CBase, 
                       public MGlxTvObserver ,             // for TV Out
                       public MAlfActionObserver           //FOR layout change notifications                      
    {
    
public:
    /**
     * Get the single instance of the utility and increment the reference count.
     * The caller must call Close() when the utility is no longer needed.
     * @return Pointer to the single instance of the UiUtility.
     */
    IMPORT_C static CGlxUiUtility* UtilityL();
    
    /**
     * Decrement the reference count of the utility and delete it if the count
     * becomes zero.
     */
    IMPORT_C void Close();

    /**
     * Get a pointer to the Alf environment.
     * @return The ALF environment owned by the AlfUtility.
     */
    IMPORT_C CAlfEnv* Env() const;
    
    /**
     * Get a pointer to the Alf display.
     * @return The ALF display owned by ALF.
     */
    IMPORT_C CAlfDisplay* Display() const;

    /**
     * Shows the Alf display in full screen.
     */
    IMPORT_C static void ShowAlfDisplayL();

    /**
     * Hides the Alf display so that Avkon screen can take over the whole screen
     * and Avkon dialog can show the status pane.
     */
    IMPORT_C static void HideAlfDisplayL();

    /**
     * Return the GlxTextureManager owned by the UiUtility.
     * This pointer is only guaranteed to be valid until the UiUtility is Close()d.
     * @return The GlxTextureManager owned by the UiUtility
     */
    IMPORT_C CGlxTextureManager& GlxTextureManager();

    /**
     * @return The current direction of the switch between views
     */
    IMPORT_C TGlxNavigationDirection ViewNavigationDirection();

    /**
     * Set the current direction of the switch between views
     * @param aDirection The direction of the navigation
     */
    IMPORT_C void SetViewNavigationDirection(TGlxNavigationDirection aDirection);

    
    /**
      * Helper function to return display size
      * @return Display size
      */
    IMPORT_C TSize DisplaySize() const;

    /**
     * Get an id of a text style corresponding to the font and size
     * @param aFontId  The Avkon logical font Id
     * @param aSizeInPixels The size of the text
     * @return The Id of the Text style in the UiTextStyleManager
     */
    IMPORT_C TInt TextStyleIdL(TInt aFontId, TInt aSizeInPixels);

    /**
     * Set the screen orientation of the application.
     * @param aOrientation  The new orientation.
     */
    IMPORT_C void SetAppOrientationL(TGlxOrientation aOrientation);
    /**
     * @return The current screen orientation of the application.
     */
    IMPORT_C TGlxOrientation AppOrientation() const;
    
    /**
     * ScreenFurniture
     * @return pointer to CGlxScreenFurniture
     */
    IMPORT_C CGlxScreenFurniture* ScreenFurniture();

    /**
     * CHgContextUtility
     * @return pointer to CHgContextUtility
     */
    //IMPORT_C CHgContextUtility* ContextUtility();
    
    /**
     * This method can be used to check whether pen support is enabled.
     * @return ETrue if pen support is enabled, otherwise EFalse.
     */
    IMPORT_C TBool IsPenSupported();
    //Nitz Review Added
    /**
     * This method can be used to set the image size required for Rotation.
     * @param aRotatedImageSize the image size to be set.
     */
    IMPORT_C void SetRotatedImageSize(TSize aRotatedImageSize);
    //Nitz Review Added end
    
    /**
     * This method can be used to Get the image size required for Rotation.
     * @return the image size for the image to be rotated.
     */
    IMPORT_C TSize GetRotatedImageSize();
    
	/**
     * This method can be used to get Icon Size for Grid Widget
     * @return const TSize.
     */
    IMPORT_C TSize GetGridIconSize();
    IMPORT_C TBool IsExitingState();
    
    IMPORT_C void SetExitingState(TBool aIsExiting);
    
    /**
     * Returns Visible items granularity based on feature 
     * layout (i.e., qHD, VGA)  
     * @return TInt visible items granularity / count
     */
    IMPORT_C TInt VisibleItemsInPageGranularityL();
    
public: // from class MGlxTvObserver

    /**
     * @ref MGlxTvObserver::HandleTvStatusChangedL
     */
    IMPORT_C void HandleTvStatusChangedL( TTvChangeType aChangeType );


public://from MAlfActionObserver
    
    /**
     * @ref MAlfActionObserver::HandleActionL
     */
    void  HandleActionL (const TAlfActionCommand &aActionCommand); 
public:

	/**
	 *Add MGlxSkinChangeObserver to skin change observers array. 
	 *@param aObserver Reference of the MGlxSkinChangeObserver.
	 */
    IMPORT_C void AddSkinChangeObserverL(MGlxSkinChangeObserver& aObserver);
    
    
    /**
	 *Remove MGlxSkinChangeObserver from skin change observers array. 
	 *@param aObserver Reference of the MGlxSkinChangeObserver.
	 */
    IMPORT_C void RemoveSkinChangeObserver(MGlxSkinChangeObserver& aObserver);   
    

private: 
    /**
     * C++ default constructor.
     */
    CGlxUiUtility();
    /**
     * 2nd phase constructor.
     */
    void ConstructL();
    /**
     * Destructor.
     */
    ~CGlxUiUtility();

    /** 2-phase constructor */
    static CGlxUiUtility* NewL();
    
private:
    /**
     * Create the secondary TV Out Display
     */
     void CreateTvOutDisplayL();

     /**
     * Destroy the secondary TV Out Display
     */
     void DestroyTvOutDisplay();
     /**
     * Calculate the Grid Icon Size for Grid Widget. It always calculate
     * size for Landscape orientation. This is done to avoid saving two
     * different thumbnail icons in Thumbnail database.
     */
     void GridIconSizeL();
    // internal class to contain details of a style
private:
    class TGlxTextStyle
        {
    public:
        TGlxTextStyle( TInt aFontId, TInt aSizeInPixels, TInt aStyleId )
        : iFontId(aFontId), iSizeInPixels( aSizeInPixels ), iStyleId( aStyleId )
            {};
    public:
        TInt iFontId;
        TInt iSizeInPixels;
        TInt iStyleId;
        };
        
private:        
    /** Pointer to the appui (not owned) */
    CCoeAppUi* iAppUi;
    
    /** The Alf environment (owned) */
    CAlfEnv* iEnv;    
    
    /** The Alf display used by the control (not owned) */
    CAlfDisplay* iAlfDisplay;
    
    /** The texture manager (owned) */
    CGlxTextureManager* iGlxTextureManager;
    
    /** Avkon control hosting the TV Alf display (owned) */
    CAlfDisplay* iTvDisplay;
    
    /** Pointer to TvOut monitoring object (owned) */
    CGlxTv* iGlxTvOut;
    
    /** Pointer to resolution manager object (owned) */
    CGlxResolutionManager* iGlxResolutionManager;

    /** Application settings model */
    CGlxSettingsModel* iSettingsModel;
    
    /// Current navigation direction between views
    TGlxNavigationDirection iNavigationDirection;

    /** Array of details of created styles */
    RArray<TGlxTextStyle> iTextStyles;

    /** Screen orientation of the application */
    TGlxOrientation iOrientation;

    /** Avkon control for monitoring skin changes*/
    CGlxSkinChangeMonitor* iGlxSkinChangeMonitor;
    
    TBool iShared;
	TSize iGridIconSize;
    
    CGlxScreenFurniture* iScreenFurniture;
    TBool iIsExiting;
    //Added to give the Image size to the Coverflow Widget for Rotation
    TSize iRotatedImageSize;
    
//    CHgContextUtility* iContextUtility;
    };


#endif // C_GLXALFUTILITY_H
