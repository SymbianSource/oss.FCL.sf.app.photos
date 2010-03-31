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
* Description:    Implementation class for details data provider
*
*/




#include "glxdetailsmulmodelproviderimpl.h"   // This is the implementation class of details data provider 
#include "glxattributerequirement.h"          // This class will set the required attribute to the medialist
#include "glxbindingset.h"                    // This basically holds the bindings required to populate the visual item
#include "glxbinding.h"                       // This class holds the bindings required to populate the visual item
#include "glxmulbindingsetfactory.h"          // CreateBindingSetL creates instance of Binding set
#include <mglxmedialist.h>                    // Interface for reading lists of media items
#include <glxlog.h>                           // Logging
#include <glxtracer.h>
#include <eikmenup.h>                         // A helper class for extending CEikMenuPaneItem without breaking binary  
#include <glxuistd.h>                         // attribute/thumbnail fetch context priority definitions
#include "glxdetailsboundcommand.h"           // Handles user commands
#include "glxerrormanager.h"                       // for handling the error attribute that may be attached to items
#include "glxthumbnailattributeinfo.h"
#include "glxdetailsboundcommand.hrh" 
/** Error Id EMPY-7MKDHP **/
#include "mglxmetadatadialogobserver.h" 		//for call back to dailog
#include <glxcommandhandlers.hrh>
using namespace Alf;

// ----------------------------------------------------------------------------
// NewL
// ----------------------------------------------------------------------------
//
CGlxDetailsMulModelProviderImpl* CGlxDetailsMulModelProviderImpl::NewL(
     CAlfEnv& aEnv, 
     Alf::IMulWidget& aWidget, 
     MGlxMediaList& aMediaList, 
     Alf::mulwidget::TLogicalTemplate aDefaultTemplate, 
     TInt aDataWindowSize )
  	{
  	TRACER("CGlxDetailsMulModelProviderImpl::NewL");
  	CGlxDetailsMulModelProviderImpl* self = new ( ELeave )CGlxDetailsMulModelProviderImpl( aWidget, aMediaList, aDefaultTemplate, aDataWindowSize);
  	CleanupStack::PushL( self );
  	self->ConstructL( aEnv, aDefaultTemplate, aDataWindowSize );
  	CleanupStack::Pop( self );
  	return self;
  	}
  	
// ----------------------------------------------------------------------------
// ConstructL
// ----------------------------------------------------------------------------
//  	
void CGlxDetailsMulModelProviderImpl::ConstructL(
    CAlfEnv& aEnv, 
    Alf::mulwidget::TLogicalTemplate aDefaultTemplate,
    TInt aDataWindowSize )
    {
    TRACER("CGlxDetailsMulModelProviderImpl::ConstructL");
    BaseConstructL( aEnv, aDefaultTemplate, aDataWindowSize );
    iMediaList.AddMediaListObserverL(this);
   	iAttributeRequirements = CGlxAttributeRequirements::NewL( iMediaList );
   	AddWidgetEventHandler();
    }
// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//	            	
CGlxDetailsMulModelProviderImpl::~CGlxDetailsMulModelProviderImpl()
	{
	TRACER("CGlxDetailsMulModelProviderImpl::~CGlxDetailsMulModelProviderImpl");
    iEntries.ResetAndDestroy();
	iEntries.Close();
	iMediaList.RemoveMediaListObserver( this );
	delete 	iAttributeRequirements;				
	}

// ----------------------------------------------------------------------------
// AddEntryL
// ----------------------------------------------------------------------------
//	    					
void CGlxDetailsMulModelProviderImpl::AddEntryL( const TGlxMulBindingSetFactory& aFactory )
	{
	TRACER("CGlxDetailsMulModelProviderImpl::AddEntryL");
	CGlxBindingSet* bindingSet = aFactory.CreateBindingSetL( *this );
	CleanupStack::PushL( bindingSet);
	iEntries.Append( bindingSet );
	CleanupStack::Pop( bindingSet );
    TSize Size;
    bindingSet->AddRequirementsL(*iAttributeRequirements,Size);
	InsertItemsL( iEntries.Count() - 1, 1 );
	}
					
// ----------------------------------------------------------------------------
// OfferCommandL
// ----------------------------------------------------------------------------
//	    					
TBool CGlxDetailsMulModelProviderImpl::OfferCommandL( TInt aCommandId )
	{
	TRACER("CGlxDetailsMulModelProviderImpl::OfferCommandL");
	MGlxBoundCommand* command = BoundCommand( FocusIndex() );
    if ( command ) 
        {
        return command->OfferCommandL( aCommandId , iMediaList );
        }
    return EFalse;				
	}

// ----------------------------------------------------------------------------
// HasCommandL
// ----------------------------------------------------------------------------
//
TBool CGlxDetailsMulModelProviderImpl::HasCommandL( CEikMenuPane& aMenuPane, const TInt aCommandId )
    {
    TRACER("CGlxDetailsMulModelProviderImpl::HasCommandL");
    TInt count = aMenuPane.NumberOfItemsInPane();
    TInt i = -1;
    while ( ++i < count )
        {
        if ( aCommandId == aMenuPane.ItemDataByIndexL( i ).iCommandId )
            {
            return ETrue;
            }
        }
    return EFalse; 
    }
// ----------------------------------------------------------------------------
// InitMenuL
// ----------------------------------------------------------------------------
//	    					
void CGlxDetailsMulModelProviderImpl::InitMenuL( CEikMenuPane& aMenu )
	{
	TRACER("CGlxDetailsMulModelProviderImpl::InitMenuL");
	//CEikMenuPane::SetItemDimmed will panic if the menu item does not exist
 	if ( HasCommandL( aMenu, KGlxEditBoundMenuCommandId ) )
		{
		// Disable by default, bound command will enable if needed
		aMenu.SetItemDimmed( KGlxEditBoundMenuCommandId, ETrue );
		const MGlxBoundCommand* command = BoundCommand( FocusIndex() );
		if ( command )
			{
			command->InitMenuL( aMenu );
			}
		}
	else
		{
		// no implementation	
		}
	
	
	if ( HasCommandL( aMenu, KGlxDeleteBoundMenuCommandId ) )
		{
		// Disable by default, bound command will enable if needed
		aMenu.SetItemDimmed( KGlxDeleteBoundMenuCommandId, ETrue );
		const MGlxBoundCommand* command = BoundCommand( FocusIndex() );
		if ( command )
			{
			command->InitMenuL( aMenu );
			}
		}
		else
		{
		// no implementation	
		}
		
	if ( HasCommandL( aMenu, KGlxViewBoundMenuCommandId ) )
		{
		// Disable by default, bound command will enable if needed
		aMenu.SetItemDimmed( KGlxViewBoundMenuCommandId, ETrue );
		const MGlxBoundCommand* command = BoundCommand( FocusIndex() );
		if ( command )
			{
			command->InitMenuL( aMenu );
			}
		}
	else
		{
		// no implementation	
		}
	if ( HasCommandL( aMenu, EGlxCmdAiwShowMap ) )
		{
		// Disable by default, bound command will enable if needed
		aMenu.SetItemDimmed( EGlxCmdAiwShowMap, ETrue );
		const MGlxBoundCommand* command = BoundCommand( FocusIndex() );
		if ( command )
			{
			command->InitMenuL( aMenu );
			}
		}
	else
		{
		// no implementation	
		}	
}

// ----------------------------------------------------------------------------
// CGlxDetailsMulModelProviderImpl
// ----------------------------------------------------------------------------
//	    					
CGlxDetailsMulModelProviderImpl::CGlxDetailsMulModelProviderImpl(
    Alf::IMulWidget& aWidget, 
	MGlxMediaList& aMediaList,
	Alf::mulwidget::TLogicalTemplate /*aDefaultTemplate*/,
	TInt /*aDataWindowSize*/ ):
	    CGlxMulModelProviderBase(aWidget), iMediaList( aMediaList )
	{
	}

// ----------------------------------------------------------------------------
// HandleBindingChanged
// ----------------------------------------------------------------------------
//	    					
void CGlxDetailsMulModelProviderImpl::HandleBindingChanged( const CGlxBinding& /*aBinding*/ )
	{
	 for(TInt i = 0; i < iEntries.Count(); i++)
         {
         UpdateItems( i, 1 );
         }
	}

// ----------------------------------------------------------------------------
// HandleFocusChanged
// ----------------------------------------------------------------------------
//	    
void CGlxDetailsMulModelProviderImpl::HandleFocusChanged( TInt aNewFocus, TInt aPreviousFocus )
	{
	TRACER("CGlxDetailsMulModelProviderImpl::HandleFocusChanged");
	if ( aPreviousFocus != -1 )
	    {
        if ( CGlxBinding::EUpdateRequested == 
            iEntries[ aPreviousFocus ]->HandleFocusChanged( EFalse ) )
            {
            UpdateItems( aPreviousFocus, 1 );
            }
        }
    
    if ( aNewFocus != -1 )
        {
        if ( CGlxBinding::EUpdateRequested == 
            iEntries[ aNewFocus ]->HandleFocusChanged( ETrue ) )
            {
            UpdateItems( aNewFocus, 1 );
            }
        }
	}

// ----------------------------------------------------------------------------
// HandleAttributesAvailableL
// ----------------------------------------------------------------------------
//	
void CGlxDetailsMulModelProviderImpl::HandleAttributesAvailableL( TInt aIndex, 
        const RArray<TMPXAttribute>& aAttributes, MGlxMediaList* aList )
    {
    TRACER("CGlxDetailsMulModelProviderImpl::HandleAttributesAvailableL");
    if ( aList->FocusIndex() == aIndex )
        {
        for(TInt i = 0; i < iEntries.Count(); i++)
            {
            if(iEntries[i]->HasRelevantAttributes(aAttributes) )
                {
		        UpdateItems( i, 1 );
                }
            }
        }
    }

// ----------------------------------------------------------------------------
// ProvideData
// ----------------------------------------------------------------------------
//
void  CGlxDetailsMulModelProviderImpl::ProvideData (int aIndex, int aCount, 
    MulDataPath /*aPath*/)
    {
    TRACER("CGlxDetailsMulModelProviderImpl::ProvideData");
	for( int i = aIndex;i < aIndex + aCount ; i++)
		{
		if( KErrNotFound != iMediaList.FocusIndex())
			{
			SetDataT( *iEntries[ i ], iMediaList.Item(iMediaList.FocusIndex()), i ) ;
			}
		}       
    }       
        
// ----------------------------------------------------------------------------
// HandleItemAddedL
// ----------------------------------------------------------------------------
//
void  CGlxDetailsMulModelProviderImpl::HandleItemAddedL(TInt /*aStartIndex*/, TInt /*aEndIndex*/,
    MGlxMediaList* /*aList*/ )
    {
    TRACER("CGlxDetailsMulModelProviderImpl::HandleItemAddedL");
    TInt count = iEntries.Count();
    for(int i = 0; i < count; i++)
        {
	     UpdateItems( i , 1 );
        }
    }
// ----------------------------------------------------------------------------
// HandleError
// ----------------------------------------------------------------------------
//
void CGlxDetailsMulModelProviderImpl::HandleError( TInt aError )
    {
    TRACER("CGlxDetailsMulModelProviderImpl::HandleError");
    TRAP_IGNORE( DoHandleErrorL( aError ) );
    }

// ----------------------------------------------------------------------------
// HandleError
// ----------------------------------------------------------------------------
//
void CGlxDetailsMulModelProviderImpl::DoHandleErrorL( TInt /*aError*/ )
    {
    TRACER("CGlxDetailsMulModelProviderImpl::DoHandleErrorL");
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
// HideLocationMenuItem
// ----------------------------------------------------------------------------
//
void CGlxDetailsMulModelProviderImpl::HideLocationMenuItem( CEikMenuPane& aMenu )
    {
    if ( HasCommandL( aMenu, EGlxCmdAiwBase ) )
        {
        const MGlxBoundCommand* command = BoundCommand( FocusIndex() );
        if ( command )
            {
            command->HideLocationMenuItem( aMenu );
            }
        }
    }

// ----------------------------------------------------------------------------
// EnableMskL
// ----------------------------------------------------------------------------
//
void CGlxDetailsMulModelProviderImpl::EnableMskL()
    {
    TRACER("CGlxDetailsMulModelProviderImpl::EnableMskL");
    MGlxBoundCommand* command = BoundCommand( FocusIndex() );
    if ( command )
        {
        command->ChangeMskL();
        }
    }

// ----------------------------------------------------------------------------
// SetDetailsDailogObserver
// ----------------------------------------------------------------------------
//
void CGlxDetailsMulModelProviderImpl::SetDetailsDailogObserver( MGlxMetadataDialogObserver& aObserver )
    {
    TRACER("CGlxDetailsMulModelProviderImpl::SetDetailsDailogObserver");
	/** Error Id EMPY-7MKDHP **/
	MGlxBoundCommand* command = BoundCommand( FocusIndex() );
	if ( command )
	    {
	    command->SetDetailsDailogObserver( aObserver ) ;
	    }
    }


// ----------------------------------------------------------------------------
// HandleCommandCompleteL
// ----------------------------------------------------------------------------
//  
void CGlxDetailsMulModelProviderImpl::HandleCommandCompleteL(TAny* /*aSessionId*/,
       CMPXCommand* /*aCommandResult*/, TInt aError, MGlxMediaList* /*aList*/)
    {
    TRACER("CGlxDetailsMulModelProviderImpl::HandleCommandCompleteL");
    TGlxMedia media = iMediaList.Item(iMediaList.FocusIndex()) ;
    media.DeleteLocationAttribute();
    if ( aError == KErrNone )
        {
	   	UpdateItems( FocusIndex(), 1 );   
        }
    }
