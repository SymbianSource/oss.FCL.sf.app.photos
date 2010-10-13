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
* Description:    Metadata dialog implementation
*
*/


#ifndef C_GLXMETADATACONTAINER_H__
#define C_GLXMETADATACONTAINER_H__ 

// INCLUDES

#include <glxmedialist.h>				    //media list
#include <alf/alfenv.h>
#include <aknsettingitemlist.h>     // for CAknSettingItem
#include "mglxmedialistobserver.h"
#include <glxattributecontext.h>  

/*
 * whenever some operation in details view happens,we need to refresh the toolbar area.
 * Through this observer, we shall notify the main class to refresh the toolbar area.   
 */
class MToolbarResetObserver
    {
public:
    virtual void HandleToolbarResetting(TBool aVisible)= 0;
    };

//FORWARD DECLARATION
class MGlxMediaList;
class CGlxDetailsMulModelProvider;
class MGlxMetadataDialogObserver;
class CGlxCommandHandlerAddToContainer;

#include "glxmedia.h"

// CLASS DECLARATION
/**
 * CGlxMetadataContainer
 * 
 * Metadata Container implementation
 */
NONSHARABLE_CLASS( CGlxMetadataContainer ) : public CAknSettingItemList,
public MGlxMediaListObserver

    {
public:  // Constructors and destructor
    void ViewDynInitMenuPaneL(TInt aResourceId, CEikMenuPane *aMenuPane);
	/**
	* Symbian standard tow phase construction.Construct object of CGlxMetadataContainer object.
	* 
	* @param aRect rect for control
	* @param aMediaList media list
	* @return Metadata container object
	*/
    static CGlxMetadataContainer* NewL( const TRect& aRect,
                                        MGlxMetadataDialogObserver& aDialogObserver,
                                        const TDesC& aUri,MToolbarResetObserver& aResetToolbarObs );
    
    /**
	* Symbian standard tow phase construction.Construct object of CGlxMetadataContainer object.
	* 
	* @param aRect rect for control
 	* @param aMediaList media list
	* @return Metadata container object
	*/  
    static CGlxMetadataContainer* NewLC( const TRect& aRect,
                                         MGlxMetadataDialogObserver& aDialogObserver,
                                         const TDesC& aUri,MToolbarResetObserver& aResetToolbarObs);
	
    /**
    * Destructor
    */       
    ~CGlxMetadataContainer();

public: 
 		
   // @ref CAknSettingItemList::CreateSettingItemL
   CAknSettingItem* CreateSettingItemL(TInt aIdentifier);
   
   // @ref CAknSettingItemList::HandleListBoxEventL
   void HandleListBoxEventL(CEikListBox* aListBox,
                                 TListBoxEvent aEventType);
   void HandleListboxChangesL();
   
   //Enable disble the options based on the current selected item
   TBool IsItemModifiable();
   TBool IsLicenseItem();
   TBool IsLocationItem();
   
   //Delete the location information
   void RemoveLocationL();
   //Change MSK
	void ChangeMskL();
	//Enable marquee support
	void EnableMarqueingL();
private:

		/**
		* C++ constructor.
		* 
		* @param aMediaList media list
		*/
		CGlxMetadataContainer(MGlxMetadataDialogObserver& aDialogObserver,
			MToolbarResetObserver& aResetToolbarObs);
		
		/**
		* Symbian 2nd phase constructor
		* @param aRect rect for this control
		* @para aUri uri for media item
		*/		
		void ConstructL( const TRect& aRect, const TDesC& aUri);
		
		/** 
		*  Create media list with URI filter
		*  This function creates the collection path filter and finally the MediaList 
		*/
		void CreateMediaListForSelectedItemL( );
		/** 
		*  Create media list to get the tags list for the particular Item.
		*  This function creates the collection path filter and finally the MediaList 
		*  
		*/
		void CreateTagsMediaListL();
		/** 
		*  Create media list to get the Albums list for the particular Item.
		*  This function creates the collection path filter and finally the MediaList 
		*
		*  @param aItemURI uri or file name of item
		*/
		void CreateAlbumsMediaListL();	    
		//Set attributes to the settings list once fetched from medialist.
		void SetAttributesL(TMPXAttribute attribute);
		//Set the modifed name or description to the list and MDS
		void SetNameDescriptionL(TInt aItem);
		//Sets the duration and license items based on the selected item
		void SetDurationLIicenseItemVisibilityL();
		//updates the tags item on callback.
		void UpdateTagsL();
		//updates the Albums item on callback.
		void UpdateAlbumsL();
		//Update the details field.
		void EditItemL(TInt aIndex, TBool /*aCalledFromMenu*/);
		/** 
		*  Parse the drive, path & extension from the old uri,
		*  And return the modified uri by appending the new title
		*
		*  @param aTitleText new title entered by user.
		*  @return TFileName as modified uri
		*/
		TFileName ParseFileName(const TDesC& aTitleText);	

		/**
		*  Refresh MediaList with modified FileName.
		*
		*  @param aModifiedUri is the new media uri
		*/
		void RefreshMediaListL(const TDesC& aModifiedUri);

public:
		//MedialistObserver APIS
		void HandleItemAddedL( TInt aStartIndex, TInt aEndIndex, MGlxMediaList* aList );
		void HandleItemRemovedL( TInt aStartIndex, TInt aEndIndex, MGlxMediaList*/* aList */);
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
		void HandleErrorL();
		MGlxMediaList& MediaList();
		void HandleCommandCompleteL(TAny* aSessionId, 
		        CMPXCommand* /*aCommandResult*/, TInt aError, MGlxMediaList* aList);


private: 	//data
    
		MGlxMetadataDialogObserver& iDialogObesrver;
		//Flag to check if the item is a video.
		TBool iVideo;
		//To check if marquee is enabled.
		TBool iMarquee;
		//to set visible duration and license items based on the Item.
		TBool iSetVisible;
		TBool iLocationinfo;
		RBuf iTextSetter;
		RBuf iTagSetter;
		RBuf iAlbumSetter;

		//(Owns) must always represent the media's current uri.
		HBufC* iUri;

		MGlxMediaList* iItemMediaList;
		MGlxMediaList* iTagMediaList;
		MGlxMediaList* iAlbumMediaList;
		CGlxDefaultAttributeContext* iAlbumContext;
		CGlxDefaultAttributeContext* iTagContext;
		TGlxSelectionIterator iSelectionIterator;
		CGlxAttributeContext* iMainListAttributecontext;   
		MToolbarResetObserver& iResetToolbarObs; // observer

    	//Flag to indicate rename command is started
    	TBool iRenameStarted;

	};

#endif //C_GLXMETADATACONTAINER_H__

//End of file

