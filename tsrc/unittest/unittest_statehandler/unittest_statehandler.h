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
#ifndef __UNITTEST_STATEHANDLER_H__
#define __UNITTEST_STATEHANDLER_H__

#include <QtTest/QtTest>

class GlxStateManager;

class TestGlxStateManager : public QObject
{
    Q_OBJECT
    
signals:

private slots:
    /**
     * called before anything else when test starts
     */
    void initTestCase();

  /**
     * will be called before each testfunction is executed.
     *
     */
    void init(); 
    void removeModelTestCase1();
    void removeModelTestCase2();
    void createStateTestCase();
    void createModelTestCase1();
    void createModelTestCase2();
    void createGridModelTestCase1();
    void createGridModelTestCase2();
    
    
    /**
     * will be called after every testfunction.
     *
     */
    void cleanup();
    
    /**
     * will be called after testing ends
     *
     */
    void cleanupTestCase();

private:
  
    /**
     * object under test
     */  
    GlxStateManager *mStateManager;
};

#endif //__UNITTEST_STATEHANDLER_H__
