/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/

/*glxmlwrapper_p.h*/

#ifndef GLXMLWRAPPER_P_H
#define GLXMLWRAPPER_P_H
#include <QObject>
#include <glxfiltergeneraldefs.h>
#include <glxlistdefs.h>
#include <mpxcollectionobserver.h>
#include <qdatetime.h>
#include "glxmedialistiterator.h"
#include "glxmlwrapper.h"
//Forward Declarations
class MGlxMediaList;
class CGlxMLGenericObserver;
class CGlxThumbnailContext;
class HbIcon;
class CGlxDefaultAttributeContext;
class CGlxDefaultThumbnailContext;
class CGlxDefaultListAttributeContext;

//to use first call GlxMLWrapperPrivate::Instance then set the mode by calling GlxMLWrapperPrivate::SetContextMode()
//CLASS Declaration
class GlxMLWrapperPrivate : public QObject
{
    
public:
	/**
     * Two-phased constructor.
     *
     * @return Pointer to newly created object. NULL if creation fails. Avoiding Leave as would be called by a QT wrapper Class
     */
    static GlxMLWrapperPrivate* Instance(GlxMLWrapper* aMLWrapper, int aCollectionId, int aHierarchyId, TGlxFilterItemType aFilterType);								
    /**
     * Destructor.
     */
   ~GlxMLWrapperPrivate();
    /**
     * for setting the attribute context mode will be used mainly for retreiving thumbnail of different sizes.
     */
	 void SetContextMode(GlxContextMode aContextMode);
	 /**
     * for retreiving the Media List Count.
     */
	 int GetItemCount();
	 /*
	  * for retrieving the focus Index
	  */  
	 int GetFocusIndex() const;
	 /*
	  * for setting the focus Index
	  */
	 void SetFocusIndex(int aItemIndex);

	 /*
	  * for setting the focus Index
	  */
	 void SetSelectedIndex(int aItemIndex);

	 /*
	  * for retrieving the visible window Index
	  */  
	 int GetVisibleWindowIndex();
	/*
	 * for setting the visible window Index
	 */
	 void SetVisibleWindowIndex(int aItemIndex);

public: 
    void HandleItemAddedL( TInt aStartIndex, TInt aEndIndex, MGlxMediaList* aList );
    void HandleItemRemovedL( TInt aStartIndex, TInt aEndIndex, MGlxMediaList*/* aList */);
    void HandleAttributesAvailableL( TInt aItemIndex, 
        const RArray<TMPXAttribute>& aAttributes, MGlxMediaList* aList );    
    void HandleFocusChangedL( NGlxListDefs::TFocusChangeType aType, TInt aNewIndex, TInt aOldIndex, MGlxMediaList* aList );
    void HandleItemSelectedL(TInt aIndex, TBool aSelected, MGlxMediaList* aList );
    void HandleMessageL( const CMPXMessage& aMessage, MGlxMediaList* aList );
    void HandleError( TInt aError );
  //todo remove comment  void HandleCommandCompleteL( CMPXCommand* aCommandResult, TInt aError,MGlxMediaList* aList );
    void HandleMediaL( TInt aListIndex, MGlxMediaList* aList );
    void HandleItemModifiedL( const RArray<TInt>& aItemIndexes, MGlxMediaList* aList );

public:
	/**
	* RetrieveItemIcon()
	*/
	HbIcon* RetrieveItemIcon(int index, GlxTBContextType aTBContextType);
	QString RetrieveListTitle(int index);
	QString RetrieveListSubTitle(int index);
	QString RetrieveItemUri(int index);
	QSize   RetrieveItemDimension(int index);
	QDate   RetrieveItemDate(int index);
	int     RetrieveItemFrameCount(int aItemIndex);
	
		
private:

    /**
     * C++ default constructor.
     */
    GlxMLWrapperPrivate(GlxMLWrapper* aMLWrapper);

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL(int aCollectionId, int aHierarchyId, TGlxFilterItemType aFilterType);
	/**
     * Create an instance of Media List.
     *
     * @leave with error code if creation fails.
     */
	void CreateMediaListL(int aCollectionId, int aHierarchyId, TGlxFilterItemType aFilterType);
	void CreateMediaListAlbumItemL(int aCollectionId, int aHierarchyId, TGlxFilterItemType aFilterType);
	void SetThumbnailContextL(GlxContextMode aContextMode);
	void SetListContextL(GlxContextMode aContextMode);
	void CreateGridContextL();
	void CreateLsFsContextL();
	void CreatePtFsContextL();
	void RemoveGridContext();
	void RemoveLsFsContext();
	void RemovePtFsContext();
	void RemoveListContext();
	//for the attribute filtering
	TInt CheckTBAttributesPresenceandSanity(TInt aItemIndex,
		const RArray<TMPXAttribute>& aAttributes, TMPXAttribute aThumbnailAttribute );
	void CheckGridTBAttribute(TInt aItemIndex, const RArray<TMPXAttribute>& aAttributes);
	void CheckPtFsTBAttribute(TInt aItemIndex, const RArray<TMPXAttribute>& aAttributes);
	void CheckLsFsTBAttribute(TInt aItemIndex, const RArray<TMPXAttribute>& aAttributes);
	void CheckListAttributes(TInt aItemIndex, const RArray<TMPXAttribute>& aAttributes);
	/*
	 * convert the CFbsbitmap to HbIcon
	 */
	HbIcon *convertFBSBitmapToHbIcon(CFbsBitmap* aBitmap);
	/*
	 * convert the CFbsbitmap to HbIcon and scale down upto itemWidth and itemHeight
	 */
	HbIcon * convertFBSBitmapToHbIcon(CFbsBitmap* aBitmap, TInt itemWidth, TInt itemHeight);

private:
    // Instance of Media List
    MGlxMediaList* iMediaList;
	//observer for ML
	CGlxMLGenericObserver* iMLGenericObserver;
	GlxContextMode iContextMode;
	//Parent QT based Wrapper
	GlxMLWrapper* iMLWrapper;
	//contexts
	// Thumbnail context
	CGlxThumbnailContext* iGridThumbnailContext;

	// FS related thumbnail contexts
	CGlxDefaultThumbnailContext* iPtFsThumbnailContext;
	CGlxDefaultThumbnailContext* iLsFsThumbnailContext; 
	CGlxDefaultThumbnailContext* iFocusFsThumbnailContext;
    CGlxThumbnailContext* iFocusGridThumbnailContext;

	//List related contexts
	// Fetch context for retrieving title attribute
    CGlxDefaultListAttributeContext* iTitleAttributeContext;

    // Fetch context for retrieving subtitle
    CGlxDefaultListAttributeContext* iSubtitleAttributeContext;
	
    CGlxThumbnailContext* iListThumbnailContext;
    // for thumbnail context
    TGlxFromVisibleIndexOutwardListIterator iThumbnailIterator;

	// Iterators for Grid and FS
    TGlxFromManualIndexBlockyIterator iBlockyIterator;
    TGlxFromManualIndexBlockyIterator iBlockyIteratorForFocus;

	//Variables for checking the active contexts
	//todo merge all these variables into 1 and have bitwise operation on them
	TBool iGridContextActivated;
	TBool iLsFsContextActivated;
	TBool iPtFsContextActivated;
	TBool iLsListContextActivated; //currently not used as we have not implemented the logic for 3 thumbnails
	TBool iPtListContextActivated; 
	TBool iSelectionListContextActivated;
};
#endif //GLXMLWRAPPER_P_H 