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
* Description:    Implementation of collection manager API for external use
*
*/




#ifndef __C_GLXCOLLECTIONMANAGERIMPL_H__
#define __C_GLXCOLLECTIONMANAGERIMPL_H__

#include <e32base.h>
#include <e32std.h>
#include <mpxcollectionobserver.h>

#include "glxcollectionmanager.h"

class CGlxCollectionInfo;
class MMPXCollectionUtility;
class CMPXCollectionPath;

/**
 * Collection manager internal implementation.
 *
 * @lib glxuiutilities.lib
 */
NONSHARABLE_CLASS( CGlxCollectionManagerImpl ) 
	: public CBase, public MMPXCollectionObserver
    {
public:
	/**
	 * Constructor
	 */
    CGlxCollectionManagerImpl();

    /**
     * Destructor
     */
    ~CGlxCollectionManagerImpl();

    /**
     * Adds a file to container asynchronously.
     * @param aUri URI of file to be added to the container.
     * @param aContainer Container to which item should be added.
     * @param aRequestStatus Request status that will be signalled when 
     *         request is complete or fails due to an error. aRequestStatus must
     *         be an iStatus member variable of an active object. Using
     *         User::WaitForRequest() will not work.
     */
    void AddToContainerL(const TDesC& aUri, 
    		CGlxCollectionManager::TCollection aContainer, TRequestStatus& aRequestStatus);

    /**
     * Adds a file to container asynchronously.
     * @param aUri URI of file to be added to the container.
     * @param aConatinerId MDS ID of container (album, tag, etc.)
     * @param aRequestStatus Request status that will be signalled when 
     *         request is complete or fails due to an error. aRequestStatus must
     *         be an iStatus member variable of an active object. Using
     *         User::WaitForRequest() will not work.
     */
     void AddToContainerL(const TDesC& aUri, 
        TUint32 aContainerId, TRequestStatus& aRequestStatus);

    /**
     * Retrieves collection info
     * @param aRequestStatus Request status that will be signalled when 
     *         request is complete or fails due asynchronous error
     * @param aInfo Contain the collection information upon return
     *              Must contain the collection info when called
     */
    void CollectionInfoL(CGlxCollectionInfo& aInfo,
        TRequestStatus& aRequestStatus);

    /**
     * Cancel an outstanding request
     */
    void Cancel();

public: // from MMPXCollectionObserver
	/**
	 * See @ref MMPXCollectionObserver::HandleCollectionMessageL
	 */
    void HandleCollectionMessageL(const TMPXCollectionMessage& aMessage);
    
	/**
	 * See @ref MMPXCollectionObserver::HandleCollectionMessageL
	 */
    void HandleCollectionMessageL(const CMPXMessage& aMsg);
    
	/**
	 * See @ref MMPXCollectionObserver::HandleOpenL
	 */
    void HandleOpenL(const CMPXMedia& aEntries,
                             TInt aIndex,TBool aComplete,TInt aError);
	/**
	 * See @ref MMPXCollectionObserver::HandleOpenL
	 */
    void HandleOpenL(const CMPXCollectionPlaylist& aPlaylist ,TInt aError);  
    
	/**
	 * See @ref MMPXCollectionObserver::HandleCollectionMediaL
	 */
    void HandleCollectionMediaL(const CMPXMedia& aMedia, 
            TInt aError);
    
	/**
	 * See @ref MMPXCollectionObserver::HandleCommandComplete
	 */
    void HandleCommandComplete(CMPXCommand* aCommandResult, TInt aError);
    
private:

    /**
     * Initialise the request.
     * Stores a pointer to aRequestStatus and sets aRequestStatus to KRequestPending.
     * @param aRequestStatus request to be signaled when asynchronous
     * operation completes.
     */
	void InitializeRequest(TRequestStatus& aRequestStatus);
	
	/**
	 * Completes the request.
	 * Calls User::RequestComplete() on the request set by InitializeRequest().
	 * @aError error message passed to User::RequestComplete()
	 */
	void CompleteRequest(TInt aError);

	/**
	 * Creates and opens an MPX collection utility.
	 */ 
	void OpenCollectionL();

   /**
     * Closes the MPX collection utitlity.
     */
	void CloseCollection();

    /**
     * Performs the addition of an item to a container.
     * Assumes that the collection is open.
     */
	void DoAddToContainerL();

	/**
	 * Helper method that is called by the two public AddToContainerL overloads
	 * and in turn calls DoAddToContainer.
     * @param aUri URI of file to be added to the container.
     * @param aRequestStatus Request status that will be signalled when 
     *         request is complete or fails due to an error. aRequestStatus must
     *         be an iStatus member variable of an active object. Using
     *         User::WaitForRequest() will not work.
	 */
    void AddToContainerL(const TDesC& aUri, TRequestStatus& aRequestStatus);
    
    /*
     *  Retrieves collection info
     *  Assumes that the collection is open.
     */
    void DoCollectionInfoL();

private:
   /**
     * Collection utitility (onwed)
     */
	MMPXCollectionUtility*  iCollectionUtility;
	
    /**
     * Collection UID.
     */
	TUid iCollectionId;

   /**
     * Container id
     */
	TMPXItemId iContainerId;

   /**
     * Uri (owned)
     */
	HBufC* iUri;

    /**
     * Collection info (not owned)
     */
	CGlxCollectionInfo* iCollectionInfo;
    
	/**
	 * Request status (not owned)
	 */
	TRequestStatus* iRequestStatus;
    
    enum TState
    	{
    	EClosed,
    	EOpeningForAdd,
    	EOpeningForCollectionInfo,
    	EOpening,
    	EOpen,
    	EAdding,
    	ERequestingMedia
    	};
    	
    /** 
     * In the following states an asynchronous request is outstanding:
     * 
     * EOpeningForAdd,
     * EOpeningForCollectionInfo
     * EOpening
     * EAdding
     * ERequestingMedia
     *
     */    
    TState iState;	
    };

#endif // __C_GLXCOLLECTIONMANAGERIMPL_H__
