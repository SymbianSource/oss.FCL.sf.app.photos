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
* Description:    Container class for fetcher plugin
*
*/




#ifndef GLXFETCHERCONTAINER_H_
#define GLXFETCHERCONTAINER_H_

// INCLUDES

// Photos Headers
#include <mglxmedialistprovider.h>
#include <glxmedialistiterator.h>                       // Interface for going through items in
#include <glxmedialistfactory.h>                        // MGlxMediaListFactory

// Ganes Headers
#include <ganes/HgScrollBufferObserverIface.h>          // HG Scrollbuffer interface
#include <ganes/HgSelectionObserverIface.h>             // HG Selection Observer interface
#include <ganes/HgMarkingObserverIface.h>

// User Includes
#include "mglxtitleobserver.h"                          // Title observer

// FORWARD Declaration
class MGlxMediaList;
class MGlxEventObserver;
class CGlxFetcherCommandHandler;
class CGlxGridViewMLObserver;
class CGlxThumbnailContext;
class CHgGrid;
class CGlxUiUtility;
class CGlxDRMUtility;
/**
*  CGlxFetcherContainer
* 
*/
class CGlxFetcherContainer : public CCoeControl,
                             public MGlxTitleObserver,
                             public MGlxMediaListProvider,
                             public MHgScrollBufferObserver,
                             public MHgSelectionObserver,
                             public MHgMarkingObserver
    {
public:

    /**
     * Two-phase constructor NewL
     *
     * @param aFilterType the filter item type
     * @param aTitle the title
     */
	static CGlxFetcherContainer* NewL(
	    TGlxFilterItemType aFilterType ,const TDesC& aTitle, 
	                MGlxEventObserver& aEventObserver, TBool aMultiSelectionEnabled );

	/** Destructor 
	 *
	 */
	~CGlxFetcherContainer();
	
    /** 
     *  RetrieveUrisL
     */
     TBool RetrieveUrisL( CDesCArray& aSelectedFiles,TBool& aFetchUri );
    
    /** 
     *  DoExecuteL
     */
     TBool DoExecuteL(TInt aCommandId);
     
     /** 
     *  OfferKeyEventL, from CCoeControl
     */
     TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType );	

     /** 
     *  CreateAndDisplayGridL
     */
     void CreateAndDisplayGridL( ); 
     
     /** 
      *  SetPreviousTitleL
      *  Sets the status pane title with what it was for the calling application
      */
     void SetPreviousTitleL();
     
     /** 
      *  SetFileAttached
      *  Sets the File attached flag.
      */
     void SetFileAttached(TBool aIsFileAttached);
     
public:     
     /** 
      *  HandleSizeChange
      */
     void HandleSizeChangeL ( );
     
     /*
      * MopSupplyObject
      * Framework call for the skin updates 
      */
     TTypeUid::Ptr MopSupplyObject(TTypeUid aId);
     
public:// From MGlxTitleObserver

    void UpdateTitleL();	      

public:
    MGlxMediaList& MediaList();
    
protected: // from MHgScrollBufferObserver
    void Request(TInt aRequestStart, TInt aRequestEnd, THgScrollDirection aDirection);
    void Release(TInt aReleaseStart, TInt aReleaseEnd);

protected: // from MHgMarkingObserver
    void HandleMarkingL( TInt aIndex, TBool /*aMarked*/ );

protected: // from MHgSelectionObserver
    void HandleSelectL( TInt aIndex );
    void HandleOpenL( TInt aIndex );

private:
    /** CreateMediaListL
     *  This function creates the collection path filter and finally the MediaList 
     */
    void CreateMediaListL();
   
    /** 
     *  2nd phase Constructor
     */
	void ConstructL();

    /** 
     *  Constructor
     */
	CGlxFetcherContainer( TGlxFilterItemType aFilterType, const TDesC& aTitle ,
	        MGlxEventObserver& aEventObserver, TBool aMultiSelectionEnabled);
	
    /** 
     *  SetTitleL
     */
    void SetTitleL(const TDesC& aTitleText);

    /** 
     *  Create Grid Widget
     */
	void CreateHgGridWidgetL();

    /** 
     *  Create The Medialist observer 
     *  An interface required for the HG Grid data updates with the photos data 
     *  Taken from GlxGridview
     */
	void CreateGridMediaListObserverL();
	
	/*
	 * HandleMultipleMarkingL
	 * To handle multiple marking, if marked then unmark, else vice versa
	 */
	void HandleMultipleMarkingL(TInt aIndex);
	
    /**
     * Set the Icons
     */
    void SetIconsL(TInt aIndex);
	
    /**
     * This doesnt add up any value to the code, just to satisfy the compiler
     */
    void RequestL(TInt aRequestStart, TInt aRequestEnd);
    
private:
    MGlxMediaList* iMediaList;				    // MGlxMedialist , to Create MediaList
    CHgGrid* iHgGrid;                           // HGGrid 
    CGlxGridViewMLObserver* iGlxGridMLObserver; // Medilaist observer from GlxGridview
    // Iterator 
    TGlxFromManualIndexBlockyIterator iBlockyIterator; 
    CAknsBasicBackgroundControlContext* iBgContext; //Own for background context
    
	TGlxFilterItemType iFilterType;             // Filtertype, Image /Video

    const TDesC& iTitle;                        // To Store the title 
    HBufC* iPreviousTitle;                      // Previous Title
    
    // CommandHandler to handle marking for multiple fetcher 
    // (Owned)
    CGlxFetcherCommandHandler* iFetcherCommandHandler;
    MGlxEventObserver& iEventObserver;              // Event Observer to pass doubletap event to dialog

    // Thumbnail context
    CGlxThumbnailContext* iThumbnailContext;
    TBool iMultiSelectionEnabled;                   // Flag to notify if multiple fetcher called
    TBool iMultipleMarkNotStarted;                  // Multiple mark started flag
    
    CGlxUiUtility* iUiUtility;

    // Provides DRM related functionality
    CGlxDRMUtility* iDRMUtility;

    // Grid Icon size from FS view
    TSize iGridIconSize;
    //Is File Attached
	TBool iIsFileAttached;
    };

#endif /*GLXFETCHERCONTAINER_H_*/
