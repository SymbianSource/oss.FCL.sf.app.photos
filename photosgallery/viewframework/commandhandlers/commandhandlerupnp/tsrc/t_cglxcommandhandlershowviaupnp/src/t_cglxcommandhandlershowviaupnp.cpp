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
* Description:  CommandHandlerShowViaUPnP unit test cases
*
*/




//  CLASS HEADER
#include "t_cglxcommandhandlershowviaupnp.h"

//  EXTERNAL INCLUDES
#include <digia/eunit/eunitmacros.h>
#include <digia/eunit/eunitdecorators.h>

#include <glxcommandhandlers.hrh>
#include <mglxmedialist.h>
#include <upnpshowcommand.h>

//  INTERNAL INCLUDES
#include "glxupnprenderer.h" 

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
    virtual void CancelPreviousRequests() {};
    RArray<TGlxMedia> iItems;
    RPointerArray<CGlxMedia> iMedia;
    RArray<TInt> iSelectedItems;
    TInt iFocusIndex;
    TBool iStaticItemsEnabled;
    };
// CONSTRUCTION
t_cglxcommandhandlershowviaupnp* t_cglxcommandhandlershowviaupnp::NewL()
    {
    t_cglxcommandhandlershowviaupnp* self = t_cglxcommandhandlershowviaupnp::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

t_cglxcommandhandlershowviaupnp* t_cglxcommandhandlershowviaupnp::NewLC()
    {
    t_cglxcommandhandlershowviaupnp* self = new( ELeave ) t_cglxcommandhandlershowviaupnp();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// Destructor (virtual by CBase)
t_cglxcommandhandlershowviaupnp::~t_cglxcommandhandlershowviaupnp()
    {
    }

// Default constructor
t_cglxcommandhandlershowviaupnp::t_cglxcommandhandlershowviaupnp()
    {
    }

// Second phase construct
void t_cglxcommandhandlershowviaupnp::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }


//Create the media List

MGlxMediaList& t_cglxcommandhandlershowviaupnp::MediaList()
    {
#if 0
    EUNIT_PRINT(_L("Entering  MediaList"));
    CMPXCollectionPath* path = CMPXCollectionPath :: NewL();
    CleanupStack::PushL(path);      
    EUNIT_PRINT(_L("MediaList :: MPXCollectionPath created"));
    iList = MGlxMediaList::InstanceL(*path,TGlxHierarchyId(NULL),NULL);
    EUNIT_PRINT(_L("Exiting  MediaList"));
    CleanupStack::PopAndDestroy(path);         
    return *iList;
#endif
    }



//  METHODS
//Test the NewL() without medialist
void t_cglxcommandhandlershowviaupnp::SetupL(  )
    {          
    iCommandShowUpnp = CGlxCommandHandlerShowViaUpnp :: NewL(NULL, EFalse);
    }

//  Medialist created 
void t_cglxcommandhandlershowviaupnp::SetupL1(  )
    { 
    //create the medialist
    iList = new (ELeave) _CGlxTestMediaList;
    EUNIT_PRINT(_L("SetupL1 :: created Medialist"));
    iCommandShowUpnp = CGlxCommandHandlerShowViaUpnp :: NewL(this, EFalse);
    EUNIT_PRINT(_L("Exiting  SetupL1"));
    }


   
//Test DoIsDisabled

//Command Id ShowViaUpnp
void t_cglxcommandhandlershowviaupnp::T_DoIsDisabled_Show()
    {       
    TBool result =  ETrue;                                   
    EUNIT_ASSERT_EQUALS(result,(iCommandShowUpnp->DoIsDisabled(EGlxShowViaUpnp,*iList)));                                        
    }

//Command Id StopShowing
void t_cglxcommandhandlershowviaupnp::T_DoIsDisabled_Stop()
    {
    TBool result =  ETrue;   
    EUNIT_ASSERT_EQUALS(result,(iCommandShowUpnp->DoIsDisabled(EGlxStopShowing,*iList)));  
    }


//Command Id Null
void t_cglxcommandhandlershowviaupnp::T_DoIsDisabled_NULL()
    {
    TBool result =  ETrue;   
    EUNIT_ASSERT_EQUALS(result,(iCommandShowUpnp->DoIsDisabled(NULL,*iList)));  
    }

//Command Id Invalid 
void t_cglxcommandhandlershowviaupnp::T_DoIsDisabled_Invalid()
    {
    TBool result =  ETrue;   
    EUNIT_ASSERT_EQUALS(result,(iCommandShowUpnp->DoIsDisabled(100,*iList)));  
    }

//Show Command Available

//Command Id ShowViaUpnp
void t_cglxcommandhandlershowviaupnp::T_DoIsDisabled_Show_Available()
    { 
    EUNIT_PRINT(_L("Entering  T_DoIsDisabled_Show_Available"));
    TBool result =  ETrue;                                   
    CUpnpShowCommand::SetUpIsAvailable(ETrue);
    
    EUNIT_ASSERT_EQUALS(result,(iCommandShowUpnp->DoIsDisabled(EGlxShowViaUpnp,*iList)));                                        
    }

//Command Id StopShowing
void t_cglxcommandhandlershowviaupnp::T_DoIsDisabled_Stop_Available()
    {
    TBool result =  EFalse;     
    CUpnpShowCommand::SetUpIsAvailable(ETrue);
    GlxUpnpRenderer::StartShowingL();
    EUNIT_ASSERT_EQUALS(result,(iCommandShowUpnp->DoIsDisabled(EGlxStopShowing,*iList)));  
    GlxUpnpRenderer::StopShowingL();    
    }


//Command Id Null
void t_cglxcommandhandlershowviaupnp::T_DoIsDisabled_NULL_Available()
    {
    TBool result =  ETrue; 
    CUpnpShowCommand::SetUpIsAvailable(ETrue);
      
    EUNIT_ASSERT_EQUALS(result,(iCommandShowUpnp->DoIsDisabled(NULL,*iList)));  
    }

//Command Id Invalid 
void t_cglxcommandhandlershowviaupnp::T_DoIsDisabled_Invalid_Available()
    {
    TBool result =  ETrue;   
    CUpnpShowCommand::SetUpIsAvailable(ETrue);
    
    EUNIT_ASSERT_EQUALS(result,(iCommandShowUpnp->DoIsDisabled(100,*iList)));  
    }




//Test DoExecuteL

//Command Id start
void t_cglxcommandhandlershowviaupnp::T_DoExecuteL_Show()
    {
    TBool result =  ETrue;    
    EUNIT_ASSERT_EQUALS(result,(iCommandShowUpnp->DoExecuteL(EGlxShowViaUpnp,*iList))); 
    GlxUpnpRenderer::StopShowingL();                             
    }


//Command Id Stop
void t_cglxcommandhandlershowviaupnp::T_DoExecuteL_Stop()
    {
    GlxUpnpRenderer::StartShowingL();
    TBool result =  ETrue;  
    EUNIT_ASSERT_EQUALS(result,(iCommandShowUpnp->DoExecuteL(EGlxStopShowing,*iList)));            
    }

//Command Id NUll - return EFalse command should not be handled
void t_cglxcommandhandlershowviaupnp::T_DoExecuteL_NULL()
    {
    TBool result =  EFalse;  
    EUNIT_ASSERT_EQUALS(result,(iCommandShowUpnp->DoExecuteL(NULL,*iList)));
    }

//Command Id Invalid -return EFalse command should not be handled
void t_cglxcommandhandlershowviaupnp::T_DoExecuteL_Invalid()
    {
    TBool result =  EFalse;  
    EUNIT_ASSERT_EQUALS(result,(iCommandShowUpnp->DoExecuteL(100,*iList)));
    }


//Tear down
void t_cglxcommandhandlershowviaupnp::Teardown(  )
    {
    EUNIT_PRINT(_L("before teardown"));
    
    if(iCommandShowUpnp)
        {
        delete iCommandShowUpnp;                  
        }    

    if(iList)   
        {
        EUNIT_PRINT(_L("before closing list"));
        delete iList;
        iList = NULL;
        }   
    EUNIT_PRINT(_L("After teardown"));        
    }
    

//Check the creation
void t_cglxcommandhandlershowviaupnp::T_CGlxCommandHandlerShowViaUpnpL(  )
    {
    EUNIT_ASSERT_DESC( iCommandShowUpnp, "Creation Failed");
    }
    
    

//  TEST TABLE
EUNIT_BEGIN_TEST_TABLE(
    t_cglxcommandhandlershowviaupnp,
    "Add test suite description here.",
    "UNIT" )


//Set up function without Media List

EUNIT_TEST(
    "Create-nomedialist",
    "CGlxCommandHandlerShowViaUpnp",
    "NewL",
    "UNIT",
    SetupL, T_CGlxCommandHandlerShowViaUpnpL, Teardown)
    
   
//Set up function without Media List

EUNIT_TEST(
    "Create-medialist",
    "CGlxCommandHandlerShowViaUpnp",
    "NewL-medialist",
    "UNIT",
    SetupL1, T_CGlxCommandHandlerShowViaUpnpL, Teardown)


//DoIsDisable the command handler with UpnpShowCommand Not available

EUNIT_TEST(
    "Create1",
    "CGlxCommandHandlerShowViaUpnp",
    "Disable_show_NotAvailable",
    "UNIT",
    SetupL, T_DoIsDisabled_Show, Teardown)

EUNIT_TEST(
    "Create2",
    "CGlxCommandHandlerShowViaUpnp",
    "Disable_stop_NotAvailable",
    "UNIT",
    SetupL, T_DoIsDisabled_Stop, Teardown)

EUNIT_TEST(
    "Create3",
    "CGlxCommandHandlerShowViaUpnp",
    "Disable_Null_NotAvailable",
    "UNIT",
    SetupL, T_DoIsDisabled_NULL, Teardown)


EUNIT_TEST(
    "Create4",
    "CGlxCommandHandlerShowViaUpnp",
    "Disable_Invalid_NotAvailable",
    "UNIT",
    SetupL, T_DoIsDisabled_Invalid, Teardown)

//Disabling the command handler with UpnpShowCommand Available

/*EUNIT_TEST(
    "Create5",
    "CGlxCommandHandlerShowViaUpnp",
    "Disable_stop_Available",
    "UNIT",
    SetupL1, T_DoIsDisabled_Show_Available, Teardown)*/

EUNIT_TEST(
    "Create6",
    "CGlxCommandHandlerShowViaUpnp",
    "Disable_Null_Available",
    "UNIT",
    SetupL1, T_DoIsDisabled_NULL_Available, Teardown)


EUNIT_TEST(
    "Create7",
    "CGlxCommandHandlerShowViaUpnp",
    "Disable_Invalid_Available",
    "UNIT",
    SetupL1, T_DoIsDisabled_Invalid_Available, Teardown) 
  
  
/*EUNIT_TEST(
    "Create8",
    "CGlxCommandHandlerShowViaUpnp",
    "Disable_show_NotAvailable",
    "UNIT",
    SetupL1, T_DoIsDisabled_Stop_Available, Teardown)
    

//DoExecuteL with with UpnpShowCommand Not available

EUNIT_TEST(
    "Create9",
    "CGlxCommandHandlerShowViaUpnp",
    "Execute_Show",
    "UNIT",
    SetupL1, T_DoExecuteL_Show, Teardown)
    
EUNIT_TEST(
    "Create10",
    "CGlxCommandHandlerShowViaUpnp",
    "Execute_stop",
    "FUNCTIONALITY",
    SetupL1, T_DoExecuteL_Stop, Teardown) */
    
EUNIT_TEST(
    "Create11",
    "CGlxCommandHandlerShowViaUpnp",
    "Execute_Null",
    "UNIT",
    SetupL1, T_DoExecuteL_NULL, Teardown)
    
EUNIT_TEST(
    "Create12",
    "CGlxCommandHandlerShowViaUpnp",
    "Execute_Invalid",
    "UNIT",
    SetupL1, T_DoExecuteL_Invalid, Teardown)

//DoExecuteL without medialist
/*/

EUNIT_TEST(
    "Create",
    "CGlxCommandHandlerShowViaUpnp",
    "Execute_stop",
    "UNIT",
    SetupL, T_DoExecuteL_Stop, Teardown)
    
EUNIT_TEST(
    "Create",
    "CGlxCommandHandlerShowViaUpnp",
    "Execute_Null",
    "FUNCTIONALITY",
    SetupL, T_DoExecuteL_NULL, Teardown)
    
EUNIT_TEST(
    "Create",
    "CGlxCommandHandlerShowViaUpnp",
    "Execute_Invalid",
    "UNIT",
    SetupL, T_DoExecuteL_Invalid, Teardown)
    
    
EUNIT_TEST(
    "Create",
    "CGlxCommandHandlerShowViaUpnp",
    "Execute_Invalid",
    "UNIT",
    SetupL, T_DoExecuteL_Show, Teardown)       */ 
   

EUNIT_END_TEST_TABLE

//  END OF FILE

