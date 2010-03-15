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
* Description:  Interface to Alf Objects  
*
*/




#include "glxmulmodelproviderbase.h"

#include <e32err.h>
#include <alf/alfenv.h>
#include <alf/alfevent.h>
#include <alf/ialfwidgetfactory.h>
#include <alf/alfwidgetenvextension.h>
//#include <osn/ustring.h>
#include <mul/imulwidget.h>
#include <mul/mulevent.h>
#include <mul/mulvisualitem.h>
#include <glxlog.h>  //Logging
#include <glxtracer.h>
#include "glxbinding.h"
#include "glxcommandbindingutility.h"
#include <glxnavigationalstate.h>
#include <glxnavigationalstatedefs.h>

#include <data_caging_path_literals.hrh>// for KDC_APP_RESOURCE_DIR
#include <glxuiutilities.rsg>
#include <glxgeneraluiutilities.h>
#include <glxtexturemanager.h>
#include <glxicons.mbg>
#include <glxuistd.h>
#include <mul/imulcoverflowwidget.h>               // An interface for Multimedia coverflow Widget

using namespace Alf;

static const char* const KListWidget = "ListWidget";
static const char* const KGridWidget = "GridWidget";
static const char* const KCoverFlowWidget = "CoverflowWidget";

//@todo to be uncommented when using command binding
//#include "glxboundcommand.h"

// ----------------------------------------------------------------------------
// BaseConstructL
// ----------------------------------------------------------------------------
//
void CGlxMulModelProviderBase::BaseConstructL(
     CAlfEnv& aEnv, 
     Alf::mulwidget::TLogicalTemplate aDefaultTemplate,
     TInt aDataWindowSize )
    {
    TRACER("CGlxMulModelProviderBase::BaseConstructL");
   
    // Create navigational state 
    iNavigationalState = CGlxNavigationalState::InstanceL();

    iEnv = &aEnv;

    iDefaultTemplate = aDefaultTemplate;
        
    iDataWindowSize = aDataWindowSize;

    }
    
// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
//
CGlxMulModelProviderBase::CGlxMulModelProviderBase( IMulWidget& aWidget )
        : iWidget( aWidget ),iPreviousFocusIndex(-1)
    {
    }

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//
CGlxMulModelProviderBase::~CGlxMulModelProviderBase()
    { 
    TRACER("CGlxMulModelProviderBase::~CGlxMulModelProviderBase");
    //   remove event handler
	iWidget.RemoveEventHandler( *this );
    if ( iNavigationalState )
          {
          iNavigationalState->Close();
          }
	}    
 
// ----------------------------------------------------------------------------
// HandleItemAddedL
// ----------------------------------------------------------------------------
// 
void CGlxMulModelProviderBase::HandleItemAddedL( TInt /*aStartIndex*/, TInt 
	/*aEndIndex*/, MGlxMediaList* /*aList*/ )
	{

	}

// ----------------------------------------------------------------------------
// HandleItemRemoved
// ----------------------------------------------------------------------------
//	
void CGlxMulModelProviderBase::HandleItemRemovedL( TInt /*aStartIndex*/, TInt 
	/*aEndIndex*/, MGlxMediaList* /*aList*/ )
	{

	}

// ----------------------------------------------------------------------------
// HandleAttributesAvailableL
// ----------------------------------------------------------------------------
//	
void CGlxMulModelProviderBase::HandleAttributesAvailableL( TInt /*aItemIndex*/, 
	const RArray<TMPXAttribute>& /*aAttributes*/, MGlxMediaList* /*aList*/ )
	{

	}
	
// ----------------------------------------------------------------------------
// HandleFocusChangedL
// ----------------------------------------------------------------------------
//	
void CGlxMulModelProviderBase::HandleFocusChangedL( NGlxListDefs::
	TFocusChangeType /*aType*/, TInt /*aNewIndex*/, TInt /*aOldIndex*/, MGlxMediaList* /*aList*/ )
	{

	}

// ----------------------------------------------------------------------------
// HandleItemSelected
// ----------------------------------------------------------------------------
//	
void CGlxMulModelProviderBase::HandleItemSelectedL(TInt aIndex, 
	TBool aSelected, MGlxMediaList* /*aList*/ )
	{
	TRACER("CGlxMulModelProviderBase::HandleItemSelectedL");
	std::vector<int> itemIndex;	
	itemIndex.push_back( aIndex);	
	// changed code ---statr
	if(aSelected) 
	    {
	    iModel->SetMarkedIndices(MulWidgetDef::EMulMark,itemIndex);
	    }
	else 
	    {	
	    iModel->SetMarkedIndices(MulWidgetDef::EMulUnmark,itemIndex);	
	    }
	}

// ----------------------------------------------------------------------------
// accept
// ----------------------------------------------------------------------------
//	
bool CGlxMulModelProviderBase::accept( Alf::CAlfWidgetControl& /*aControl*/, const 
	TAlfEvent& /*aEvent*/ ) const
	{
	TRACER("CGlxMulModelProviderBase::accept");
	//Need implementation
	return NULL;
	}

// ----------------------------------------------------------------------------
// offerEvent
// ----------------------------------------------------------------------------
//
AlfEventStatus CGlxMulModelProviderBase::offerEvent( Alf::CAlfWidgetControl& 
		/*aControl*/, const TAlfEvent& aEvent )
	{
	TRACER("CGlxMulModelProviderBase::offerEvent");
	AlfEventStatus response = EEventNotHandled;
	if ( aEvent.IsCustomEvent() )
		{
		switch ( aEvent.CustomParameter() ) 
			{
			case KAlfActionIdDeviceLayoutChanged:
				{
				HandleOrientationChanged();
				response = EEventNotHandled;
				}
				break;
			case Alf::ETypeHighlight:
				{
				if ( !iWithinFocusChangeCall )
					{
					HandleFocusChanged( FocusIndex(), iPreviousFocusIndex );
					}
				response = EEventHandled;
				}
				break;
			default:
				break;
			}
		}
	return response;
	}

// ----------------------------------------------------------------------------
// setActiveStates
// ----------------------------------------------------------------------------
//
void CGlxMulModelProviderBase::setActiveStates( unsigned int /*aStates*/ ) 
    {
    
    }

// ----------------------------------------------------------------------------
// setEventHandlerData
// ----------------------------------------------------------------------------
//  
void CGlxMulModelProviderBase::setEventHandlerData( const Alf::AlfWidgetEventHandlerInitData& /*aData*/ )
    {
    
    }

// ----------------------------------------------------------------------------
// eventHandlerData
// ----------------------------------------------------------------------------
//  
Alf::AlfWidgetEventHandlerInitData* CGlxMulModelProviderBase::eventHandlerData() 
    {
    TRACER("CGlxMulModelProviderBase::eventHandlerData");
    return NULL; 
    }

// ----------------------------------------------------------------------------
// makeInterface
// ----------------------------------------------------------------------------
//    
Alf::IAlfInterfaceBase* CGlxMulModelProviderBase::makeInterface(const Alf::IfId& /*aType */)
    {
    TRACER("CGlxMulModelProviderBase::makeInterface");
    return this;
    }

// ----------------------------------------------------------------------------
// eventHandlerType
// ----------------------------------------------------------------------------
//    
IAlfWidgetEventHandler::AlfEventHandlerType CGlxMulModelProviderBase::
    eventHandlerType() 
	{
	TRACER("CGlxMulModelProviderBase::eventHandlerType");
	// To Avoid Compiler warning
	return IAlfWidgetEventHandler::ELogicalEventHandler;
	}

// ----------------------------------------------------------------------------
// eventExecutionPhase
// ----------------------------------------------------------------------------
//    
IAlfWidgetEventHandler::AlfEventHandlerExecutionPhase 
    CGlxMulModelProviderBase::eventExecutionPhase()
	{
	TRACER("CGlxMulModelProviderBase::eventExecutionPhase");
	return EBubblingPhaseEventHandler;
	}

// ----------------------------------------------------------------------------
// UpdateItemL
// ----------------------------------------------------------------------------
//	
void CGlxMulModelProviderBase::UpdateItems( TInt aIndex, TInt aCount )
    {
    TRACER("CGlxMulModelProviderBase::UpdateItems");
    try 
        {
        iModel->Update( aIndex, aCount );
        }
    catch (...)
        {
        throw std::bad_alloc();
        }
    }

// ----------------------------------------------------------------------------
// SetDataT
// ----------------------------------------------------------------------------
//	
void CGlxMulModelProviderBase::SetDataT( const CGlxBinding& aBinding,
        const TGlxMedia& aMedia, TInt aAtIndex )
    {
    TRACER("CGlxMulModelProviderBase::SetDataT");
    iModel->SetData( aAtIndex, CreateItemT( aBinding, aMedia, 
        IsFocused( aAtIndex ) ) );
    }
    
// ----------------------------------------------------------------------------
// CreateItemT
// ----------------------------------------------------------------------------
//    
auto_ptr<MulVisualItem> CGlxMulModelProviderBase::CreateItemT( 
    const CGlxBinding& aBinding, const TGlxMedia& aMedia, TBool aIsFocused )     
    {
    TRACER("CGlxMulModelProviderBase::CreateItemT");
    auto_ptr<MulVisualItem> item( new ( EMM ) MulVisualItem() );
    aBinding.PopulateT( *item, aMedia, aIsFocused );
    return item;
    }
// ----------------------------------------------------------------------------
// CreateModelL
// ----------------------------------------------------------------------------
//    
    
void CGlxMulModelProviderBase::CreateModelL()
    {
    
    const char* KLoadNameModel( "mulmodel" );
    try 
        {

        IAlfWidgetFactory& widgetFactory = AlfWidgetEnvExtension::widgetFactory(*iEnv);

        std::auto_ptr< Alf::IMulModel > model( 
                widgetFactory.createModel< IMulModel >( KLoadNameModel ) );
        iModel = model.release(); 
        if( iDataWindowSize > 0 )
            {
            iModel->SetDataWindow(iDataWindowSize);
            }
        iModel->SetDataProvider( *this );
        iModel->SetTemplate( iDefaultTemplate );
        }
    catch (...)
        {
        User::Leave( KErrNoMemory );
        }
    }
// ----------------------------------------------------------------------------
// InsertItemsL
// ----------------------------------------------------------------------------
//    
    
void CGlxMulModelProviderBase::InsertItemsL( TInt aIndex, TInt aCount, TInt aMediaListFocusIndex )
    {
    TRACER("CGlxMulModelProviderBase::InsertItemsL");
    // The order of calling should be Model->Insert(..), 
    // Model->SetHighlight(..) and Widget.SetModel(..)
    // Change the order once bug EPBA-7JRDXV ( SetModel Crashes if highlight 
    // index is set >0 before setting model. )is fixed. 
    if(!iWidget.model())
        {
        CreateModelL();
        }
    
    if(iWidget.model())
        {
        iModel->SetTemplate( iDefaultTemplate );
        }
    
    iModel->Insert( aIndex, aCount );     
    
    if( aMediaListFocusIndex >= 0 )
        {
        SetFocusIndex( aMediaListFocusIndex );
        }
    
    if(!iWidget.model())
        {
        iWidget.SetModel( iModel->Accessor() );
        
        CGlxUiUtility* uiUtility = CGlxUiUtility::UtilityL();
        CleanupClosePushL( *uiUtility );
        TSize defaultIconSize = uiUtility->GetGridIconSize();
        TFileName resFile(KDC_APP_BITMAP_DIR);
        resFile.Append(KGlxIconsFilename);
        TInt defaultTextureId =  uiUtility->
                        GlxTextureManager().CreateIconTextureL( 
                        EMbmGlxiconsQgn_prop_image_notcreated, resFile, 
                        defaultIconSize ).Id();
                    
        static_cast<IMulCoverFlowWidget&>(iWidget).SetDefaultImage( defaultTextureId ) ;
        CleanupStack::PopAndDestroy( uiUtility );
        }    

    }

// ----------------------------------------------------------------------------
// RemoveItem
// ----------------------------------------------------------------------------
//    
void CGlxMulModelProviderBase::RemoveItems( TInt aIndex, TInt aCount )
    {
    TRACER("CGlxMulModelProviderBase::RemoveItems");
    // RemoveItems does not throw according to model documentation
    iModel->Remove( aIndex, aCount );
    }    

// ----------------------------------------------------------------------------
// SetFocusIndex
// ----------------------------------------------------------------------------
//    
void CGlxMulModelProviderBase::SetFocusIndex( TInt aIndex )
    {
    TRACER("CGlxMulModelProviderBase::SetFocusIndex");
    iModel->SetHighlight( aIndex );
    }
// ----------------------------------------------------------------------------
// IsFocused
// ----------------------------------------------------------------------------
//

TBool CGlxMulModelProviderBase::IsFocused( TInt aIndex ) const 
    {
    TRACER("CGlxMulModelProviderBase::IsFocused");
    return aIndex == FocusIndex() && aIndex != LAST_INDEX;
    }



// ----------------------------------------------------------------------------
// FocusIndex
// ----------------------------------------------------------------------------
//

TInt CGlxMulModelProviderBase::FocusIndex() const
    {
    TRACER("CGlxMulModelProviderBase::FocusIndex");
    TInt index = 0;
    /// @todo use multiline widget instead of this widget hack. Currently model does not have interface for highlight.
    
    // We are comparing depending on teh widget name which we are setting during the 
    // widget creation, but for that we had to do dynamic_cast which fails on hardware
    // and is not the preffered way of doing, so as of now we have to check using widget name
    
    if(UString(KListWidget) == UString(iWidget.widgetName() ) )
        {
        index = iModel->Highlight();
        }
    else if ( UString(KGridWidget) == UString(iWidget.widgetName() ) )
        {
        index = iModel->Highlight();
        }
    else if (UString(KCoverFlowWidget) == UString(iWidget.widgetName() ) )
        {
        index = iModel->Highlight();
        // Check if Model count is zero in FS view, activate back grid view.
        if (iModel->Count() ==0)
            {
            // if Count is Zero, set the navigation state to 
            // EGlxNavigationBackwards before going back to grid view
            CGlxUiUtility* uiUtility = CGlxUiUtility::UtilityL();
            CleanupClosePushL( *uiUtility );
            uiUtility->SetViewNavigationDirection(EGlxNavigationBackwards);
            CleanupStack::PopAndDestroy( uiUtility );
            
            TRAP_IGNORE( iNavigationalState->ActivatePreviousViewL() );            
            }
        }
    else
        {
        return KErrGeneral;
        }
    
    return index;
    }
    
// ----------------------------------------------------------------------------
// HandleMessageL
// ----------------------------------------------------------------------------
//    
 void CGlxMulModelProviderBase::HandleMessageL( const CMPXMessage& /*aMessage*/, 
	MGlxMediaList* /*aList*/ )
    {
    
    }
  
// ----------------------------------------------------------------------------
// HandleError
// ----------------------------------------------------------------------------
//
void CGlxMulModelProviderBase::HandleError( TInt /*aError*/ ) 
    {
    
    }
  
// ----------------------------------------------------------------------------
// HandleCommandCompleteL
// ----------------------------------------------------------------------------
//  
void CGlxMulModelProviderBase::HandleCommandCompleteL( TAny* /*aSessionId*/, CMPXCommand* /*aCommandResult*/, 
	TInt /*aError*/, MGlxMediaList* /*aList*/ )
    {
    
    }
  
// ----------------------------------------------------------------------------
// HandleMediaL
// ----------------------------------------------------------------------------
//  
void CGlxMulModelProviderBase::HandleMediaL( TInt /*aListIndex*/, MGlxMediaList* /*aList*/ )
    {
    
    }
  
// ----------------------------------------------------------------------------
// HandleItemModifiedL
// ----------------------------------------------------------------------------
//  
void CGlxMulModelProviderBase::HandleItemModifiedL( const RArray<TInt>& /*aItemIndexes*/,
	MGlxMediaList* /*aList*/ )
    {
    
    }

// ----------------------------------------------------------------------------
// HandleFocusChanged
// ----------------------------------------------------------------------------
//
//@todo: Implementation to be done later depending on the scenario	
void CGlxMulModelProviderBase::HandleFocusChanged( TInt /*aNewFocusIndex*/, 
			TInt /*aPreviousFocusIndex*/ )
	{
	// default implementation empty
	}

  
   
// ----------------------------------------------------------------------------
// BoundCommand
// ----------------------------------------------------------------------------
//
MGlxBoundCommand* CGlxMulModelProviderBase::BoundCommand( TInt aVisualItemIndex ) const
    {
    TRACER("CGlxMulModelProviderBase::BoundCommand");
   	if ( iModel->Count() > 0 )
        {
        return GlxCommandBindingUtility::Get( Item( aVisualItemIndex ) );
        }
    return NULL;
    }
    
// ----------------------------------------------------------------------------
// Item
// ----------------------------------------------------------------------------
//
const Alf::MulVisualItem& CGlxMulModelProviderBase::Item( TInt aIndex ) const
	{
	TRACER("CGlxMulModelProviderBase::Item");
	ASSERT( aIndex >= 0 && aIndex < iModel->Count() );
	return *(iModel->Data( aIndex ));
	}

//-----------------------------------------------------------------------------
// HandleResolutionChanged
//-----------------------------------------------------------------------------
//
void CGlxMulModelProviderBase::HandleOrientationChanged()
	{
	
	}

//-----------------------------------------------------------------------------
// HandleResolutionChanged
//-----------------------------------------------------------------------------
//
void CGlxMulModelProviderBase::AddWidgetEventHandler()
    {
    iWidget.AddEventHandler( *this );
    }

//EOF
