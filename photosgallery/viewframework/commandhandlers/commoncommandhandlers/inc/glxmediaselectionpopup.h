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
* Description:    Selection popup
*
*/




#ifndef __GLXMEDIASELECTIONPOPUP_H__
#define __GLXMEDIASELECTIONPOPUP_H__

// INCLUDE FILES

#include <aknlists.h>
#include <aknPopup.h>
#include <bamdesca.h>
#include <e32std.h>
#include <eiklbx.h>
#include <glxmediaid.h>
#include <glxmedialistiterator.h>
#include <mglxmedialist.h>
#include <mglxmedialistobserver.h>
#include <mglxmedialistprovider.h>
#include <mpxcollectionobserver.h>
#include <mpxcollectionpath.h>
#include <mpxcollectionutility.h>
#include <mpxmedia.h>
#include <mpxmediaarray.h>

// FORWARD DECLARATION
class CAknPopupList;
class CAknSinglePopupMenuStyleListBox;
class CGlxAttributeContext;
class CGlxMediaSelectionItem;
class CGlxNewMediaPopup;
class CGlxStaticMediaList;

const TInt KMaxTitleStringLength = 80;

/**
 * CGlxMediaListAdaptor
 * 
 * Allows the titles in a media list
 * to be accessed via the MDesCArray interface.
 * The array elements are formatted for use by
 * a CAknSingleGraphicPopupMenuStyleListBox.
 * i.e. "<n>\t Camera Album" where <n> is the index of 
 * an icon in the icon array.
 *
 * @internal reviewed 06/06/2007 by Dave Schofield
 */
NONSHARABLE_CLASS( CGlxMediaListAdaptor )
    : public CBase, public MDesCArray
	{
public:
	/**
	 * Constructor
	 * @param aMediaList a static media list
	 * @param aMultiSelection set multi selection icon indices.
	 */
    IMPORT_C CGlxMediaListAdaptor(const MGlxMediaList* aMediaList, TBool aMultiSelection);

    /**
     * Enabled / Disable the CGlxMediaListAdaptor
     * if the adaptor is disabled, MdcaCount() will return 0
     * even if there are items in the media list.
     * @param aEnabled ETrue to enable EFalse to disable.
     **/
    void SetEnabled(TBool aEnabled);
    
    /**
     * Returns a pointer to the media list.
     */
    const MGlxMediaList* MediaList();
    
    /**
     * Returns ETrue if multi selection is
     * enabled else EFalse.
     */
    TBool MultiSelectionEnabled();
    
    /**
	  * Returns ETrue if a static item is selected
	  * otherwise EFalse.
	  */
    TBool IsStaticItemSelected();
    
    /**
     * Save the status whether selected item is static.
     */
    void SetStaticItemSelected(TBool aSelected);
    
public: // from MDesCArray 
	/** See @ref MDesCArray::MdcaCount */
    IMPORT_C TInt MdcaCount() const;
	/** See @ref MDesCArray::MdcaPoint */
    IMPORT_C TPtrC MdcaPoint(TInt aIndex) const;	
    
private:    
    const MGlxMediaList* iMediaList; // not owned
    
    /**
     * A temporary buffer to store the current title string
     */
    mutable TBuf<KMaxTitleStringLength> iCurrentTitleString;
    
    /**
     * ETrue if multi selection is enabled else EFalse
     */
    TBool iMultiSelection;
    
    /**
     * If ETrue, MdcaCount() returns the number of items in
     * the media list (iMediaList). If EFasle MdcaCount()
     * returns 0.
     */
    TBool iEnabled;
    
    /**
	 * Indicates if a static item is selected.
	 */
	TBool iStaticItemSelected;
	};

/**
 * CGlxSingleGraphicPopupMenuStyleList
 * 
 * Adds behaviour to CAknPopupList:
 * If the currently selected item is user-defined item,
 * the item is marked & 'OK' key is shown; Pressing
 * the OK key will result in the item being actioned.
 * 
 * Else on selecting static item, the input text dialog is shown
 * with default media name, to create a new media.
 * 
 */
NONSHARABLE_CLASS( CGlxSingleGraphicPopupMenuStyleList )
   : public CAknPopupList
    {
public: // new
	
	/**
	 * Two phase constructor
	 */
	static CGlxSingleGraphicPopupMenuStyleList* NewL( 
	   CEikListBox* aListBox, 
	   TInt aCbaResource,
	   AknPopupLayouts::TAknPopupLayouts aType = AknPopupLayouts::EMenuWindow);

public: 
	//From CAknPopupList
	void HandleListBoxEventL (CEikListBox *aListBox, TListBoxEvent aEventType) ;
	void HandlePointerEventL(const TPointerEvent& aPointerEvent);
    };

/**
 * CGlxSingleGraphicPopupMenuStyleList
 * 
 * Adds behaviour to CAknSingleGraphicPopupMenuStyleListBox:
 * For 'Add Album', default functionality is performed.
 * For Tags editor, If a static item or user-defined item is selected 
 * then we show 'OK' & 'Cancel' as SoftKeys(SK). Otherwise 'Cancel' SK.
 * If 'static Item' is selected, sends 'EnterKeyPressed' event to ListBox observer.  
 */
NONSHARABLE_CLASS( CGlxSingleGraphicPopupMenuStyleListBox )
   : public CAknSingleGraphicPopupMenuStyleListBox 					
    {
public:
	/** See @ref CCoeControl::OfferKeyEventL */
    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType); 
    };

/**
 * CGlxMediaSelectionPopup 
 * 
 * Generic Selection popup class definition. Shows a popup from user 
 * can select a media object, or to create a new one. Retrieves
 * popup title and "new item" text from collection plugin,
 * to avoid need for client to configure the class.
 *
 * @image html glx_command_media_selection_popup_class_diagram.png
 * @image rtf glx_command_media_selection_popup_class_diagram.png
 *
 * The media selection popup uses CGlxCommandHandlerNewMedia to create
 * a new media object
 *
 * @image html glx_command_select_media_sequence_diagram.png
 * @image rtf glx_command_select_media_sequence_diagram.png
 *
 * @lib glxcommoncommandhandlers.lib
 *
 * @internal reviewed 06/06/2007 by Dave Schofield
 */
NONSHARABLE_CLASS(CGlxMediaSelectionPopup)
                              : public CActive , 
								public MGlxMediaListObserver, 
								public MListBoxItemChangeObserver,
								public MGlxMediaListProvider
	{
public:
	IMPORT_C CGlxMediaSelectionPopup();
	
    /**
     * Execute the popup.
     * The L in the method name indicates the function might leave.
     * The D in the method name indicates that the method will destroy the object it is called on. i.e 
     * delete the this pointer.
     * @param aPath a path containing the UID of the collection to select items from.
     * @param aAccepted will be set to ETrue if OK was pressed on the dialog else EFalse.
     * @param aMultiSelection Create a multi selection dialog.
     * @param aEnableContainerCreation Enable container creation.
     * @param aFilter Filter items displayed by the popup.
     * @return if aAccepted is ETrue: A CMPXCollectionPath containing the selection 
     *                               (the caller takes ownership of this object).
     *         if aAccepted is EFalse: NULL
     **/
    IMPORT_C CMPXCollectionPath* ExecuteLD(CMPXCollectionPath& aPath, 
                                            TBool& aAccepted, 
                                            TBool aMultiSelection, 
                                            TBool aEnableContainerCreation,
                                            CMPXFilter* aFilter = NULL);
private:
	/**
    * Destructor
    */
    ~CGlxMediaSelectionPopup();

public: // from CActive
     /** See @ref CActive::RunL */
	void RunL();
    /** See @ref CActive::DoCancel */
	void DoCancel();
    
private: // from MListBoxItemChangeObserver
    /** see @ref MListBoxItemChangeObserverL::ListBoxItemsChanged **/
	void ListBoxItemsChanged(CEikListBox* aListBox);

private: // from MGlxMediaListObserver
    /** see @ref MGlxMediaListObserver::HandleItemAddedL **/
	void HandleItemAddedL( TInt aStartIndex, TInt aEndIndex,
                            MGlxMediaList* aList );

    /** see @ref MGlxMediaListObserver::HandleMediaL **/
    void HandleMediaL( TInt aListIndex, MGlxMediaList* aList );

    /** see @ref MGlxMediaListObserver::HandleItemRemovedL **/
    void HandleItemRemovedL( TInt aStartIndex, TInt aEndIndex,
                            MGlxMediaList* aList );

    /** see @ref MGlxMediaListObserver::HandleItemModifiedL **/
    void HandleItemModifiedL( const RArray<TInt>& aItemIndexes, 
                            MGlxMediaList* aList );

    /** see @ref MGlxMediaListObserver::HandleAttributesAvailableL **/
    void HandleAttributesAvailableL( TInt aItemIndex,    
            const RArray<TMPXAttribute>& aAttributes, MGlxMediaList* aList );
    
    /** see @ref MGlxMediaListObserver::HandleFocusChangedL **/
    void HandleFocusChangedL( NGlxListDefs::TFocusChangeType aType,
                    TInt aNewIndex, TInt aOldIndex, MGlxMediaList* aList );

    /** see @ref MGlxMediaListObserver::HandleItemSelectedL **/
    void HandleItemSelectedL( TInt aIndex, TBool aSelected,
                            MGlxMediaList* aList );

    /** see @ref MGlxMediaListObserver::HandleMessageL **/
    void HandleMessageL( const CMPXMessage& aMessage, MGlxMediaList* aList );
    
    /** see @ref MGlxMediaListObserver::HandleError **/
    void HandleError( TInt aError );

private: // from MGlxMediaListProvider
	/** see @ref MGlxMediaList::MediaList **/
	MGlxMediaList& MediaList();
    
private:
	/**
	 * Adds the glxcommoncommandhandlers resource file.
	 */
    void AddResourceFileL();
    
    /**
     * Instantiates the attirbute context and adds it to the
     * static media list.
     */
    void SetupAttributeContextL();

    /**
     * Initialises the icon array.
     */
    void InitIconsL();
    
    /**
     * Adds the static item(s) to the static
     * media list. The static item represent
     * the creation of a new container (album
     * or tag)
     */
    void AddNewMediaCreationItemL();
    
    /**
     * Complete the active object.
     * Invokes an asynchronous call to RunL()
     */
    void CompleteSelf();
    
    /**
     * Fetches the 'New Media Item Title' and the 'Media Popup Title'.
     */
    void FetchTitlesL();

    /**
     * Instantiates the CGlxSingleGraphicPopupMenuStyleList and
     * CAknSingleGraphicPopupMenuStyleListBox
     * @param aMultiSelection Create a multi selection dialog.
     */
    void ConstructPopupListL(TBool aMultiSelection);
    
    /**
     * Retruns ETrue, if item index (i.e 'aIndex') 
     * lies between TOP & BOTTOM item.
     */
    TBool IsListBoxItemVisible(TInt aIndex);
    
private:
	/**
     *  The list of selected items. (Owned)
     */
    CGlxMediaListAdaptor* iMediaListAdaptor; 
	    
    /**
     *  The list box used by popup. (Owned)
     */
    CAknSingleGraphicPopupMenuStyleListBox* iListBox;
    
    /**
     *  The popup list. (Owned)
     */
    CGlxSingleGraphicPopupMenuStyleList* iPopupList;
    
    /**
     * Resource offset
     */
    TInt iResourceOffset;

    /**
     * Media list. (Owned)
     */
    MGlxMediaList* iMediaList;
    
    /**
     * Attribute context. (Owned)
     */
    CGlxAttributeContext* iAttributeContext;
    
    /**
     * Sequential iterator.
     */
    TGlxSequentialIterator iIterator;
    
    /**
     * Enable container creation.
     * ETrue if container creation is enabled.
     */
    TBool iEnableContainerCreation;  
    
    /**
     * Media popup title. (Owned)
     */
    HBufC* iSelectMediaPopupTitle;
    
    /**
     * New media item title. (Owned)
     */
    HBufC* iNewMediaItemTitle;
        
    /**
     * The ID of the collection used to retrieve collection
     * specific attributes.
     */
    TGlxMediaId iCollectionId;
    
    /**
     * A call to HandleItemAdditionL on the list box
     * is required when title attributes become available.
     */
    TBool iHandleItemAdditionRequired;
	};

#endif // __GLXMEDIASELECTIONPOPUP_H__
