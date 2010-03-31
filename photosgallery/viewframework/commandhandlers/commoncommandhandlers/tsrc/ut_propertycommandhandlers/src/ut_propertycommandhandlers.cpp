/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CommandHandlerDetails unit test cases
*
*/




//  CLASS HEADER
#include "UT_propertycommandhandlers.h"

//#include <glxcommandhandlerproperties.h>
#include <mglxmedialist.h>
#include <mpxcollectionutility.h>
#include <mpxcollectionpath.h>

#include <glxcommandhandlers.hrh>
//  EXTERNAL INCLUDES
#include <EUnitMacros.h>

//  INTERNAL INCLUDES

template<typename T> 
inline void CGlxMedia::SetTObjectValueL(const TMPXAttribute& aAttribute, T aValue) 
	{
    TAny* ptr = User::AllocL(sizeof(T));
    memcpy(ptr, &aValue, sizeof(T));

    TInt i = Index(aAttribute);
    if (KErrNotFound != i)
        {
        // Replace the existing value
        TValue& value = iValues[i];
        value.iValue = ptr; // Takes ownership
        value.iType = EMPXTypeTObject;
        }
    else
        {
        TInt err = iValues.Append(TValue(ptr, EMPXTypeTObject));
        
        // If appended value ok, try to append the attribute. 
        if (KErrNone == err) 
        	{
	        err = iAttributes.Append(aAttribute);
	        if (err != KErrNone)
	        	{
	        	// Failed, undo the append
	        	iValues.Remove(iValues.Count() - 1);
	        	}
       		}
       		
        if (KErrNone != err) 
        	{
        	User::Free(ptr);
        	User::Leave(err);
        	}
        }
   	}
   	
//
CGlxMedia::CGlxMedia(const TGlxMediaId& aId)
		: iId(aId), iUsedByLists(2) // On average, two list as users assumed
    {
    }
    
CGlxMedia::~CGlxMedia()
    {
    iUsedByLists.Close();

	Reset();
    iValues.Close(); 
    iAttributes.Close();
    }    
       	


struct _CGlxTestMediaList: public CBase, public MGlxMediaList
	{
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

    virtual TInt Count(TCountType aType = ECountAll) const
	//virtual TInt Count() const 
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

	virtual TInt Index(const TGlxMediaId& aId) const 
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
	    return *(MMPXCollection*)NULL;
	    }

	virtual const TBool IsSelected(TInt aIndex) const 
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

    virtual TGlxIdSpaceId IdSpaceId() const 
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
            TMPXAttribute uriAttr(KMPXMediaGeneralUri);
            media->SetTextValueL (uriAttr,_L("testing"));
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
        
    CMPXCollectionPath* PathLC() const
    	{
    	return NULL;
    	}
    
    TInt SelectionCount() const
    	{
    	return iSelectedItems.Count();
    	}
    	
    TInt SelectedItemIndex(TInt aSelectionIndex) const
    	{
    	return 0;
    	}
 
    virtual TBool IsPopulated() const
        {
        return ETrue;
        }
   
    RArray<TGlxMedia> iItems;
    RPointerArray<CGlxMedia> iMedia;
    RArray<TInt> iSelectedItems;
    TInt iFocusIndex;
	};
	
// CONSTRUCTION

MGlxMediaList& UT_propertycommandhandlers::MediaList() 
	{
	return *iMediaList;
	}

UT_propertycommandhandlers* UT_propertycommandhandlers::NewL()
    {
    UT_propertycommandhandlers* self = UT_propertycommandhandlers::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

UT_propertycommandhandlers* UT_propertycommandhandlers::NewLC()
    {
    UT_propertycommandhandlers* self = new( ELeave ) UT_propertycommandhandlers();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
  
    
// Destructor (virtual by CBase)
UT_propertycommandhandlers::~UT_propertycommandhandlers()
    {
    }

// Default constructor
UT_propertycommandhandlers::UT_propertycommandhandlers()
    {
    }

// Second phase construct
void UT_propertycommandhandlers::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS



void UT_propertycommandhandlers::SetupL(  )
    {
	iMediaList = new (ELeave) _CGlxTestMediaList;
	iMediaList->AppendL(0, ETrue);
	iMediaList->AppendL(1, ETrue);
	iMediaList->AppendL(2, ETrue);
	iMediaList->AppendL(3, ETrue);
	iMediaList->AppendL(4, ETrue);
	iMediaList->iFocusIndex = 0;
	
	iTagManagerCommand = CGlxCommandHandlerProperties::NewTagManagerCommandHandlerL(this);
	iMetadataCommand = CGlxCommandHandlerProperties::NewMetadataViewCommandHandlerL(this);
    }

void UT_propertycommandhandlers::Teardown(  )
    {
    	
	delete iTagManagerCommand;
    iTagManagerCommand = NULL;
	delete iMetadataCommand;
    iMetadataCommand = NULL;	
	delete iMediaList;
    iMediaList = NULL;
    }

void UT_propertycommandhandlers::PropertyCommandHandlerNewL(  )
    {
    EUNIT_ASSERT( NULL != iTagManagerCommand );
    EUNIT_ASSERT( NULL != iMetadataCommand );
    }
    
void UT_propertycommandhandlers::PropertyCommandHandlerExecuteL(  )
    {
    //check that metadata command is consumend
    EUNIT_ASSERT( iMetadataCommand->ExecuteL(EGlxCmdProperties) );
    //check that tag manager command is not consumend
    EUNIT_ASSERT( !iMetadataCommand->ExecuteL(EGlxCmdTagManager) );
    
    //check that tag manager command is consumend
    EUNIT_ASSERT( iTagManagerCommand->ExecuteL(EGlxCmdTagManager) );
    //check that metadata command is not consumend
    EUNIT_ASSERT( !iTagManagerCommand->ExecuteL(EGlxCmdProperties) );
    }

//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE(
    UT_propertycommandhandlers,
    "Property command handler unit testing.",
    "Property Command handler" )

EUNIT_TEST(
    "NewL ",
    "CGlxCommandHandlerProperties",
    "NewL",
    "FUNCTIONALITY",
    SetupL, PropertyCommandHandlerNewL, Teardown)
    
EUNIT_TEST(
    "ExecuteL ",
    "CGlxCommandHandlerProperties",
    "ExecuteL",
    "FUNCTIONALITY",
    SetupL, PropertyCommandHandlerExecuteL, Teardown)


EUNIT_END_TEST_TABLE

//  END OF FILE
