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
* Description:    Upnp Command Handlers.
*
*/




//  CLASS HEADER
#include "t_glxcommandhandlercopytohomenetwork.h"

//  EXTERNAL INCLUDES
#include <digia/eunit/eunitmacros.h>
#include <digia/eunit/eunitdecorators.h>

#include <glxcommandhandlers.hrh>
#include <mglxmedialist.h>
#include <upnpshowcommand.h>

//  INTERNAL INCLUDES
#include "glxupnprenderer.h" 
#include <upnpcopycommand.h>
#include <glxattributeretriever.h>

TInt GlxAttributeRetriever::RetrieveL(const MGlxFetchContext& /*aContext*/, 
		MGlxMediaList& /*aList*/, TBool /*aShowDialog*/)
	{
	return KErrNone;
	}

TBool CUpnpCopyCommand::gUpnpCopyCommandEnabled = EFalse;

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
    void CancelPreviousRequests() {};
    RArray<TGlxMedia> iItems;
    RPointerArray<CGlxMedia> iMedia;
    RArray<TInt> iSelectedItems;
    TInt iFocusIndex;
    TBool iStaticItemsEnabled;
    };
 
// CONSTRUCTION
t_cglxcommandhandlercopytohomenetwork* t_cglxcommandhandlercopytohomenetwork::NewL()
    {
    t_cglxcommandhandlercopytohomenetwork* self = t_cglxcommandhandlercopytohomenetwork::NewLC();
    CleanupStack::Pop();
    return self;
    }

t_cglxcommandhandlercopytohomenetwork* t_cglxcommandhandlercopytohomenetwork::NewLC()
    {
    t_cglxcommandhandlercopytohomenetwork* self = new( ELeave ) t_cglxcommandhandlercopytohomenetwork();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// Destructor (virtual by CBase)
t_cglxcommandhandlercopytohomenetwork::~t_cglxcommandhandlercopytohomenetwork()
    {
    }

// Default constructor
t_cglxcommandhandlercopytohomenetwork::t_cglxcommandhandlercopytohomenetwork()
    {
    }

// Second phase construct
void t_cglxcommandhandlercopytohomenetwork::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }


//Create the media List

MGlxMediaList& t_cglxcommandhandlercopytohomenetwork::MediaList()
    {
    CMPXCollectionPath* path = CMPXCollectionPath :: NewL();
    CleanupStack::PushL(path);      

    iList = MGlxMediaList::InstanceL(*path,TGlxHierarchyId(NULL),NULL);
    CleanupStack::PopAndDestroy(path);     

    return *iList;
    }

//  Medialist created 
void t_cglxcommandhandlercopytohomenetwork::SetupL(  )
    { 
    iList = NULL;
    iCommandHandlerCopyToHomeNetwork = NULL;
    //create the medialist
   // MediaList();       
  //  iCommandHandlerCopyToHomeNetwork = CGlxCommandHandlerCopyToHomeNetwork :: NewL(*this);
    iList = new (ELeave) _CGlxTestMediaList;
    iCommandHandlerCopyToHomeNetwork = new (ELeave) CGlxCommandHandlerCopyToHomeNetwork(*this); 
    }

//Tear down
void t_cglxcommandhandlercopytohomenetwork::Teardown(  )
    {
    EUNIT_PRINT(_L("before teardown"));
    
    delete iCommandHandlerCopyToHomeNetwork;
    iCommandHandlerCopyToHomeNetwork = NULL;

    if(iList)   
        {
        delete iList;
        iList = NULL;
        }   
    EUNIT_PRINT(_L("After teardown"));        
    }
    

void t_cglxcommandhandlercopytohomenetwork::T_AddCopyToHomeNetworkCommandL_L()
	{
/// @bug:minor:kihoikka:release5.5: 1/02/2008: you could have a different setup if you dont always want the instance created
	delete iCommandHandlerCopyToHomeNetwork; // we don't want to use the iCommandHandlerCopyToHomeNetwork instance created by SetupL because ConstructL will have been called
	iCommandHandlerCopyToHomeNetwork = NULL;
	iCommandHandlerCopyToHomeNetwork = new (ELeave) CGlxCommandHandlerCopyToHomeNetwork(*this); // We don't want to call ConstructL()
	EUNIT_ASSERT_EQUALS_DESC(iCommandHandlerCopyToHomeNetwork->iCommandInfoArray.Count(), 0, "command info array count it not zero prior to calling AddCopyToHomeNetworkCommandL");
	iCommandHandlerCopyToHomeNetwork->AddCopyToHomeNetworkCommandL();
	EUNIT_ASSERT_EQUALS_DESC(iCommandHandlerCopyToHomeNetwork->iCommandInfoArray.Count(), 1, "command info array count it not 1 after calling AddCopyToHomeNetworkCommandL");
	CGlxMediaListCommandHandler::TCommandInfo& commandInfo = iCommandHandlerCopyToHomeNetwork->iCommandInfoArray[0];
	
	EUNIT_ASSERT_EQUALS_DESC(commandInfo.iCommandId, EGlxCopyToHomeNetwork, "command id is not EGlxCopyToHomeNetwork");
	EUNIT_ASSERT_EQUALS_DESC(commandInfo.iMinSelectionLength, 1, "command min selection length is not 1");
	EUNIT_ASSERT_EQUALS_DESC(commandInfo.iDisallowDRM, ETrue, "Disalow DRM is not true");
	}


void t_cglxcommandhandlercopytohomenetwork::T_DoExecuteL_L()
	{
	EUNIT_ASSERT_EQUALS_DESC(ETrue, iCommandHandlerCopyToHomeNetwork->DoExecuteL(EGlxCopyToHomeNetwork, *iList), "EGlxCopyToHomeNetwork was not consumed");
	EUNIT_ASSERT_EQUALS_DESC(EFalse, iCommandHandlerCopyToHomeNetwork->DoExecuteL(0, *iList), "unknown command was consumed");
	}

void t_cglxcommandhandlercopytohomenetwork::T_DoIsDisabled_L()
	{
	CUpnpCopyCommand::gUpnpCopyCommandEnabled = ETrue;
	EUNIT_ASSERT_EQUALS_DESC(ETrue, iCommandHandlerCopyToHomeNetwork->DoIsDisabled(0, *iList), "unknown command is enabled")
	EUNIT_ASSERT_EQUALS_DESC(EFalse, iCommandHandlerCopyToHomeNetwork->DoIsDisabled(EGlxCopyToHomeNetwork, *iList), "EGlxCopyToHomeNetwork command is disabled");
	CUpnpCopyCommand::gUpnpCopyCommandEnabled = EFalse;
	EUNIT_ASSERT_EQUALS_DESC(ETrue, iCommandHandlerCopyToHomeNetwork->DoIsDisabled(0, *iList), "unknown command is enabled")
	EUNIT_ASSERT_EQUALS_DESC(ETrue, iCommandHandlerCopyToHomeNetwork->DoIsDisabled(EGlxCopyToHomeNetwork, *iList), "EGlxCopyToHomeNetwork command is enabled");
	}

   
//  TEST TABLE
EUNIT_BEGIN_TEST_TABLE(
    t_cglxcommandhandlercopytohomenetwork,
    "Unit tests for CGlxCommandHandlerCopyToHomeNetwork",
    "UNIT" )

EUNIT_TEST(
    "AddCopyToHomeNetworkCommandL",
    "CGlxCommandHandlerCopyToHomeNetwork",
    "T_AddCopyToHomeNetworkCommandL_L",
    "UNIT",
    SetupL, T_AddCopyToHomeNetworkCommandL_L, Teardown)

  
EUNIT_TEST(
    "DoExecuteL",
    "CGlxCommandHandlerCopyToHomeNetwork",
    "T_DoExecuteL_L",
    "UNIT",
    SetupL, T_DoExecuteL_L, Teardown)

EUNIT_TEST(
    "DoIsDisabled",
    "CGlxCommandHandlerCopyToHomeNetwork",
    "T_DoIsDisabled_L",
    "UNIT",
    SetupL, T_DoIsDisabled_L, Teardown) 
  
EUNIT_END_TEST_TABLE

//  END OF FILE

