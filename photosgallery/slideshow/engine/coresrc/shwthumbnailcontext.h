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
* Description:    Slideshow thumbnail context
 *
*/




#ifndef __SHWTHUMBNAILCONTEXT_H__
#define __SHWTHUMBNAILCONTEXT_H__

// INCLUDES
#include <e32std.h>
#include <e32base.h>

#include <glxmedialistiterator.h>
#include <glxthumbnailcontext.h>

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
 *  CShwThumbnailContext
 *
 */
NONSHARABLE_CLASS( CShwThumbnailContext ) : public CBase,
	public MGlxMediaListIterator
	{
	public:

		/**
		 * Constructor.
           * @param aIndex, the medialist index for the thumbnail to load
           * @param aSize, the size for the thumbnail
		 */
		static CShwThumbnailContext* NewLC( TInt aIndex, TSize aSize );

		/**
		 * Destructor.
		 */
		~CShwThumbnailContext();

	public: // the API
		
		/**
		 * Retrieve the index
		 * @return the index of the media list item associated with this context
		 */
		TInt Index();

		/// @ref MGlxFetchContext::RequestCountL
		TInt RequestCountL(const MGlxMediaList* aList) const;

		/**
		 * Retrieve the context.
		 * @return the context stored in this class
		 */
		MGlxFetchContext* Context();

	public:	// from MGlxMediaListIterator

		/// @ref MGlxMediaListIterator::SetToFirst
		void SetToFirst( const MGlxMediaList* aList );
		/// @ref MGlxMediaListIterator::operator++
		TInt operator++( TInt );
		/// @ref MGlxMediaListIterator::InRange
		TBool InRange( TInt aIndex ) const;
		
	private: // Constructors

        /**
         * C++ constructor.
         */
        CShwThumbnailContext( TInt aIndex, TSize aSize );

        /**
         * 2nd stage constructor
         */
        void ConstructL();
		
	private:

        /// Own: the high quality thumbnail context
        CGlxThumbnailContext* iHighQualityContext;
        /// Own: flag set to ETrue when all attributes have been iterated
        TBool iIterated;
        /// Own: the size of the thumbnail for this context
        TSize iSize;
        /// Own: the currrent focus item
        TInt iCurrentIndex;

	};

#endif // __SHWTHUMBNAILCONTEXT_H__
