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
 * Description:    Publish the downloads collection content in the photosuite
 *
 */

#ifndef GLXCONTENTHARVESTERPLUGINDOWNLOADS_H_
#define GLXCONTENTHARVESTERPLUGINDOWNLOADS_H_



// INCLUDES
#include <liwcommon.h>

#include <glxmediaid.h>             // for TGlxMediaId
#include <mglxmedialistobserver.h>  // for MGlxMediaListObserver
#include <glxmedialistiterator.h>   // for TGlxSequentialIterator

#include <contentharvesterplugin.h>
#include <contentharvesterpluginuids.hrh>
#include "glxcontentharvesterplugin.h"
#include "glxcontentharvesterpluginbase.h"

// FORWARD DECLARATIONS
class MGlxMediaList;
class CLiwGenericParamList;
class CLiwServiceHandler;
class CPeriodic;
class MLiwInterface;
class CGlxAttributeContext;
class CGlxThumbnailContext;

// CLASS DECLARATION
/**
 *  CP  Content Harvester plugin
 *
 */

class CGlxContentHarvesterPluginDownloads : public CGlxContentHarvesterPluginBase,
public MGlxMediaListObserver, public MLiwNotifyCallback
    {
public: // Constructors and destructor
    /**
     * Two-phased constructor.
     */
    static CGlxContentHarvesterPluginDownloads* NewLC( MLiwInterface* aCPSInterface,
                                                       MGlxContentHarvesterPlugin* aCHplugin );

    /**
     * Destructor.
     */
    virtual ~CGlxContentHarvesterPluginDownloads();

    // from base class 

    /**
     * Here the preview Thumbnails are updated 
     */
    void UpdateDataL(); 

    /**
     * This is a call back from the Matrix Menu whenever there is change in focus
     */
    TInt HandleNotifyL(
            TInt aCmdId,
            TInt aEventId,
            CLiwGenericParamList& aEventParamList,
            const CLiwGenericParamList& aInParamList);

    /**
     * updates  media list with coressponding context depending on the focus  
     * to show the thumbnails
     */
    void UpdatePreviewThumbnailListL( );

    /**
     * Returns the collection items count 
     */      
    TInt Count();
    
private:

    /**
     * Perform the second phase construction 
     */      
    void ConstructL();

    /**
     * Default constructor.
     */
    CGlxContentHarvesterPluginDownloads( MLiwInterface* aCPSInterface,
                                         MGlxContentHarvesterPlugin* aCHplugin );

    /**
     * Update data in Content Publisher Storage
     */
    void UpdateDataInCPSL(TInt aHandle);

    /**
     * This retrievs the bitmap handle and updates the index of the 
     * thumbnail to be shown
     */
    TInt UpdateItem(TInt aItemIndex); 

    /*
     * This Updates the thumbnail/Preview list with thumbnails already present
     */
    void HandleItemChanged();

    /*
     * This Function is called when the collection recives a focus and losses the focus
     * @param aOn :True   : The collection is in focus
     *            : False : The collection lost the focus 
     */
    void ActivateL( TBool aOn );

    /**
     * Delets the medial list created. 
     */
    void DestroyMedialist();

    /**
     * Creates a media list
     */    
    void CreateMedialistL();

    /**
     * Adds the context and observers to the media list  
     */
    void AddContextAndObserverL();

    /**
     * Adds the context and observers to the media list  
     */
    void RemoveContextAndObserver();

    //Call back from the Media list for any error occured 
    void HandleError( TInt /*aError*/ );


private: // from MGlxMediaListObserver

    /**
     * @ref MGlxMediaListObserver::HandleItemAddedL
     */
    void HandleItemAddedL( TInt aStartIndex, TInt aEndIndex, MGlxMediaList* aList );
    /**
     * @ref MGlxMediaListObserver::HandleMediaL
     */
    void HandleMediaL( TInt aListIndex, MGlxMediaList* aList );
    /**
     * @ref MGlxMediaListObserver::HandleItemRemoved
     */
    void HandleItemRemovedL( TInt aStartIndex, TInt aEndIndex, MGlxMediaList* aList );
    /**
     * @ref MGlxMediaListObserver::HandleItemModifiedL
     */
    void HandleItemModifiedL( const RArray<TInt>& aItemIndexes, MGlxMediaList* aList );
    /**
     * @ref MGlxMediaListObserver::HandleAttributesAvailableL
     */
    void HandleAttributesAvailableL(
            TInt aItemIndex, const RArray<TMPXAttribute>& aAttributes, MGlxMediaList* aList );
    /**
     * @ref MGlxMediaListObserver::HandleFocusChangedL
     */
    void HandleFocusChangedL( NGlxListDefs::TFocusChangeType aType, TInt aNewIndex,
            TInt aOldIndex, MGlxMediaList* aList );
    /**
     * @ref MGlxMediaListObserver::HandleItemSelected
     */
    void HandleItemSelectedL( TInt aIndex, TBool aSelected, MGlxMediaList* aList );
    /**
     * @ref MGlxMediaListObserver::HandleMessageL
     */
    void HandleMessageL( const CMPXMessage& aMessage, MGlxMediaList* aList );

private: // data

    /**
     * Instance of CPS interface used for update with CPS.
     */
    MLiwInterface* iCPSInterface;
    CLiwGenericParamList* iInParamList;
    CLiwGenericParamList* iOutParamList;
    CPeriodic* iPeriodic;

    /// Owned: Media list
    MGlxMediaList* iMediaList;
    
    /// Own: Attribute Context
	CGlxAttributeContext* iUriAttributeContext;
	
	CGlxAttributeContext* iThumbnailAttributeContext;

    /// Own: Thumbnail Iterator
    TGlxSequentialIterator iThumbnailIterator;

    RArray<TInt> iPreviewItemCount;

    // Preview thumbnail index
    TInt iProgressIndex;

    //this variable holds the value ,whether the collection is in focus or not
    TBool iIsFocused;
    };

#endif /*GLXCONTENTHARVESTERPLUGINDOWNLOADS_H_*/
