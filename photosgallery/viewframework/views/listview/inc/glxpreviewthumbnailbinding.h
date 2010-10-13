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
*  Description : Class header for preview thumbnail in list view
*
*/


#ifndef _GLXPREVIEWTHUMBNAILBINDING_H_
#define _GLXPREVIEWTHUMBNAILBINDING_H_


#include "glxbinding.h"              // The binding is created by the binding set factory.
#include "glxmedialistiterator.h"    // Interface for going through items in the list in a  non-sequential order
#include "mglxmedialistobserver.h"   // Observes for changes in media list
#include <memory>                    

class GlxThumbnailVariantType;
class CGlxThumbnailContext;          // Fetch context to retrieve thumbnails

/**
 * Observer interface
 */
class MPreviewTNObserver 
    {
public:
    virtual void PreviewTNReadyL(CFbsBitmap* aBitmap, CFbsBitmap* aMask) = 0;
    };
    
// CLASS DECLARATION
/**
 * Implementation of preview thumbnail binding for the 
 * list view. 
 * 
 */ 
    
 NONSHARABLE_CLASS(CGlxPreviewThumbnailBinding) : public CBase,
                                                  public MGlxMediaListObserver
 	{
 public:
 
	/**
	  * Two phase construction
	  * @param aMulTag Mul field that the binding corresponds to
	  */
	static CGlxPreviewThumbnailBinding* NewL (MPreviewTNObserver& aObserver);
	
	/**
	 * Two phase construction
	 */
	static CGlxPreviewThumbnailBinding* NewLC(MPreviewTNObserver& aObserver ); // takes ownership

	
	/**
	 * Destructor
	 */
	~CGlxPreviewThumbnailBinding();
	
private:

	 /**
	 * Constructor
	 */
	CGlxPreviewThumbnailBinding(MPreviewTNObserver& aObserver);
	
	/**
	 * Two phase construction
	 */
	void ConstructL();
	

public:
	
	/**
     * Scale the source bitmap to list size
     * aSrcBitmap - Source bitmap
     * aDestBitmap - Scaled destination bitmap
     */
      void ScaleBitmapToListSizeL(CFbsBitmap* aSrcBitmap, CFbsBitmap* aDestBitmap);
	
      void HandleItemChangedL(const CMPXCollectionPath& aPath, 
              TBool aIsRefreshNeeded, TBool aBackwardNavigation);
      void StartTimer();
      void StopTimer();
      
      // From MGlxMediaListObserver
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
     
private:
	/**
	 * IsTimeL
	 */
	static TInt IsTimeL( TAny* aSelf );
	
	/**
	 * TimerTickedL
	 */
	void TimerTickedL();
	
private:
	MPreviewTNObserver& iObserver;	
	
	// Timer that checks for the time expiration
	CPeriodic* iTimer;
	
	//Medialist that holds the thumbnail information
	MGlxMediaList* iMediaList;
	
	// Thumbnail Attrib context
    CGlxAttributeContext* iThumbnailContext;	
	
	// for thumbnail context
	TGlxSequentialIterator iThumbnailIterator;
		
	// flag that says whether timer has expired
	TBool iTimerTicked;
	
	TBool iIsRefreshNeeded;
    
    TSize iGridIconSize;
    
    // Holds the numbers of trials count,after which PreviewTNReady callback
    // has to be called
    TInt iTrialCount;
        
    CMPXFilter* iPreviewFilter;
};

#endif //_GLXPREVIEWTHUMBNAILBINDING_H_