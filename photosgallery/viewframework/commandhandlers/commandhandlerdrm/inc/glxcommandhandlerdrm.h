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
* Description:   DRM rights handler 
*
*/




/**
 * @internal reviewed 03/07/2007 by Rowland Cook
 */

#ifndef C_GLXCOMMANDHANDLERDRM_H
#define C_GLXCOMMANDHANDLERDRM_H

// INCLUDES
#include <e32base.h>
#include <glxmedialistcommandhandler.h>

#include <mglxmedialistobserver.h>
#include <mglxvisuallistobserver.h>

// FORWARD DECLARATIONS
class CGlxDRMUtility;
class CGlxDrmIconManager;
class CGlxDefaultAttributeContext;
class MGlxVisualList;
class CGlxMedia;
class CGlxUiUtility;
class CGlxVisualListManager;
// CLASS DECLARATION

//class

/**
 *  CGlxDRMCommandHandler class 
 *  Handler for DRM related commands
 *  @glxdrmcommandhandler.lib
 *  @author M Byrne
 */
NONSHARABLE_CLASS( CGlxCommandHandlerDrm ) : public CGlxMediaListCommandHandler,
                                    public MGlxMediaListObserver,
                                    public MGlxVisualListObserver                                                   
    {
public : //constructor and desctructor
    /**
     * Two-phase constructor
     * @param aMediaListProvider object that provides the media list
     * @param aIsContainerList 
     */
IMPORT_C static CGlxCommandHandlerDrm* NewL(
        MGlxMediaListProvider* aMediaListProvider, TBool aIsContainerList);
        
    /** Destructor */
    IMPORT_C ~CGlxCommandHandlerDrm();
    
    /** See @ref CGlxCommandHandler::DoActivateL */
    void DoActivateL(TInt aViewId);
    
    /** See @ref MGlxCommandHandler::Deactivate */   
    void Deactivate();
    
public: // From MGlxMediaListObserver    
    /// See @ref MGlxMediaListObserver::HandleItemAddedL
    virtual void HandleItemAddedL(TInt aStartIndex, TInt aEndIndex, MGlxMediaList* aList);
    
    /// See @ref MGlxMediaListObserver::HandleMediaL
    virtual void HandleMediaL(TInt aListIndex, MGlxMediaList* aList);
    
    /// See @ref MGlxMediaListObserver::HandleItemRemovedL
    virtual void HandleItemRemovedL(TInt aStartIndex, TInt aEndIndex, MGlxMediaList* aList);
    
    /// See @ref MGlxMediaListObserver::HandleItemModifiedL
    virtual void HandleItemModifiedL(const RArray<TInt>& aItemIndexes, MGlxMediaList* aList);
    
    /// See @ref MGlxMediaListObserver::HandleAttributesAvailableL
    virtual void HandleAttributesAvailableL(TInt aItemIndex,     
        const RArray<TMPXAttribute>& aAttributes, MGlxMediaList* aList);
            
    /// See @ref MGlxMediaListObserver::HandleFocusChangedL
    virtual void HandleFocusChangedL(NGlxListDefs::TFocusChangeType aType, TInt aNewIndex, TInt aOldIndex, MGlxMediaList* aList);
    
    /// See @ref MGlxMediaListObserver::HandleItemSelectedL
    virtual void HandleItemSelectedL(TInt aIndex, TBool aSelected, MGlxMediaList* aList);
    
    /// See @ref MGlxMediaListObserver::HandleMessageL
    virtual void HandleMessageL(const CMPXMessage& aMessage, MGlxMediaList* aList);
    
public: // from MGlxVisualListObserver
    /** @see MGlxVisualListObserver::HandleFocusChangedL */
    void HandleFocusChangedL( TInt aFocusIndex, TReal32 aItemsPerSecond, MGlxVisualList* aList, NGlxListDefs::TFocusChangeType aType );
    /** @see MGlxVisualListObserver::HandleSizeChanged */
    void HandleSizeChanged( const TSize& aSize, MGlxVisualList* aList );
    /** @see MGlxVisualListObserver::HandleVisualRemoved */
    void HandleVisualRemoved( const CAlfVisual* aVisual,  MGlxVisualList* aList );
    /** @see MGlxVisualListObserver::HandleVisualAddedL */
    void HandleVisualAddedL( CAlfVisual* aVisual, TInt aIndex, MGlxVisualList* aList ); 
    
protected:
    /** see CGlxMediaListCommandHandler::DoExecuteL */
    TBool DoExecuteL(TInt aCommandId, MGlxMediaList& aList);
    
    /** @see CGlxMediaListCommandHandler::DoIsDisabled */
    TBool DoIsDisabled(TInt aCommandId, MGlxMediaList& aList) const;
         
private:
    /** Second phase constructor */
    void ConstructL(TBool aIsContainerList);

    /** Constructor */
    CGlxCommandHandlerDrm(MGlxMediaListProvider* aMediaListProvider);
    
    /**
      * Determine if DRM rights need to be consuned based on thumbnail
      * or visual size
      * @param aItemIndex index of item in media list
      * @param aImageSize size of image
      * @param aBitmapSize bitmap size
      * @return ETrue if rights are to be consumed
      */
    TBool ConsumeRightsBasedOnSize(TInt aItemIndex, TSize aImageSize, 
                                                        TSize aBitmapSize);
    
    /**
      * Consume DRM rights if required
      */                                                    
    void ConsumeDRMRightsL();     
    
    /**
      * Get size of thumbnail (assuming largest used if DRM rights 
      * are valid).
      * @param aMedia CGlxMedia item for current item
      * @return TSize of thumbnail
      */
    TSize ThumbnailSize(const CGlxMedia* aMedia);                                                  
     
private:
    /// Resource file offset
    TInt iResourceOffset;
    // Does the list contain containers or items
    TInt iIsContainerList;  
    
    /** Poniter to instance of DRM utility (owned) */
    CGlxDRMUtility* iDrmUtility;    
    
    /** DRM icon manager - owned */
    CGlxDrmIconManager* iDrmIconManager;
    
    /** Fetch context to retrieve DRM releated attributes (owned) */
    CGlxDefaultAttributeContext* iFetchContext;
    
    /** Visual list manager (owned) */
    CGlxVisualListManager* iVisualListManager;
    
    /** Visual list used by DRM command handler (owned) */
    MGlxVisualList* iVisualList;
    
    /** URI for media list item (owned) */
    mutable HBufC*  iUrl;
    
    /** flag to indicate whether full-screen view active
     based on dummy view-state commands */
    TBool iInFullScreen;
    
    /** HUI utility class (owned) */
    CGlxUiUtility* iUiUtility;
    
    /** have we already checked rights for current item */
    TBool iDrmRightsChecked;
    };
    
#endif // C_GLXDRMCOMMANDHANDLER_H



