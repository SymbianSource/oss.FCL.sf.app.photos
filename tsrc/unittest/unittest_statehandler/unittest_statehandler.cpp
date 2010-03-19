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
* Description: 
*
*/
#include "hbmainwindow.h"
#include "hbapplication.h"

#include "unittest_statehandler.h"
#include "glxstatemanager.h"
#include "glxbasestate.h"
#include "glxgridstate.h"
#include "glxfullscreenstate.h"
#include <glxcollectionpluginall.hrh>
#include "glxmediaid.h"

int main(int argc, char *argv[])
{
    Q_UNUSED(argc);
    HbApplication app(argc, argv);	    

    HbMainWindow *mMainWindow = new HbMainWindow();
    TestGlxStateManager tv;

    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\teststatehandler.txt";

    int res = QTest::qExec(&tv, 3, pass);

    return res;
}

// -----------------------------------------------------------------------------
// initTestCase
// -----------------------------------------------------------------------------
//
void TestGlxStateManager::initTestCase()
{
    mStateManager = 0;
}

// -----------------------------------------------------------------------------
// init
// -----------------------------------------------------------------------------
//
void TestGlxStateManager::init()
{
    mStateManager = new GlxStateManager();
    QVERIFY(mStateManager);
    QVERIFY(mStateManager->mViewManager);
    QVERIFY(mStateManager->mActionHandler);
}

void TestGlxStateManager::removeModelTestCase1()
{
    mStateManager->createGridModel(ALBUM_ITEM_S, FORWARD_DIR);
    mStateManager->removeCurrentModel();
    QVERIFY(mStateManager->mAlbumGridMediaModel == 0 );   
}

void TestGlxStateManager::removeModelTestCase2()
{
    mStateManager->createModel(GLX_LISTVIEW_ID);
    mStateManager->removeCurrentModel();
    QVERIFY(mStateManager->mAlbumMediaModel == 0 );    
}


void TestGlxStateManager::createStateTestCase()
{
    GlxState * state = mStateManager->createState(GLX_GRIDVIEW_ID);
    QVERIFY(state);
    QCOMPARE(state->id(), GLX_GRIDVIEW_ID);
    delete state;
    state = NULL;
    
    state = mStateManager->createState(GLX_LISTVIEW_ID);
    QVERIFY(state);
    QCOMPARE(state->id(), GLX_LISTVIEW_ID);
    delete state;
    state = NULL;
    
    state = mStateManager->createState(1000);
    QVERIFY(state == 0);
}

void TestGlxStateManager::createModelTestCase1()
{
    mStateManager->createModel(GLX_LISTVIEW_ID);
    QVERIFY(mStateManager->mAlbumMediaModel);
    mStateManager->removeCurrentModel();
}

void TestGlxStateManager::createModelTestCase2()
{
    mStateManager->mCurrentState = mStateManager->createState(GLX_FULLSCREENVIEW_ID);
    mStateManager->mCurrentState->setState(EXTERNAL_S);
    mStateManager->createModel(GLX_FULLSCREENVIEW_ID);    
    QVERIFY(mStateManager->mAllMediaModel);
    
    mStateManager->removeCurrentModel();
    delete mStateManager->mCurrentState;
    mStateManager->mCurrentState = NULL;
}

void TestGlxStateManager::createGridModelTestCase1()
{
    mStateManager->createGridModel(ALL_ITEM_S, NO_DIR);
    QCOMPARE(mStateManager->mCollectionId, KGlxCollectionPluginAllImplementationUid);
    QVERIFY(mStateManager->mAllMediaModel);
    mStateManager->removeCurrentModel();
}

void TestGlxStateManager::createGridModelTestCase2()
{
    mStateManager->createGridModel(ALBUM_ITEM_S, FORWARD_DIR);
    QCOMPARE(mStateManager->mCollectionId, (int )KGlxAlbumsMediaId);
    QVERIFY(mStateManager->mAlbumGridMediaModel); 
    mStateManager->removeCurrentModel();
}


// -----------------------------------------------------------------------------
// cleanup
// -----------------------------------------------------------------------------
//
void TestGlxStateManager::cleanup()
{
    if(mStateManager)
    {
        delete mStateManager;
        mStateManager = 0;
    }  
}

// -----------------------------------------------------------------------------
// cleanupTestCase
// -----------------------------------------------------------------------------
//
void TestGlxStateManager::cleanupTestCase()
{

}
