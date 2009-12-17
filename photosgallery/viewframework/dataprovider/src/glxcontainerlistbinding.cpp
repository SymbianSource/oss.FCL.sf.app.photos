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
* Description:    CGlxContainerListBinding This class holds the list of 
*                 tags/albums associated with a media item
*
*/




#include "glxcontainerlistbinding.h"        // his class holds the list of tags/albums associated with a media item

#include <glxfilterfactory.h>        		// for TGlxFilterFactory
#include <glxuistd.h>                       // for attribute priority
#include <mul/mulvisualitem.h>              // Client need to use this class to add data in data model       

using namespace Alf;

//CONSTANTS
const TInt  KTagMediaListId = 0x2000D248;
const TInt  KRangeOffset = 50;

// ----------------------------------------------------------------------------
// NewL
// ----------------------------------------------------------------------------
//
CGlxContainerListBinding* CGlxContainerListBinding::NewL( const Alf::mulvisualitem::TVisualAttribute& aMulTag /*const char* const
        aMulTag*/, const CMPXCollectionPath& aPath )
	{
	CGlxContainerListBinding* self = CGlxContainerListBinding::NewLC( aMulTag, aPath);
	CleanupStack::Pop( self );
	return self;
	}
	

// ----------------------------------------------------------------------------
// NewLC
// ----------------------------------------------------------------------------
//	
CGlxContainerListBinding* CGlxContainerListBinding::NewLC( const Alf::mulvisualitem::TVisualAttribute& aMulTag /*const char* const
        aMulTag*/, const CMPXCollectionPath& aPath )
	{
	CGlxContainerListBinding* self = new ( ELeave ) CGlxContainerListBinding();
	CleanupStack::PushL( self );
	self->ConstructL( aMulTag, aPath );
	return self;
	}

// ----------------------------------------------------------------------------
// ConstructL
// ----------------------------------------------------------------------------
//
void CGlxContainerListBinding::ConstructL( const Alf::mulvisualitem::TVisualAttribute& aMulTag /*const char* const aMulTag*/, 
        const CMPXCollectionPath& aPath )
	{
	CGlxSingleTagBinding::BaseConstructL( aMulTag );
	iContainerPath = CMPXCollectionPath::NewL( aPath );
	
	// attributes for Title of tags and albums
	iFetchContext = CGlxDefaultAttributeContext::NewL();
	iFetchContext->AddAttributeL( KMPXMediaGeneralTitle );
	iFetchContext->SetRangeOffsets(KRangeOffset,KRangeOffset);
	}

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//
CGlxContainerListBinding::~CGlxContainerListBinding()
	{
	CloseMediaList();
	}
	
// ---------------------------------------------------------------------------
// PopulateT
// ---------------------------------------------------------------------------
//
void CGlxContainerListBinding::PopulateT( Alf::MulVisualItem& aItem,
        const TGlxMedia& aMedia, TBool /*aIsFocused*/ ) const
	{
    const TInt KCommaLength = 2;
    _LIT(KComma,",");

    //T is used for throws as per C++ standard.Hence used instead of "L"
    //Ignoring this for code scanner warnings - Leaving functions called in non-leaving functions.
    
	if(!iContainerList)
		{
		CreateMediaListL( aMedia.Id() );
		}

	if( iContainerList )
		{
		CGlxUStringConverter* stringConverter = CGlxUStringConverter::NewL();
		CleanupStack::PushL(stringConverter );

		TInt tagListcount = iContainerList->Count();
		//if tags count is grater than 1 then display tags saperated by comma
		if( tagListcount > 0 )
			{
			HBufC* stringBuffer = NULL;
			
			for(TInt i=0; i < tagListcount; i++ )
				{
				HBufC* string = NULL;
				stringConverter->AsStringL( iContainerList->Item(i),    
				    KMPXMediaGeneralTitle, NULL, string );
				if( string )
					{
					if( stringBuffer )
					    {
					    stringBuffer = stringBuffer->ReAlloc( stringBuffer->Length() +  string->Length() );
					    stringBuffer->Des().Append ( string->Des() );
					    }
				    
					if( !stringBuffer )
					    {
	   				    stringBuffer = string->Alloc();
					    }
						
				    delete string;
				    string = NULL;
					}
				if( i < tagListcount -1 )
					{
			    	if( stringBuffer )
			    	    {
    					stringBuffer = stringBuffer->ReAlloc( stringBuffer->Length() +  KCommaLength );
    					stringBuffer->Des().Append( KComma );
			    	    }
					}
				}
					
			if( stringBuffer )
			    {
    			aItem.SetAttribute( MulTag(), *stringBuffer );
    			delete stringBuffer;
    			stringBuffer = NULL;
			    }
			}
		else
			{		
			aItem.SetAttribute( MulTag(), _L("") );
			}
		CleanupStack::PopAndDestroy(stringConverter );
		}
		
}
// ---------------------------------------------------------------------------
// CreateMediaListL
// ---------------------------------------------------------------------------
//
void CGlxContainerListBinding::CreateMediaListL(TGlxMediaId aMediaId) const
    {
    //create filter that excludes all containers that do not contain the specified item
    CMPXFilter* itemFilter  = TGlxFilterFactory::CreateExcludeContainersWithoutItemFilterL( aMediaId );
    CleanupStack::PushL(itemFilter);

    TGlxFilterProperties filterProperty;
    filterProperty.iSortOrder = EGlxFilterSortOrderAlphabetical;
    filterProperty.iSortDirection = EGlxFilterSortDirectionAscending;
    //creates a combined filter, items will be sorted by the filters specified
    CMPXFilter* filter  = TGlxFilterFactory::CreateCombinedFilterL( filterProperty,
                                                                        itemFilter, EFalse);
    CleanupStack::PushL(filter); 
    //create media list
    iContainerList = MGlxMediaList::InstanceL( *iContainerPath, TGlxHierarchyId(KTagMediaListId), filter );
    iContainerList->AddContextL( iFetchContext, KGlxFetchContextPriorityNormal );
    iContainerList->AddMediaListObserverL( const_cast<CGlxContainerListBinding*>(this));
    CleanupStack::PopAndDestroy(filter);
    CleanupStack::PopAndDestroy(itemFilter);
    }
// ---------------------------------------------------------------------------
// CloseMediaListL
// ---------------------------------------------------------------------------
//
void CGlxContainerListBinding::CloseMediaList()
    {
    if(iContainerList)
        {
        iContainerList->RemoveMediaListObserver( this );
		if (iFetchContext)
			{
			iContainerList->RemoveContext( iFetchContext );
			delete iFetchContext;
			iFetchContext = NULL;
			}
        iContainerList->Close();
        }
    if (iContainerPath)
		{
		delete iContainerPath;
		iContainerPath= NULL;
		}
    }
    
// ---------------------------------------------------------------------------
// HandleItemAddedL
// ---------------------------------------------------------------------------
//
void CGlxContainerListBinding::HandleItemAddedL( TInt /*aStartIndex*/, TInt /*aEndIndex*/, 
        MGlxMediaList* /*aList*/ )
    {
    //No implementation
    }
    
// ---------------------------------------------------------------------------
// HandleItemRemovedL
// ---------------------------------------------------------------------------
//
void CGlxContainerListBinding::HandleItemRemovedL( TInt /*aStartIndex*/, 
        TInt /*aEndIndex*/, MGlxMediaList* /*aList*/ )
    {
    //No implementation    
    }
    
// ---------------------------------------------------------------------------
// HandleAttributesAvailableL
// ---------------------------------------------------------------------------
//
void CGlxContainerListBinding::HandleAttributesAvailableL( TInt /*aItemIndex*/, 
    const RArray<TMPXAttribute>& /*aAttributes*/, MGlxMediaList* aList )
	{
	if( aList == iContainerList )
		{
		Update();
		}
	}

// ----------------------------------------------------------------------------
// HandleFocusChanged
// checks whether the focus has changed,returns the response status 
// ----------------------------------------------------------------------------
//  
CGlxBinding::TResponse CGlxContainerListBinding::HandleFocusChanged( 
        TBool /*aIsGained*/ )
    {
    //@todo - Need further implementation
    //return aIsGained ? EUpdateRequested : ENoUpdateNeeded ;
    //need to remove once bug ESPK-7G6GJX is fixed
    return ENoUpdateNeeded ;
    }
    
// ---------------------------------------------------------------------------
// HandleFocusChangedL
// ---------------------------------------------------------------------------
//
void CGlxContainerListBinding::HandleFocusChangedL( NGlxListDefs::TFocusChangeType /*aType*/, 
        TInt /*aNewIndex*/, TInt /*aOldIndex*/, MGlxMediaList* /*aList*/ )
    {
    //No implementation   
    }

// ---------------------------------------------------------------------------
// HandleItemSelectedL
// ---------------------------------------------------------------------------
//
void CGlxContainerListBinding::HandleItemSelectedL(TInt /*aIndex*/,
        TBool /*aSelected*/, MGlxMediaList* /*aList*/ ) 
    {
    //No implementation    
    }

// ---------------------------------------------------------------------------
// HandleMessageL
// ---------------------------------------------------------------------------
//
void CGlxContainerListBinding::HandleMessageL( const CMPXMessage& /*aMessage*/,
        MGlxMediaList* /*aList*/ ) 
    {
    //No implementation    
    }

// ---------------------------------------------------------------------------
// HandleMediaL
// ---------------------------------------------------------------------------
//
void CGlxContainerListBinding::HandleMediaL( TInt /*aListIndex*/,
        MGlxMediaList* /*aList*/ ) 
    {
    //No implementation    
    }

// ---------------------------------------------------------------------------
// HandleItemModifiedL
// ---------------------------------------------------------------------------
//
void CGlxContainerListBinding::HandleItemModifiedL( const RArray<TInt>& /*aItemIndexes*/,
        MGlxMediaList* /*aList*/ )
    {
    //No implementation    
    }
