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
* Description:  Implementation class for Medialist Access for Data   
*
*/




#include "glxmedialistmulmodelproviderimpl.h"      // The actual implemntation for providign the data to the clients is done here
#include "glxbindingset.h"                         // This basically holds the bindings required to populate the visual item
#include <mglxmedialist.h>                         // Interface for reading lists of media items
#include "glxthumbnailinfo.h"
#include "glxattributerequirement.h"               // This class will set the required attribute to the medialist
#include "glxerrormanager.h"                       // for handling the error attribute that may be attached to items
#include "glxthumbnailattributeinfo.h"
#include "glxmulbindingsetfactory.h"               // CreateBindingSetL creates instance of Binding set
#include <mul/imulcoverflowwidget.h>               // An interface for Multimedia coverflow Widget
#include <glxresolutionutility.h>                  // Singleton containing resolution utility methods
#include <glxtracer.h>
#include <glxlog.h>                                // Logging
#include <glxuiutility.h>                          // Alf utils lib glxalfutils.lib
#include "glxtexturemanager.h"


static const char* const KListWidget = "ListWidget";
static const char* const KCoverFlowWidget = "CoverflowWidget";

using namespace Alf;

// ----------------------------------------------------------------------------
// NewL
// ----------------------------------------------------------------------------
//
CGlxMediaListMulModelProviderImpl* CGlxMediaListMulModelProviderImpl::NewL( 
	CAlfEnv& aEnv,
	IMulWidget& aWidget,
	MGlxMediaList& aMediaList,
	const TGlxMulBindingSetFactory& aFactory,
	Alf::mulwidget::TLogicalTemplate aDefaultTemplate,
	TInt aDataWindowSize )
    {
    TRACER("CGlxMediaListMulModelProviderImpl::NewL");
    CGlxMediaListMulModelProviderImpl* self = 
        new ( ELeave) CGlxMediaListMulModelProviderImpl( aWidget, aMediaList );
    CleanupStack::PushL( self );
    self->ConstructL( aEnv, aFactory, aDefaultTemplate, aDataWindowSize );
    CleanupStack::Pop( self );
    return self;
    }
    
// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
//
CGlxMediaListMulModelProviderImpl::CGlxMediaListMulModelProviderImpl( 
    IMulWidget& aWidget, MGlxMediaList& aMediaList )
        : CGlxMulModelProviderBase( aWidget ), iMediaList( aMediaList ),iWidget(aWidget)
    {
    }

// ----------------------------------------------------------------------------
// ConstructL
// ----------------------------------------------------------------------------
//
void CGlxMediaListMulModelProviderImpl::ConstructL( CAlfEnv& aEnv, 
													const TGlxMulBindingSetFactory& aFactory, 
													Alf::mulwidget::TLogicalTemplate aDefaultTemplate, TInt aDataWindowSize ) 
    {
    TRACER("CGlxMediaListMulModelProviderImpl::ConstructL");
    BaseConstructL( aEnv, aDefaultTemplate, aDataWindowSize );
	iMediaList.AddMediaListObserverL(this);
	iUiUtility = CGlxUiUtility::UtilityL();
	iAttributeRequirements = CGlxAttributeRequirements::NewL( iMediaList);
    iBindingSet = aFactory.CreateBindingSetL( *this );
        
    TInt height;
    TInt width;
    iWidget.GetIconSize(aDefaultTemplate,Alf::mulvisualitem::KMulIcon1,height,width); 
    TSize thumbnailSize(width,height);

	iBindingSet->AddRequirementsL( *iAttributeRequirements ,thumbnailSize );
	if( iMediaList.Count() > 0 )
	    {
	    HandleItemAddedL( 0, iMediaList.Count() - 1, &iMediaList );
		if(!iUiUtility->IsPenSupported())
			{			
			CMPXCollectionPath* path = iMediaList.PathLC( NGlxListDefs::EPathFocusOrSelection );
			iBindingSet->HandleItemChangedL( *path ); 
			CleanupStack::PopAndDestroy( path );
			}
	    }
	    
	if(UString( KListWidget ) == UString(iWidget.widgetName() ) ||  UString( KCoverFlowWidget ) == UString(iWidget.widgetName() ) )
	    {
	    AddWidgetEventHandler();
	    }
    } 

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//
CGlxMediaListMulModelProviderImpl::~CGlxMediaListMulModelProviderImpl()
    {
    TRACER("CGlxMediaListMulModelProviderImpl::~CGlxMediaListMulModelProviderImpl");
    delete iBindingSet;
    delete iAttributeRequirements;
    iMediaList.RemoveMediaListObserver( this );
    if (iUiUtility)
        {
        iUiUtility->Close();
        }
    
    }

// ----------------------------------------------------------------------------
// HandleBindingChanged
// ----------------------------------------------------------------------------
// 
void CGlxMediaListMulModelProviderImpl::HandleBindingChanged( 
        const CGlxBinding& aBinding )
    {
    TRACER("CGlxMediaListMulModelProviderImpl::HandleBindingChanged");
    SetDataT( aBinding, iMediaList.Item(iMediaList.FocusIndex()), 
                iMediaList.FocusIndex() );
    
    }
    
// ----------------------------------------------------------------------------
// HandleFocusChanged
// ----------------------------------------------------------------------------
//     
void CGlxMediaListMulModelProviderImpl::HandleFocusChanged( TInt 
	    /*aNewFocusIndex*/, TInt /*aPreviousFocusIndex*/ )	
	{
	TRACER("CGlxMediaListMulModelProviderImpl::HandleFocusChanged");
    if (CGlxBinding::EUpdateRequested == iBindingSet->HandleFocusChanged( ETrue ) )
        {
        UpdateItems( iMediaList.FocusIndex(), 1 );
        }
   	}  
	
// ----------------------------------------------------------------------------
// HandleFocusChangedL
// ----------------------------------------------------------------------------
//	
void CGlxMediaListMulModelProviderImpl::HandleFocusChangedL( NGlxListDefs::TFocusChangeType /*aType*/, 
        TInt aNewIndex, TInt aOldIndex, MGlxMediaList* aList )	  
	{
	TRACER("CGlxMediaListMulModelProviderImpl::HandleFocusChangedL");
	CMPXCollectionPath* path = iMediaList.PathLC( NGlxListDefs::EPathFocusOrSelection );
	if ( aNewIndex != FocusIndex() )
	    {
	    // focus was changed not by the widget, but by some other reason (e.g., item insertion)
	   	//Aki changes -  todo
	    SetFocusIndex( aNewIndex );
	    }
	iBindingSet->HandleItemChangedL( *path );  // Item changed is handled first before Updating the List - see line 168 moved from here
	if( aList->FocusIndex() >=0 && aOldIndex <= aList->Count() -1 && aOldIndex!=-1 ) 
	    {
        UpdateItems( aOldIndex, 1 );    
        UpdateItems( aNewIndex, 1 ); 
        }
	
	CleanupStack::PopAndDestroy( path );
	}

// ----------------------------------------------------------------------------
// HandleItemAddedL
// ----------------------------------------------------------------------------
//
void CGlxMediaListMulModelProviderImpl::HandleItemAddedL( TInt aStartIndex, 
        TInt aEndIndex, MGlxMediaList* aList )
    {
    TRACER("CGlxMediaListMulModelProviderImpl::HandleItemAddedL");
    GLX_LOG_INFO1("HandleItemAddedL::aStartIndex %d", aStartIndex);
    GLX_LOG_INFO1("HandleItemAddedL::aEndIndex %d", aEndIndex);
    
    InsertItemsL( aStartIndex, aEndIndex - aStartIndex + 1, aList->FocusIndex() );
    }

// ----------------------------------------------------------------------------
// HandleItemRemoved
// ----------------------------------------------------------------------------
//
void CGlxMediaListMulModelProviderImpl::HandleItemRemovedL( TInt aStartIndex, 
        TInt aEndIndex, MGlxMediaList* /*aList*/ )
    {
    TRACER("CGlxMediaListMulModelProviderImpl::HandleItemRemovedL");
    // hack of making the flag iIsReleventAttribute to Etrue will be removed 
    // once the bug ESPK-7G6GJX
    //iIsReleventAttribute = ETrue;
    RemoveItems( aStartIndex, aEndIndex - aStartIndex + 1 );
    }

// ----------------------------------------------------------------------------
// HandleAttributesAvailableL
// ----------------------------------------------------------------------------
//
void CGlxMediaListMulModelProviderImpl::HandleAttributesAvailableL( TInt aIndex, 
        const RArray<TMPXAttribute>& aAttributes, MGlxMediaList* /*aList*/ )
    {
    TRACER("CGlxMediaListMulModelProviderImpl::HandleAttributesAvailableL");
    GLX_LOG_INFO1("HandleAttributesAvailableL::Index %d", aIndex);
    
     if(iBindingSet->HasRelevantAttributes( aAttributes ) )
        {
        UpdateItems( aIndex, 1 );
        }
    }

// ----------------------------------------------------------------------------
// HandleError
// ----------------------------------------------------------------------------
//
void CGlxMediaListMulModelProviderImpl::HandleError( TInt aError )
    {
    TRACER("CGlxMediaListMulModelProviderImpl::HandleError");
    TRAP_IGNORE( DoHandleErrorL( aError ) );
    }

// ----------------------------------------------------------------------------
// HandleError
// ----------------------------------------------------------------------------
//
void CGlxMediaListMulModelProviderImpl::DoHandleErrorL( TInt /*aError*/ )
    {
    TRACER("CGlxMediaListMulModelProviderImpl::DoHandleErrorL");
    // Check for items for which fetching a thumbnail has failed, and replace
    // the default icon with broken icon
	TInt count = iMediaList.Count();
	for ( TInt i = 0; i < count; i++ )
	    {
	    const TGlxMedia& item = iMediaList.Item( i );
        TInt thumbnailError = GlxErrorManager::HasAttributeErrorL(
                                item.Properties(), KGlxMediaIdThumbnail );
        if( thumbnailError )
            {
            UpdateItems( i , 1 );
            }
	    }
    }
    
// ----------------------------------------------------------------------------
// ProvideData
// ----------------------------------------------------------------------------
//
void  CGlxMediaListMulModelProviderImpl::ProvideData (int aIndex, int aCount, 
	MulDataPath /*aPath*/)
    {
    TRACER("CGlxMediaListMulModelProviderImpl::ProvideData");
    GLX_LOG_INFO1("ProvideData::aCount %d", aCount);
    
    if( KErrNotFound != iMediaList.FocusIndex() )
        {
        for(int i = aIndex; i < aIndex + aCount ; i++)
            {
		    		SetDataT( *iBindingSet, iMediaList.Item(i), i );            
            }
        }
    else
    	return;
    
    }

void CGlxMediaListMulModelProviderImpl::HandleOrientationChanged()
	{
	TRACER("CGlxMediaListMulModelProviderImpl::HandleOrientationChanged");
	//Fix for ESLM-7VGCS8
    //Since iBindingSet->AddRequirementsL() will change the iFsThumbnailContext's
    //thumbnail size due to which  thumbnail fetched for old context is getting
    //deleted in clean-up. But actually S60 TNM provides same thumbnail for both
    //the configurations requests,So it is unneccesary to delete the existing
    //thumbnail and request for the same. To avoid this ,iBindingSet->AddRequirementsL()
    //and UpdateItems() is removed so that  the context will not be changed
    //and the thumbnail deletion will not happen.
  	}

// ----------------------------------------------------------------------------
// HandleItemModifiedL
// ----------------------------------------------------------------------------
//  
void CGlxMediaListMulModelProviderImpl::HandleItemModifiedL( const RArray<TInt>& aItemIndexes,
    MGlxMediaList* aList )
    {
    //Bug fix for PKAA-7NRBYZ
    //First remove all the textures from the list in the texture manager
    //otherwise it would not create a new texture for the modified image.
    for(TInt index = 0;index<aItemIndexes.Count();index++)
        {
        TGlxMedia media = aList->Item(aItemIndexes[index]);

        iUiUtility->GlxTextureManager().RemoveTexture(media.Id(),ETrue);
        }            
    //Now update the items, this would create the new textures and update the view.
    for(TInt index = 0;index<aItemIndexes.Count();index++)
        UpdateItems(aItemIndexes[index],1);
    }
