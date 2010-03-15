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
* Description:    Cloud View Context menu control
 *
*/

#ifndef GLXTAGSCONTEXTMENUCONTROL_H_
#define GLXTAGSCONTEXTMENUCONTROL_H_

#include <e32base.h>
#include <e32std.h>
#include <alf/alfcontrol.h>
#include "glxbubbletimer.h"                         //for timer functionality

//Forward Declaration
class CAlfGridLayout;
class CGlxUiUtility;

/**
 *MGlxItemMenuObserver is a class to handle the menu items in grid layout
 */
class MGlxItemMenuObserver
    {
public:
    /**
     * To Handle menu items in Grid layout
     * @param aCommand command to be handled
     */
    virtual void HandleGridMenuListL(TInt aCommand) = 0;
    };

class CGlxTagsContextMenuControl  : public CAlfControl, public MGlxTimerNotifier
        
    {
public:
    /**     
     *  Perform the two phase construction
     *  @param aItemMenuObserver - handles the menu items in grid layout
     *  @return Pointer to newly created object
     */
    static CGlxTagsContextMenuControl* NewL( MGlxItemMenuObserver& aItemMenuObserver );
    /**     
     *  Perform the two phase construction
     *  @param aItemMenuObserver - handles the menu items in grid layout
     *  @return Pointer to newly created object
     */
    static CGlxTagsContextMenuControl* NewLC( MGlxItemMenuObserver& aItemMenuObserver );
    /**
     * Destroy the object and release all memory objects
     */
    ~CGlxTagsContextMenuControl();
    
public://MGlxTimernotifier
    /**
     * Virtual Function from MGlxTimernotifier
     */
    void TimerComplete();
    
public:
    /**
     * Sets the grid layout on the screen with the Point specified
     * @param aPoint - Point from which grid has to be displayed
     */
    void SetDisplay(const TPoint& aPoint);  
    /**
     * Hides/shows the grid layout
     * @param aOpacity - Opacity value
     */
    void ShowItemMenu(TBool aOpacity);
    /**
     * Hides/shows the grid layout
     * @return value of menu's visibility
     */
    TBool ItemMenuVisibility();

    /**
    * Sets the drawable screen rect 
    * It also handles coordinates changed from Orientation
    * @param aRect - rect area where the Tags are drawn
    */
    void SetViewableRect(TRect aRect);
    
private:
    /**     
     *  constructor 
     *  @param aItemMenuObserver - handles the menu items in grid layout
     */
    CGlxTagsContextMenuControl( MGlxItemMenuObserver& aItemMenuObserver );

    /** 
     *  Perform the second phase of two phase construction
     */
    void ConstructL();
    
    /** 
     *  Creates the font 
     *  @return the fontId
     */
    TInt CreateFontL();
    /** 
     *  Creates the menulist and sets the Text style 
     *  @param aFontId - Sets the text style
     */
    void CreateMenuListL(TInt aFontId);
    
    /**     
     *  Perform the two phase construction
     *  @param  aEvent - key Event to handle
     *  @return EKeyWasConsumed if key event is processed,
     *         otherwise EKeyWasNotConsumed
     */
    TBool OfferEventL(const TAlfEvent& aEvent);

private:

    //Ui Utility, not owning, only close
    CGlxUiUtility* iUiUtility;
    
    //Observer to handle the menu items in grid layout
    MGlxItemMenuObserver& iItemMenuObserver;          
    
    //Grid layout, not owning
    CAlfGridLayout* iGrid;
    
    //Slideshow text, not owning
    CAlfTextVisual* iSlideshowTextVisual;     
    
    //Delete text, not owning
    CAlfTextVisual* iDeleteTextVisual;
    
    //Rename text, not owning
    CAlfTextVisual* iRenameTextVisual;        
    
    // Anchor Layout
    CAlfAnchorLayout* iMainVisual;                
   
    //variable used for providing delay
    CGlxBubbleTimer* iTimer;                                 
   
    //Viewable rect
    TRect iViewableRect;                                    
    
    //flag for stylus menu visibility
    TBool iItemMenuVisibility;                                         
    };

#endif /* GLXTAGSCONTEXTMENUCONTROL_H_ */
