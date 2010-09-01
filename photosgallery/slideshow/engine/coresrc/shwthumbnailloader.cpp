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
* Description:    Utility that contains HUI related slideshow code
 *
*/




// INCLUDES
#include "shwthumbnailloader.h"

// DEPENDENCIES
#include <mpxattributespecs.h>
#include <mpxmediadrmdefs.h>
#include <glxlog.h>
#include <glxtracer.h>

#include <glxmedialistiterator.h>
#include <mglxmedialistobserver.h>
#include <mglxmedialist.h>
#include <glxthumbnailcontext.h>
#include <glxattributecontext.h>
#include <glxmediageneraldefs.h>
#include <glxattributeretriever.h>

#include "shwslideshowenginepanic.h"
#include "shwthumbnailcontext.h"
#include "shwcallback.h"

#include "shwconstants.h"   // for context priorities

using namespace NShwSlideshow;

/**
 * CShwThumbnailLoaderImpl
 * Implementation dependencies for the thumbnail loading
 */
NONSHARABLE_CLASS( CShwThumbnailLoader::CShwThumbnailLoaderImpl )
	: public CBase,
	public MGlxMediaListObserver
	{
	public:

		/**
		 * Constructor
		 * inlined as only ever called inside this cpp
		 * @param aMedialist the media list
		 */
		inline CShwThumbnailLoaderImpl( 
			MGlxMediaList& aMedialist, 
			MShwThumbnailLoadObserver& aErrorHandler );

		/**
		 * Destructor. 
		 * inlined as only ever called inside this cpp
		 */
		inline ~CShwThumbnailLoaderImpl();

		/**
		 * 2nd phase constructor
		 * inlined as only ever called inside this cpp
		 */
		inline void ConstructL();
		
		/** 
		 * @ref CShwThumbnailLoader::LoadAndNotifyL
		 * inlined as only ever called inside this cpp
		 */
		inline void LoadAndNotifyL( TInt aIndex, TSize aSize );

		/** 
		 * @ref CShwThumbnailLoader::Unload
		 * inlined as only ever called inside this cpp
		 */
		inline void Unload( TInt aIndex );

		/** 
		 * @ref CShwThumbnailLoader::ImageSizeL
		 * inlined as only ever called inside this cpp
		 */
		inline TSize ImageSizeL( TInt aIndex );

	public: // from MGlxMediaListObserver

		/// @ref MGlxMediaListObserver::HandleItemAddedL
		void HandleItemAddedL( 
			TInt aStartIndex, TInt aEndIndex, MGlxMediaList* aList );
		/// @ref MGlxMediaListObserver::HandleMediaL
		void HandleMediaL( 
			TInt aListIndex, MGlxMediaList* aList );
		/// @ref MGlxMediaListObserver::HandleItemRemovedL
		void HandleItemRemovedL( 
			TInt aStartIndex, TInt aEndIndex, MGlxMediaList* aList );
		/// @ref MGlxMediaListObserver::HandleItemModifiedL
		void HandleItemModifiedL( 
			const RArray<TInt>& aItemIndexes, MGlxMediaList* aList );	
		/// @ref MGlxMediaListObserver::HandleAttributesAvailableL
		void HandleAttributesAvailableL(
			TInt aItemIndex, 
			const RArray<TMPXAttribute>& aAttributes, 
			MGlxMediaList* aList );
		/// @ref MGlxMediaListObserver::HandleFocusChangedL
		void HandleFocusChangedL( 
			NGlxListDefs::TFocusChangeType aType, 
			TInt aNewIndex, 
			TInt aOldIndex, 
			MGlxMediaList* aList );
		/// @ref MGlxMediaListObserver::HandleItemSelectedL
		void HandleItemSelectedL( 
			TInt aIndex, TBool aSelected, MGlxMediaList* aList );
		/// @ref MGlxMediaListObserver::HandleMessageL
		void HandleMessageL( 
			const CMPXMessage& aMessage, MGlxMediaList* aList );
		/// @ref MGlxMediaListObserver::HandleError
		void HandleError( TInt aError );

	private: // Implementation

        // implementation of the error handler
        inline void DoHandleErrorL();
		// Helper functions to find an element in the array given an index
		// used in three places, load, unload and handleattributes available
		// @param the index of the element to find
		// @return the index of the element in the array, KErrNotFound otherwise
		inline TInt Find( TInt aIndex );		
		// @return the context if found, NULL otherwise
		inline CShwThumbnailContext* FindContext( TInt aIndex );
		// Helper function to create the size context
		inline void AddSizeContextL();
        /// Helper function to notify client of successfull or failed thumbnail load
        inline void NofifyClientIfInterestedL( TInt aIndex, TBool aSuccess );
		// Helper function to remove high quality context from given index
		inline void RemoveHighQualityContext( TInt aIndex );

	public: // TCallBack API

		// Helper function to notify thumbnail completion asynchronously
		inline TInt CompletedNotifyL();
		// Helper function to notify thumbnail failure asynchronously
		inline TInt ErrorNotifyL();
		// Helper function to handle the error
		inline TInt AsyncErrorHandleL();

	private:	// Implementation & Data

		/// Ref: the media list
		MGlxMediaList& iMedialist;
		/// Ref: the error handler
		MShwThumbnailLoadObserver& iThumbnailObserver;
		/// Own: the array for the thumbnail notifications
		RArray< TInt > iNotificationIndexes;
		/// Own: the array for already completed indexes
		RArray< TInt > iCompletedIndexes;
		/// Own: the array for already faulty indexes
		RArray< TInt > iErrorIndexes;
		/// Own: the array of high quality contexts
		RPointerArray< CShwThumbnailContext > iHighQualityContexts;
		/// Own: the size context
		CGlxDefaultAttributeContext* iSizeContext;
		/// Own: async callback needed in case image is already loaded
		CAsyncCallBack* iCompletedCallBack;
		/// Own: async callback needed in case image had already an error
		CAsyncCallBack* iErrorCallBack;
		/// Own: async callback for showing the out of memory system dialog
		CAsyncCallBack* iErrorHandlerCallBack;
		/// Own: the error code
		TInt iError;

	};

// -----------------------------------------------------------------------------
// C++ Constructor. Save a few bits of rom with inlining
// -----------------------------------------------------------------------------
inline CShwThumbnailLoader::CShwThumbnailLoader()
	{
	}

// -----------------------------------------------------------------------------
// NewL. Static construction
// -----------------------------------------------------------------------------
CShwThumbnailLoader* CShwThumbnailLoader::NewL(
	MGlxMediaList& aMedialist, MShwThumbnailLoadObserver& aErrorHandler )
	{
	TRACER("CShwThumbnailLoader::NewLs");
	CShwThumbnailLoader* self = new( ELeave ) CShwThumbnailLoader;
	CleanupStack::PushL( self );

// 2nd phase >>
	// create implementation
	self->iImpl = 
		new( ELeave ) CShwThumbnailLoaderImpl( aMedialist, aErrorHandler );
	// call 2nd phase
	self->iImpl->ConstructL();
// << 2nd phase
	// pop the stack
	CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
CShwThumbnailLoader::~CShwThumbnailLoader()
	{
	TRACER("CShwThumbnailLoader::~CShwThumbnailLoader");
	delete iImpl;
	}

// -----------------------------------------------------------------------------
// LoadThumbnailAndNotifyL.
// -----------------------------------------------------------------------------
void CShwThumbnailLoader::LoadAndNotifyL( TInt aIndex, TSize aSize )
	{
	TRACER("CShwThumbnailLoader::LoadAndNotifyL");
	// forward
	iImpl->LoadAndNotifyL( aIndex, aSize );
	}

// -----------------------------------------------------------------------------
// Unload.
// -----------------------------------------------------------------------------
void CShwThumbnailLoader::Unload( TInt aIndex )
	{
	TRACER("CShwThumbnailLoader::Unload");
	// forward
	iImpl->Unload( aIndex );
	}

// -----------------------------------------------------------------------------
// CShwThumbnailLoaderImpl::ImageSizeL.
// -----------------------------------------------------------------------------
TSize CShwThumbnailLoader::ImageSizeL( TInt aIndex )
	{
	TRACER("CShwThumbnailLoader::ImageSizeL");
	// forward
	return iImpl->ImageSizeL( aIndex );
	}

// -----------------------------------------------------------------------------
// CShwThumbnailLoaderImpl::C++ Constructor. Save a few bits of rom with inlining
// -----------------------------------------------------------------------------
inline CShwThumbnailLoader::CShwThumbnailLoaderImpl::CShwThumbnailLoaderImpl(
	MGlxMediaList& aMedialist, MShwThumbnailLoadObserver& aErrorHandler )
	: iMedialist( aMedialist ),
	iThumbnailObserver( aErrorHandler )
	{
	TRACER("CShwThumbnailLoader::CShwThumbnailLoaderImpl::CShwThumbnailLoaderImpl()");
	}

// -----------------------------------------------------------------------------
// CShwThumbnailLoaderImpl::Destructor.
// -----------------------------------------------------------------------------
inline CShwThumbnailLoader::CShwThumbnailLoaderImpl::~CShwThumbnailLoaderImpl()
    {
    TRACER("CShwThumbnailLoader::CShwThumbnailLoaderImpl::~CShwThumbnailLoaderImpl()");
    // delete asynch callbacks, delete also cancels them
    delete iCompletedCallBack;
    delete iErrorCallBack;
    delete iErrorHandlerCallBack;

    // size context, remove NULL does nothing
    iMedialist.RemoveContext( iSizeContext );
    delete iSizeContext;

    // Remove all contexts from the media list
    TInt count = iHighQualityContexts.Count();
    while ( count-- > 0 )
        {
        // get the fetch context so that we can remove it from the media list
        MGlxFetchContext* context = iHighQualityContexts[ count ]->Context();
        // high quality context, remove NULL does nothing
        iMedialist.RemoveContext( context );
        };
    // delete the contexts array, this deletes the CShwThumbnailContexts
    iHighQualityContexts.ResetAndDestroy();

    // remove us from media list observers
    iMedialist.RemoveMediaListObserver( this );

    // close notification indexes array
    iNotificationIndexes.Close();
    // close also the already completed indexes array
    iCompletedIndexes.Close();
    // close also the errornous indexes array
    iErrorIndexes.Close();
    }

// -----------------------------------------------------------------------------
// CShwThumbnailLoaderImpl::ConstructL
// -----------------------------------------------------------------------------
inline void CShwThumbnailLoader::CShwThumbnailLoaderImpl::ConstructL()
    {
    TRACER("CShwThumbnailLoader::CShwThumbnailLoaderImpl::ConstructL");
    // retrieve the size of images with own context
    AddSizeContextL();

    // add us as media list observer
    iMedialist.AddMediaListObserverL( this );

    // create async callback object to give 
    // callbacks once scheduler runs, give this high priority so that it 
    // is faster than the timers we have
    iCompletedCallBack = new( ELeave ) CAsyncCallBack( CActive::EPriorityHigh );
    // set the callback
    iCompletedCallBack->Set(TShwCallBack<CShwThumbnailLoaderImpl,
            &CShwThumbnailLoaderImpl::CompletedNotifyL> (this));

    // create async callback to give error callbacks once scheduler runs
    iErrorCallBack = new( ELeave ) CAsyncCallBack( CActive::EPriorityHigh );
    // set the callback
    iErrorCallBack->Set(TShwCallBack<CShwThumbnailLoaderImpl,
            &CShwThumbnailLoaderImpl::ErrorNotifyL> (this));
        
    // create async callback to give callback once we run onto errors
    iErrorHandlerCallBack = new( ELeave ) CAsyncCallBack( CActive::EPriorityHigh );
    // set the callback
    iErrorHandlerCallBack->Set(TShwCallBack<CShwThumbnailLoaderImpl,
            &CShwThumbnailLoaderImpl::AsyncErrorHandleL> (this));
    }

// -----------------------------------------------------------------------------
// CShwThumbnailLoaderImpl::ImageSizeL.
// -----------------------------------------------------------------------------
inline TSize CShwThumbnailLoader::CShwThumbnailLoaderImpl::ImageSizeL( 
	TInt aIndex )
    {
    TRACER("CShwThumbnailLoader::CShwThumbnailLoaderImpl::ImageSizeL");
    GLX_LOG_INFO1( "CShwThumbnailLoader::ImageSizeL for index %d", aIndex );

	// default to KErrNotFound
	TSize size( KErrNotFound, KErrNotFound );

    // get the media item
    TGlxMedia item = iMedialist.Item( aIndex );
    // get its dimensions, if they are not available, size is not modified
    // if size is not available, GetDimensions returns EFalse
    if( !item.GetDimensions( size ) )
        {
        // size was not available so try to fetch it with attribute retriever
        (void)GlxAttributeRetriever::RetrieveL(
            *iSizeContext, iMedialist, 
            EFalse /*aShowDialog*/ );
        // get the media item again since its just a copy and the old one
        // might not have had the CGlxMedia* set
        item = iMedialist.Item( aIndex );
        // cast to (void) tells the compiler we ignore
        // the fetcher error as there is nothing to do if the fetch fails
        // in that case the size will be (KErrNotFound,KErrNotFound)
        // try getting the dimensions again
        (void)item.GetDimensions( size );
        }

    GLX_LOG_INFO2( 
        "CShwThumbnailLoader::ImageSizeL (%d,%d)", size.iWidth, size.iHeight );

    return size;
    }

// -----------------------------------------------------------------------------
// CShwThumbnailLoaderImpl::LoadAndNotifyL.
// -----------------------------------------------------------------------------
inline void CShwThumbnailLoader::CShwThumbnailLoaderImpl::LoadAndNotifyL( 
	TInt aIndex, TSize aSize )
	{
	TRACER("CShwThumbnailLoader::CShwThumbnailLoaderImpl::LoadAndNotifyL");
	GLX_LOG_INFO1( "CShwThumbnailLoader::LoadAndNotifyL %d", aIndex );

	// check whether a context already exists for this index
	CShwThumbnailContext* context = FindContext( aIndex );
	if( !context )
		{
		// create new context for the index with the given size
		context = CShwThumbnailContext::NewLC( aIndex, aSize, iMedialist );
		iHighQualityContexts.AppendL( context );
		CleanupStack::Pop( context );
		// add the context to the medialist with priority
		iMedialist.AddContextL( 
			context->Context(), KHighQualityContextPriority );
		}

    // add the index to notifications array so that we know to give 
    // either success or error notification once for the request
	// Note that we can have multiple requests for same index
	iNotificationIndexes.InsertInOrderAllowRepeatsL( aIndex );

	// need to check if the thumbnail is already loaded
	// when request count is zero (KErrNone) the thumbnail is fully loaded
	TInt requestCount = context->RequestCountL( &iMedialist );
	if( requestCount == KErrNone )
		{
		// thumbnail is already there
		// we cant give the callback right away as the client is expecting
		// this method to be always asynchronous
		GLX_LOG_INFO1( 
			"CShwThumbnailLoader::Thumbnail already loaded %d", aIndex );
    	// insert index to completed array
    	// Note that we can have multiple requests for same index
    	iCompletedIndexes.InsertInOrderAllowRepeatsL( aIndex );
		// cancel the old callback
		iCompletedCallBack->Cancel();
		// make the callback
		iCompletedCallBack->CallBack();
		}
	else if ( requestCount < KErrNone )
		{
		// thumbnail had already and error
		GLX_LOG_INFO2( 
			"CShwThumbnailLoader::Thumbnail error %d at index, %d", 
			    requestCount, aIndex );
		// we cant give the callback right away as the client is expecting
		// this method to be always asynchronous
    	// Note that we can have multiple requests for same index
    	iErrorIndexes.InsertInOrderAllowRepeatsL( aIndex );
		// cancel the old callback
		iErrorCallBack->Cancel();
		// make the callback
		iErrorCallBack->CallBack();
		}
	}

// -----------------------------------------------------------------------------
// CShwThumbnailLoaderImpl::Unload
// -----------------------------------------------------------------------------
inline void CShwThumbnailLoader::CShwThumbnailLoaderImpl::Unload( TInt aIndex )
	{
	TRACER("CShwThumbnailLoader::CShwThumbnailLoaderImpl::Unload");
	GLX_LOG_INFO1( "CShwThumbnailLoader::Unload %d", aIndex );
    
    // client is no longer interested on this indes so remove it from the
    // notifications
	TInt index;
	while( ( index = iNotificationIndexes.Find( aIndex ) ) != KErrNotFound )
		{
		// remove the index from array
		iNotificationIndexes.Remove( index );
		}

    // remove context for this index
    RemoveHighQualityContext( aIndex );
	}

// -----------------------------------------------------------------------------
// CShwThumbnailLoaderImpl::HandleItemAddedL.
// -----------------------------------------------------------------------------
void CShwThumbnailLoader::CShwThumbnailLoaderImpl::HandleItemAddedL(
	TInt /*aStartIndex*/, TInt /*aEndIndex*/, MGlxMediaList* /*aList*/ )
	{
	}

// -----------------------------------------------------------------------------
// CShwThumbnailLoaderImpl::HandleMediaL.
// -----------------------------------------------------------------------------
void CShwThumbnailLoader::CShwThumbnailLoaderImpl::HandleMediaL(
	TInt /*aListIndex*/, MGlxMediaList* /*aList*/ )
	{
	}
// -----------------------------------------------------------------------------
// CShwThumbnailLoaderImpl::HandleItemRemovedL.
// -----------------------------------------------------------------------------
void CShwThumbnailLoader::CShwThumbnailLoaderImpl::HandleItemRemovedL(
	TInt /*aStartIndex*/, TInt /*aEndIndex*/, MGlxMediaList* /*aList*/ )
	{
	TRACER("CShwThumbnailLoader::CShwThumbnailLoaderImpl::HandleItemRemovedL");
	// check if we still got some items to show
	if( iMedialist.Count() < 1 )
	    {
		// the media list is empty so call our error handler
		iThumbnailObserver.HandleMediaListEmpty();
	    }
	}

// -----------------------------------------------------------------------------
// CShwThumbnailLoaderImpl::HandleItemModifiedL.
// -----------------------------------------------------------------------------
void CShwThumbnailLoader::CShwThumbnailLoaderImpl::HandleItemModifiedL( 
	const RArray<TInt>& /*aItemIndexes*/, MGlxMediaList* /*aList*/ )
	{
	}

// -----------------------------------------------------------------------------
// CShwThumbnailLoaderImpl::HandleAttributesAvailableL.
// -----------------------------------------------------------------------------
void CShwThumbnailLoader::CShwThumbnailLoaderImpl::HandleAttributesAvailableL(
	TInt aItemIndex, const RArray<TMPXAttribute>& aAttributes/**/, 
	MGlxMediaList* /*aList*/ )
	{
	TRACER("CShwThumbnailLoader::CShwThumbnailLoaderImpl::HandleAttributesAvailableL");
	//done to verify context availability
	TInt attributeCount = aAttributes.Count();
	for(TInt i = 0; i < attributeCount; i++ )
		{
		TMPXAttribute attr = aAttributes[i];	
		}
		
	GLX_LOG_INFO1( 
	    "CShwThumbnailLoader::HandleAttributesAvailableL %d", aItemIndex );
	// check if the thumbnail was fully loaded, first find the related context
	CShwThumbnailContext* context = FindContext( aItemIndex );
	if( context )
		{
		// Check if there was an error
		TInt requestCount = context->RequestCountL( &iMedialist );
		if ( requestCount == KErrNone )
			{
			// no error, notify client
			GLX_LOG_INFO1( 
				"CShwThumbnailLoader::Thumbnail loaded %d", aItemIndex );
			// notidy client of success if it is interested on this index
			NofifyClientIfInterestedL( aItemIndex, ETrue );
			}
		else if ( requestCount < 0 )
			{
			GLX_LOG_INFO1( 
				"CShwThumbnailLoader::Thumbnail load failed %d", aItemIndex );
    		// remove the context since there is an error in loading it
    		// need to remove first as error handler may add new contexts
    		RemoveHighQualityContext( aItemIndex );
			// notidy client of error if it is interested on this index
			NofifyClientIfInterestedL( aItemIndex, EFalse );
			}
		}
	}

// -----------------------------------------------------------------------------
// CShwThumbnailLoaderImpl::HandleFocusChangedL.
// -----------------------------------------------------------------------------
void CShwThumbnailLoader::CShwThumbnailLoaderImpl::HandleFocusChangedL(
	NGlxListDefs::TFocusChangeType /*aType*/, TInt /*aNewIndex*/, 
	TInt /*aOldIndex*/, MGlxMediaList* /*aList*/ )
	{
	}

// -----------------------------------------------------------------------------
// CShwThumbnailLoaderImpl::HandleItemSelectedL.
// -----------------------------------------------------------------------------
void CShwThumbnailLoader::CShwThumbnailLoaderImpl::HandleItemSelectedL(
	TInt /*aIndex*/, TBool /*aSelected*/, MGlxMediaList* /*aList*/ )
	{
	}

// -----------------------------------------------------------------------------
// CShwThumbnailLoaderImpl::HandleMessageL.
// -----------------------------------------------------------------------------
void CShwThumbnailLoader::CShwThumbnailLoaderImpl::HandleMessageL(
	const CMPXMessage& /*aMessage*/, MGlxMediaList* /*aList*/ )
	{
	}

// -----------------------------------------------------------------------------
// CShwThumbnailLoaderImpl::HandleError.
// -----------------------------------------------------------------------------
void CShwThumbnailLoader::CShwThumbnailLoaderImpl::HandleError(
    TInt aError )
	{
	TRACER("CShwThumbnailLoader::CShwThumbnailLoaderImpl::HandleError");
	GLX_LOG_INFO1( "CShwThumbnailLoader::CShwThumbnailLoaderImpl::HandleError %d", aError );
	// need to remember the error
	iError = aError;
	// make asynch callback since we cant leave, this is the perfect way to 
	// handle error as the real processing will be in RunL so it can leave and
	// can show a system error note. It is safe to call CallBack even is previous
	// one did not complete
    iErrorHandlerCallBack->CallBack();
	}

// -----------------------------------------------------------------------------
// CShwThumbnailLoaderImpl::DoHandleErrorL.
// -----------------------------------------------------------------------------
void CShwThumbnailLoader::CShwThumbnailLoaderImpl::DoHandleErrorL()
	{
	TRACER("CShwThumbnailLoader::CShwThumbnailLoaderImpl::DoHandleErrorL");
	GLX_LOG_INFO1( "CShwThumbnailLoader::DoHandleError %d", iError );
	// variable to tell if the error was in some of our fetch contexts
	TBool errorInHighQualityContexts = EFalse;
	// check if it was any of our high quality fetch contexts 
	TInt count = iHighQualityContexts.Count();
	// ask all high quality contexts
	while ( count-- > 0 )
		{
		CShwThumbnailContext* context = iHighQualityContexts[ count ];
		// need to TRAP in case RequestCountL leaves, since if it does
		// we need to tell our observer that there is a problem
		// requestCount needs to be volatile so that it does get
		// optimized to a register and get its initial value returned in leave
		volatile TInt requestCount = KErrNone;
		TRAPD( error, requestCount = context->RequestCountL( &iMedialist ) );
		// ask if this context had an error or RequestCountL did leave
		if( ( requestCount < KErrNone )|| 
		    ( error != KErrNone ) )
			{
			// take the index as cant use context once it is removed
			TInt index = context->Index();
			// remove the context since there is an error in loading it
    		// need to remove first as error handler may add new contexts
			RemoveHighQualityContext( index );
			// notidy client of error if it is interested on this index
			NofifyClientIfInterestedL( index, EFalse );
			// set the flag to tell that there was an error in context
			errorInHighQualityContexts = ETrue;
			}
		}
	// check if it was out of memory and in our contexts
	if( KErrNoMemory == iError && errorInHighQualityContexts )
	    {
    	// just leave with KErrNoMemory, system will then show the dialog
    	User::Leave( KErrNoMemory );
	    }
	}

// -----------------------------------------------------------------------------
// CShwThumbnailLoaderImpl::Find.
// -----------------------------------------------------------------------------
inline TInt CShwThumbnailLoader::CShwThumbnailLoaderImpl::Find( TInt aIndex )
	{
	TRACER("CShwThumbnailLoader::CShwThumbnailLoaderImpl::Find");
	GLX_LOG_INFO1( "CShwThumbnailLoader::Find %d", aIndex );
	// take the count
	TInt index = iHighQualityContexts.Count();
	// browse through all indexes ( count-1 -> 0 )
	while ( index-- > 0 )
		{
		// Check if the index belongs to the array
		CShwThumbnailContext* context = iHighQualityContexts[ index ];
		if ( context->Index() == aIndex )
			{
			break;
			}
		}
	// if match was not found, index is -1 which is KErrNotFound
	return index;
	}

// -----------------------------------------------------------------------------
// CShwThumbnailLoaderImpl::FindContext.
// -----------------------------------------------------------------------------
inline CShwThumbnailContext* 
	CShwThumbnailLoader::CShwThumbnailLoaderImpl::FindContext( TInt aIndex )
	{
	TRACER("CShwThumbnailLoader::CShwThumbnailLoaderImpl::FindContext");
	GLX_LOG_INFO1( "CShwThumbnailLoader::FindContext %d", aIndex );
	// by default return NULL
	CShwThumbnailContext* context = NULL;
	// reuse the Find
	TInt index = Find( aIndex );
	// if it was found
	if( index != KErrNotFound )
		{
		// set the context
		context = iHighQualityContexts[ index ];
		}
	// and return it
	return context;
	}

// -----------------------------------------------------------------------------
// CShwThumbnailLoaderImpl::AddSizeContextL
// -----------------------------------------------------------------------------
inline void CShwThumbnailLoader::CShwThumbnailLoaderImpl::AddSizeContextL()
	{
	TRACER("CShwThumbnailLoader::CShwThumbnailLoaderImpl::AddSizeContextL");
	// Create the fetch context to retrieve the attribute
	iSizeContext = CGlxDefaultAttributeContext::NewL();
    // Set the range offsets relative to the focus item
    // e.g. 0, 1, [2], 3, 4. Focus @ 2 => frontOffset = 2, rearOffset = 2
	iSizeContext->SetRangeOffsets( KSizeContextOffset, KSizeContextOffset );

	// Add the size attribute to the context
	iSizeContext->AddAttributeL( KGlxMediaGeneralDimensions );

    // Add Drm attribute to the context, 
	// By default Drm attrib would be ETrue if not fetched already and 
	// shall result in reduced thumbnail size request
	iSizeContext->AddAttributeL(KMPXMediaDrmProtected);

	// add the context with its priority
	iMedialist.AddContextL( iSizeContext, KSizeContextPriority );
	}

// -----------------------------------------------------------------------------
// CShwThumbnailLoaderImpl::NofifyClientIfInterestedL.
// -----------------------------------------------------------------------------
void CShwThumbnailLoader::CShwThumbnailLoaderImpl::NofifyClientIfInterestedL(
    TInt aIndex, TBool aSuccess )
    {
    TRACER("CShwThumbnailLoader::CShwThumbnailLoaderImpl::NofifyClientIfInterestedL");
    GLX_LOG_INFO1( "CShwThumbnailLoaderImpl::NofifyClientIfInterestedL %d", aIndex );
	// go through all the notification indexes
	TInt arrayIndex = iNotificationIndexes.Find( aIndex );
	if( KErrNotFound != arrayIndex )
	    {
	    // need to remove the index first as handle may insert new ones
		// remove the index from array so we dont call twice
		iNotificationIndexes.Remove( arrayIndex );
		if( aSuccess )
		    {
    	    // let the client know that the thumbnail is there
    	    iThumbnailObserver.HandleThumbnailLoadedL( aIndex );
		    }
		else
		    {
    	    // let the client know that there was an error
    	    iThumbnailObserver.HandleThumbnailLoadFailureL( aIndex );
		    }
	    }
    }

// -----------------------------------------------------------------------------
// CShwThumbnailLoaderImpl::RemoveHighQualityContext
// -----------------------------------------------------------------------------
inline void CShwThumbnailLoader::CShwThumbnailLoaderImpl::
    RemoveHighQualityContext( TInt aIndex )
    {
    TRACER("CShwThumbnailLoader::CShwThumbnailLoaderImpl::RemoveHighQualityContext");
    GLX_LOG_INFO1( "CShwThumbnailLoaderImpl::RemoveHighQualityContext %d", aIndex );
	// check if there is a context for this index
	TInt arrayIndex = Find( aIndex );
	if( KErrNotFound != arrayIndex )
		{
		// get the context
		CShwThumbnailContext* context = iHighQualityContexts[ arrayIndex ];
		// remove the context from the media list
		iMedialist.RemoveContext( context->Context() );
		// and remove the context from the array
		iHighQualityContexts.Remove( arrayIndex );
		// finally delete the context
		delete context;
		}
    }

// -----------------------------------------------------------------------------
// CShwThumbnailLoaderImpl::CompletedNotifyL
// -----------------------------------------------------------------------------
inline TInt CShwThumbnailLoader::CShwThumbnailLoaderImpl::CompletedNotifyL()
    {
    TRACER("CShwThumbnailLoader::CShwThumbnailLoaderImpl::CompletedNotifyL");
    // then we need to also remove all notifications for the given index
    TInt count = iCompletedIndexes.Count();
    while( count-- > 0 )
        {
        // take the index
        TInt index = iCompletedIndexes[ count ];
        // remove the index from array before calling observer as observer
        // may add new indexes to the array
        iCompletedIndexes.Remove( count );
		// notidy client of success if it is interested on this index
		NofifyClientIfInterestedL( index, ETrue );
        }
    // need to return value to please TCallBack API
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CShwThumbnailLoaderImpl::ErrorNotifyL
// -----------------------------------------------------------------------------
inline TInt CShwThumbnailLoader::CShwThumbnailLoaderImpl::ErrorNotifyL()
    {
    TRACER("CShwThumbnailLoader::CShwThumbnailLoaderImpl::ErrorNotifyL");
    // then we need to also remove all notifications for the given index
    TInt count = iErrorIndexes.Count();
    while( count-- > 0 )
        {
        // take the index
        TInt index = iErrorIndexes[ count ];
        // remove the index from array before calling observer as observer
        // may add new indexes to the array
        iErrorIndexes.Remove( count );
		// remove the context since there is an error in loading it
		// need to remove first as error handler may add new contexts
		RemoveHighQualityContext( index );
		// notidy client of error if it is interested on this index
		NofifyClientIfInterestedL( index, EFalse );
        }
    // need to return value to please TCallBack API
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CShwThumbnailLoaderImpl::AsyncErrorHandleL
// -----------------------------------------------------------------------------
inline TInt CShwThumbnailLoader::CShwThumbnailLoaderImpl::AsyncErrorHandleL()
    {
    TRACER("CShwThumbnailLoader::CShwThumbnailLoaderImpl::AsyncErrorHandleL");
    // handle the error
	DoHandleErrorL();
    // need to return value to please TCallBack API
    return KErrNone;
    }
