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
* Description:    Cloud view implementation
 *
*/




#ifndef C_GLXCONTAINERINFOBUBBLE_H
#define C_GLXCONTAINERINFOBUBBLE_H

// INCLUDES
#include <mglxmedialistobserver.h> //for base class
#include "glxbubbletimer.h" //for timer functionality
#include "glxinfobubble.h"   //info bubble 


//Forward Declaration
class CGlxDefaultThumbnailContext;
class CGlxDefaultAttributeContext;
class MGlxMediaList;


// CLASS DECLARATION

/**
 *  CGlxContainerInfoBubble
 *  It Fetches attributes from medialist and launch Info bubble 
 *  
 */
class CGlxContainerInfoBubble : public CGlxInfoBubble, public
MGlxTimerNotifier, public MGlxMediaListObserver

	{
public:
	/** 	
	 *  @function NewL
	 *  @since S60 3.2
	 *  Perform the two phase construction
	 *  @param aMediaList - medialist from which attributes will be fetched
	 *  @param aEnv - Env Variable of Alf
	 *  @param aOwnerControl - Owner control which will be the parent of Infobubble
	 *  @return Pointer to newly created object
	 */

	IMPORT_C static CGlxContainerInfoBubble *NewL(MGlxMediaList *aMediaList,
			CAlfEnv &aEnv, CAlfControl &aOwnerControl);

	/** 	
	 *  @function NewLC
	 *  @since S60 3.2
	 *  Perform the two phase construction
	 *  @param aMediaList - medialist from which attributes will be fetched
	 *  @param aEnv - Env Variable of Alf
	 *  @param aOwnerControl - Owner control which will be the parent of Infobubble
	 *  @return Pointer to newly created object
	 */

	IMPORT_C static CGlxContainerInfoBubble *NewLC(MGlxMediaList *aMediaList,
			CAlfEnv &aEnv, CAlfControl &aOwnerControl);

	/**
	 * Virtual Function from MGlxTimernotifier
	 */
	void TimerComplete();

	/**
	 * Destructor
	 */
	~CGlxContainerInfoBubble();

	/** 	
	 *  @function SetFocus
	 *  @since S60 3.2
	 *  @param aPos - Reference position for Info Bubble
	 *  @return none
	 */
	void SetFocus(TPoint aPos);

	/** 	
	 *  @function C++ default constructor
	 *  @param aMediaList - medialist from which attributes will be fetched
	 *  @param aEnv - Env Variable of Alf
	 *  @since S60 3.2
	 */
	CGlxContainerInfoBubble(MGlxMediaList *aMediaList);

	/** 
	 *  @function ConstructL
	 *  @since S60 3.2
	 *  Perform the second phase of two phase construction
	 *  @param aEnv - Env Variable of Alf
	 *  @param aOwnerControl - Owner control which will be the parent of Infobubble
	 */
	void ConstructL(CAlfEnv &aEnv, CAlfControl &aOwnerControl);
	
public:
	//medialist observer 
	/**
	 *  @function HandleItemAddedL
	 *  Notification that media items were added to the list
	 *  @param aStartIndex First item that was added (inserted)
	 *  @param aEndIndex Last item that was added (inserted)
	 *  @param aList List that this callback relates to
	 */
	void HandleItemAddedL(TInt aStartIndex, TInt aEndIndex,
					MGlxMediaList* aList);

	/**
	 *  @function HandleMediaL
	 *  Notification that media object is now available for an item 
	 *  @param Index of the item 
	 *  @param aList List that this callback relates to
	 */
	void HandleMediaL(TInt aListIndex, MGlxMediaList* aList);

	/**
	 *  @function HandleItemRemoved
	 *  Notification that media item was removed from the list
	 *  @param aStartIndex First item that was removed 
	 *  @param aEndIndex Last item that was removed
	 *  @param aList List that this callback relates to
	 */
	void HandleItemRemovedL(TInt aStartIndex, TInt aEndIndex,
			MGlxMediaList* aList);

	/**
	 *  @function HandleItemModifiedL
	 *  Notification that media item was changed
	 *  @param aItemIndexes Indexes of items that were changed
	 *  @param aList List that this callback relates to
	 */
	void HandleItemModifiedL(const RArray<TInt>& aItemIndexes,
			MGlxMediaList* aList);

	/**
	 *  @function HandleAttributesAvailableL
	 *  Notification that an attribute is available
	 *  @param aItemIndex Index of the for which the thumbnail is available
	 *  @param aAttributes Array of attributes that have become available
	 *  @param aList List that this callback relates to 
	 */
	void HandleAttributesAvailableL(TInt aItemIndex,
			const RArray<TMPXAttribute>& aAttributes, MGlxMediaList* aList);

	/**
	 *  @function HandleFocusChangedL
	 *  Notification that focus has moved
	 *  @param aType the direction of the focus change
	 *  @param aNewIndex the new index after the focus change
	 *  @param aOldIndex the old index before the focus change
	 *  @param aList List that this callback relates to
	 */
	void HandleFocusChangedL(NGlxListDefs::TFocusChangeType aType, TInt aNewIndex,
			TInt aOldIndex, MGlxMediaList* aList);

	/**
	 *  @function HandleItemSelected
	 *  Notification that an item has been selected/deselected
	 *  @param aIndex Index of the item that has been selected/deselected
	 *  @param aSelected Boolean to indicate selection/deselection
	 *  @param aList List that the selection relates to
	 */
	void HandleItemSelectedL(TInt aIndex, TBool aSelected, MGlxMediaList* aList);

	/**
	 *  @function HandleMessageL
	 *  Notification from the collection.  E.g. Items added/modified/deleted and progress notifications
	 *  @param aMessage Message notification from the collection
	 *  @param aList List that the notification relates to
	 */
	void HandleMessageL(const CMPXMessage& aMessage, MGlxMediaList* aList);

private:

	/** 	
	 *  @function ShowNow : Calls Info Bubble for the focussed item.
	 *  @since S60 3.2
	 *  @return none
	 */
	void ShowNowL();	
	
	/** 
	 *	Sets the attributes
	 *  @return none
	 */
	void SetAttributesL();
	
    /** 	
    *  Sets the attributes
    *  @ param attributes array
    *  @ param attributes count
    *  @return none
    */	
	void SetAttributesL(const RArray<TMPXAttribute>& aAttributes, const TInt& aAttributescount);

    /** 	
    *  Get the MediaTitle
    *  @param media
    *  @return if the title changed
    */
	TBool GetMediaTitleL(const TGlxMedia& aMedia);
    
    /** 	
    *  Sets the MediaSubTitle
    *  @param media
    *  @return if the subtitle changed
    */
	TBool GetMediaSubTitleL(const TGlxMedia& aMedia);
    
    /** 	
    *  Sets the MediaThumbNail
    *  @param aMedia,media
	*  @param aIdSpaceId,Id-space identifier
	*  @param aSize,thumbnail size
	*  @param thumbnailattribute
    *  @return if new texture was created
    */    
    TBool GetMediaThumbNailL(const TGlxMedia& aMedia, TGlxIdSpaceId& aIdSpaceId, 
        TSize& aSize, TMPXAttribute& aAttrThumbnail);

private:
	/**medialist not owned */
	MGlxMediaList *iMediaList;
	
	/**flag for Timer completion */
	TBool iTimerComplete;
	
	/** iThumbnailContext for getting Thumbnail form medialist*/
	CGlxDefaultThumbnailContext *iThumbnailContext;
	
	/** for getting attribute form medialist*/
	CGlxDefaultAttributeContext *iAttributeContext;
	
	/**position used for Info Bubble */
	TPoint iPos;
	
	/**variable used for providing delay */
	CGlxBubbleTimer *iTimer;	

	/**Stores the first line of text */
	HBufC* iMediaTitle;
	
	/**stores the second line of text */
	HBufC* iMediaSubTitle;
	
	/**Image variable for Thumbnail texture*/
	CAlfTexture* iTexture;
	
	/** The id of the thumbnail texture retrieved from the texture manager */
	TInt iThumbnailId;
	};

#endif // C_GLXCONTAINERINFOBUBBLE_H

// End of File
