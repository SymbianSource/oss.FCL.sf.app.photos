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
* Description:    set the required attribute to the medialist
*
*/




#include "glxattributerequirement.h"    // This class will set the required attribute to the medialist
#include <mpxattribute.h>               // MPX Attribute data type class
#include <glxuistd.h>                   // attribute/thumbnail fetch context priority definitions
#include <glxlog.h>                     // Logging
#include <glxtracer.h>
#include <AknUtils.h>
#include <glxuiutility.h>
// ----------------------------------------------------------------------------
// Two phase construction
// ----------------------------------------------------------------------------
//
EXPORT_C CGlxAttributeRequirements*  CGlxAttributeRequirements::NewL( 
    MGlxMediaList& aMediaList )
	{
	TRACER("CGlxAttributeRequirements::NewL");
	CGlxAttributeRequirements* self = CGlxAttributeRequirements::NewLC(   
	    aMediaList );
	CleanupStack::Pop( self );
	self->ConstructL();
	return self;
	}

// ----------------------------------------------------------------------------
// Two phase construction
// ----------------------------------------------------------------------------
//
CGlxAttributeRequirements*  CGlxAttributeRequirements::NewLC( MGlxMediaList& 
    aMediaList )
	{
	TRACER("CGlxAttributeRequirements::NewLC");
	CGlxAttributeRequirements* self = new ( ELeave ) CGlxAttributeRequirements(
	     aMediaList );
	CleanupStack::PushL( self );
	return self;
	}

// ----------------------------------------------------------------------------
// Deafult constructor
// ----------------------------------------------------------------------------
//	
CGlxAttributeRequirements::CGlxAttributeRequirements( MGlxMediaList& aMediaList):
    iMediaList( aMediaList )
	{
	}
	
// ----------------------------------------------------------------------------
// ConstructL
// ----------------------------------------------------------------------------
//	
void CGlxAttributeRequirements::ConstructL()
    {
        
    }

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//
CGlxAttributeRequirements::~CGlxAttributeRequirements()
	{
	TRACER("CGlxAttributeRequirements::~CGlxAttributeRequirements");
    for( TInt i = 0;i <=iArributeContext.Count()-1; i++ )
       { 
        iMediaList.RemoveContext(iArributeContext[i]);
        }
    iArributeContext.ResetAndDestroy();
    
	//remove the context and delete the same
	iMediaList.RemoveContext(iFsThumbnailContext);
    delete iFsThumbnailContext;
    
    for( TInt i = 0;i <=iThumbnailContext.Count()-1; i++ )
	    {
	    iMediaList.RemoveContext(iThumbnailContext[i]);
	    }
    iThumbnailContext.ResetAndDestroy();
	}

// ----------------------------------------------------------------------------
// Creating the attribute context and setting it to the medialist
// ----------------------------------------------------------------------------
//	
void CGlxAttributeRequirements::AddAttributeL( const TMPXAttribute& aAttribute )
    {
    TRACER("CGlxAttributeRequirements::AddAttributeL");	
	iArributeContext.AppendL(CGlxDefaultAttributeContext::NewL() );
	iArributeContext[iArributeContext.Count()-1]->SetRangeOffsets(10,10);
	iArributeContext[iArributeContext.Count()-1]->AddAttributeL( aAttribute );
	iMediaList.AddContextL( iArributeContext[iArributeContext.Count()-1], KMaxTInt );		
    }

// ----------------------------------------------------------------------------
// Creating the thumbnail context and setting it to the medialist
// ----------------------------------------------------------------------------
//	
void CGlxAttributeRequirements::AddThumbnailL( const TSize& aSize )
    {
    TRACER("CGlxAttributeRequirements::AddThumbnailL");
    if( aSize.iHeight > 200 )
    	{
    	iFromFocusOutwardIterator.SetRangeOffsets( 5,5 );
    	//check if Fullscreen thumbnail context is initialized
		if(!iFsThumbnailContext)
			{
			//created new context, set the priority higher than grid thumbnail requests and set the default spec based on the 
			//current orientation
			iFsThumbnailContext = CGlxThumbnailContext::NewL(&iFromFocusOutwardIterator);
			iFsThumbnailContext->SetDefaultSpec(aSize.iWidth, aSize.iHeight);
			iMediaList.AddContextL(iFsThumbnailContext, KGlxFetchContextPriorityGridViewFullscreenVisibleThumbnail);
			}			
		else
			{
			//Fullscreen context already created, hence, modify the default spec, to take care of orientation change
			iFsThumbnailContext->SetDefaultSpec(aSize.iWidth, aSize.iHeight);
			}
    	}
    else
    	{
        if (AknLayoutUtils::PenEnabled())
            {
            iThumbnailContext.AppendL(CGlxThumbnailContext::NewL(&iBlockyIterator));
            }
        else
            {
            CGlxUiUtility* uiUtility  = CGlxUiUtility::UtilityL();
            CleanupClosePushL(*uiUtility);    
            TInt visItems = uiUtility->VisibleItemsInPageGranularityL();
            CleanupStack::PopAndDestroy(uiUtility);            
            
            //we are fetching visible items on priority, 
            //5 pages would be good to have as it uses FromFocusOutwardIterator
            iFromFocusOutwardIterator.SetRangeOffsets(3*visItems , 2*visItems );
            iThumbnailContext.AppendL(CGlxThumbnailContext::NewL(&iFromFocusOutwardIterator)); 
            }
        iThumbnailContext[iThumbnailContext.Count()-1]->SetDefaultSpec (aSize.iWidth, aSize.iHeight );
        iMediaList.AddContextL( iThumbnailContext[iThumbnailContext.Count()-1], KGlxFetchContextPriorityNormal );
    	}
    }
