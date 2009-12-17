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
* Description:  CGlxMediaListCommandHandler unit test cases
*
*/




//  CLASS HEADER
#include "UT_CGlxMediaListCommandHandler.h"

//  EXTERNAL INCLUDES
#include <digia/eunit/eunitmacros.h>
#include <glxmedia.h>
#include <glxmediageneraldefs.h>
#include <mglxmedialistprovider.h>
#include <mglxmedialist.h>
#include <mpxmediadrmdefs.h>
#include <mpxmediageneraldefs.h>

// -----------------------------------------------------------------------------
// Set the value for a specific attribute
// -----------------------------------------------------------------------------
//
//
CGlxMedia::CGlxMedia(const TGlxMediaId& aId)
	: iId(aId)
    {
    }
    
CGlxMedia::~CGlxMedia()
    {
    iUsers.Close();

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
	    return iSelectedItems.Find(aIndex);
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
    	
    TInt SelectedItemIndex(TInt aSelectionIndex) const
    	{
    	TInt idx = KErrNotFound;
    	if(aSelectionIndex<iSelectedItems.Count())
    	    {
    	    idx = iSelectedItems[aSelectionIndex];
    	    }
    	return idx;
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
    
    virtual void SetFocusInitialPosition(NGlxListDefs::TFocusInitialPosition aFocusInitialPosition) {};
    
    virtual void ResetFocus(){};

    virtual void SetVisibleWindowIndexL( TInt aIndex) {};
    
    virtual TInt VisibleWindowIndex() const {};
    
    RArray<TGlxMedia> iItems;
    RPointerArray<CGlxMedia> iMedia;
    RArray<TInt> iSelectedItems;
    TInt iFocusIndex;
    TBool iStaticItemsEnabled;
	};

//  INTERNAL INCLUDES
#include "glxmedialistcommandhandler.h"

// CONSTRUCTION
UT_CGlxMediaListCommandHandler* UT_CGlxMediaListCommandHandler::NewL()
    {
    UT_CGlxMediaListCommandHandler* self = UT_CGlxMediaListCommandHandler::NewLC();
    CleanupStack::Pop();

    return self;
    }

UT_CGlxMediaListCommandHandler* UT_CGlxMediaListCommandHandler::NewLC()
    {
    UT_CGlxMediaListCommandHandler* self = new( ELeave ) UT_CGlxMediaListCommandHandler();
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }

// Destructor (virtual by CBase)
UT_CGlxMediaListCommandHandler::~UT_CGlxMediaListCommandHandler()
    {
    delete iML;
    }

// Default constructor
UT_CGlxMediaListCommandHandler::UT_CGlxMediaListCommandHandler()
    {
    }

// Second phase construct
void UT_CGlxMediaListCommandHandler::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS



void UT_CGlxMediaListCommandHandler::SetupL(  )
    {
    }

void UT_CGlxMediaListCommandHandler::Teardown(  )
    {
    delete iML;
    iML = NULL;
    }


struct CTestHandler : public CGlxMediaListCommandHandler, public MGlxMediaListProvider
    {
    CTestHandler(MGlxMediaList* aList) : CGlxMediaListCommandHandler(this) 
        {
        iList = aList;
        }
        
    virtual TBool DoExecuteL(TInt /*aCommandId*/, MGlxMediaList& /*aList*/)
        {
        return ETrue;
        }
        
    virtual MGlxMediaList& MediaList() 
        {
        return *iList;
        }
        
    virtual TBool DoIsDisabled(TInt aCommandId, MGlxMediaList& /*aMediaList*/) const
        {
        EUNIT_ASSERT( aCommandId == iCommandId );
        return iDisabled;
        }
    
    TBool iDisabled;
    TInt iCommandId;
    MGlxMediaList* iList;     
    };

// -----------------------------------------------------------------------------
// Test: static TInt SelectionLength(MGlxMediaList& aMediaList);
// -----------------------------------------------------------------------------
//
void UT_CGlxMediaListCommandHandler::Test_SelectionLengthL(  )
    { 
#if 1 // This unit test needs to be re-wirtten after changes to SelectionLength() method.
    iML = new (ELeave) _CGlxTestMediaList;
    
    CTestHandler* ch = new (ELeave) CTestHandler(iML);
    CleanupStack::PushL(ch);
  
    EUNIT_ASSERT( ch->SelectionLength() == 0);
    
    iML->AppendL(1, ETrue);
    iML->AppendL(2, ETrue);
    iML->AppendL(3, ETrue);
    iML->iFocusIndex = 0;

    EUNIT_ASSERT( ch->SelectionLength() == 1);
    iML->iFocusIndex = 1;
    EUNIT_ASSERT( ch->SelectionLength() == 1);

    iML->SetSelectedL(0, ETrue);
    EUNIT_ASSERT( ch->SelectionLength() == 1);
    iML->SetSelectedL(1, ETrue);
    EUNIT_ASSERT( ch->SelectionLength() == 2);
    iML->SetSelectedL(2, ETrue);
    EUNIT_ASSERT( ch->SelectionLength() == 3);

    CleanupStack::PopAndDestroy(ch);
    delete iML;
    iML = NULL;
#endif    
    }

// -----------------------------------------------------------------------------
// Test: IMPORT_C virtual TBool IsDisabled(TInt aCommandId, MGlxMediaList& aList) const;
// -----------------------------------------------------------------------------
//
void UT_CGlxMediaListCommandHandler::Test_IsDisabledL()
    {
    iML = new (ELeave) _CGlxTestMediaList;
    iML->iFocusIndex = -1;

    CTestHandler* ch = new (ELeave) CTestHandler(iML);
    CleanupStack::PushL(ch);
    ch->iCommandId = 1;
    CGlxMediaListCommandHandler::TCommandInfo info(1);
    ch->AddCommandL(info);
    
    //////////////////////////////////////////////////////
    // NO FILTERING
    // Empty view, no filtering
    EUNIT_ASSERT_DESC( !ch->IsDisabledL(1, *iML), 
        "in empty view with no filtering command is not disabled" );
    ch->CommandInfo(1).iMinSelectionLength = 1;
    EUNIT_ASSERT( ch->IsDisabledL(1, *iML) );
    ch->CommandInfo(1).iMinSelectionLength = 0;
      
    // Add items
   iML->AppendL(1, ETrue);
   iML->AppendL(2, ETrue);
   iML->AppendL(3, ETrue);
   iML->AppendL(4, ETrue);
   iML->iFocusIndex = 0;
     
    // Empty view, no filtering
    EUNIT_ASSERT( !ch->IsDisabledL(1, *iML) );

    // Selection 0-1
    iML->SetSelectedL(0, ETrue);
    iML->SetSelectedL(1, ETrue);
    EUNIT_ASSERT( !ch->IsDisabledL(1, *iML) );
     
    //////////////////////////////////////////////////////
    // Filter size
    EUNIT_ASSERT( !ch->IsDisabledL(1, *iML) );
    // Selection 0-1
    ch->CommandInfo(1).iMinSelectionLength = 1;
    ch->CommandInfo(1).iMaxSelectionLength = 2;
    EUNIT_ASSERT( !ch->IsDisabledL(1, *iML) );
    iML->SetSelectedL(2, ETrue);
    // Selection 0-1-2
    EUNIT_ASSERT( ch->IsDisabledL(1, *iML) );
    iML->SetSelectedL(2, EFalse);
    // Selection 0-1
    EUNIT_ASSERT( !ch->IsDisabledL(1, *iML) );
    iML->SetSelectedL(1, EFalse);
    // Selection 0
    EUNIT_ASSERT( !ch->IsDisabledL(1, *iML) );
    iML->SetSelectedL(0, EFalse);
    // Selection none (not empty view(
     EUNIT_ASSERT( !ch->IsDisabledL(1, *iML) );
    
    //////////////////////////////////////////////////////
    // Filter category
        {
        EUNIT_ASSERT( !ch->IsDisabledL(1, *iML) );
        ch->CommandInfo(1).iMinSelectionLength = 0;
        ch->CommandInfo(1).iMaxSelectionLength = KMaxTInt;
        iML->MediaObj(0)->SetTObjectValueL<TMPXGeneralCategory>(KMPXMediaGeneralCategory, EMPXImage);
        iML->MediaObj(1)->SetTObjectValueL<TMPXGeneralCategory>(KMPXMediaGeneralCategory, EMPXImage);
        iML->MediaObj(2)->SetTObjectValueL<TMPXGeneralCategory>(KMPXMediaGeneralCategory, EMPXVideo);
        iML->MediaObj(3)->SetTObjectValueL<TMPXGeneralCategory>(KMPXMediaGeneralCategory, EMPXImage);

        // Require all images
        ch->CommandInfo(1).iCategoryFilter = EMPXImage;
        ch->CommandInfo(1).iCategoryRule = CGlxMediaListCommandHandler::TCommandInfo::ERequireAll;

        // No selection, focus on 0
        iML->iFocusIndex = 0;
        EUNIT_ASSERT( !ch->IsDisabledL(1, *iML) );
        // No selection, focus on 2
        iML->iFocusIndex = 2;
        EUNIT_ASSERT( ch->IsDisabledL(1, *iML) );

        // Selection 0,1
        iML->SetSelectedL(0, ETrue);
        iML->SetSelectedL(1, ETrue);
        EUNIT_ASSERT( !ch->IsDisabledL(1, *iML) );
        // Selection 0,1,2
        iML->SetSelectedL(2, ETrue);
        EUNIT_ASSERT_DESC( ch->IsDisabledL(1, *iML), "no all are images so IsDisabled" );
        // Selection 0,1,2,3
        iML->SetSelectedL(3, ETrue);
        EUNIT_ASSERT_DESC( ch->IsDisabledL(1, *iML), "no all are images so IsDisabled" );
            
        // Disable if all are not images
        ch->CommandInfo(1).iCategoryRule = CGlxMediaListCommandHandler::TCommandInfo::EForbidAll;
        EUNIT_ASSERT( !ch->IsDisabledL(1, *iML) );
        iML->SetSelectedL(2, EFalse);
        EUNIT_ASSERT( ch->IsDisabledL(1, *iML) );

        // Cleanup for next test
        ch->CommandInfo(1).iCategoryRule = CGlxMediaListCommandHandler::TCommandInfo::EIgnore;
        iML->SetSelectedL(0, EFalse);
        iML->SetSelectedL(1, EFalse);
        iML->SetSelectedL(3, EFalse);
        }
        
    //////////////////////////////////////////////////////
    // Filter drm
        {
       EUNIT_ASSERT( !ch->IsDisabledL(1, *iML) );
        iML->MediaObj(0)->SetTObjectValueL<TBool>(KMPXMediaDrmProtected, ETrue);
        iML->MediaObj(1)->SetTObjectValueL<TBool>(KMPXMediaDrmProtected, ETrue);
        iML->MediaObj(2)->SetTObjectValueL<TBool>(KMPXMediaDrmProtected, EFalse);
        iML->MediaObj(3)->SetTObjectValueL<TBool>(KMPXMediaDrmProtected, ETrue);

        // dont allow DRM
        ch->CommandInfo(1).iDisallowDRM = ETrue;

        // No selection, focus on 0
        iML->iFocusIndex = 0;
        EUNIT_ASSERT( ch->IsDisabledL(1, *iML) );
        // No selection, focus on 2
        iML->iFocusIndex = 2;
        EUNIT_ASSERT( !ch->IsDisabledL(1, *iML) );

        // Selection 1,2
        iML->SetSelectedL(0, ETrue);
        iML->SetSelectedL(1, ETrue);
        EUNIT_ASSERT( ch->IsDisabledL(1, *iML) );
        // Selection 1,2,3
        iML->SetSelectedL(2, ETrue);
        EUNIT_ASSERT( !ch->IsDisabledL(1, *iML) );

        // Cleanup for next test
        ch->CommandInfo(1).iDisallowDRM = EFalse;
        iML->SetSelectedL(0, EFalse);
        iML->SetSelectedL(1, EFalse);
        iML->SetSelectedL(2, EFalse);
        }
        
    //////////////////////////////////////////////////////
    // Filter system items
        {
        EUNIT_ASSERT( !ch->IsDisabledL(1, *iML) );
        iML->MediaObj(0)->SetTObjectValueL<TBool>(KGlxMediaGeneralSystemItem, ETrue);
        iML->MediaObj(1)->SetTObjectValueL<TBool>(KGlxMediaGeneralSystemItem, ETrue);
        iML->MediaObj(2)->SetTObjectValueL<TBool>(KGlxMediaGeneralSystemItem, EFalse);
        iML->MediaObj(3)->SetTObjectValueL<TBool>(KGlxMediaGeneralSystemItem, ETrue);

        // Dont allow system items
        ch->CommandInfo(1).iDisallowSystemItems = ETrue;

        // No selection, focus on 0
        iML->iFocusIndex = 0;
        EUNIT_ASSERT( ch->IsDisabledL(1, *iML) );
        // No selection, focus on 2
        iML->iFocusIndex = 2;
        EUNIT_ASSERT( !ch->IsDisabledL(1, *iML) );

        // Selection 1,2
        iML->SetSelectedL(0, ETrue);
        iML->SetSelectedL(1, ETrue);
        EUNIT_ASSERT( ch->IsDisabledL(1, *iML) );
        // Selection 1,2,3
        iML->SetSelectedL(2, ETrue);
        EUNIT_ASSERT( !ch->IsDisabledL(1, *iML) );
        }   
         
    //////////////////////////////////////////////////////
    // Filter deriving class
        {
        EUNIT_ASSERT( !ch->IsDisabledL(1, *iML) );
        ch->iDisabled = ETrue;
        EUNIT_ASSERT( ch->IsDisabledL(1, *iML) );
        ch->iDisabled = EFalse;
        }   

    
    //////////////////////////////////////////////////////
    // Combined Filter
        {
        // create a few more items
        iML->AppendL(5, ETrue);
        iML->AppendL(6, ETrue);
        iML->AppendL(7, ETrue);

        // set attributes
        // item 0 is a DRM protected video
        iML->MediaObj(0)->SetTObjectValueL<TMPXGeneralCategory>(KMPXMediaGeneralCategory, EMPXVideo);
        iML->MediaObj(0)->SetTObjectValueL<TBool>(KMPXMediaDrmProtected, ETrue);
        iML->MediaObj(0)->SetTObjectValueL<TBool>(KGlxMediaGeneralSystemItem, EFalse);
        iML->MediaObj(0)->SetTObjectValueL<TInt>(KGlxMediaGeneralFramecount, 1);
        iML->MediaObj(0)->SetTObjectValueL<TInt>(KMPXMediaGeneralCount, 1);
        // item 1 is a Animated GIF, non DRM protected
        iML->MediaObj(1)->SetTObjectValueL<TMPXGeneralCategory>(KMPXMediaGeneralCategory, EMPXImage);
        iML->MediaObj(1)->SetTObjectValueL<TBool>(KMPXMediaDrmProtected, EFalse);
        iML->MediaObj(1)->SetTObjectValueL<TBool>(KGlxMediaGeneralSystemItem, EFalse);
        iML->MediaObj(1)->SetTObjectValueL<TInt>(KGlxMediaGeneralFramecount, 10);
        iML->MediaObj(1)->SetTObjectValueL<TInt>(KMPXMediaGeneralCount, 1);
        // item 2 is an Album with 5 items, also a system item
        iML->MediaObj(2)->SetTObjectValueL<TMPXGeneralCategory>(KMPXMediaGeneralCategory, EMPXAlbum);
        iML->MediaObj(2)->SetTObjectValueL<TBool>(KMPXMediaDrmProtected, EFalse);
        iML->MediaObj(2)->SetTObjectValueL<TBool>(KGlxMediaGeneralSystemItem, ETrue);
        iML->MediaObj(2)->SetTObjectValueL<TInt>(KGlxMediaGeneralFramecount, 1);
        iML->MediaObj(2)->SetTObjectValueL<TInt>(KMPXMediaGeneralCount, 5);
        // item 3 is a DRM protected image
        iML->MediaObj(3)->SetTObjectValueL<TMPXGeneralCategory>(KMPXMediaGeneralCategory, EMPXImage);
        iML->MediaObj(3)->SetTObjectValueL<TBool>(KMPXMediaDrmProtected, ETrue);
        iML->MediaObj(3)->SetTObjectValueL<TBool>(KGlxMediaGeneralSystemItem, EFalse);
        iML->MediaObj(3)->SetTObjectValueL<TInt>(KGlxMediaGeneralFramecount, 1);
        iML->MediaObj(3)->SetTObjectValueL<TInt>(KMPXMediaGeneralCount, 1);
        // item 4 is a non DRM protected image, but a system item
        iML->MediaObj(4)->SetTObjectValueL<TMPXGeneralCategory>(KMPXMediaGeneralCategory, EMPXImage);
        iML->MediaObj(4)->SetTObjectValueL<TBool>(KMPXMediaDrmProtected, EFalse);
        iML->MediaObj(4)->SetTObjectValueL<TBool>(KGlxMediaGeneralSystemItem, ETrue);
        iML->MediaObj(4)->SetTObjectValueL<TInt>(KGlxMediaGeneralFramecount, 1);
        iML->MediaObj(4)->SetTObjectValueL<TInt>(KMPXMediaGeneralCount, 1);
        // item 5 is a non DRM protected video
        iML->MediaObj(5)->SetTObjectValueL<TMPXGeneralCategory>(KMPXMediaGeneralCategory, EMPXVideo);
        iML->MediaObj(5)->SetTObjectValueL<TBool>(KMPXMediaDrmProtected, EFalse);
        iML->MediaObj(5)->SetTObjectValueL<TBool>(KGlxMediaGeneralSystemItem, EFalse);
        iML->MediaObj(5)->SetTObjectValueL<TInt>(KGlxMediaGeneralFramecount, 1);
        iML->MediaObj(5)->SetTObjectValueL<TInt>(KMPXMediaGeneralCount, 1);
        // item 6 is an Album with 0 items
        iML->MediaObj(6)->SetTObjectValueL<TMPXGeneralCategory>(KMPXMediaGeneralCategory, EMPXAlbum);
        iML->MediaObj(6)->SetTObjectValueL<TBool>(KMPXMediaDrmProtected, EFalse);
        iML->MediaObj(6)->SetTObjectValueL<TBool>(KGlxMediaGeneralSystemItem, EFalse);
        iML->MediaObj(6)->SetTObjectValueL<TInt>(KGlxMediaGeneralFramecount, 1);
        iML->MediaObj(6)->SetTObjectValueL<TInt>(KMPXMediaGeneralCount, 0);

        // set selection
        ch->CommandInfo(1).iMinSelectionLength = 0;
        ch->CommandInfo(1).iMaxSelectionLength = 7;
        // Drop all videos
        ch->CommandInfo(1).iCategoryFilter = EMPXVideo;
        ch->CommandInfo(1).iCategoryRule = CGlxMediaListCommandHandler::TCommandInfo::EForbidAll;
        // Dont allow system items
        ch->CommandInfo(1).iDisallowSystemItems = ETrue;
        // dont allow DRM
        ch->CommandInfo(1).iDisallowDRM = ETrue;
        // dont allow animated gif
        ch->CommandInfo(1).iDisallowAnimatedGIFs = ETrue;
        // allow only containers with 1 or more items
        ch->CommandInfo(1).iMinSlideshowPlayableContainedItemCount = 1;

        // Set selection, select all
        iML->SetSelectedL(0, ETrue);
        iML->SetSelectedL(1, ETrue);
        iML->SetSelectedL(2, ETrue);
        iML->SetSelectedL(3, ETrue);
        iML->SetSelectedL(4, ETrue);
        iML->SetSelectedL(5, ETrue);
        iML->SetSelectedL(6, ETrue);
        EUNIT_ASSERT_DESC( ch->IsDisabledL(1, *iML), "no valid items to show so IsDisabled" );
        // Allow system items
        ch->CommandInfo(1).iDisallowSystemItems = EFalse;
        EUNIT_ASSERT_DESC( !ch->IsDisabledL(1, *iML), "valid items to show so not disabled" );
        // unselect item 4
        iML->SetSelectedL(4, EFalse);
        EUNIT_ASSERT_DESC( !ch->IsDisabledL(1, *iML), "valid items to show so not disabled" );
        // unselect item 2
        iML->SetSelectedL(2, EFalse);
        EUNIT_ASSERT_DESC( ch->IsDisabledL(1, *iML), "no valid items to show so IsDisabled" );
        // re-select item 4
        iML->SetSelectedL(4, ETrue);
        EUNIT_ASSERT_DESC( !ch->IsDisabledL(1, *iML), "valid items to show so not disabled" );
        // unselect item 4
        iML->SetSelectedL(4, EFalse);
        EUNIT_ASSERT_DESC( ch->IsDisabledL(1, *iML), "no valid items to show so IsDisabled" );
        // Allow DRM
        ch->CommandInfo(1).iDisallowDRM = EFalse;
        EUNIT_ASSERT_DESC( !ch->IsDisabledL(1, *iML), "valid items to show so not disabled" );
        // unselect item 3
        iML->SetSelectedL(3, EFalse);
        EUNIT_ASSERT_DESC( ch->IsDisabledL(1, *iML), "no valid items to show so IsDisabled" );
        }
        
        
    CleanupStack::PopAndDestroy(ch);

    delete iML;
    iML = NULL;
    }

//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE(
    UT_CGlxMediaListCommandHandler,
    "Medialist command handler",
    "UNIT" )

EUNIT_TEST(
    "SelectionLength",
    "CGlxMediaListCommandHandler",
    "SelectionLength",
    "FUNCTIONALITY",
    SetupL, Test_SelectionLengthL, Teardown)

/*EUNIT_TEST(
    "IsDisabledL",
    "CGlxMediaListCommandHandler",
    "IsDisabledL",
    "FUNCTIONALITY",
    SetupL, Test_IsDisabledL, Teardown)*/

EUNIT_END_TEST_TABLE

//  END OF FILE
