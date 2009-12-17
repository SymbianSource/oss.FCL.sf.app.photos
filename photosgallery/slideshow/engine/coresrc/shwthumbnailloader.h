/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    Utility that handles thumbnail loading
 *
*/




#ifndef __CSHWTHUMBNAILLOADER_H__
#define __CSHWTHUMBNAILLOADER_H__

// INCLUDES
#include <e32std.h>
#include <e32base.h>

// FORWARD DECLARATIONS
class MGlxMediaList;

// CLASS DECLARATIONS

/**
 * MShwThumbnailLoadObserver
 * Interface to handle errors in thumbnail loading
 * for the slide show
 */
NONSHARABLE_CLASS( MShwThumbnailLoadObserver )
	{
	public:

		/**
		 * This method gets called when given thumbnail is loaded
		 * @param aIndex the index for which thumbnail is loaded
		 */
		virtual void HandleThumbnailLoadedL( TInt aIndex ) = 0;

		/**
		 * This method gets called when thumbnail loading
		 * fails for a given index
		 * @param aIndex the index for which thumbnail load fails
		 */
		virtual void HandleThumbnailLoadFailureL( TInt aIndex ) = 0;
    
		/**
		 * This method gets called when the media list
		 * becomes empty
		 */
        virtual void HandleMediaListEmpty() = 0;

	};

/**
 * CShwThumbnailLoader
 * Class to handle proper thumbnail loading and image size retrieval
 * for the slide show
 * @lib shwslideshowengine.lib
 */
NONSHARABLE_CLASS( CShwThumbnailLoader ) 
	: public CBase
	{
	public: // Constructors and destructor

		/**
		 * Constructor.
		 * @param aMedialist used for thumbnail loading
		 */
		static CShwThumbnailLoader* NewL( 
			MGlxMediaList& aMedialist,
			MShwThumbnailLoadObserver& aErrorHandler );
		
		/**
		 * Destructor.
		 */
		~CShwThumbnailLoader();

	public: // the API

		/**
		 * This method is used to load a thumbnail and get a callback once it
		 * is loaded. Note! This method is always asynchronous.
		 * @param aIndex the index
		 * @param aSize the thumbnail size
		 */
		void LoadAndNotifyL( TInt aIndex, TSize aSize );
		
		/**
		 * This method is used to unload a thumbnail
		 * @param aIndex the index
		 */
		void Unload( TInt aIndex );
		
		/**
		 * This method retrieves the size of an image for a given index.
		 * It is the caller's responsibility to give a valid index!
		 * If the imagesize is not yet loaded the method returns 
		 * TSize( KErrNotFound, KErrNotFound );
		 * @param aIndex the index
		 * @return the size of the image
		 */
		TSize ImageSizeL( TInt aIndex );
		
	private:	// Data & Implementation

		/// C++ constructor, no derivation intended
		CShwThumbnailLoader();

		/// Own: The implementation details
		class CShwThumbnailLoaderImpl;
		CShwThumbnailLoaderImpl* iImpl;

	};

#endif // __CSHWTHUMBNAILLOADER_H__
