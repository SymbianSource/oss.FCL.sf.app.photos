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
* Description:    Visual list manager
*
*/




/**
 * @internal reviewed 04/07/2007 by M Byrne
 */

#include "glxvisuallistwindow.h" 

#include <coemain.h>
#include <alf/alfimagevisual.h>
#include <alf/alfanchorlayout.h>
#include <alf/alfenv.h> 
#include <alf/alfbrush.h>
#include <alf/alfcontrolgroup.h>
#include <alf/alfborderbrush.h>
#include <alf/alfdecklayout.h>
#include <glxlog.h>
#include <glxtracer.h>
#include <mpxmediageneraldefs.h>
#include <AknUtils.h>
#include "mglxmedialistobserver.h"
#include "glxuiutility.h"
#include "glxitemvisual.h"
#include <glxtexturemanager.h>
#include "glxvisuallistcontrol.h"
#include <mglxanimation.h>
#include <mglxmedialist.h>
#include <glxthumbnailutility.h>
#include <glxuistd.h>
#include "glxlistwindow.h"
#include "glxitemvisual.h"
// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
CGlxVisualListWindow* CGlxVisualListWindow::NewL( CGlxVisualListControl* aControl,
    MGlxMediaList* aMediaList, CGlxUiUtility* aUiUtility,
        CAlfImageVisual::TScaleMode aThumbnailScaleMode )
    {
    TRACER("CGlxVisualListWindow::NewL");
    GLX_LOG_INFO("CGlxVisualListWindow::NewL");
    CGlxVisualListWindow* self = new (ELeave) CGlxVisualListWindow();
    CleanupStack::PushL( self );
    
    self->iControl = aControl;    
    self->iMediaList = aMediaList;
    self->iUiUtility = aUiUtility;
    self->iScaleMode = aThumbnailScaleMode;
    self->ConstructL(); // Construct base class
    
    CleanupStack::Pop( self );
    return self;
  
    }
      
      
 CGlxVisualListWindow::CGlxVisualListWindow(): 
 	CGlxListWindow( static_cast< MGlxWindowObjectFactory& > ( *this ) )
	 {
	 	
	 }
 
CBase* CGlxVisualListWindow::CreateObjectL() const
	{
	TRACER("CGlxVisualListWindow::CreateObjectL");
    GLX_LOG_INFO("CGlxVisualListWindow::CreateObjectL");
	
	CGlxVisualObject* object = CGlxVisualObject::NewLC( *(const_cast<CGlxVisualListWindow*>(this)) ,*iMediaList  );
	CleanupStack::Pop( object );
	return object;
	
	}
	
	
 void CGlxVisualListWindow::SetupObject(TInt aIndex, CBase& aObject ) 
	 {
	 TRACER("CGlxVisualListWindow::SetupObject");
	 GLX_LOG_INFO("CGlxVisualListWindow::SetupObject");
	 TRAP_IGNORE(SetupObjectL( aIndex, aObject ));
	 }
// -----------------------------------------------------------------------------
// SetupObjectL
// -----------------------------------------------------------------------------
void CGlxVisualListWindow::SetupObjectL( TInt aIndex, CBase& aObject )
    {
    TRACER("CGlxVisualListWindow::SetupObjectL 2");
    GLX_LOG_INFO("CGlxVisualListWindow::SetupObjectL 2");
    CGlxVisualObject& object = static_cast< CGlxVisualObject& >( aObject );
	// set the index
	object.SetIndex( aIndex );
    
    // No easy way to get rid of this trap. Would be possible to do
    // multiple objects within the same trap by modifying the CGlxListWindowBase
    // but would be surprising if this TRAP is a actually a performance problem

    // create texture from thumbnail 
    //TSize visSize = object.Visual()->Size().ValueNow().AsSize();
	TSize visSize =iUiUtility->DisplaySize();
    
    CAlfTexture* texture = NULL; 
    
    // media will be null if aMedia's Properties are not available
    // so furtur operation need to be cancle
    if ( iMediaList->Item( aIndex ).Properties() == NULL)
        {
        texture = &iUiUtility->Env()->TextureManager().BlankTexture(); 
        } 
    else
        {
        TRAPD(err, texture = &iUiUtility->GlxTextureManager().CreateThumbnailTextureL(
                iMediaList->Item( aIndex ),iMediaList->IdSpaceId( aIndex ), visSize, &object ));
        if(err != KErrNone)
            {        
            texture = &iUiUtility->Env()->TextureManager().BlankTexture();        
            }
        }   
    // add the texture and its id for the visual object
	object.SetImage(TAlfImage( *texture  ));
	
	TSize imageSize ;
	iMediaList->Item( aIndex ).GetDimensions(imageSize);	
    // Set scale mode to EScaleFitInside if the Image is bigger then screen
    // or to scale the grid size thumbnails if full thumbnail is not available while fast swipe
	if ( imageSize.iWidth >= visSize.iWidth || imageSize.iHeight >= visSize.iHeight 
			||(texture->Size().iHeight < imageSize.iHeight  && texture->Size().iWidth < imageSize.iWidth ))
		{
		object.SetScaleMode(CAlfImageVisual::EScaleFitInside);		
		}		
			    
    // show the object. do this before notifying control, to show
    // at the image if control leaves
    object.SetVisible( ETrue );
    
    // notify observer
    iControl->HandleVisualAddedL( object.Visual(), aIndex );
    }

 void CGlxVisualListWindow::CleanupObject(TInt aListIndex, CBase& aObject) 
	{
	TRACER("CGlxVisualListWindow::CleanupObject");
    GLX_LOG_INFO("CGlxVisualListWindow::CleanupObject");
	CGlxVisualObject& object = static_cast< CGlxVisualObject& >( aObject );

    object.Reset();
    
	iControl->HandleVisualRemoved( object.Visual() );	
	}

// -----------------------------------------------------------------------------
// GetObjectL
// -----------------------------------------------------------------------------
CGlxVisualObject* CGlxVisualListWindow::GetObjectL( TInt aListIndex )
	{
	TRACER("CGlxVisualListWindow::GetObjectL");
    GLX_LOG_INFO1("CGlxVisualListWindow::GetObjectL %d", aListIndex);
	// create the object
    CGlxVisualObject* listObj = 
        CGlxVisualObject::NewLC( *this, *iMediaList );
    CleanupStack::Pop( listObj );
	return listObj;
	}

// -----------------------------------------------------------------------------
// CleanupObject
// -----------------------------------------------------------------------------
void CGlxVisualListWindow::CleanupObject( TInt aWindowIndex )
	{
	TRACER("CGlxVisualListWindow::CleanupObject 2");
    GLX_LOG_INFO1("CGlxVisualListWindow::CleanupObject 2 %d",aWindowIndex);
	CGlxVisualObject* item = ObjectByIndex( aWindowIndex);
	CAlfVisual* visual = item->Visual();
	
	iControl->HandleVisualRemoved( visual );
	}

// -----------------------------------------------------------------------------
// PostObjectsAdded
// -----------------------------------------------------------------------------	
void CGlxVisualListWindow::PostObjectsAdded( RArray<TInt>& aAddedAtListIndexes )	
    {
    /*
   TInt count = aAddedAtListIndexes.Count();
    // step through array and notify our observers
    // cant leave so need to just ignore the error
    TRAP_IGNORE( 
        {
        for(TInt i = 0; i < count; i++ )
            {
            TInt listIdx = aAddedAtListIndexes[i];
            //TInt idx =  ListIndexToWindowIndex( listIdx );
            //CGlxVisualObject* item = ObjectByIndex( idx);
    	    //iControl->HandleVisualAddedL( item->Visual(), listIdx );
            }
        }); 
       */
    }

// -----------------------------------------------------------------------------
// SetFocusIndexL
// -----------------------------------------------------------------------------
void CGlxVisualListWindow::SetFocusIndexL( TInt aFocusIndex )
	{
	TRACER("CGlxVisualListWindow::SetFocusIndexL");
    GLX_LOG_INFO1("CGlxVisualListWindow::SetFocusIndexL %d",aFocusIndex);
	CGlxListWindow::SetFocusIndex( aFocusIndex );
	
	UpdatePositions();

	}

// -----------------------------------------------------------------------------
// UpdatePositions
// -----------------------------------------------------------------------------
void CGlxVisualListWindow::UpdatePositions()
	{
	TRACER("CGlxVisualListWindow::UpdatePositions");
    GLX_LOG_INFO("CGlxVisualListWindow::UpdatePositions");
	// Set positions of items in the window to current list index.
    TGlxWindowIterator iterator = Iterator();
    TInt index = 0;
    while ( KErrNotFound != ( index = iterator++ ) )
        {
		CGlxVisualObject* visualObj = ObjectByIndex( index );
		// set index for the visual
		visualObj->SetIndex( index );
		// get screen size
		TSize screenSize = iUiUtility->DisplaySize();
		// update the layout data
		visualObj->RefreshLayout(screenSize);                        
        }
	}

// -----------------------------------------------------------------------------
// Layout
// -----------------------------------------------------------------------------
CAlfLayout* CGlxVisualListWindow::Layout()
	{
	TRACER("CGlxVisualListWindow::Layout");
    GLX_LOG_INFO("CGlxVisualListWindow::Layout");
	return static_cast<CAlfLayout*>(iControl->iParentLayout);
	}

// -----------------------------------------------------------------------------
// VisualOwner
// -----------------------------------------------------------------------------
CAlfControl& CGlxVisualListWindow::VisualOwner()
	{
	TRACER("CGlxVisualListWindow::VisualOwner");
    GLX_LOG_INFO("CGlxVisualListWindow::VisualOwner");
	return *iControl;
	}

// -----------------------------------------------------------------------------
// BorderBrush
// -----------------------------------------------------------------------------
CAlfBrush* CGlxVisualListWindow::BorderBrush()
	{
	TRACER("CGlxVisualListWindow::BorderBrush");
    GLX_LOG_INFO("CGlxVisualListWindow::BorderBrush");
	return iControl->iBorderBrush;
	}

// -----------------------------------------------------------------------------
// ThumbnailScaleMode
// -----------------------------------------------------------------------------
CAlfImageVisual::TScaleMode CGlxVisualListWindow::ThumbnailScaleMode()
    {
    TRACER("CGlxVisualListWindow::ThumbnailScaleMode");
    GLX_LOG_INFO("CGlxVisualListWindow::ThumbnailScaleMode");
    return iScaleMode;
    }

// -----------------------------------------------------------------------------
// Return visual object by index
// -----------------------------------------------------------------------------
CGlxVisualObject* CGlxVisualListWindow::ObjectByIndex( TInt aIndex ) 
    {
    TRACER("CGlxVisualListWindow::ObjectByIndex");
    GLX_LOG_INFO("CGlxVisualListWindow::ObjectByIndex");
    return static_cast< CGlxVisualObject* >( At( aIndex ) );
    }

