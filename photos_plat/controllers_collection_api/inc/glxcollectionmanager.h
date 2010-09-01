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




#ifndef __C_GLXCOLLECTIONMANAGER_H__
#define __C_GLXCOLLECTIONMANAGER_H__

#include <e32base.h>
#include <e32std.h>
#include <mpxcollectionobserver.h>

class CGlxCollectionInfo;
class CGlxCollectionManagerImpl;


/**
 * Collection manager allows accessing and managing collections (only albums
 * currently supported). 
 *
 * Only one request may be issued at one time. Functions panic if two requests
 * are issued at the same time
 *
 * The interface is intended to be used by non-MPX applications. 
 * Not intended to be used by Gallery internally, as there are more efficient 
 * and architecturally aligned means existing.
 *
 * @lib glxuiutilities.lib
 */
NONSHARABLE_CLASS( CGlxCollectionManager )
 	: public CBase
	{
public: 
    /**
     * Special collections, for which id is not know by client
     */
    enum TCollection
        {
        /// Camera album 
        ECollectionCameraAlbum
        };
public:
    /**
     * Two-phase constructor
     */
    IMPORT_C static CGlxCollectionManager* NewL();

    /**
     * Destructor
     */
    IMPORT_C ~CGlxCollectionManager();

    /**
     * Adds a file to collection asynchronously.
     * @param aUri URI of file to be added to collection
     * @param aCollection Specific collection to which item should be added
     * @param aRequestStatus Request status that will be signalled when 
     *         request is complete or fails due to an error. aRequestStatus must
     *         be an iStatus member variable of an active object. Using
     *         User::WaitForRequest() will not work.
     */
    IMPORT_C void AddToCollectionL(const TDesC& aUri, 
        TCollection aCollection, TRequestStatus& aRequestStatus);

    /**
     * Adds a file to collection asynchronously.
     * @param aUri URI of file to be added to collection
     * @param aCollectionId MDS ID of collection (album, tag, etc.)
     * @param aRequestStatus Request status that will be signalled when 
     *         request is complete or fails due to an error. aRequestStatus must
     *         be an iStatus member variable of an active object. Using
     *         User::WaitForRequest() will not work.
     */
    IMPORT_C void AddToCollectionL(const TDesC& aUri, 
        TUint32 aCollectionId, TRequestStatus& aRequestStatus);

    /**
     * Retrieves collection info
     * @param aInfo Contains the collection information upon return
     *         Must contain the collection id when called.
     *         Must not be deleted before aRequestStatus is signalled.
     * @param aRequestStatus Request status that will be signalled when 
     *         request is complete or fails due to an error. aRequestStatus must
     *         be an iStatus member variable of an active object. Using
     *         User::WaitForRequest() will not work.
     */
    IMPORT_C void CollectionInfoL(CGlxCollectionInfo& aInfo,
        TRequestStatus& aRequestStatus);

    /**
     * Cancel an ongoing request
     */
    IMPORT_C void Cancel();

private:
	/**
	 * Default constructor.
	 */
	CGlxCollectionManager();
	
	/**
	 * Second-phase constructor
	 */
	void ConstructL();
    
private:
	/**
	 * Internal implementation (owned).
	 */
    CGlxCollectionManagerImpl* iImplementation;
	};

#endif // __C_GLXCOLLECTIONMANAGER_H__