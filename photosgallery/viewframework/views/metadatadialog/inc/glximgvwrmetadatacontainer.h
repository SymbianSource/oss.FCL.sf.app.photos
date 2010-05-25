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
/*
 * glximgvwrmetadatacontainer.h
 *
 *  Created on: Oct 22, 2009
 *      Author: sourbasu
 */

#ifndef GLXIMGVWRMETADATACONTAINER_H_
#define GLXIMGVWRMETADATACONTAINER_H_


#include <glxmedialist.h>                   //media list
#include <alf/alfenv.h>
#include <aknsettingitemlist.h>     // for CAknSettingItem
#include "mglxmedialistobserver.h"
#include <glxattributecontext.h>  
#include <glximageviewermanager.h>

//FORWARD DECLARATION
class MGlxMediaList;
class CGlxDetailsMulModelProvider;
class CGlxCommandHandlerAddToContainer;

#include "glxmedia.h"

// CLASS DECLARATION
/**
 * CGlxImgVwrMetadataContainer
 * 
 * Metadata Container implementation
 */
NONSHARABLE_CLASS( CGlxImgVwrMetadataContainer ) : public CAknSettingItemList,
public MGlxMediaListObserver

    {
public:  // Constructors and destructor
    void ViewDynInitMenuPaneL(TInt aResourceId, CEikMenuPane *aMenuPane);
    /**
    * Symbian standard tow phase construction.Construct object of CGlxImgVwrMetadataContainer object.
    * 
    * @param aRect rect for control
    * @param aMediaList media list
    * @return Metadata container object
    */
    static CGlxImgVwrMetadataContainer* NewL( const TRect& aRect,const TDesC& aUri);
    
    /**
    * Symbian standard tow phase construction.Construct object of CGlxImgVwrMetadataContainer object.
    * 
    * @param aRect rect for control
    * @param aMediaList media list
    * @return Metadata container object
    */  
    static CGlxImgVwrMetadataContainer* NewLC( const TRect& aRect,const TDesC& aUri);
    
    /**
    * Destructor
    */       
    ~CGlxImgVwrMetadataContainer();

public: 
        
   // @ref CAknSettingItemList::CreateSettingItemL
   CAknSettingItem* CreateSettingItemL(TInt aIdentifier);
   
   // @ref CAknSettingItemList::HandleListBoxEventL
   void HandleListBoxEventL(CEikListBox* aListBox,
                                 TListBoxEvent aEventType);
   void HandleListboxChangesL();
   
   //Enable disble the options based on the current selected item
   TBool IsItemModifiable();
   //Change MSK
    void ChangeMskL();
    //Enable marquee support
    void EnableMarqueingL();
    
    
    
    
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

private:

        /**
        * C++ constructor.
        * 
        * @param aMediaList media list
        */
        CGlxImgVwrMetadataContainer(const TDesC& aUri);
        
        /**
        * Symbian 2nd phase constructor
        * @param aRect rect for this control
        */      
        void ConstructL( const TRect& aRect);
        void CreateMediaListForSelectedItemL();
        void SetAttributesL(TMPXAttribute attribute);
        void EditItemL(TInt aIndex, TBool aCalledFromMenu);
        void SetNameDescriptionL(TInt aItem);
        void SetDurationLIicenseItemVisibilityL();
        TBool IsLicenseItem();

        /**
         * Create Image Viewer manager Instance
         */
        void CreateImageViewerInstanceL();
        
        /**
         * Delete Image Viewer manager Instance
         */
        void DeleteImageViewerInstance();

   
private:    //data
    
        //Flag to check if the item is a video.
        TBool iVideo;
        //To check if marquee is enabled.
        TBool iMarquee;
        //to set visible license items based on the Item.
        TBool iSetVisible;
        RBuf iTextSetter;
        const TDesC& iUri ;
        MGlxMediaList* iItemMediaList;
        CGlxAttributeContext* iMainListAttributecontext;
        TGlxSelectionIterator iSelectionIterator;
        // For image viewer, not own
        CGlxImageViewerManager* iImageViewerInstance;
    };



#endif /* GLXIMGVWRMETADATACONTAINER_H_ */
