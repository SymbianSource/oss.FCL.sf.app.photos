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
* Description:  CommandHandlerMarking unit test cases
*
*/




//  CLASS HEADER
#include "t_glxcommandhandlermarking.h"

//  EXTERNAL INCLUDES
#include <EUnitMacros.h>
#include <EUnitDecorators.h>

#include <avkon.hrh>

#include <mglxmedialist.h>
#include <glxattributeretriever.h>
#include <glxscreenfurniture.h>

//  INTERNAL INCLUDES

//
CGlxMedia::CGlxMedia(const TGlxMediaId& aId)
		: iId(aId)
    {
    }
    
CGlxMedia::~CGlxMedia()
    {
	Reset();
    iValues.Close(); 
    iAttributes.Close();
    }    
       	
class _CGlxTestMediaList: public CBase, public MGlxMediaList
	{
public:
	~_CGlxTestMediaList() 
	    {
	    iItems.Close();
        iMedia.ResetAndDestroy();
        iSelectedItems.Close();
	    }
	    
	virtual void Close() 
	    {
	    };
	virtual TGlxMediaListId Id() const 
	    {
	    return KGlxIdNone;
	    }

	virtual TInt Count(NGlxListDefs::TCountType /*aType*/) const 
	    {
	    return iItems.Count();
	    }
	
	virtual TInt FocusIndex() const 
	    {
	    return iFocusIndex;
	    }

	virtual void SetFocusL(NGlxListDefs::TFocusSetType /*aType*/, TInt aValue) 
	    {
	    iFocusIndex = aValue;
	    }
	
	virtual const TGlxMedia& Item(TInt aIndex) const 
	    {
	    return iItems[aIndex];
	    }

	virtual TInt Index(const TGlxIdSpaceId& /* aIdSpaceId */, const TGlxMediaId& aId) const 
	    {
	    TInt count = iItems.Count();
	    for (TInt i = 0; i < count; i++)
	        {
	        if (iItems[i].Id() == aId)
	            {
	            return i;
	            }
	        }
	    return KErrNotFound;
	    }
	
	virtual void AddMediaListObserverL(MGlxMediaListObserver* /*aObserver*/) 
	    {
	    }
	
	virtual void RemoveMediaListObserver(MGlxMediaListObserver* /*aObserver*/) 
	    {
	    }

    virtual void AddContextL(const MGlxFetchContext* /*aContext*/, TInt /*aPriority*/) 
        {
        }

	virtual void RemoveContext(const MGlxFetchContext* /*aContext*/) 
	    {
	    }
	
	virtual MMPXCollection& Collection() const 
	    {
	    // we know that this method is not called in our tests, this is just to make the code compile
	    MMPXCollection* empty = NULL;
	    return *empty;
	    }

	virtual TBool IsSelected(TInt aIndex) const 
	    {
	    TInt idx = iSelectedItems.Find(aIndex);
	    return (idx != KErrNotFound);
	    }

	virtual void SetSelectedL(TInt aIndex, TBool aSelected) 
	    {
	    if (aSelected)
	        {
            iSelectedItems.InsertInOrder(aIndex);
	        }
	    else 
	        {
	        iSelectedItems.Remove(iSelectedItems.Find(aIndex));
	        }
	    }

	virtual const TArray<TInt> Selection() const 
	    {
	    return iSelectedItems.Array();
	    }

	virtual void CommandL(CMPXCommand& /*aCommand*/) 
	    {
	    }

	virtual void CancelCommand() 
	    {
	    }

	virtual void SetFilterL(CMPXFilter* /*aFilter*/) 
	    {
	    }

	virtual CMPXFilter* Filter() const 
	    {
	    return NULL;
	    }

    virtual TGlxIdSpaceId IdSpaceId(TInt /*aIndex*/) const 
        {
        return KGlxIdNone;
        }
    
    void AppendL(TInt aId, TBool aCreateMedia) 
        {
        if (aCreateMedia) 
            {
            CGlxMedia* media = new (ELeave) CGlxMedia(TGlxMediaId(aId));
            CleanupStack::PushL(media);
            iMedia.AppendL(media);
            CleanupStack::Pop(media);
            iItems.AppendL(TGlxMedia(TGlxMediaId(aId), media));
            }
        else 
            {
            iItems.AppendL(TGlxMedia(TGlxMediaId(aId)));
            }
        }
        
    CGlxMedia* MediaObj(TInt aIndex) const 
        {
        return const_cast<CGlxMedia*>(static_cast<const CGlxMedia*>(iItems[aIndex].Properties()));
        }
        
    CMPXCollectionPath* PathLC(NGlxListDefs::TPathType /*aType*/) const
    	{
    	return NULL;
    	}
    
    TInt SelectionCount() const
    	{
    	return iSelectedItems.Count();
    	}
    	
    TInt SelectedItemIndex(TInt /*aSelectionIndex*/) const
    	{
    	return 0;
    	}
    	
    TBool IsPopulated() const
        {
        return ETrue;
        }
    
    void AddStaticItemL( CGlxMedia* /*aStaticItem*/,
        NGlxListDefs::TInsertionPosition /*aTargetPosition*/ ) {};
    
    void RemoveStaticItem(const TGlxMediaId& /*aItemId*/) {};
    
    void SetStaticItemsEnabled( TBool aEnabled ) { iStaticItemsEnabled = aEnabled; };
    
    TBool IsStaticItemsEnabled() const { return iStaticItemsEnabled; };
    
    void SetFocusInitialPosition(NGlxListDefs::TFocusInitialPosition aFocusInitialPosition){};

    void ResetFocus(){};        
 
    void SetVisibleWindowIndexL( TInt aIndex ){};
    TInt VisibleWindowIndex() const {};
    
    RArray<TGlxMedia> iItems;
    RPointerArray<CGlxMedia> iMedia;
    RArray<TInt> iSelectedItems;
    TInt iFocusIndex;
    TBool iStaticItemsEnabled;
	};
	
	
TInt GlxAttributeRetriever::RetrieveL(const MGlxFetchContext&, MGlxMediaList&, TBool)
    {
    return KErrNone;
    }
    


CGlxMarkingCommandHandlerTester* CGlxMarkingCommandHandlerTester::NewL()
	{
	CGlxMarkingCommandHandlerTester* me = new(ELeave)CGlxMarkingCommandHandlerTester();
	CleanupStack::PushL(me);
	me->ConstructL();
	CleanupStack::Pop( me );
	return me;
	}
	
CGlxMarkingCommandHandlerTester::~CGlxMarkingCommandHandlerTester()
	{
    if (iMarker)
        {
        iMarker->Deactivate();
        }

	iCommandHandlerList.ResetAndDestroy();
	iCommandHandlerList.Close();
	
	delete iMediaList;
	}


CGlxMarkingCommandHandlerTester::CGlxMarkingCommandHandlerTester()
	{
	
	}
	
void CGlxMarkingCommandHandlerTester::ConstructL()
	{
	EUNIT_PRINT(_L("CGlxMarkingCommandHandlerTester::ConstructL"));
	
	iMediaList = new (ELeave) _CGlxTestMediaList;
	
	EUNIT_PRINT(_L("Construct command handler"));
	iMarker = CGlxCommandHandlerMarking::NewL(this,ETrue);
	
	iCommandHandlerList.AppendL(iMarker);
	
	iMarker->ActivateL(0);
	
	EUNIT_PRINT(_L("CGlxMarkingCommandHandlerTester::ConstructL complete"));
	}
	
MGlxMediaList& CGlxMarkingCommandHandlerTester::MediaList()
	{
	return *iMediaList;
	}
	
TBool CGlxMarkingCommandHandlerTester::ExecuteL(TInt aCommand)
	{
	return iCommandHandlerList[0]->ExecuteL( aCommand );
	}
	
TBool CGlxMarkingCommandHandlerTester::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
	{
	TKeyResponse resp = iMarker->OfferKeyEventL(aKeyEvent, aType);
	return (resp == EKeyWasConsumed);
	}
	
void CGlxMarkingCommandHandlerTester::HandleFocusChangedL(NGlxListDefs::TFocusChangeType aType, TInt aNewIndex, TInt aOldIndex)
	{
	iMarker->HandleFocusChangedL(aType, aNewIndex, aOldIndex, iMediaList);
	}

// CONSTRUCTION
ut__commandhandlermarking* ut__commandhandlermarking::NewL()
    {
    ut__commandhandlermarking* self = ut__commandhandlermarking::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

ut__commandhandlermarking* ut__commandhandlermarking::NewLC()
    {
    ut__commandhandlermarking* self = new( ELeave ) ut__commandhandlermarking();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// Destructor (virtual by CBase)
ut__commandhandlermarking::~ut__commandhandlermarking()
    {
    }

// Default constructor
ut__commandhandlermarking::ut__commandhandlermarking()
    {
    }

// Second phase construct
void ut__commandhandlermarking::ConstructL()
    {
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS


void ut__commandhandlermarking::SetupL(  )
    {
    EUNIT_PRINT(_L("ut__commandhandlermarking::SetupL"));
    iTester = CGlxMarkingCommandHandlerTester::NewL();
    }
    

void ut__commandhandlermarking::Teardown(  )
    {
    delete iTester;
    iTester = NULL;
    }
    

void ut__commandhandlermarking::T_TestCreateL(  )
    {
    }
    
void ut__commandhandlermarking::T_TestExecuteNoItemsL()
	{
	EUNIT_PRINT(_L("ut__commandhandlermarking::T_TestExecuteNoItemsL"));
	TBool exe = iTester->ExecuteL(EAknCmdMark);
	EUNIT_ASSERT_DESC( exe == EFalse, "Mark unsupported ");
	
	exe = iTester->ExecuteL(EAknCmdUnmark);
	EUNIT_ASSERT_DESC( exe == EFalse, "Unmark unsupported");
	
	exe = iTester->ExecuteL(EAknMarkAll);
	EUNIT_ASSERT_DESC( exe == EFalse, "Mark all unsupported");
	
	exe = iTester->ExecuteL(EAknUnmarkAll);
	EUNIT_ASSERT_DESC( exe == EFalse, "Unmark all unsupported");
	
	exe = iTester->ExecuteL(-1);
	EUNIT_ASSERT_DESC( exe == EFalse, "unknown command -1");
	
	exe = iTester->ExecuteL(1024);
	EUNIT_ASSERT_DESC( exe == EFalse, "unknown command 1024");
	}
	

	
void ut__commandhandlermarking::T_TestExecuteItemsL()
	{
	EUNIT_PRINT(_L("ut__commandhandlermarking::T_TestExecuteItemsL"));
	
	_CGlxTestMediaList* ml = dynamic_cast<_CGlxTestMediaList*>(&iTester->MediaList());
	
	ml->AppendL(0, ETrue);
    ml->AppendL(1, ETrue);
    ml->AppendL(2, ETrue);
    ml->AppendL(3, ETrue);
    ml->AppendL(4, ETrue);
    ml->iFocusIndex = 0;
	
	TBool exe = iTester->ExecuteL(EAknCmdMark);
	EUNIT_ASSERT_DESC( exe, "Mark supported");
	
	exe = iTester->ExecuteL(EAknCmdMark);
	EUNIT_ASSERT_DESC( exe == EFalse, "Already marked");
	
	exe = iTester->ExecuteL(EAknCmdUnmark);
	EUNIT_ASSERT_DESC( exe, "Unmark supported");
	
	exe = iTester->ExecuteL(EAknCmdUnmark);
	EUNIT_ASSERT_DESC( exe == EFalse, "Unmark not supported");
	
	exe = iTester->ExecuteL(EAknMarkAll);
	EUNIT_ASSERT_DESC( exe, "Mark all");
	
	exe = iTester->ExecuteL(EAknCmdMark);
	EUNIT_ASSERT_DESC( exe == EFalse, "Cannot mark");
	
	exe = iTester->ExecuteL(EAknMarkAll);
	EUNIT_ASSERT_DESC( exe == EFalse, "Mark all not supported");
	
	exe = iTester->ExecuteL(EAknUnmarkAll);
	EUNIT_ASSERT_DESC( exe, "Unmark all");
	
	exe = iTester->ExecuteL(EAknUnmarkAll);
	EUNIT_ASSERT_DESC( exe==EFalse, "Unmark all not supported");
	
	exe = iTester->ExecuteL(EAknCmdUnmark);
	EUNIT_ASSERT_DESC( exe == EFalse, "Unmark not supported");
	
	exe = iTester->ExecuteL(EAknCmdMark);
	EUNIT_ASSERT_DESC( exe, "Mark supported");
	
	EUNIT_PRINT(_L("ut__commandhandlermarking::T_TestExecuteItemsL change focus pos"));

	ml->iFocusIndex = 1;
	exe = iTester->ExecuteL(EAknCmdMark);
	EUNIT_ASSERT_DESC( exe, "Mark supported");
	
	exe = iTester->ExecuteL(EAknCmdUnmark);
	EUNIT_ASSERT_DESC( exe, "Unmark supported");
	
	ml->iFocusIndex = 0;
	
	exe = iTester->ExecuteL(EAknCmdMark);
	EUNIT_ASSERT_DESC( exe == EFalse, "Already marked");
	
	ml->iFocusIndex = 1;
	exe = iTester->ExecuteL(EAknCmdUnmark);
	EUNIT_ASSERT_DESC( exe == EFalse, "not marked");
	}

void ut__commandhandlermarking::T_TestOfferKeyL()
	{
	_CGlxTestMediaList* ml = dynamic_cast<_CGlxTestMediaList*>(&iTester->MediaList());
	
	ml->AppendL(0, ETrue);
    ml->AppendL(1, ETrue);
    ml->AppendL(2, ETrue);
    ml->AppendL(3, ETrue);
    ml->AppendL(4, ETrue);
    ml->iFocusIndex = 0;

  	TKeyEvent kev = { 0 };
  	kev.iModifiers |= EModifierShift;	  
  
  	// shift key pressed
    iTester->OfferKeyEventL(kev, EEventKeyDown);
    
    // change focus
    iTester->HandleFocusChangedL( NGlxListDefs::EForward, 4,0);
    
    // all items should now be marked
    TBool exe = iTester->ExecuteL(EAknCmdMark);
	EUNIT_ASSERT_DESC( exe == EFalse, "Cannot mark");
	
	exe = iTester->ExecuteL(EAknMarkAll);
	EUNIT_ASSERT_DESC( exe == EFalse, "All already marked");
	
	// shift key disabled
	kev.iModifiers = 0;
	iTester->OfferKeyEventL(kev, EEventKeyUp);
	
	iTester->OfferKeyEventL(kev, EEventKeyDown);
    
    // changing focus has no effect
    iTester->HandleFocusChangedL( NGlxListDefs::EBackward, 0,4);
    
    exe = iTester->ExecuteL(EAknCmdMark);	
	EUNIT_ASSERT_DESC( exe == EFalse, "Cannot mark");
	
	exe = iTester->ExecuteL(EAknMarkAll);
	EUNIT_ASSERT_DESC( exe == EFalse, "All already marked");
	
	kev.iModifiers |= EModifierShift;	  
  
  	// shift key down
    iTester->OfferKeyEventL(kev, EEventKeyDown);
    
    iTester->HandleFocusChangedL( NGlxListDefs::EForward, 4,0);
    
    // all items have been unmarked
    exe = iTester->ExecuteL(EAknUnmarkAll);
	EUNIT_ASSERT_DESC( exe == EFalse, "None marked");
	
	exe = iTester->ExecuteL(EAknCmdMark);
	EUNIT_ASSERT_DESC( exe, "Can mark");
	
	exe = iTester->ExecuteL(EAknCmdMark);
	EUNIT_ASSERT_DESC( exe == EFalse, "Already marked");
	
	// select with shift down
	kev.iScanCode=EStdKeyDevice3;
	TBool resp = iTester->OfferKeyEventL(kev, EEventKey);
	EUNIT_ASSERT_DESC( resp, "Event consumed");
	
	// item has been unmarked
	exe = iTester->ExecuteL(EAknCmdMark);
	EUNIT_ASSERT_DESC( exe, "Can mark");
	
	// shift key up
	// select should have no effect on marking
	kev.iScanCode=EStdKeyDevice3;
	kev.iModifiers = 0;
	resp = iTester->OfferKeyEventL(kev, EEventKeyDown);
	EUNIT_ASSERT_DESC( resp == EFalse, "Event notconsumed");
	
	exe = iTester->ExecuteL(EAknCmdMark);
	EUNIT_ASSERT_DESC( exe == EFalse, "Already marked");
	}

//  TEST TABLE
EUNIT_BEGIN_TEST_TABLE(
    ut__commandhandlermarking,
    "Add test suite description here.",
    "MODULE" )

EUNIT_TEST(
    "Test0",
    "Create marking command handler",
    "Test0",
    "FUNCTIONALITY",
    SetupL, T_TestCreateL, Teardown)
    
EUNIT_TEST(
    "Test1",
    "Test execute no items",
    "Test1",
    "FUNCTIONALITY",
    SetupL, T_TestExecuteNoItemsL, Teardown)
    
EUNIT_TEST(
    "Test2",
    "Test execute items",
    "Test2",
    "FUNCTIONALITY",
    SetupL, T_TestExecuteItemsL, Teardown)
    
 EUNIT_TEST(
    "Test3",
    "Test offer key events",
    "Test3",
    "FUNCTIONALITY",
    SetupL, T_TestOfferKeyL, Teardown)
    

EUNIT_END_TEST_TABLE

//  END OF FILE
