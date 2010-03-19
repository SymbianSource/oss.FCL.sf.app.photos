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
#ifndef __UNITTEST_MEDIALISTWRAPPER_H__
#define __UNITTEST_MEDIALISTWRAPPER_H__

#include <QtTest/QtTest>

class GlxMLWrapper;

class TestGlxMLWrapper : public QObject
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
 
    
    void testGetItemCount(); 
    void testGetFocusIndex();
    void testSetFocusIndex();
    void testSetContextMode();
	void testRetrieveItemUri();
	void testRetrieveItemUriName();
	void testRetrieveItemIcon();
	void testRetrieveItemDateIsNotNull();
	void testRetrieveItemDateIsValid();
	void testRetrieveItemDateValidate();
    void testGetVisibleWindowIndex();
    void testSetVisibleWindowIndex();
    void testItemsAdded();
    void testItemsRemoved();
    void testHandleReceivedIcon();
    void testHandleIconCorrupt();
    void testHandleListItemAvailable();
    void testRetrieveItemDateIsNull();
    // void testRetrieveListTitle();
    // void testRetrieveListSubTitle();
    // void testRetrieveItemDimension();
    // void testHandleGeneralError();

private:
  
    /**
     * object under test
     */  
    GlxMLWrapper *mTestObject;
};

#endif //__UNITTEST_MEDIALISTWRAPPER_H__
