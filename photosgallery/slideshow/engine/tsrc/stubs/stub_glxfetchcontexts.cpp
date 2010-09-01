/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    Stubs for thumbnail and attribute context
 *
*/




#include "stub_glxfetchcontexts.h"

// this flag defines the return value for CGlxThumbnailContext::RequestCountL
extern TInt gThumbnailContextRequestCount = 1;
// this flag defines the return value for CGlxAttributeContext::RequestCountL
extern TInt gAttributeContextRequestCount = 1;
// this flag defines the return value for GlxAttributeRetriever::RetrieveL
extern TInt gGlxAttributeRetrieverRetrieveL = 1;
// this flag defines the return value for TGlxMedia::GetDimensions
extern TBool gTGlxMediaGetDimensions = ETrue;
// this flag defines if the test is meant to be an alloc test
extern TBool gFetchContextAllocTest = EFalse;

// helper macro to do an alloc if we are in alloc test
#define MAKE_ALLOC_L \
	if( gFetchContextAllocTest ) \
		{ TInt* alloc = new( ELeave ) TInt; \
		delete alloc; }

// -----------------------------------------------------------------------------
// Stub for GlxAttributeRetriever -->
// -----------------------------------------------------------------------------
EXPORT_C TInt GlxAttributeRetriever::RetrieveL(
    const MGlxFetchContext& /*aContext*/, 
    MGlxMediaList& /*aList*/, 
    TBool /*aShowDialog*/ )
    {
    return gGlxAttributeRetrieverRetrieveL;
    }
	

// -----------------------------------------------------------------------------
// <-- Stub for GlxAttributeRetriever
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Stub for TGlxMedia -->
// -----------------------------------------------------------------------------
EXPORT_C TBool TGlxMedia::GetDimensions( TSize& /*aSize*/ ) const
    {
    return gTGlxMediaGetDimensions;
    }
// -----------------------------------------------------------------------------
// <-- Stub for TGlxMedia
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Stub for CGlxThumbnailContext  -->
// -----------------------------------------------------------------------------
CGlxThumbnailContext* CGlxThumbnailContext::NewL(
	MGlxMediaListIterator* aIterator )
	{
	return new( ELeave ) CGlxThumbnailContext( aIterator );
	}

CGlxThumbnailContext::CGlxThumbnailContext(
	MGlxMediaListIterator* /*aIterator*/ )
	{
	}

CGlxThumbnailContext::~CGlxThumbnailContext()
	{
	}
	
void CGlxThumbnailContext::AddSpecForItemL(
	TInt /*aWidth*/, TInt /*aHeight*/, TInt /*aFocusOffset*/ )
	{
	MAKE_ALLOC_L
	}

void CGlxThumbnailContext::SetDefaultSpec( 
	TInt /*aWidth*/, TInt /*aHeight*/ )
	{
	}

void CGlxThumbnailContext::SetHighQualityOnly( TBool /*aHighQualityOnly*/ )
    {
    }

// From MGlxFetchContext
void CGlxThumbnailContext::AllAttributesL(
	const MGlxMediaList* /*aList*/, 
	TInt /*aListIndex*/, 
	RArray<TMPXAttribute>& /*aAttributes*/ ) const
	{
	MAKE_ALLOC_L
	}

TInt CGlxThumbnailContext::AttributeRequestL(
	const MGlxMediaList* /*aList*/, 
	RArray<TInt>& /*aItemIndices*/, 
	RArray<TMPXAttribute>& /*aAttributes*/, 
	CMPXAttributeSpecs*& /*aDetailedSpecs*/ ) const
	{
	MAKE_ALLOC_L
	return 0;
	}

TInt CGlxThumbnailContext::RequestCountL(
	const MGlxMediaList* /*aList*/) const
	{
	MAKE_ALLOC_L
	return gThumbnailContextRequestCount;
	}

void CGlxThumbnailContext::HandleResolutionChanged()
    {
    }

TInt CGlxThumbnailContext::SelectItemL(
	const MGlxMediaList* /*aList*/, 
	TInt& /*aError*/) const
	{
	MAKE_ALLOC_L
	return 0;
	}

TUint CGlxThumbnailContext::GetThumbnailScoreL(
	TInt /*aIndexInList*/, 
	const MGlxMediaList* /*aList*/, 
	TInt /*aDistance*/, 
	TInt& /*aError*/) const
	{
	MAKE_ALLOC_L
	return 0;
	}

const CGlxThumbnailContext::TFetchSpec& CGlxThumbnailContext::SpecForIndex(
	TInt /*aListIndex*/, const MGlxMediaList* /*aList*/ ) const
	{
    return iDefaultSpec;
	}
// -----------------------------------------------------------------------------
// <-- Stub for CGlxThumbnailContext
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Stub for CGlxDefaultThumbnailContext -->
// -----------------------------------------------------------------------------
CGlxDefaultThumbnailContext* CGlxDefaultThumbnailContext::NewL()
	{
	return new( ELeave ) CGlxDefaultThumbnailContext;
	}

CGlxDefaultThumbnailContext::~CGlxDefaultThumbnailContext()
	{
	}

void CGlxDefaultThumbnailContext::SetRangeOffsets(
	TInt /*aFrontOffset*/, TInt /*aRearOffset*/ )
	{
	}

CGlxDefaultThumbnailContext::CGlxDefaultThumbnailContext()
	 : CGlxThumbnailContext( NULL )
	{
	}
// -----------------------------------------------------------------------------
// <-- Stub for CGlxDefaultThumbnailContext
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Stub for TGlxFromFocusOutwardIterator -->
// -----------------------------------------------------------------------------
TGlxFromFocusOutwardIterator::TGlxFromFocusOutwardIterator()
	{
	}
	
TGlxFromFocusOutwardIterator::~TGlxFromFocusOutwardIterator()
	{
	}

void TGlxFromFocusOutwardIterator::SetToFirst(const MGlxMediaList* )
	{
	}

TInt TGlxFromFocusOutwardIterator::operator++(TInt)
	{
	return 0;
	}

TBool TGlxFromFocusOutwardIterator::InRange(TInt ) const
	{
	return EFalse;
	}
	
// -----------------------------------------------------------------------------
// <-- Stub for TGlxFromFocusOutwardBlockyIterator
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Stub for TGlxFromFocusOutwardIterator -->
// -----------------------------------------------------------------------------
TGlxFromFocusOutwardBlockyIterator::TGlxFromFocusOutwardBlockyIterator()
	{
	}
	
TGlxFromFocusOutwardBlockyIterator::~TGlxFromFocusOutwardBlockyIterator()
	{
	}

void TGlxFromFocusOutwardBlockyIterator::SetToFirst(const MGlxMediaList* )
	{
	}

TInt TGlxFromFocusOutwardBlockyIterator::operator++(TInt)
	{
	return 0;
	}

TBool TGlxFromFocusOutwardBlockyIterator::InRange(TInt ) const
	{
	return EFalse;
	}
	
// -----------------------------------------------------------------------------
// <-- Stub for TGlxFromFocusOutwardBlockyIterator
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Stub for CGlxAttributeContext -->
// -----------------------------------------------------------------------------
CGlxAttributeContext::CGlxAttributeContext(MGlxMediaListIterator* )
	{
	}

CGlxAttributeContext::~CGlxAttributeContext()
	{
	}

void CGlxAttributeContext::AddAttributeL(const TMPXAttribute& )
	{
	MAKE_ALLOC_L
	}

void CGlxAttributeContext::RemoveAttribute(const TMPXAttribute& )
	{
	}

void CGlxAttributeContext::SetGranularity(TUint )
	{
	}

TInt CGlxAttributeContext::AttributeRequestL(const MGlxMediaList* , RArray<TInt>& , 
	RArray<TMPXAttribute>& , CMPXAttributeSpecs*& ) const
	{
	MAKE_ALLOC_L
	return 0;
	}

void CGlxAttributeContext::AllAttributesL(const MGlxMediaList* , TInt , 
	RArray<TMPXAttribute>& ) const
	{
	MAKE_ALLOC_L
	}

TInt CGlxAttributeContext::RequestCountL(const MGlxMediaList* ) const
	{
	MAKE_ALLOC_L
	return gAttributeContextRequestCount;
	}
// -----------------------------------------------------------------------------
// <-- Stub for CGlxAttributeContext
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Stub for CGlxDefaultAttributeContext -->
// -----------------------------------------------------------------------------
CGlxDefaultAttributeContext* CGlxDefaultAttributeContext::NewL()
	{
	return new( ELeave ) CGlxDefaultAttributeContext;
	}

CGlxDefaultAttributeContext::~CGlxDefaultAttributeContext()
	{
	}

CGlxDefaultAttributeContext::CGlxDefaultAttributeContext()
	: CGlxAttributeContext( NULL )
	{
	}

void CGlxDefaultAttributeContext::SetRangeOffsets(
	TInt , TInt )
	{
	}
// -----------------------------------------------------------------------------
// <-- Stub for CGlxDefaultAttributeContext
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Stub for TMPXAttribute -->
// -----------------------------------------------------------------------------
EXPORT_C TMPXAttribute::TMPXAttribute(TInt , TUint )
	{
	}

EXPORT_C TMPXAttribute::TMPXAttribute(const TMPXAttributeData& /*aData*/)
    {
    }

EXPORT_C TBool TMPXAttribute::Match( const TMPXAttribute& , const TMPXAttribute&  )
	{
	return EFalse;
	}
// -----------------------------------------------------------------------------
// <-- Stub for TMPXAttribute
// -----------------------------------------------------------------------------
