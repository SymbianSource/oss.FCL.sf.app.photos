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
* Description:    Manager of visual lists
*
*/




/**
 * @internal reviewed 31/07/2007 by Rowland Cook
 */
/**
 * @internal reviewed 04/07/2007 by M Byrne
 */


#include "glxvideoiconmanager.h"

#include <data_caging_path_literals.hrh>
//#include <uiacceltk/huitexture.h>
#include <alf/alfvisual.h>
#include <mpxmediageneraldefs.h>

#include <glxlog.h>
#include <glxtracer.h>

#include <glxicons.mbg> // icons 
#include <glxmedia.h>
#include <glxuistd.h>
#include <glxuiutility.h>
#include <glxtexturemanager.h>
#include <mglxmedialist.h>
#include "mglxvisuallist.h"

const TInt KGlxLargeVideoIconWidth = 30;
const TInt KGlxLargeVideoIconHeight = 169; 

const TInt KGlxSmallVideoIconWidth = 10;
const TInt KGlxSmallVideoIconHeight = 56; 

const TReal32 KGlxVideoIconWidth = 0.18;
const TReal32 KGlxVideoIconHeight = 1.0;

// ---------------------------------------------------------------------------
// 1st phase constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxVideoIconManager* CGlxVideoIconManager::NewL(
                           MGlxMediaList& aMediaList, MGlxVisualList& aVisualList)
	{
	TRACER("CGlxVideoIconManager::NewL");
	GLX_LOG_INFO("CGlxVideoIconManager::NewL ");
	CGlxVideoIconManager* self = 
	                new(ELeave)CGlxVideoIconManager(aMediaList, aVisualList);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//	
CGlxVideoIconManager::~CGlxVideoIconManager()
	{
	TRACER("CGlxVideoIconManager::~CGlxVideoIconManager");
	GLX_LOG_INFO("CGlxVideoIconManager::~CGlxVideoIconManager");
	iMediaList.RemoveMediaListObserver(this);
	iVisualList.RemoveObserver(this);
	}

// ---------------------------------------------------------------------------
// HandleAttributesAvailableL
// ---------------------------------------------------------------------------
// 
void CGlxVideoIconManager::HandleAttributesAvailableL(TInt aItemIndex,     
    const RArray<TMPXAttribute>& aAttributes, MGlxMediaList* /*aList*/)
	{
	TRACER("CGlxVideoIconManager::HandleAttributesAvailableL");
	GLX_LOG_INFO("CGlxVideoIconManager::HandleAttributesAvailableL ");
    if ( KErrNotFound != aAttributes.Find(KMPXMediaGeneralCategory,
                                            TMPXAttribute::Match) )
        {
        AddIconIfVideoL( aItemIndex );
        }
	}

// ---------------------------------------------------------------------------
// HandleFocusChangedL
// ---------------------------------------------------------------------------
//        
void CGlxVideoIconManager::HandleFocusChangedL( TInt aFocusIndex, TReal32 /*aItemsPerSecond*/, 
                MGlxVisualList* /*aList*/, NGlxListDefs::TFocusChangeType /*aType*/ )
    {
    TRACER("CGlxVideoIconManager::HandleFocusChangedL");
    GLX_LOG_INFO("CGlxVideoIconManager::HandleFocusChangedL ");
    if( aFocusIndex >= 0 && aFocusIndex < iMediaList.Count() )
	    {
	    // replace video icon if necessary
	    if( iFocusIndex != KErrNotFound && iFocusIndex < iMediaList.Count() )
	        {
	        AddIconIfVideoL( iFocusIndex );
	        }
	    	  	      
	    iFocusIndex = aFocusIndex;
	    
	    AddIconIfVideoL( aFocusIndex );
	    }  
    }
	
// ---------------------------------------------------------------------------
// HandleVisualAddedL
// ---------------------------------------------------------------------------
// 
void CGlxVideoIconManager::HandleVisualAddedL( CAlfVisual* /*aVisual*/, 
    TInt aIndex, MGlxVisualList* /*aList*/ )
	{
	TRACER("CGlxVideoIconManager::HandleVisualAddedL");
	GLX_LOG_INFO("CGlxVideoIconManager::HandleVisualAddedL ");
	if(aIndex >= 0 && aIndex < iMediaList.Count() )
	    {
	    AddIconIfVideoL( aIndex );
	    }   
	}

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//	
CGlxVideoIconManager::CGlxVideoIconManager(MGlxMediaList& aMediaList, 
                                            MGlxVisualList& aVisualList)
	: CGlxIconManager(aMediaList, aVisualList)
	{
	// No  implementation
	}

// ---------------------------------------------------------------------------
// 2nd phase construction
// ---------------------------------------------------------------------------
//	
void CGlxVideoIconManager::ConstructL()
	{
	TRACER("CGlxVideoIconManager::ConstructL");
	GLX_LOG_INFO("CGlxVideoIconManager::ConstructL ");
	BaseConstructL();

    TFileName resFile(KDC_APP_BITMAP_DIR);
    resFile.Append(KGlxIconsFilename);
    iLargeVideoIcon = &(iUiUtility->GlxTextureManager().
              CreateIconTextureL( EMbmGlxiconsQgn_indi_media_fullscreen_play, 
                    resFile, TSize( KGlxLargeVideoIconWidth, 
                                KGlxLargeVideoIconHeight ) ) );
                    
    iSmallVideoIcon = &(iUiUtility->GlxTextureManager().
              CreateIconTextureL( EMbmGlxiconsQgn_indi_media_fullscreen_play, 
                    resFile, TSize( KGlxSmallVideoIconWidth, 
                                KGlxSmallVideoIconHeight ) ) );
	
	// add as observers
	iMediaList.AddMediaListObserverL(this);	
	iVisualList.AddObserverL(this);
	
	
	// check for any visual already present in list
	TInt itemCount = iMediaList.Count();
	
	iFocusIndex = iMediaList.FocusIndex();
	
	for(TInt i = 0; i < itemCount; i++)
	    {
        AddIconIfVideoL(i);
	    }
	}

    
// ---------------------------------------------------------------------------
// AddIconIfVideoL
// ---------------------------------------------------------------------------
//    
void CGlxVideoIconManager::AddIconIfVideoL( TInt aIndex ) 
    {
    TRACER("CGlxVideoIconManager::AddIconIfVideoL");
    GLX_LOG_INFO("CGlxVideoIconManager::AddIconIfVideoL ");
    TGlxMedia item = iMediaList.Item( aIndex );
    if( EMPXVideo == item.Category() )                                               
        {
        if( aIndex == iMediaList.FocusIndex() )
            {
            iVisualList.RemoveIcon( aIndex, *iSmallVideoIcon );
            iVisualList.AddIconL( aIndex, *iLargeVideoIcon, 
                        NGlxIconMgrDefs::EGlxIconTopLeft, ETrue, ETrue, 0,
                                        KGlxVideoIconWidth, KGlxVideoIconHeight );
            }
        else
            {
            iVisualList.RemoveIcon( aIndex, *iLargeVideoIcon );
            iVisualList.AddIconL( aIndex, *iSmallVideoIcon, 
                            NGlxIconMgrDefs::EGlxIconTopLeft, ETrue, ETrue, 0,
                                        KGlxVideoIconWidth, KGlxVideoIconHeight );
            }                                        
        }
    }
