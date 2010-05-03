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

#include "unittest_statehandler.h"
#include "glxstatemanager.h"
#include "glxbasestate.h"
#include "glxgridstate.h"
#include "glxfullscreenstate.h"
#include <glxcollectionpluginall.hrh>
#include "glxmediaid.h"


// -----------------------------------------------------------------------------
// initTestCase
// -----------------------------------------------------------------------------
//
void TestGlxStateManager::initTestCase()
{    
    mStateManager = new GlxStateManager();
    //mStateManager->setupItems();
    mStateManager->mCurrentState = mStateManager->createState(GLX_GRIDVIEW_ID);
    mStateManager->mCurrentState->setState(ALL_ITEM_S);
    
    QVERIFY(mStateManager);
    QVERIFY(mStateManager->mViewManager);
    QVERIFY(mStateManager->mActionHandler == 0);
}

// -----------------------------------------------------------------------------
// init
// -----------------------------------------------------------------------------
//
void TestGlxStateManager::init()
{
    
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
    GlxState *state = mStateManager->mCurrentState;
    mStateManager->mCurrentState = state->previousState();
    delete state;
    state = NULL;
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
    
}

// -----------------------------------------------------------------------------
// cleanupTestCase
// -----------------------------------------------------------------------------
//
void TestGlxStateManager::cleanupTestCase()
{
    if(mStateManager)
    {
        QCoreApplication::processEvents(); //To:Do remove it once mainwindow delete hang problem will resolve
        delete mStateManager;
        mStateManager = 0;
    }  
}

QTEST_MAIN(TestGlxStateManager)
#include "moc_unittest_statehandler.cpp"
