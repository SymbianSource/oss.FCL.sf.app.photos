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
*  Description : Grid view Observers
*
*/


#ifndef GLXGRIDVIEWMLOBSERVER_H_
#define GLXGRIDVIEWMLOBSERVER_H_

// Photos Headers
#include <mglxmedialistobserver.h>

// FORWARD DECLARATIONS
class CGlxMediaList;
class CHgGrid;
class CGlxDRMUtility;
//class CHgContextUtility;

// CLASS DECLARATION
class CGlxGridViewMLObserver: public CBase,
                              public MGlxMediaListObserver

    {
public:

    /**
     * Two-phased constructor.
     *
     * @return Pointer to newly created object.
     */
    IMPORT_C static CGlxGridViewMLObserver* NewL(MGlxMediaList& aMediaList,
            CHgGrid* aHgGrid);

    /**
     * Destructor.
     */
    virtual ~CGlxGridViewMLObserver();

public: // from MGlxMediaListObserver
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
    void HandlePopulatedL( MGlxMediaList* aList );

private:

    /**
     * C++ default constructor.
     */
    CGlxGridViewMLObserver(MGlxMediaList& aMediaList,CHgGrid* aHgGrid);

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

    /**
     * HandleErrorL is added to make the compiler happy by reducing codescanner warnings
     * No extra purpose
     */
    void HandleErrorL();

    /**
     * Checks the availability of relevant thumbnail(Quality or Speed) attribute
     * @param aIndex Media index to check the thumbnail attribute
     * @return ETrue if relevant thumbnail available 
     *         EFalse if relevant thumbnail not available 
     */    
    TBool HasRelevantThumbnail(TInt aIndex);
    
    /**
     * RefreshScreen - Refreshes the Hg Grid based on the attributes available index 
     * 
     */
    void RefreshScreen(TInt aItemIndex,const RArray<TMPXAttribute>& aAttributes);
    
    /**
     * UpdateItemsL - Update the items with the DRM/video icon and  date/time
     * 
     */
    void UpdateItemsL (TInt aItemIndex,const RArray<TMPXAttribute>& aAttributes);
    
    /**
     * DisplayErrorNote - Show an error note based on input parameter
     * @param aError The error based on which the error note is to be shown. 
     */
	void DisplayErrorNote(TInt aError);
    
private:

    MGlxMediaList& iMediaList;
	
    // Instance of HG Grid
    CHgGrid* iHgGrid;

    // Provides DRM related functionality
    CGlxDRMUtility* iDRMUtility;
	
    TSize iGridIconSize;
    
    //No of visible items in a page/view
    TInt  iItemsPerPage;

    // Has the error in disk (e.g disk error) been shown to the user?  
    // If a disk is full the user should not be intimated everytime over and over again.
    TInt  iDiskErrorIntimated;

    // Modified Indexes array to refresh the screen
    RArray<TInt> iModifiedIndexes;

//    CHgContextUtility* iContextUtility;
    
    //Grid Thumbnail Attribs
    TMPXAttribute iQualityTnAttrib;
    TMPXAttribute iSpeedTnAttrib;    
    };
#endif //GLXGRIDVIEWMLOBSERVER_H_
