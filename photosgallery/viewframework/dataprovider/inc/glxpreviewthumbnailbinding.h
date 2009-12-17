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
* Description:    Implementation of preview thumbnail binding for the 
*                 list view
*
*/




#ifndef _GLXPREVIEWTHUMBNAILBINDING_H_
#define _GLXPREVIEWTHUMBNAILBINDING_H_


#include "glxbinding.h"              // The binding is created by the binding set factory.
#include "glxmedialistiterator.h"    // Interface for going through items in the list in a  non-sequential order
#include "mglxmedialistobserver.h"   // Observes for changes in media list
#include <memory>                    


class GlxThumbnailVariantType;
class CGlxUiUtility;                  
class CGlxThumbnailContext;          // Fetch context to retrieve thumbnails
    
// CLASS DECLARATION
/**
 * Implementation of preview thumbnail binding for the 
 * list view. 
 * 
 */ 
    
 NONSHARABLE_CLASS(CGlxPreviewThumbnailBinding) : public CGlxSingleTagBinding,
                                                  public MGlxMediaListObserver
 	{
 public:
 
	/**
	  * Two phase construction
	  * @param aMulTag Mul field that the binding corresponds to
	  */
	static CGlxPreviewThumbnailBinding* NewL ();
	
	/**
	 * Two phase construction
	 */
	static CGlxPreviewThumbnailBinding* NewLC( ); // takes ownership

	
	/**
	 * Destructor
	 */
	~CGlxPreviewThumbnailBinding();
	
	/**
	 * HasFirstThumbnail
	 */
	TBool HasFirstThumbnail( const RArray< TMPXAttribute >& aAttributes );
	
private:

	 /**
	 * Constructor
	 */
	CGlxPreviewThumbnailBinding();
	
	/**
	 * Two phase construction
	 */
	void ConstructL();
	

public:	
 	 //From CGlxBinding
      void PopulateT( Alf::MulVisualItem& aItem, const TGlxMedia& aMedia, 
		TBool aIsFocused ) const;
      CGlxBinding::TResponse HandleFocusChanged( TBool aIsGained );
      void HandleItemChangedL(const CMPXCollectionPath& aPath );
      
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
          
      
private:
	
	/**
	 * CreateThumbnail
	 */
	
    std::auto_ptr< GlxThumbnailVariantType > CreateThumbnailL( TInt aIndex )const;
    
	/**
	 * MediaIndexBySlotIndex
	 */
	TInt MediaIndexBySlotIndex( TInt aProgressIndex, TInt aSlotIndex ) const;
	
	/**
	 * IsTime
	 */
	static TInt IsTime( TAny* aSelf );
	
	/**
	 * TimerTicked
	 */
	void TimerTicked();
	
	/**
     * Sets initial thumbnail to visual item depending upon slot
     */
	void SetInitialThumbnailAttributeL(Alf::MulVisualItem& aItem, TInt aSlot ) const;
	
	/**
     * Sets initial thumbnail to visual item simultaneously
     */
	void SetThumbnailAttributeL(Alf::MulVisualItem& aItem ) const;
	
	/**
     * changes the next slot that has to be filled 
     */
	void PreviewSlotIndex( TInt aSlot, TInt aSlotIndex ) const;
	
private:
	
	// Timer that checks for the time expiration
	CPeriodic* iTimer;
	
	//Medialist that holds the thumbnail information
	MGlxMediaList* iMediaList;
	
	// Thumbnail context
	CGlxThumbnailContext* iThumbnailContext;
	
	// for thumbnail context
	TGlxSequentialIterator iThumbnailIterator;
	
	// Preview thumbnail index
	TInt iProgressIndex;
	
	// keeps track of which index to fill with thumbnail
	mutable TInt iSlotIndex;
	
	// Flag that has information whether thumbnail is available
	TBool iStartedShowingThumbnails;
	
	// flag that says whether timer has expired
	TBool iTimerTicked;
	
	// whether current orientation is landscape or potrait
    TBool iCurrentOrientationLandscape;
    
    //it holds the initial number of thumbnail to be displayed
    RArray<TInt> iPreviewItemCount;
    
    // holds how many thumbnail to be shown initially
    TInt iSlots;
	
	CGlxUiUtility* iUiUtility;
	
};
    

#endif //_GLXPREVIEWTHUMBNAILBINDING_H_
