/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of collection manager API for external use
* 
*
*/


//INTERNAL INCLUDES
#include "glxcollectionmanagerao.h"

//EXTERNAL INCLUDES
#include <mpxmedia.h>
#include <mpxcollectionpath.h>
#include <mpxmediageneraldefs.h>
#include <stringloader.h>
#include <f32file.h>
#include <s32file.h>
#include <glxcollectioninfo.h>
#include <glxcollectionpluginalbums.hrh>
#include <glxcollectionplugintags.hrh>
#include <StifItemParser.h>
#include <TestScripterInternal.h>
#include "ut_collectionmanager.h"

_LIT( KTagCollection, "KGlxTagCollectionPluginImplementationUid" );
_LIT( KAlbumCollection, "KGlxCollectionPluginAlbumsImplementationUid" );
_LIT( KCameraCollection, "ECollectionCameraAlbum" );

/*
 * Constructor for the TElement class
 */
TElement::TElement()
    {
    //No need to initialise anything
    }


// -----------------------------------------------------------------------------
// CGlxCollectionManagerAO::CGlxCollectionManagerAO
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CGlxCollectionManagerAO::CGlxCollectionManagerAO(MObserver& aObserver):CActive(EPriorityStandard),iObserver(aObserver)
                    {

                    }

// -----------------------------------------------------------------------------
// CTest_Active::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CGlxCollectionManagerAO* CGlxCollectionManagerAO::NewL(MObserver& aObserver)
    {
    CGlxCollectionManagerAO* self = new (ELeave) CGlxCollectionManagerAO(aObserver);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CGlxCollectionManagerAO::~CGlxCollectionManagerAO()
    {
    if(iCollectionManager)
        {
        delete iCollectionManager;
        iCollectionManager = NULL;
        }
    if ( iCollectionInfo )
        {
        delete iCollectionInfo;
        iCollectionInfo = NULL;
        }
    if(fixflat)
        {
        delete fixflat;
        fixflat =NULL;
        }
    Cancel();
    }


// -----------------------------------------------------------------------------
// CGlxCollectionManagerAO::DoCancel()
// -----------------------------------------------------------------------------
//
void CGlxCollectionManagerAO::DoCancel()
    {
    }

// -----------------------------------------------------------------------------
// CGlxCollectionManagerAO::RunL()
// -----------------------------------------------------------------------------
//

void CGlxCollectionManagerAO::RunL()
    {
    iObserver.RequestComplete(iStatus.Int());
    }

// -----------------------------------------------------------------------------
// CGlxCollectionManagerAO::RunError()
// -----------------------------------------------------------------------------
//

TInt CGlxCollectionManagerAO::RunError(TInt aError)
    {
    return aError;
    }

// -----------------------------------------------------------------------------
// CGlxCollectionManagerAO::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CGlxCollectionManagerAO::ConstructL()
    {
    CActiveScheduler::Add(this);
    fixflat = new (ELeave) CArrayFixFlat<TElement>(3);
    }
/*
 * CGlxCollectionManagerAO::test_Func
 * The test function to test the async APIs
 */
void CGlxCollectionManagerAO::TestingFunc(CStifItemParser& aItem)
    {
    TPtrC string;
    TInt i = 0;
    TElement aElement;
    while ( aItem.GetNextString ( string ) == KErrNone )
        {
        aElement.iData.Copy(string);
        fixflat->InsertL(i,aElement);
        i++;
        }
    ParseUserInput(fixflat);
    }


// -----------------------------------------------------------------------------
// TestL
// -----------------------------------------------------------------------------
//
void CGlxCollectionManagerAO::AddToCollectionTestL()
    {

#ifdef __WINSCW__
    _LIT( KPath, "E:\\Images\\sample.jpg");
#else
    _LIT( KPath, "c:\\data\\Images\\sample.jpg");
#endif

    if (!iCollectionManager)
        {
        iCollectionManager = CGlxCollectionManager::NewL();
        }
    TInt err = KErrNone;
    TRAP(err,iCollectionManager->AddToCollectionL(KPath, 1, iStatus));

    if( !IsActive() )
        SetActive();
}

/*
 * CTest_Active::TestCollectionInfoL()
 * This function checks CollectionInfoL() api.
 * 
 */
void CGlxCollectionManagerAO::TestCollectionInfoL()
	{
    if (!iCollectionManager)
        {
        iCollectionManager = CGlxCollectionManager::NewL();
        }
    if ( iCollectionInfo )
        {
        delete iCollectionInfo;
        iCollectionInfo = NULL;
        }
    iCollectionInfo = CGlxCollectionInfo::NewL( KGlxCollectionPluginAlbumsImplementationUid );
    iCollectionManager->CollectionInfoL( *iCollectionInfo, iStatus );
    if( !IsActive() )
        SetActive();

    }
/*
 * CTest_Active::TestCancel()
 * This function tests the Cancel() api.
 * It assumes that a request has been placed and then
 * the cancel is been issued.
 */
void CGlxCollectionManagerAO::TestCancel()
	{
    if (!iCollectionManager)
        {
        iCollectionManager = CGlxCollectionManager::NewL();
        }
    if (!iCollectionManager)
        {
        iCollectionManager = CGlxCollectionManager::NewL();
        }
    if ( iCollectionInfo )
        {
        delete iCollectionInfo;
        iCollectionInfo = NULL;
        }
    iCollectionInfo = CGlxCollectionInfo::NewL( KGlxCollectionPluginAlbumsImplementationUid );
    iCollectionManager->CollectionInfoL( *iCollectionInfo, iStatus );
    if( !IsActive() )
        SetActive();

    if(iCollectionManager)
        {
        iCollectionManager->Cancel();
        }
    }

/*
 * @to do: This function later need to be modified
 * For the time, whatever user passes from the .cfg file
 * it matches the string && assigns the collectionid or collection
 */
void CGlxCollectionManagerAO::ParseUserInput(CArrayFixFlat<TElement>* aArray)
    {
    TInt count1 = aArray->Count();
    if((*fixflat)[1].iData == KAlbumCollection)
        {
        iCollectionId = KGlxCollectionPluginAlbumsImplementationUid;
        }
    else if((*fixflat)[1].iData == KTagCollection)
        {
        iCollectionId = KGlxTagCollectionPluginImplementationUid;
        }
    else if((*fixflat)[1].iData == KCameraCollection)
        {
        iCollection = CGlxCollectionManager::ECollectionCameraAlbum;
        }
    }

