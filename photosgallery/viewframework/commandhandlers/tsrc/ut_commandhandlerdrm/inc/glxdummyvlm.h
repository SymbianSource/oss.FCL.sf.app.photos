/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    List control unit test
*
*/




//  INTERNAL INCLUDES

#include <mglxvisuallist.h>
#include <glxvisuallistmanager.h>


#include <Alf/AlfImageVisual.h>
#include <Alf/AlfControl.h>

#include <glxuiutility.h>

// test control
class CTestControl : public CAlfControl
	{
	public:
		static CTestControl* NewL( CAlfEnv& aEnv )
			{
			return new (ELeave) CTestControl( aEnv );
			}
		CTestControl( CAlfEnv& aEnv )
	     //   : CAlfControl( aEnv )
	    	{
	    	}	
	};

// ---------------------------------------------------------------------------

class CGlxDummyVisualList : public MGlxVisualList
    {
public:

    static CGlxDummyVisualList* NewL();
    ~CGlxDummyVisualList();
    
private:    // From MGlxVisualList
    void ConstructL();
    
    TGlxVisualListId Id() const { return TGlxVisualListId(1); }
    CAlfVisual* Visual(TInt aListIndex) 
        { 
        if( aListIndex == 0 )
            {
            return NULL;
            }
        else
            {
            return iVisual;
            }
        }
    CGlxVisualObject* Item(TInt /*aListIndex*/) { return NULL; }
    TInt ItemCount(NGlxListDefs::TCountType aType = NGlxListDefs::ECountAll) const { return 0; }
    TInt FocusIndex() const { return 0; }
    CAlfControlGroup* ControlGroup() const { return NULL; }
    void AddObserverL(MGlxVisualListObserver* /*aObserver*/) { }
    void RemoveObserver(MGlxVisualListObserver* /*aObserver*/) { }
    void AddLayoutL(MGlxLayout* /*aLayout*/) { }
    void RemoveLayout(const MGlxLayout* /*aLayout*/) { }
    TGlxViewContextId AddContextL(TInt /*aFrontVisibleRangeOffset*/,
            TInt /*aRearVisibleRangeOffset*/) { return TGlxViewContextId(1); }
    void RemoveContext(const TGlxViewContextId& /*aContextId*/) { }
    void NavigateL(TInt /*aIndexCount*/) { }
    TSize Size() const { return TSize(); }
    void BringVisualsToFront() { }
    void EnableAnimationL(TBool /*aAnimate*/, TInt /*aIndex*/) { }
    /// @ref MGlxVisualList::SetDefaultIconBehaviour
    void SetDefaultIconBehaviourL( TBool /*aEnable*/ ) { }
    
    void AddIconL( TInt , const CAlfTexture& , 
            NGlxIconMgrDefs::TGlxIconPosition ,
            TBool , TBool ,  TInt, TReal32, TReal32  ) { }
            
    TBool RemoveIcon( TInt , const CAlfTexture&  ) { return ETrue; }
    
    void SetIconVisibility( TInt , const CAlfTexture&, TBool  ) { }
    
private:
    CGlxUiUtility* iUiUtility;
    CTestControl* iTestControl;
    CAlfImageVisual* iVisual;
    };

// -----------------------------------------------------------------------------
// NewL
// -----------------------------------------------------------------------------
// 
CGlxDummyVisualList* CGlxDummyVisualList::NewL()
    {
    CGlxDummyVisualList* self = new(ELeave)CGlxDummyVisualList();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//    
CGlxDummyVisualList::~CGlxDummyVisualList()
    {
    delete iTestControl;
    
    if ( iUiUtility )
        {
        iUiUtility->Close();
        }
    }

// -----------------------------------------------------------------------------
// ConstructL
// -----------------------------------------------------------------------------
//    
void CGlxDummyVisualList::ConstructL()
    {
    iUiUtility = CGlxUiUtility::UtilityL();

    iTestControl = CTestControl::NewL( *iUiUtility->Env() );
    
    // create the visual, ownership goes to iControl
	iVisual = CAlfImageVisual::AddNewL( *iTestControl );
    }


// -----------------------------------------------------------------------------
// ManagerL
// -----------------------------------------------------------------------------
//
EXPORT_C CGlxVisualListManager* CGlxVisualListManager::ManagerL()
    {
    return NULL;
    }

// -----------------------------------------------------------------------------
// Close
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxVisualListManager::Close()
    {
    }

// -----------------------------------------------------------------------------
// AllocListL
// -----------------------------------------------------------------------------
//
EXPORT_C MGlxVisualList* CGlxVisualListManager::AllocListL(
        MGlxMediaList& /*aMediaList*/, CAlfEnv& /*aEnv*/,
        CAlfDisplay& /*aDisplay*/,
    	CAlfImageVisual::TScaleMode /*aThumbnailScaleMode*/ )
    {
    return CGlxDummyVisualList::NewL();
    }
    
// -----------------------------------------------------------------------------
// ListL
// -----------------------------------------------------------------------------
//
EXPORT_C MGlxVisualList* CGlxVisualListManager::ListL(
        MGlxMediaList& /*aMediaList*/, CAlfEnv& /*aEnv*/,
        CAlfDisplay& /*aDisplay*/,
    	CAlfImageVisual::TScaleMode /*aThumbnailScaleMode*/ )
    {
    return CGlxDummyVisualList::NewL();
    }

// -----------------------------------------------------------------------------
// Removes a reference to the list, an deletes it if no more references remain
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxVisualListManager::ReleaseList(MGlxVisualList* aList)
    {
    delete static_cast<CGlxDummyVisualList*>(aList);
    }

//  END OF FILE
