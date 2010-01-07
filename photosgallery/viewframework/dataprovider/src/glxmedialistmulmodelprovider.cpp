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
* Description:    This class provides the necessary abstraction of the implemented class
*
*/




#include <glxlog.h>                               // Logging
#include <glxtracer.h>
#include "glxmedialistmulmodelprovider.h"         // CGlxMediaListMulModelProvider is the provider class which provides data for  list view, grid view
#include "glxmedialistmulmodelproviderimpl.h"     // The actual implemntation for providign the data to the clients is done here

using namespace Alf;

// ----------------------------------------------------------------------------
// NewL
// ----------------------------------------------------------------------------
//
EXPORT_C CGlxMediaListMulModelProvider* CGlxMediaListMulModelProvider::NewL( CAlfEnv& aEnv,
																	IMulWidget& aWidget, 
																	MGlxMediaList& aMediaList, 
																	const TGlxMulBindingSetFactory& aFactory, 
																    Alf::mulwidget::TLogicalTemplate aDefaultTemplate,
																	TInt aDataWindowSize )
    {
    TRACER("CGlxMediaListMulModelProvider::NewL");
    CGlxMediaListMulModelProvider* self = new ( ELeave) CGlxMediaListMulModelProvider();
    CleanupStack::PushL( self );
    self->ConstructL( aEnv, aWidget, aMediaList, aFactory, aDefaultTemplate, aDataWindowSize );
    CleanupStack::Pop( self );
    return self;
    }
 
CGlxMediaListMulModelProvider::CGlxMediaListMulModelProvider()
    {
    }
 
// ----------------------------------------------------------------------------
// ConstructL
// ----------------------------------------------------------------------------
//
void CGlxMediaListMulModelProvider::ConstructL(CAlfEnv& aEnv, 
				                            	Alf::IMulWidget& aWidget, 
				                            	MGlxMediaList& aMediaList,
				                            	const TGlxMulBindingSetFactory& aFactory, 
				                            	Alf::mulwidget::TLogicalTemplate aDefaultTemplate, TInt aDataWindowSize )
    {
    TRACER("CGlxMediaListMulModelProvider::ConstructL");
    iImpl = CGlxMediaListMulModelProviderImpl::NewL( aEnv, aWidget, aMediaList, 
        aFactory, aDefaultTemplate, aDataWindowSize );
    }
    
// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//
CGlxMediaListMulModelProvider::~CGlxMediaListMulModelProvider()
    {
    TRACER("CGlxMediaListMulModelProvider::~CGlxMediaListMulModelProvider");
	delete iImpl;
  	}
  	
// ----------------------------------------------------------------------------
// UpdateItems
// ----------------------------------------------------------------------------
//
EXPORT_C void CGlxMediaListMulModelProvider::UpdateItems( TInt aIndex, TInt aCount )
    {
    TRACER("CGlxMediaListMulModelProvider::UpdateItems");
    iImpl->UpdateItems(aIndex, aCount);
    }
