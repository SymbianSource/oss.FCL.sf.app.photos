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
* Description:    This class provides the necessary abstraction of the 
*                implemented class
*
*/



#include  <glxlog.h>                             // Logging
#include <glxtracer.h>
#include "glxdetailsmulmodelprovider.h"          // This class provides the necessary abstraction of the details provider
#include "glxdetailsmulmodelproviderimpl.h"      // This is the implementation class of details data provider
/** Error Id EMPY-7MKDHP **/
#include "mglxmetadatadialogobserver.h" 		//for call back to dailog
using namespace Alf;

// ----------------------------------------------------------------------------
// NewL
// ----------------------------------------------------------------------------
//
EXPORT_C CGlxDetailsMulModelProvider* CGlxDetailsMulModelProvider::NewL(
     CAlfEnv& aEnv,
	 Alf::IMulWidget& aWidget,
	 MGlxMediaList& aMediaList,
	 Alf::mulwidget::TLogicalTemplate aDefaultTemplate,
	 TInt aDataWindowSize )
    {
    TRACER("CGlxDetailsMulModelProvider::NewL");
    CGlxDetailsMulModelProvider* self = new( ELeave )	CGlxDetailsMulModelProvider();
    CleanupStack::PushL( self );
    self->ConstructL( aEnv, aWidget, aMediaList, aDefaultTemplate, aDataWindowSize );
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// ConstructL
// ----------------------------------------------------------------------------
//	 	    
void CGlxDetailsMulModelProvider::ConstructL(
     CAlfEnv& aEnv, Alf::IMulWidget& aWidget,
     MGlxMediaList& aMediaList, 
     Alf::mulwidget::TLogicalTemplate aDefaultTemplate,
     TInt aDataWindowSize)
    {
    TRACER("CGlxDetailsMulModelProvider::ConstructL");
    iImpl = CGlxDetailsMulModelProviderImpl::NewL( aEnv, aWidget, aMediaList, aDefaultTemplate, aDataWindowSize );
    }

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//	    
EXPORT_C CGlxDetailsMulModelProvider::~CGlxDetailsMulModelProvider()
	{
	TRACER("CGlxDetailsMulModelProvider::~CGlxDetailsMulModelProvider");
	delete iImpl;	
	}

// ----------------------------------------------------------------------------
// AddEntryL
// ----------------------------------------------------------------------------
//	
EXPORT_C void CGlxDetailsMulModelProvider::AddEntryL( const TGlxMulBindingSetFactory& aFactory )
	{
	TRACER("CGlxDetailsMulModelProvider::AddEntryL");	
		iImpl->AddEntryL( aFactory );	
	}

// ----------------------------------------------------------------------------
// OfferCommandL
// ----------------------------------------------------------------------------
//		
EXPORT_C TBool CGlxDetailsMulModelProvider::OfferCommandL( TInt aCommandId )
	{
	TRACER("CGlxDetailsMulModelProvider::OfferCommandL"); 
	 return iImpl->OfferCommandL(aCommandId);
	}

// ----------------------------------------------------------------------------
// InitMenuL
// ----------------------------------------------------------------------------
//			
EXPORT_C void CGlxDetailsMulModelProvider::InitMenuL( CEikMenuPane& aMenu )
	{
	TRACER("CGlxDetailsMulModelProvider::InitMenuL");
	iImpl->InitMenuL(aMenu);	
	}
// ----------------------------------------------------------------------------
// HideLocationMenuItem
// ----------------------------------------------------------------------------
//
EXPORT_C void CGlxDetailsMulModelProvider::HideLocationMenuItem( CEikMenuPane& aMenu )
    {
    iImpl->HideLocationMenuItem(aMenu);
    }

// ----------------------------------------------------------------------------
// EnableMskL
// ----------------------------------------------------------------------------
//
EXPORT_C void CGlxDetailsMulModelProvider::EnableMskL()
    {
    TRACER("CGlxDetailsMulModelProviderImpl::EnableMskL");
    iImpl->EnableMskL();
    }


// ----------------------------------------------------------------------------
// SetDetailsDailogObserver
// ----------------------------------------------------------------------------
//
EXPORT_C void CGlxDetailsMulModelProvider::SetDetailsDailogObserver( MGlxMetadataDialogObserver& aObserver)
    {
 	TRACER("CGlxDetailsMulModelProviderImpl::SetDetailsDailogObserver");
 	/** Error Id EMPY-7MKDHP **/
 	iImpl->SetDetailsDailogObserver( aObserver ) ;
    }
