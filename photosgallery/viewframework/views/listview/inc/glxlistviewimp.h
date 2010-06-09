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
* Description:    List view implementation
*
*/





#ifndef C_GLXLISTVIEWIMP_H
#define C_GLXLISTVIEWIMP_H


// INCLUDES
#include "glxlistview.h"
#include "glxpreviewthumbnailbinding.h"
#include "glxmmcnotifier.h"
#include <glxmedialistiterator.h>        // Interface for going through items in
#include <mglxnavigationalstateobserver.h>
#include <ganes/HgScrollBufferObserverIface.h>
#include <ganes/HgSelectionObserverIface.h>

#include <glxprogressindicator.h>
#include <harvesterclient.h>

// FORWARD DECLARATIONS
class MMPXCollectionUtility;
class CGlxPreviewThumbnailBinding;

class CHgDoubleGraphicListFlat;
class CAknsBasicBackgroundControlContext;
class CGlxNavigationalState;

//NOT: This enum is based on TGlxCollectionPluginPriority. If any
//change happen regarding priority order in that, need to change 
//in this enum also.
enum
    {
    EGlxListItemAll = 0,
    EGlxListItemAlbum,
    EGlxLIstItemTag    
    };

// CLASS DECLARATION

/**
 * Implementation of preview list view.  The view shows a vertically scrolling
 * listbox with a highlight on the selected item.  The selected item may also
 * show additional information such as extra text and preview thumbnails.
 *
 * The view uses the mullistwidget 
 *
 * When the list is empty, a static text string is displayed in the centre of
 * the view, and the middle softkey is disabled.
 *
 */

NONSHARABLE_CLASS( CGlxListViewImp ) : public CGlxListView, 
									   public MGlxMediaListObserver,
								       public MHgScrollBufferObserver,
									   public MHgSelectionObserver,
									   public MPreviewTNObserver,
									   public MDialogDismisedObserver,
                                       public MStorageNotifierObserver,
                                       public MGlxNavigationalStateObserver
    {
public:
    /**
     * Two-phased constructor.
     *
     * @param aMediaListFactory object providing a media list
     * @param aViewUid ID of the view to create.
     * @param aResourceIds containing view specific resource ids
     * @param aTitle title of the view
     *
     * @return Pointer to newly created object.
     */
    static CGlxListViewImp* NewL(MGlxMediaListFactory* aMediaListFactory, 
                                 TInt aViewUid, 
                                 const TListViewResourceIds& aResourceIds, 
                                 const TDesC& aTitle);                                 
                                 

    /**
     * Two-phased constructor.
     *
     * @param aMediaListFactory object providing a media list
     * @param aViewUid ID of the view to create.
     * @param aResourceIds containing view specific resource ids
     * @param aTitle title of the view
     *
     * @return Pointer to newly created object.
     */
    static CGlxListViewImp* NewLC(MGlxMediaListFactory* aMediaListFactory, 
                                  TInt aViewUid, 
                                  const TListViewResourceIds& aResourceIds, 
                                  const TDesC& aTitle);

    /**
     * Destructor.
     */
    virtual ~CGlxListViewImp();

    
private:
    /**
     * C++ default constructor.
     *
     * @param aViewUid ID of the view to create.
     * @param aResourceIds containing view specific resource ids
     */
    CGlxListViewImp(TInt aViewUid, const TListViewResourceIds& aResourceIds);

    /**
     * By default Symbian 2nd phase constructor is private.
     *
     * @param aMediaListFactory object providing a media list
     * @param aTitle title of the view
     */
    void ConstructL(MGlxMediaListFactory* aMediaListFactory, const TDesC& aTitle);

    /**
     * @return ID for the control group containing the list control.
     */
    TInt ControlGroupId() const;

    // from base class CAknView
    TUid Id() const;    

    void DoMLViewActivateL(const TVwsViewId& aPrevViewId, TUid aCustomMessageId,
        const TDesC8& aCustomMessage);
		
    void DoMLViewDeactivate();    
    
    TBool HandleViewCommandL(TInt aCommand);

	/**
	 *  HandleStatusPaneSizeChange.
	 *  Called by the framework when the application status pane
	 *  size is changed.
	 */
	void HandleStatusPaneSizeChange ( );

    TTypeUid::Ptr MopSupplyObject(TTypeUid aId);
    
	 /**
	  * Destroys the View Widget and Model for List Widget
	  */
	 void DestroyListViewWidget();

protected:
    void PreviewTNReadyL(CFbsBitmap* aBitmap, CFbsBitmap* aMask);
    	 
protected:
    void Request(TInt aRequestStart, TInt aRequestEnd, THgScrollDirection aDirection);
    void RequestL(TInt aRequestStart, TInt aRequestEnd);
    void Release(TInt aReleaseStart, TInt aReleaseEnd);

protected:
    void HandleSelectL( TInt aIndex );
    void HandleOpenL( TInt aIndex );   
    
private:
    void CreateListL();
    void SetDefaultThumbnailL(TInt aIndex);
    void NavigateToMainListL();
    void HandleMMCInsertionL();
    void HandleMMCRemovalL();
    void HandleNavigationalStateChangedL(){};
    /**
     * Cleans Up the cache for list view
     */    
    void CleanUpL();
    /**
     * Updates the preview thumbnail 
     * @param aIndex index to be updated
     */        
    void UpdatePreviewL(TInt aIndex);
    /**
    * Helper function used by HandleError
    * @param aError the error code to handle.
    */
   void DoHandleErrorL( TInt /*aError*/ );    

   /**
   * Refresh the list
   * @param aIndex index to be refreshed
   */
   void RefreshList(TInt aIndex);

   /**
   * Remove the medialist attribute context(s)
   */
   void RemoveContext();
   
public: // from MGlxMediaListObserver
    void HandleItemAddedL( TInt aStartIndex, TInt aEndIndex, MGlxMediaList* aList );
    void HandleItemRemovedL( TInt aStartIndex, TInt aEndIndex, MGlxMediaList* aList );
    void HandleAttributesAvailableL( TInt aItemIndex, 
        const RArray<TMPXAttribute>& aAttributes, MGlxMediaList* aList );    
	void HandleFocusChangedL( NGlxListDefs::TFocusChangeType aType, 
    TInt aNewIndex, TInt aOldIndex, MGlxMediaList* aList );
    void HandleItemSelectedL(TInt aIndex, TBool aSelected, MGlxMediaList* aList );
    void HandleMessageL( const CMPXMessage& aMessage, MGlxMediaList* aList );
    void HandleError( TInt aError );
    void HandleCommandCompleteL( CMPXCommand* aCommandResult, TInt aError, 
        MGlxMediaList* aList );
    void HandleMediaL( TInt aListIndex, MGlxMediaList* aList );
    void HandleItemModifiedL( const RArray<TInt>& aItemIndexes, MGlxMediaList* aList );
    void HandlePopulatedL( MGlxMediaList* aList );
    void HandleDialogDismissedL(); // from MDialogDismisedObserver
    void HandleForegroundEventL(TBool aForeground); 
private:    // Data

    CAknsBasicBackgroundControlContext* iBgContext; //Own
    
    CHgDoubleGraphicListFlat* iList; //Own
    
	MGlxMediaList* iFocusedChildList;
	
	CGlxPreviewThumbnailBinding* iPreviewTNBinding; //Own
	
	/// Fetch context for retrieving title attribute
	CGlxDefaultAttributeContext* iTitleAttributeContext;

    /// Fetch context for retrieving subtitle for visible items
    CGlxAttributeContext* iSubtitleAttributeContext;

    /// Fetch context for retrieving other attribs for visible items
    CGlxAttributeContext* iOtherAttribsContext;
    
    /// Thumbnail Attrib context for visible items
    CGlxAttributeContext* iThumbnailContext;    

    /// Thumbnail iterator for fetch context(s) for visible items
    TGlxFromFocusOutwardIterator iThumbnailIterator;
    
    /// Fetch context for retrieving subtitle for non-visible items
    CGlxAttributeContext* iNonVisibleSubtitleAttributeContext;

    /// Fetch context for retrieving other attribs for non-visible items
    CGlxAttributeContext* iNonVisibleOtherAttribsContext;
    
    /// Thumbnail Attrib context for non-visible items
    CGlxAttributeContext* iNonVisibleThumbnailContext;    

    /// Thumbnail iterator for fetch context(s) for non-visible items
    TGlxFromFocusOutwardIterator iNonVisibleThumbnailIterator;
    
    /// Unique ID of this Avkon view
    TInt iViewUid;

    /// View specific resource ids
    TListViewResourceIds iResourceIds;

    /// The animation being applied to the view
    MGlxAnimation* iViewAnimation;
    
    // Flag to check if all the items in list view need to be refreshed
    TBool iIsRefreshNeeded;

    TBool iBackwardNavigation;

    TInt iLastFocusedIndex;
    
    TBool iNextViewActivationEnabled;
    
    /// Title text 
    HBufC* iTitletext;

    CGlxProgressIndicator* iProgressIndicator;

    CActiveSchedulerWait* iSchedulerWait;
    
    CGlxMMCNotifier* iMMCNotifier;

    CGlxNavigationalState* iNavigationalState;
	
    TBool iMMCState;

    TSize iGridIconSize;
    };

#endif  // C_GLXLISTVIEWIMP_H

// End of File
