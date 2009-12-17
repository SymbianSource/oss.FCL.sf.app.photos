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
* Description:    Visual list control
*
*/




/**
 * @internal reviewed 04/07/2007 by M Byrne
 */
 
#ifndef C_GLXVISUALLISTCONTROL_H
#define C_GLXVISUALLISTCONTROL_H

#include <alf/alfcontrol.h>
#include <alf/alftimedvalue.h>
#include <alf/alfmappingfunctions.h>
#include <alf/alfbitmapprovider.h>
#include <alf/alfdisplay.h>
#include <alf/alfimagevisual.h>
#include <alf/alfenv.h>
#include "mglxmedialistobserver.h"
#include "mglxmedialist.h"
#include "glxlistwindow.h"
#include "glxblendlayout.h"
#include "mglxvisualobjectlayoutrefreshobserver.h"
#include "mglxvisuallist.h"

class CGlxVisualListWindow;
//class MGlxLayout;
class MGlxVisualListObserver;
class CAlfBorderBrush;
class CGlxUiUtility;
class CGlxDefaultAttributeContext;
class CGlxVisualIconManager;
class MGlxLayoutObserver;
class CAlfAnchorLayout;
/**
 *  CGlxVisualListControl
 *
 *  Item visual owner control
 *
 * @author Aki Vanhatalo
 */
class CGlxVisualListControl : public CAlfControl, public MGlxVisualList, 
		public MGlxMediaListObserver//, public MGlxVisualObjectLayoutRefreshObserver
    {
    friend class CGlxVisualListWindow;    
public:
    /**
     * Two-phased constructor.
     * @param aMediaList Media list of items the visuals represent
         * @param aEnv Alf environment for the control
         * @param aDisplay Alf display on which the control should be shown
     * @param aThumbnailScaleMode Scale mode to use for the visuals
     */
        static CGlxVisualListControl* NewLC( MGlxMediaList& aMediaList,
                            CAlfEnv& aEnv, CAlfDisplay& aDisplay,
                            CAlfImageVisual::TScaleMode aThumbnailScaleMode );

    /**
     * Destructor
     */
    ~CGlxVisualListControl();
    
    /**
     * Add reference
     * @return new new reference count 
     */
    TInt AddReference();
    
    /**
     * Remove reference
     * @return new new reference count 
     */
    TInt RemoveReference();
    
    /**
     * @return Media list of items the visuals represent
     */
        const MGlxMediaList& MediaList() const;

// From MGlxVisualList    
    	TGlxVisualListId Id() const;
    	CAlfVisual* Visual(TInt aIndex);
        CGlxVisualObject* Item(TInt aListIndex);
	virtual TInt ItemCount(NGlxListDefs::TCountType aType = NGlxListDefs::ECountAll) const; 
    	TInt FocusIndex() const; 
    	CAlfControlGroup* ControlGroup() const;
        void AddObserverL(MGlxVisualListObserver* aObserver);
    	void RemoveObserver(MGlxVisualListObserver* aObserver);
    	TGlxViewContextId AddContextL(TInt aFrontVisibleRangeOffset, 
    		TInt aRearVisibleRangeOffset);
    	void RemoveContext(const TGlxViewContextId& aContextId);
    	void AddLayoutL(MGlxLayout* aLayout);
    	void RemoveLayout(const MGlxLayout* aLayout);
     	void NavigateL(TInt aIndexCount);
     	TSize Size() const;
    	void BringVisualsToFront();
        void EnableAnimationL(TBool aAnimate, TInt aIndex);
    /// @ref MGlxVisualList::SetDefaultIconBehaviourL
    void SetDefaultIconBehaviourL( TBool aEnable );
    
        void AddIconL( TInt aListIndex, const CAlfTexture& aTexture, 
            NGlxIconMgrDefs::TGlxIconPosition aIconPos,
            TBool aForeground, TBool aStretch, TInt aBorderMargin,
            TReal32 aWidth = 1.0, TReal32 aHeight = 1.0 );
   
        TBool RemoveIcon( TInt aListIndex, const CAlfTexture& aTexture );
        void SetIconVisibility( TInt aListIndex, const CAlfTexture& aTexture, TBool aVisible );

    
                                                    
	 
// From MGlxMediaListObserver
    	void HandleItemAddedL(TInt aStartIndex, TInt aEndIndex, MGlxMediaList* aList);
    	void HandleMediaL(TInt aIndex, MGlxMediaList* aList);
    	void HandleItemRemovedL(TInt aStartIndex, TInt aEndIndex, MGlxMediaList* aList);
    	void HandleItemModifiedL(const RArray<TInt>& aItemIndexes, MGlxMediaList* aList);
    	void HandleAttributesAvailableL(TInt aItemIndex, 	
    		const RArray<TMPXAttribute>& aAttributes, MGlxMediaList* aList);
    	void HandleFocusChangedL(NGlxListDefs::TFocusChangeType aType, TInt aNewIndex, TInt aOldIndex, MGlxMediaList* aList);
    	void HandleItemSelectedL(TInt aIndex, TBool aSelected, MGlxMediaList* aList);
    	void HandleMessageL(const CMPXMessage& aMessage, MGlxMediaList* aList);

// From MHuiDisplayRefreshObserver
//	void NotifyDisplayRefreshStarted( CHuiDisplay &aDisplay );

    //From 
    
     void VisualObjectLayoutRefreshed(TInt aListIndex ,TSize aScreenSize );
    /**
     * @return The thumbnail scale mode of list
     */
    CAlfImageVisual::TScaleMode ThumbnailScaleMode();

private:
    /**
     * Constructor
     * @param aMediaList Media list of items the visuals represent
     * @param aEnv HUI environment for the control
     * @param aDisplay HUI display on which the control should be shown
     * @param aThumbnailScaleMode Scale mode to use for the visuals
     */
        CGlxVisualListControl( MGlxMediaList& aMediaList,
                            CAlfEnv& aEnv,
                            CAlfImageVisual::TScaleMode aThumbnailScaleMode );

    /**
     * Second-phase constructor
     */
        void ConstructL( CAlfEnv& aEnv, CAlfDisplay& aDisplay );

    /**
     * Find the range offsets for the window by combining all contexts.
     * @param aFrontOffset Variable to store the front offset.
     * @param aRearOffset Variable to store the rear offset.
     */
	void RangeOffsets(TInt& aFrontOffset, TInt& aRearOffsets);
    /**
     * Inform observers of a visual being removed.
     * @param aVisual The visual that is being removed.
     */
    	void HandleVisualRemoved( const CAlfVisual* aVisual );
    /**
     * Inform observers of a visual being added.
     * @param aVisual The visual that is being added.
     * @param aIndex The list index of the new visual.
     */
    	void HandleVisualAddedL( CAlfVisual* aVisual, TInt aIndex );

    private: // From CAlfControl
    /**
     * Notifies the control that one of the visuals it ows has been laid out.
     * Updates observers with new size. Public in base class, but not intended
     * to be used by clients.
     */
        void VisualLayoutUpdated( CAlfVisual& aVisual );
	
private:
        /** The media list (not owned) */
        MGlxMediaList& iMediaList;
        /** The alf env (not owned) */
        CAlfEnv& iEnv;
    /**
     * Structure to hold information about a view context.
     */
	struct TContext 
		{
		TGlxViewContextId iId;
		TInt iFrontVisibleRangeOffset;
		TInt aRearVisibleRangeOffset;
		};

private:
    /** Number of users of this object */
    TInt iReferenceCount;

    /** Window of visuals in the visible part of the media list (owned) */
    CGlxVisualListWindow* iVisualWindow;

    /** Border brush for visuals (owned) */
        CAlfBorderBrush* iBorderBrush;

    /** Control group for the control (owned) */
        CAlfControlGroup* iControlGroup;

    /** ID of the control group */
    TInt iControlGroupId;

    /** Parent layout for the visuals (owned by HUI) */
        CAlfAnchorLayout* iParentLayout;
    
    /** Current Size of the Parent Layout */
    TSize iCurrentLayoutSize;
   // TGlxBlendLayout iLayoutBlender;
    /** Ref: UI utility */
    CGlxUiUtility*  iUiUtility;	

    /** Blender of multiple MGlxLayouts */
    //MGlxLayout* iLayout;

    /** Array of view contexts (owned) */
    RArray<TContext> iContexts;

    /** ID provider for new view contexts */
    TGlxDefaultIdProvider<TGlxViewContextId> iContextIdProvider;

    /** Time of last movement */
        TTime iLastTime;

    /** Array of observers (owned) */
    RPointerArray<MGlxVisualListObserver> iObservers;

    /// Default scale mode of image visual
        CAlfImageVisual::TScaleMode iScaleMode;

    /** Fetch context for attributes needed for animated GIFs (owned) */
    CGlxDefaultAttributeContext* iAttributeContext; 

    /// Own: the visual icon manager in case default icons are wanted   
    CGlxVisualIconManager* iVisualIconManager;
    
    MGlxLayoutObserver* iLayoutObserver;
    };

#endif // C_GLXVISUALLISTCONTROL_H



