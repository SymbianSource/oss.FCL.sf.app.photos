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
* Description:    Carousel control class 
 *
*/




#ifndef C_GLXCLOUDVIEWCONTROL_H
#define C_GLXCLOUDVIEWCONTROL_H

// INCLUDES

#include <alf/ialfwidgeteventhandler.h>
#include <osn/ustring.h>
#include <alf/alfscrollevents.h>
#include <alf/ialfscrollbarwidget.h>

//use includes
#include <mglxmedialistobserver.h> 
#include "glxcontainerinfobubble.h"
#include "glxbubbletimer.h" //for timer functionality
#include "glxcloudinfo.h"
#include <alf/alfeventhandler.h>

#include <AknLayout2Def.h>
#include <aknphysicsobserveriface.h> //Physics - Kinetic scrolling listener
using namespace Alf;


// FORWARD DECLARATION
class MMPXViewUtility;
class MGlxUiCommandHandler;
class CAlfAnchorLayout;
class CAlfViewportLayout;
class CAlfControl;
class MGlxMediaList;
class CGlxDefaultAttributeContext;
class MMPXCollectionUtility;
class CGlxContainerInfoBubble;
class MTouchFeedback;
class MGlxCloudViewMskObserver;
class MGlxEnterKeyEventObserver;
class MGlxCloudViewLayoutObserver;
class CAknPhysics;
class CGlxTagsContextMenuControl ;
class MGlxItemMenuObserver;

enum TTagEventType
    {
    ECustomEventFocusDragScroll
    };
/**
 *  CGlxCloudViewControl
 *  Alf-based Cloud control
 *  Creates a weighted list of items based on the associations.
 * 
 */

class CGlxCloudViewControl : public CAlfControl,public IAlfWidgetEventHandler
                            ,public MGlxMediaListObserver
                            ,public MAknPhysicsObserver
                            ,public MGlxTimerNotifier
	{
public:

	/** 	
	 *  Perform the two phase construction
	 *  @param aEnv - Env Variable of Alf
	 *  @param aEmptyText - Specifies the text for the empty list
	 *  @param aMediaList - medialist from which attributes will be fetched
	 *  @return Pointer to newly created object
	 */
	static CGlxCloudViewControl *NewL(CAlfDisplay& aDisplay, CAlfEnv &aEnv,
			MGlxMediaList& aMediaList, const TDesC& aEmptyText
			,MGlxCloudViewMskObserver& aObserver
			,MGlxEnterKeyEventObserver& aObserverEnterKeyEvent,
			CAlfAnchorLayout *aAnchorLayout,
			MGlxCloudViewLayoutObserver& aLayoutObserver
			,MGlxItemMenuObserver& aItemMenuObserver);

	/** 	
	 *  Perform the two phase construction
	 *  @param aEnv - Env Variable of Alf
	 *  @param aEmptyText - Specifies the text for the empty list
	 *  @param aMediaList - medialist from which attributes will be fetched
	 *  @return Pointer to newly created object
	 */
	static CGlxCloudViewControl *NewLC(CAlfDisplay& aDisplay, CAlfEnv &aEnv,
			MGlxMediaList& aMediaList, const TDesC& aEmptyText
			,MGlxCloudViewMskObserver& aObserver
			,MGlxEnterKeyEventObserver& aObserverEnterKeyEvent,
			CAlfAnchorLayout *aAnchorLayout,MGlxCloudViewLayoutObserver& aLayoutObserver
			,MGlxItemMenuObserver& aItemMenuObserver);

	/**
	 * Destroy the object and release all memory objects
	 */
	~CGlxCloudViewControl();
	
public:
    /**
    * Used to identify event handlers that are capable of handling
    * a specified key or custom event. Pointer event handlers cannot be
    * identified directly, since they are dependent on the presentation.
    * Therefore, pointer event handlers should be associated with
    * a custom event ID that can be used to simulate the actual pointer event.
    *
    * @since S60 ?S60_version
    * @param aEvent The event.
    * @return <code>ETrue</code> if the event can be processed. Otherwise <code>EFalse</code>.
    */
    bool accept ( CAlfWidgetControl& aControl, const TAlfEvent& aEvent ) const;

    /**
    * @see IAlfWidgetEventHandler
    */
    virtual AlfEventStatus offerEvent ( CAlfWidgetControl& aControl, const TAlfEvent& aEvent );


    /**
    * Sets AlfWidgetEventHandlerInitData to event handler.
    *
    * @param aData A data structure which contains for example id of the event
    * handler.
    */
    virtual void setEventHandlerData( const AlfWidgetEventHandlerInitData& /*aData*/ )
        {
        }
    
    /**
    * Defines the widget states, where the event handler is active.
    * By default, if this method is not called, the event handler is expected
    * to be active in all states.
    *
    * @param aStates A bitmask defining the widget states, where the event
    * handler is active. The low 16 bits are reserved for the states
    * defined by the framework in <TODO: add reference>. The high 16 bits are
    * available for states defined by the client code.
    */
    void setActiveStates ( unsigned int aStates );

    IAlfInterfaceBase* makeInterface ( const IfId& aType );

    /**
    * Returns the type of EventHandler. 
    * @see IAlfWidgetEventHandler::AlfEventHandlerType
    *
    * @return The type of event handler.
    */
    AlfEventHandlerType eventHandlerType();

    /**
    * Returns information about the phase in the event handling cycle in
    * which the event hadler will be executed.
    * @see IAlfWidgetEventHandler::AlfEventHandlerExecutionPhase
    *
    * @return Event handler execution phase.
    */
    AlfEventHandlerExecutionPhase eventExecutionPhase();

    /**
    * Returns AlfWidgetEventHandlerInitData.
    *
    * @return  A pointer to AlfWidgetEventHandlerInitData structure which contains
    * for example id of the event handler.
    */
    virtual AlfWidgetEventHandlerInitData* eventHandlerData()
        {
        return NULL;
        }
    
    /**
    * initialises data for scrollbar model
    * @param scrollbar widget
    */
    void InitializeScrollBar(IAlfScrollBarWidget* aScrollBarWidget);
    
    /**
     * Hides/shows the grid layout
     * @param aShow - Show value
     */
    void ShowContextItemMenu(TBool aShow);
    
public://MGlxTimernotifier
    /**
     * Virtual Function from MGlxTimernotifier
     */
    void TimerComplete();
    
private:
	/** 	
	 *  Perform the two phase construction
	 *  @param  aEvent - key Event to handle
	 *  @return EKeyWasConsumed if key event is processed,
	 *         otherwise EKeyWasNotConsumed
	 */
	TBool OfferEventL(const TAlfEvent &aEvent);
	
	/** 	
	 *  @function HandleKeyUp
	 * 
	 */
	void HandleKeyUpL();
	
	
	/** 	
	 *  @since S60 3.2
	 * 
	 */
	void HandleLayoutFocusChange();
	
	/** 	
	 *  @since S60 3.2
	 * 
	 */
	void HandleKeyDownL();
	/** 	
	/** 	
	 *  @since S60 3.2
	 * 
	 */
	void UpdateLayout();
	
	/**     
	 * Displays the empty cloud view when there are no tags to diplay
	 */
	void DisplayEmptyCloudViewL();
	
	
	/** 	
	 *  constructor	
	 *  @param aEnv - Env Variable of Alf
	 *  @param aMediaList - medialist from which attributes will be fetched
	 *  @param aMediaList - medialist from which attributes will be fetched
	 */
	CGlxCloudViewControl(CAlfEnv &aEnv,
		MGlxMediaList& aMediaList,MGlxCloudViewMskObserver& aObserver,MGlxEnterKeyEventObserver& aObserverEnterKeyEvent
		,MGlxCloudViewLayoutObserver& aLayoutObserver);

	/** 	
	 *  Perform the two phase construction
	 *  @param  aStartPoint - start point for draeing the row
	 *  @param  aRowHeight - height of the row
	 *  @param  aRowStartIndex - start index of row
	 *  @param  aRowEndIndex- Index upto which items will be drawn
	 *  @return 
	 */
	TInt LayoutVisibleRows(TPoint aStartPoint,TInt aRowStartIndex, TInt
    	aRowEndIndex);
    	
    	
    	
	/** 	
	 *  Perform the two phase construction
	 *  @param  aStartRowIndex - row index 
	 *  @return 
	 */
	void LayoutVisibleArea(/*TInt aStartRowIndex*/);

	/** 	
	 *  returns the rownumber for a particular index.
	 *  @param  aItemIndex - row index 
	 *  @return rownum
	 */
	TInt RowNumber(TInt aItemIndex) const;

	/** 	
	 *  Updates the focus whenever there is a change in focus of the item.
	 */
	void FocusUpdate();

	/** 
 	 *  Perform the second phase of two phase construction
	 *  @param aEmptyText - Specifies the text for the empty list
	 */
	void ConstructL(const TDesC& aEmptyText,CAlfDisplay& aDisplay,
	        CAlfAnchorLayout *aAnchorLayout,MGlxItemMenuObserver& aItemMenuObserver);
	
	/** 
	 * Updates Row Structure 
	 * 
	 */
	void UpdateRowDataL();
	

	/** 
	 * Returns maximum value of usage Count
	 * 
	 */
    TInt MaxUsageCount();
    
    
	/** 
	 *  @param aIndex - Media List Index
	 * Returns usage count for a given index
	 * */
	TInt UsageCount(TInt aIndex);
	
	
	
	/** 
	 *  @function FetchAttributeFromCacheL
	 * 
	 * */
	void FetchAttributeFromCacheL();
	 
	 
	 /** 
	 * Utility method to add a TGlxCloudInfo struct into iCloudInfo array 
	 * @param aCloudInfo The next item to add into iCloudinfo
	 * @param aRowHeight value of aCloudInfo.iRowHeight
	 * @param aStartIndex value of aCloudInfo.iStartIndex
	 * @param aEndIndex value of aCloudInfo.iEndIndex
	 * */
	 void AppendToCloudArrayL( TGlxCloudInfo& aCloudInfo, 
	     const TInt& aStartIndex, const TInt& aEndIndex );
	 
private: // from MAknPhysicsObserver
    
	 /*
	  * Observer to get notified about new position to be displayed in
	  * @param aNewPosition The new point where the virtual port should start from 
	  * @param aDrawNow value to determine whether to draw at new position or not
	  * @param aFags Special value (not used here)
	  */
    void ViewPositionChanged( const TPoint& aNewPosition, TBool aDrawNow, TUint /*aFlags*/ );
    
    /*
     * Obsrever to get notified whether the physics emulation ended
     */
    void PhysicEmulationEnded();
    
    /*
     * Observer callback used by CAknPhysics to know our current viewposition
     * (which is with reference to displayable viewportposition + screeenheight/2) 
     * Returns current viewposition point  
     */
    TPoint ViewPosition() const;


private:
    
    TBool IsLandscape();
    
    /*
     * Initializes the physcs library with total size, displayable size, etc 
     */
    void InitPhysicsL();
    
    /** 
	 * Sets focused item color
	 **/
    void SetFocusColor();
    
    
    /** 
	 * Set the middle point of the infobuble
	 **/
    void SetBubleMidPoint(TPoint& aMidPoint);
  
   /** 
	 * Create the infobublecontainer
	 **/  
    void CreateBubleContainer();
    
   /** 
	 * Move the viewport up depending on the condition
	 *
	 **/ 
    void MoveUpIfRequired();
  
    /** 
	 * Move the viewport down depending on the condition
	 **/  
    void MoveDownIfRequired();

    /** 
	 * calculate the mid point of the bubble
	 **/
    void CalculateBubleMidPoint();
	 
public:

	void HandleItemAddedL(TInt aStartIndex, TInt aEndIndex,
        MGlxMediaList* aList);

	/**
	 *  Notification that media object is now available for an item 
	 *  @param Index of the item 
	 *  @param aList List that this callback relates to
	 */
	void HandleMediaL(TInt aListIndex, MGlxMediaList* aList);
	

	/**
	 *  Notification that media item was removed from the list
	 *  @param aStartIndex First item that was removed 
	 *  @param aEndIndex Last item that was removed
	 *  @param aList List that this callback relates to
	 */
	void HandleItemRemovedL(TInt aStartIndex, TInt aEndIndex,
			MGlxMediaList* aList);

	/**
	 *  Notification that media item was changed
	 *  @param aItemIndexes Indexes of items that were changed
	 *  @param aList List that this callback relates to
	 */
	void HandleItemModifiedL(const RArray<TInt>& aItemIndexes,
			MGlxMediaList* aList);

	/**
	 *  Notification that an attribute is available
	 *  @param aItemIndex Index of the for which the thumbnail is available
	 *  @param aAttributes Array of attributes that have become available
	 *  @param aList List that this callback relates to 
	 */
	void HandleAttributesAvailableL(TInt aItemIndex,
			const RArray<TMPXAttribute>& aAttributes, MGlxMediaList* aList);

	/**
	 *  Notification that focus has moved
	 *  @param aType the direction of the focus change
	 *  @param aNewIndex the new index after the focus change
	 *  @param aOldIndex the old index before the focus change
	 *  @param aList List that this callback relates to
	 */
	void HandleFocusChangedL(NGlxListDefs::TFocusChangeType aType, TInt aNewIndex,
			TInt aOldIndex, MGlxMediaList* aList);

	/**
	 *  Notification that an item has been selected/deselected
	 *  @param aIndex Index of the item that has been selected/deselected
	 *  @param aSelected Boolean to indicate selection/deselection
	 *  @param aList List that the selection relates to
	 */
	void HandleItemSelectedL(TInt aIndex, TBool aSelected, MGlxMediaList* aList);

	/**
	 *  Notification from the collection.  E.g. Items added/modified/deleted and progress notifications
	 *  @param aMessage Message notification from the collection
	 *  @param aList List that the notification relates to
	 */
	void HandleMessageL(const CMPXMessage& aMessage, MGlxMediaList* aList);
	
	/**
	     *  Notification from the collection that the medialist is populated	     
	     *  @param aList List that the notification relates to
	     */
	void HandlePopulatedL(MGlxMediaList* /*aList*/);
	
	/**
	 *  Notification from the collection.  E.g. Items added/modified/deleted and progress notifications
	 *  @param aIndex Index of the item for which property needs to be set
	 *  @param aScale Scale parameter to decide the properties of item
	 */
	void SetPropertiesL(TInt aIndex, TInt aScale);

	/**
	 *  Called whenever the resolution is changed
	 */
	void VisualLayoutUpdated(CAlfVisual & aVisual);
	
private:
	
	 /**
	 * Handle the pointer event
	 * @param aEvent event describing the gesture 
	 */
	TBool HandlePointerEventL( const TAlfEvent &aEvent );
	
	/**
	 *  Handles the drag events
	 *  @param pointer event generated because of drag
	 *  @return returns whether the pointer event is consumed or not
	 */
	TBool HandleDragL(const TPointerEvent& aPointerEvent);
	
	/**
	 *  checks the visual on which the drag event is generated
	 *  @param position where the drag event has ocuured
	 *  @return the index of the visual that needs to be focused 
	 */
	TInt GetAbsoluteIndex(const TPoint& aPosition);
	
	/**
	 *  Checks which visual lies below/above the focused item and sets the focus
	 *  @param the row number that needs to be checked for getting the overlaping visual
	 */	
	void SetRelativeFocusL(TInt aRowNumber);

     /**
	 *  Updates layout when orientation is changed
	 */
	void UpdateLayoutL(); 

private:
    /**
     *  Send the scroll custom event to scrollbar widget
     */
    void Scroll();
    
    /**
     *  decide the scrollbar new position
     *  @param number of steps which is used to determine the amout to be scrolled
     */
    void UpdateScrollBar(TInt aNumberOfSteps, TBool aDiff = ETrue);
    
    /**
     *  Display the scrollbar only if there are items
     *  @param visibility
     */
    void DisplayScrollBar();
    
private:

	// The Alf Environment 
	CAlfEnv& iEnv;
	
	//Parent layout for viewportayout
	CAlfAnchorLayout* iViewPortParentLayout;
	
	//ViewPortLayout
	CAlfViewportLayout* iViewPortLayout;
	
	//Anchor layout for tags
	CAlfAnchorLayout *iLayout;

    //medialist not owned
	MGlxMediaList& iMediaList; 

	//Array of Cloud visuals. 
	RPointerArray <CAlfTextVisual> iLabels;

	// Array for information of cloud visuals. 
	RArray <TGlxCloudInfo> iCloudInfo;

	//Ui Utility  
	CGlxUiUtility* iUiUtility;

	// Visual for empty string display 
	CAlfTextVisual *iEmptyString;

	// FocusIndex keeps track of the focussed item 
	TInt iScrollDirection;
	
	// FocusIndex keeps track of the focussed row 
	TInt iFocusRowIndex;

	// EndRowindex keeps track of the end row 
	TInt iEndRowIndex;

	//keeps track of the end row 
	TInt iLayoutIndex;

	// Fetch context for list item attributes 
	CGlxDefaultAttributeContext* iAttributeContext;
	
	// for storing screen height 
	TInt iScreenHeight;
	
    // List event observer 
	MGlxCloudViewMskObserver& iObserver;	
	
	// stores the text for empty view
	HBufC* iEmptyText;
	
	//Viewport position
	TAlfRealPoint iViewPortPosition;
	
	//observer to inform "open" command cloudviewimp class so that it opens the next view
	MGlxEnterKeyEventObserver& iObserverEnterKeyEvent;
	
	//observer to inform command cloudviewimp class that layout is changed
	MGlxCloudViewLayoutObserver& iLayoutObserver;
	//Viewport size
	TAlfRealSize  iViewPortSize;
	
	//Viewport virtual size
	TAlfRealSize  iViewPortVirtualSize;
	
	//Width of the screen in which is available for tags to fit in
	TInt iTagScreenWidth;
	
	//Height of the screen in which is available for tags to fit in
	TInt iTagScreenHeight;
	
	//Touch feedback instance
	MTouchFeedback* iTouchFeedback;
	
	//parent layout for all the layout's
	CAlfLayout* iParentLayout;
	
	//data structure for scrollbar
    ScrollModelChangedEventData iScrollEventData;
   
    //scrollbar element
	IAlfWidgetEventHandler *iScrollbarElement;
	
	//scrollbar widget
	IAlfScrollBarWidget *iScrollBarWidget;

	//direction of dragging
	TInt iIsDragging;
	
	TAknWindowLineLayout iScrollPaneHandle;

	//View position w.r.t. viewportposition + (screeenwidth/2, screenheight/2)
	TPoint iViewPosition;
	
	//determins whether dragging is currently going on or not
	TBool iDragging;
	
	//Notes the time before dragging, when pointer down event happens  
	TTime iStartTime;
	
	//Notes the last pointer co-ordinates
	TPoint iPrev;
	
	//The pointer coordinates before dragging when pointerDown event happened 
	TPoint iStart;
	
	//owning - Physics library object
	CAknPhysics* iPhysics;
	
	//boolean to check whether physics emulation is going on or not
	TBool iPhysicsStarted;
	
	//boolean to check if dragging really happened
	TBool iViewDragged;
	
    /**variable used for providing delay */
    CGlxBubbleTimer* iTimer;    

    //Alfcontrol to draw grid layout for handling floating bar menu items
    CGlxTagsContextMenuControl* iTagsContextMenuControl;
    
    /**flag for Timer completion */
    TBool iTimerComplete;

    /**flag to identify the next down event */
    TBool iDownEventReceived;
	};

#endif // C_GLXCLOUDVIEWCONTROL_H

