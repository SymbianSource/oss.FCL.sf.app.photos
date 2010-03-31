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
* Description:    Implementation of the CGlxSkinChangeMonitor class
*
*/




 // Class definition

#include "glxskinchangemonitor.h" 

//-----------------------------------------------------------------------------
// NewL
//-----------------------------------------------------------------------------
CGlxSkinChangeMonitor* CGlxSkinChangeMonitor::NewL()
    {
    CGlxSkinChangeMonitor* self = new (ELeave) CGlxSkinChangeMonitor( );  
    CleanupStack::PushL( self );
    // 2nd phase
    self->ConstructL( );
    CleanupStack::Pop( self );
    return self;
    }

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
CGlxSkinChangeMonitor::~CGlxSkinChangeMonitor()
    {
    iSkinChangeObservers.Close();
    }

//-----------------------------------------------------------------------------
// C++ Constructor
//-----------------------------------------------------------------------------
CGlxSkinChangeMonitor::CGlxSkinChangeMonitor( )
    {
    }
 
//-----------------------------------------------------------------------------
// Symbian 2 phase constructor
//-----------------------------------------------------------------------------    
void CGlxSkinChangeMonitor::ConstructL()
    {
    CreateWindowL();
    }
  

//-----------------------------------------------------------------------------
// HandleResourceChange
//-----------------------------------------------------------------------------
void CGlxSkinChangeMonitor::HandleResourceChange( TInt aType )
    {
    // Call base class method
    CCoeControl::HandleResourceChange(aType);
    
	// is it skin change 
   	if( KAknsMessageSkinChange == aType )
        {  
        TInt obsCount = iSkinChangeObservers.Count();
        for (TInt obsIdx = 0; obsIdx < obsCount; ++obsIdx)
            {            
            iSkinChangeObservers[obsIdx]->HandleSkinChanged();
            }     
        }
    }

//-----------------------------------------------------------------------------
// AddSkinChangedObserverL
//-----------------------------------------------------------------------------
void CGlxSkinChangeMonitor::AddSkinChangeObserverL( MGlxSkinChangeObserver& aObserver )
	{
	iSkinChangeObservers.AppendL( &aObserver );
	}
	
//-----------------------------------------------------------------------------
// RemoveSkinChangedObserver
//-----------------------------------------------------------------------------
void CGlxSkinChangeMonitor::RemoveSkinChangeObserver( MGlxSkinChangeObserver& aObserver )
	{
	TInt index = iSkinChangeObservers.Find( &aObserver );

    if ( index != KErrNotFound ) 
        {
        iSkinChangeObservers.Remove( index );
        }
	}

	
