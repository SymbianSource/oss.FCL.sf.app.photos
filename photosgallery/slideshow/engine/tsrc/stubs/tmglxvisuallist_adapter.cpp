/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Stub file for visual list adapter
 *
*/




//  CLASS HEADER
#include "TMGlxVisualList_Adapter.h"

//  EXTERNAL INCLUDES
#include <EUnitMacros.h>
#include <mglxvisuallistobserver.h>

// declate the enum to be printed as TInt
EUNIT_DECLARE_PRINTABLE_AS_TINT( MGlxVisualList_Observer::TMGlxVisualListMethodId )

//  INTERNAL INCLUDES

// CONSTRUCTION

TMGlxVisualList_Adapter::TMGlxVisualList_Adapter( MGlxVisualList_Observer* aObserver )
	: iSize( TMGlxVisualList_Adapter_Config::KDefaultSize ),
	iFocus( TMGlxVisualList_Adapter_Config::KDefaultFocus ),
	iMGlxVisualList_Observer( aObserver ),
	iMGlxVisualList( NULL )
	{
	}

TMGlxVisualList_Adapter::~TMGlxVisualList_Adapter()
	{
	// release the array
	iObservers.Close();
	}

void TMGlxVisualList_Adapter::SetAdaptee( MGlxVisualList* aAdaptee )
	{
	iMGlxVisualList = aAdaptee;
	}

//  METHODS
TGlxVisualListId TMGlxVisualList_Adapter::Id() const
	{
	// inform the test case
	iMGlxVisualList_Observer->MGlxVisualList_MethodCalled( MGlxVisualList_Observer::E_TGlxVisualListId_Id_ );
	// call the actual method
	if ( iMGlxVisualList )
		{
		return iMGlxVisualList->Id();
		}
	return TGlxVisualListId( reinterpret_cast< unsigned int >( (void*)this ) );
	}
	
CHuiVisual* TMGlxVisualList_Adapter::Visual( TInt aListIndex )
	{
	// inform the test case
	iMGlxVisualList_Observer->MGlxVisualList_MethodCalled( MGlxVisualList_Observer::E_CHuiVisual_p_Visual_TInt_ );
	// call the actual method
	if ( iMGlxVisualList )
		{
		return iMGlxVisualList->Visual( aListIndex );
		}
	// return the index as visual
	return (CHuiVisual*)aListIndex;
	}

CGlxVisualObject* TMGlxVisualList_Adapter::Item( TInt aListIndex )
    {
    // inform the test case
    iMGlxVisualList_Observer->MGlxVisualList_MethodCalled( MGlxVisualList_Observer::E_CGlxVisualObject_p_Item_TInt_ );
    // call the actual method
    if ( iMGlxVisualList )
        {
        return iMGlxVisualList->Item( aListIndex );
        }
    // return the index as item
    return (CGlxVisualObject*)aListIndex;
    }

TInt TMGlxVisualList_Adapter::ItemCount() const
	{
	// inform the test case
	iMGlxVisualList_Observer->MGlxVisualList_MethodCalled( MGlxVisualList_Observer::E_TInt_ItemCount_ );
	// call the actual method
	if ( iMGlxVisualList )
		{
		return iMGlxVisualList->ItemCount();
		}
	return iSize;
	}

TInt TMGlxVisualList_Adapter::FocusIndex() const
	{
	// inform the test case
	iMGlxVisualList_Observer->MGlxVisualList_MethodCalled( MGlxVisualList_Observer::E_TInt_FocusIndex_ );
	// call the actual method
	if ( iMGlxVisualList )
		{
		return iMGlxVisualList->FocusIndex();
		}
	return iFocus;
	}
	
CHuiControlGroup* TMGlxVisualList_Adapter::ControlGroup() const
	{
	// inform the test case
	iMGlxVisualList_Observer->MGlxVisualList_MethodCalled( MGlxVisualList_Observer::E_CHuiControlGroup_p_ControlGroup_ );
	// call the actual method
	if ( iMGlxVisualList )
		{
		return iMGlxVisualList->ControlGroup();
		}
	return NULL;
	}
	
void TMGlxVisualList_Adapter::AddObserverL( MGlxVisualListObserver* aObserver )
	{
	// inform the test case
	iMGlxVisualList_Observer->MGlxVisualList_MethodCalled( MGlxVisualList_Observer::E_void_AddObserverL_MGlxVisualListObserver_p_ );
	
	// call the actual method
	if ( iMGlxVisualList )
		{
		iMGlxVisualList->AddObserverL( aObserver );
		}

	// add the observer to array
	iObservers.AppendL( aObserver );
	}
	
void TMGlxVisualList_Adapter::RemoveObserver( MGlxVisualListObserver* aObserver )
	{
	// inform the test case
	iMGlxVisualList_Observer->MGlxVisualList_MethodCalled( MGlxVisualList_Observer::E_void_RemoveObserver_MGlxVisualListObserver_p_ );
	// call the actual method
	if ( iMGlxVisualList )
		{
		iMGlxVisualList->RemoveObserver( aObserver );
		}
	}
	
void TMGlxVisualList_Adapter::AddLayoutL( MGlxLayout* aLayout )
	{
	// inform the test case
	iMGlxVisualList_Observer->MGlxVisualList_MethodCalled( MGlxVisualList_Observer::E_void_AddLayoutL_MGlxLayout_p_ );
	
	TInt* memAlloc = new (ELeave) TInt;
	delete memAlloc;
	
	// call the actual method
	if ( iMGlxVisualList )
		{
		iMGlxVisualList->AddLayoutL( aLayout );
		}
	}
	
void TMGlxVisualList_Adapter::RemoveLayout( const MGlxLayout* aLayout )
	{
	// inform the test case
	iMGlxVisualList_Observer->MGlxVisualList_MethodCalled( MGlxVisualList_Observer::E_void_RemoveLayout_const_MGlxLayout_p_ );
	// call the actual method
	if ( iMGlxVisualList )
		{
		iMGlxVisualList->RemoveLayout( aLayout );
		}
	}
	
TGlxViewContextId TMGlxVisualList_Adapter::AddContextL( TInt aFrontVisibleRangeOffset, TInt aRearVisibleRangeOffset )
	{
	// inform the test case
	iMGlxVisualList_Observer->MGlxVisualList_MethodCalled( MGlxVisualList_Observer::E_TGlxViewContextId_AddContextL_TInt_TInt_ );
	
	TInt* memAlloc = new (ELeave) TInt;
	delete memAlloc;
	
	// call the actual method
	if ( iMGlxVisualList )
		{
		return iMGlxVisualList->AddContextL( aFrontVisibleRangeOffset, aRearVisibleRangeOffset );
		}
	return TGlxViewContextId( reinterpret_cast< unsigned int >( (void*)this) );
	}
	
void TMGlxVisualList_Adapter::RemoveContext( const TGlxViewContextId& aContextId )
	{
	// inform the test case
	iMGlxVisualList_Observer->MGlxVisualList_MethodCalled( MGlxVisualList_Observer::E_void_RemoveContext_const_TGlxViewContextId_r_ );
	// call the actual method
	if ( iMGlxVisualList )
		{
		iMGlxVisualList->RemoveContext( aContextId );
		}
	}

void TMGlxVisualList_Adapter::NavigateL( TInt aIndexCount )
	{
	// inform the test case
	iMGlxVisualList_Observer->MGlxVisualList_MethodCalled( MGlxVisualList_Observer::E_void_NavigateL_TInt_ );
	
	TInt* memAlloc = new (ELeave) TInt;
	delete memAlloc;
	
	// call the actual method
	if ( iMGlxVisualList )
		{
		iMGlxVisualList->NavigateL( aIndexCount );
		}

	// change the focus
	iFocus = (iFocus + aIndexCount)% iSize;
	// if navigated backwards, loop the index 
	if( iFocus < 0 )
		{
		iFocus = iSize - 1;
		}

	EUNIT_PRINT( _L("Visual list focus %d"), iFocus );

	for( TInt i=0; i<iObservers.Count(); i++ )
		{
		iObservers[ i ]->HandleFocusChangedL( iFocus, 0, this, NGlxListDefs::EUnknown );
		}
	}
	
TSize TMGlxVisualList_Adapter::Size() const
	{
	// inform the test case
	iMGlxVisualList_Observer->MGlxVisualList_MethodCalled( MGlxVisualList_Observer::E_TSize_Size_ );
	// call the actual method
	if ( iMGlxVisualList )
		{
		return iMGlxVisualList->Size();
		}
	return TSize( 0, 0 );
	}
	
void TMGlxVisualList_Adapter::BringVisualsToFront()
	{
	// inform the test case
	iMGlxVisualList_Observer->MGlxVisualList_MethodCalled( MGlxVisualList_Observer::E_void_BringVisualsToFront_ );
	// call the actual method
	if ( iMGlxVisualList )
		{
		iMGlxVisualList->BringVisualsToFront();
		}
	}

void TMGlxVisualList_Adapter::EnableAnimationL(TBool /*aAnimate*/, TInt /*aIndex*/)
    {
    }

// -----------------------------------------------------------------------------
// SetDefaultIconBehaviourL
// -----------------------------------------------------------------------------
void TMGlxVisualList_Adapter::SetDefaultIconBehaviourL( TBool /*aEnable*/ )
    {
    }

//  END OF FILE
