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
* Description:    Slideshow specific thumbnail context
 *
*/




// INCLUDES
#include "shwthumbnailcontext.h"

#include <glxthumbnailinfo.h>
#include <glxthumbnailattributeinfo.h>
#include <glxerrormanager.h>
#include <mglxmedialist.h>

#include <glxlog.h>
#include <glxtracer.h>

// DEPENDENCIES

// -----------------------------------------------------------------------------
// Constructor.
// -----------------------------------------------------------------------------
inline CShwThumbnailContext::CShwThumbnailContext(TInt aIndex, TSize aSize,
        MGlxMediaList& aList) :
    iSize(aSize), iCurrentIndex(aIndex), iList(aList)
    {
    }

// -----------------------------------------------------------------------------
// NewLC.
// -----------------------------------------------------------------------------
CShwThumbnailContext* CShwThumbnailContext::NewLC(TInt aIndex, TSize aSize,
        MGlxMediaList& aList)
    {
    TRACER("CShwThumbnailContext::NewLC");
    GLX_LOG_INFO1( "CShwThumbnailContext::NewL, aIndex=%d", aIndex );
    CShwThumbnailContext* self =
        new (ELeave) CShwThumbnailContext( aIndex, aSize, aList );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// ConstructL.
// -----------------------------------------------------------------------------
void CShwThumbnailContext::ConstructL()
    {
    TRACER("CShwThumbnailContext::ConstructL");
    // Create the high quality / slower context
    iHighQualityContext = CGlxThumbnailContext::NewL( this );
    // Call both setdefault and add
    iHighQualityContext->SetDefaultSpec( iSize.iWidth, iSize.iHeight );
    iHighQualityContext->AddSpecForItemL( iSize.iWidth, iSize.iHeight,
        iCurrentIndex );
    // we want to only load high quality with this context
    iHighQualityContext->SetHighQualityOnly( ETrue );
    }

// -----------------------------------------------------------------------------
// Destructor.
// -----------------------------------------------------------------------------
CShwThumbnailContext::~CShwThumbnailContext()
    {
    TRACER("CShwThumbnailContext::~CShwThumbnailContext");
    delete iHighQualityContext;
    }

// -----------------------------------------------------------------------------
// SetToFirst.
// -----------------------------------------------------------------------------
void CShwThumbnailContext::SetToFirst( const MGlxMediaList* /*aList*/ )
    {
    TRACER("CShwThumbnailContext::SetToFirst");
    // reset iterator state
    iIterated = EFalse;
    }

// -----------------------------------------------------------------------------
// CShwThumbnailLoaderImpl::operator++.
// -----------------------------------------------------------------------------
TInt CShwThumbnailContext::operator++( TInt )
    {
    TRACER("CShwThumbnailContext::operator++");
    // we want to load all the thumbnails our clients have requested 
    // notification on, nothing else
    // by default tell the thumbnail that we dont have any interesting indexes
    TInt wantedIndex = KErrNotFound;
    TInt count = iList.Count();
    // check if we were asked already and guard conditions
    if( !iIterated && (count > 0) && (iCurrentIndex < count))
        {        
        wantedIndex = iCurrentIndex;
        iIterated = ETrue;
        }
    // finally return the index that was wanted
    GLX_LOG_INFO1("CShwThumbnailContext::operator++ wantedIndex %d",wantedIndex);
    return wantedIndex;
    }

// -----------------------------------------------------------------------------
// InRange.
// -----------------------------------------------------------------------------
TBool CShwThumbnailContext::InRange( TInt aIndex ) const
    {
    TRACER("CShwThumbnailContext::InRange");
    // if we got notifications
    if ( aIndex == iCurrentIndex )
        {
        // is in range so dont remove the item :)
        // cache removes thumbnails that are no longer in range
        return ETrue;
        }
    // not in our range
    return EFalse;
    }

// -----------------------------------------------------------------------------
// RequestCountL.
// -----------------------------------------------------------------------------
TInt CShwThumbnailContext::RequestCountL(const MGlxMediaList* aList) const
    {
    TRACER("CShwThumbnailContext::RequestCountL");
    return iHighQualityContext->RequestCountL( aList );
    }

// -----------------------------------------------------------------------------
// Index.
// -----------------------------------------------------------------------------
TInt CShwThumbnailContext::Index()
    {
    TRACER("CShwThumbnailContext::Index");
    GLX_LOG_INFO1( "CShwThumbnailContext::Index %d", iCurrentIndex );
    return iCurrentIndex;
    }

// -----------------------------------------------------------------------------
// Context.
// -----------------------------------------------------------------------------
MGlxFetchContext* CShwThumbnailContext::Context()
	{
	TRACER("CShwThumbnailContext::Context");
	return iHighQualityContext;
	}
