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
 * Description:    Grid view control class
 *
 */




#ifndef CGLXGRIDVIEWCONTROL_H
#define CGLXGRIDVIEWCONTROL_H

#include <coecntrl.h>
// Ganes observerers
#include <ganes/HgScrollBufferObserverIface.h>
#include <ganes/HgSelectionObserverIface.h>
#include <ganes/HgMarkingObserverIface.h>

// Photos Headers
#include <mglxmedialistprovider.h>
#include <glxmedialistiterator.h>                       // Interface for going through items in
#include <glxmedialistfactory.h>                        // MGlxMediaListFactory

#include <glxthumbnailcontext.h>
#include <glxuistd.h>
#include <glxprogressindicator.h>

#include "glxgridvieweventobserver.h"
#include "glxmmcnotifier.h"

class CHgGrid;
class CGlxUiUtility;
class CGlxGridViewMLObserver;
class CGlxActiveMediaListRegistry;
class CGlxThumbnailContext;
class CGlxDRMUtility;
class CAknsBasicBackgroundControlContext;
class CGlxNavigationalState;

//class declaration

class CGlxGridViewContainer : 	public CCoeControl,
								public MHgScrollBufferObserver,
								public MHgSelectionObserver,
								public MHgMarkingObserver,
								public MStorageNotifierObserver
	{
public:
	/**
	 * Two-phased constructor.
	 *
	 * @since 3.0
	 * @return Pointer to newly created object.
	 */
	static CGlxGridViewContainer *NewL(MGlxMediaList *aMediaList,CGlxUiUtility* aUiUtility,MGlxGridEventObserver& aObserver,CAknToolbar* aToolbar );

	/**
	 * Two-phased constructor.
	 *
	 * @since 3.0
	 * @return Pointer to newly created object.
	 */
	static CGlxGridViewContainer *NewLC(MGlxMediaList *aMediaList,CGlxUiUtility* aUiUtility,MGlxGridEventObserver& aObserver,CAknToolbar* aToolbar);

	//destructor
	~CGlxGridViewContainer();


	TBool HandleViewCommandL(TInt aCommand);
protected: // from MHgScrollBufferObserver
	void Request(TInt aRequestStart, TInt aRequestEnd, THgScrollDirection aDirection);
	void Release(TInt aReleaseStart, TInt aReleaseEnd);

protected: // from MHgSelectionObserver
	void HandleSelectL( TInt aIndex );
	void HandleOpenL( TInt aIndex );

protected: // from MHgMarkingObserver

	void HandleMarkingL( TInt aIndex, TBool aMarked );

public: // From CCoeControl

	TTypeUid::Ptr MopSupplyObject(TTypeUid aId);
	TKeyResponse OfferKeyEventL(const TKeyEvent &aKeyEvent, TEventCode aType);

	//handle orientaion realted changes
	void HandleResourceChange(TInt aType);

private:
	/**
	 * C++ default constructor.
	 */
	CGlxGridViewContainer(MGlxMediaList *aMediaList,CGlxUiUtility* aUiUtility,MGlxGridEventObserver& aObserver,CAknToolbar* aToolbar);
	/**
	 * By default Symbian 2nd phase constructor is private.
	 */
	void ConstructL();

	void RequestL(TInt aRequestStart, TInt aRequestEnd);

	//create grid
	void CreateGridL();

	/**
	 * Set grid thumbnail context with BlokyIterator to fetch
	 * grid thumbnails
	 */
	void SetGridThumbnailContextL();

	// Create HG Grid widget
	void CreateHgGridWidgetL();

	// Create an observer and pass MediaList and HGGrid objects
	void CreateGridMediaListObserverL();

	// Create Grid once again after returning from FS as No calls for handleItem added.
	void CreateGridAfterFSDeactivatedL();
	/**
	/**
	 * Handle Multiple marking
	 */
	void HandleMultipleMarkingL(TInt aIndex , TBool aMarked );
	/**
	 * Set the Icons
	 */
	void SetIconsL(TInt index);

	//handle enterkey realted events
	void HandleEnterKeyEventL(TInt aCommand);

	/**
	 * Checks the availability of relevant thumbnail(Quality or Speed) attribute
	 * @param aIndex Media index to check the thumbnail attribute
	 * @return ETrue if relevant thumbnail available 
	 *         EFalse if relevant thumbnail not available 
	 */ 
	TBool HasRelevantThumbnail(TInt aIndex);
	/**
     * Calculate and return the client rect for HGgrid
     * Required as the tool bar is dynamically created.
     */
	TRect GetHgGridRect();
	/**
	 * handle mmc insertion event
	 */
	void HandleMMCInsertionL();
	/**
	 * handle mmc removal event
	 */
	void HandleMMCRemovalL();

    /**
     * SetIconL - Sets the grid cell icon with the given parameters
     * @param aItemIndex -  The grid cell index
     * @param aBitmapId - The bitmap id
     * @param aMaskId - The mask id
     */
    void SetIconL(TInt aItemIndex, TInt aBitmapId, TInt aMaskId);

private:
	//medialist
	MGlxMediaList* iMediaList;				    // MGlxMedialist , to Create MediaList

	//Drmutility
	CGlxUiUtility* iUiUtility;

	//No of visible items in a page/view
	TInt  iItemsPerPage;

	// HG  Grid instance
	CHgGrid* iHgGrid;

	// Medialist observer for Grid view
	CGlxGridViewMLObserver* iGlxGridMLObserver;

	// Thumbnail context
	CGlxThumbnailContext* iThumbnailContext;

	// Iterators for Grid and FS
	TGlxFromManualIndexBlockyIterator iBlockyIterator;

	// Grid Icon size from FS view
	TSize iGridIconSize;

    // Hg Grid preferred image size
	TSize iHgGridImageSize;

    // The Resource file name from which icons to be read
    TFileName iIconsFileName;  
	   
	// Bool to check backward navigation
	TBool iBackwardActivation;

	// Provides DRM related functionality
	CGlxDRMUtility* iDRMUtility;

	// Background context for the themes to be used in MopSupplyObject
	CAknsBasicBackgroundControlContext* iBgContext; //Own

	//Grid Thumbnail Attribs
	TMPXAttribute iQualityTnAttrib;
	TMPXAttribute iSpeedTnAttrib;

	// Making the default FSActivation as false
	TBool iFullscreenViewActivated;

	// Navigational state need to be changed while launching fullscreen
	CGlxNavigationalState* iNavigationalstate;

	// making the default mode of marking as false
	TBool iMultipleMarkingActive ;

	//flag to prevent call to HandleOpen( ) on multiple tap for video
	TBool iIsFSVideoViewActivating;

	//observer
	MGlxGridEventObserver& 	iGlxGridViewObserver;
	//toolbar - created dynamically.
    CAknToolbar* iToolbar;
    CGlxProgressIndicator* iProgressIndicator;
    CGlxMMCNotifier* iMMCNotifier;
    TBool iIsDialogLaunched;
    TBool iIsMMCRemoved;
	};

#endif // CGLXGRIDVIEWCONTROL_H
