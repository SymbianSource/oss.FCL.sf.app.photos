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
 * Description:    Publish the content in the photosuite
 *
 */

#ifndef C_GLXCONTENTHARVESTERPLUGIN_H
#define C_GLXCONTENTHARVESTERPLUGIN_H 

// INCLUDES

#include <liwcommon.h>

#include <mglxmedialistobserver.h>  // for MGlxMediaListObserver
#include <glxmediaid.h>             // for TGlxMediaId

#include <contentharvesterplugin.h>
#include <contentharvesterpluginuids.hrh>

// FORWARD DECLARATIONS

class MGlxMediaList;
class CGlxAttributeContext;
class TGlxSequentialIterator;
class MGlxContentHarvesterPluginEntry;
class CGlxDefaultAttributeContext;
class CMPXCollectionPath;
class CLiwGenericParamList;
class CLiwServiceHandler;


class MGlxContentHarvesterPlugin
    {
public :
    virtual void UpdatePlugins(TBool aValue=ETrue)=0;
    virtual TBool IsRefreshNeeded()=0;
    virtual void SetRefreshNeeded(TBool aRefreshNeed) =0;
    virtual TBool SuiteInFocus()=0;
    };

// CLASS DECLARATION
/**
 *  CP  Content Harvester plugin
 *
 */

class CGlxContentHarvesterPlugin : public CContentHarvesterPlugin, public MGlxMediaListObserver,
                                   public MGlxContentHarvesterPlugin 
    {
public: // Constructors and destructor
    /**
     * Two-phased constructor.
     */
    static CGlxContentHarvesterPlugin* NewL( MLiwInterface* aCPSInterface );

    /**
     * Destructor.
     */
    virtual ~CGlxContentHarvesterPlugin();

    // from base class 

    void UpdateL(); 

private:

    /**
     * Perform the second phase construction 
     */		
    void ConstructL();

    /**
     * Default constructor.
     */
    CGlxContentHarvesterPlugin( MLiwInterface* aCPSInterface );

    void UpdateDataL();

    CMPXCollectionPath* CollectionPathLC ( const TGlxMediaId& aPluginId ) const;

    void UpdateDataInCPSL(TInt aCollection);

    void FillInputListWithDataL(
            CLiwGenericParamList* aInParamList,
            const TDesC& aPublisherId, const TDesC& aContentType, 
            TInt aCollection );

    void CreateContextsL();

    void CreateMedialistL();
       
    /*To update the plugins based on the matrix menu is on foreground or background
     * @parm: aUpdate :if it is true then update all the plugins
     * @parm: aUpdate :if it is false then remove all the observers.
     */
    void UpdatePlugins(TBool aUpdate);
    
    /*
     * Sets if the refresh of the collection is required or not
     * @parm: aUpdate :if it is true,then plugin has to be updated.
     * @parm: aUpdate :if it is false,then plugin need not be updated.
    */
    void SetRefreshNeeded(TBool aRefreshNeed);
    
    /*
     * check if refresh needed flag is set 
    */
    TBool IsRefreshNeeded();
    
    void DestroyMedialist();
    
    /*
     * Check if photossuite is in focus 
     */
    TBool SuiteInFocus();
    
    //Call back from the Media list for any error occured 
    void HandleError( TInt aError );

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

    /// Owned: Media list
    MGlxMediaList* iMediaList;

    /// Own: the attribute context
    CGlxDefaultAttributeContext* iSubtitleAttributeContext;

    // Owned: array of implementation objects
    RPointerArray<MGlxContentHarvesterPluginEntry> iEntries;

    // to check if refresh needed for plugins      
    TBool iIsRefreshNeeded;

    // Collection Indexes array to refresh the screen
    RArray<TInt> iCollectionIndexes;    
    };

#endif // C_GLXCONTENTHARVESTERPLUGIN_H 
