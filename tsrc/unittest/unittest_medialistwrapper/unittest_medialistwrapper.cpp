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

#include "unittest_medialistwrapper.h"
#include "glxmlwrapper.h"
#include <glxcollectionpluginall.hrh>
#include <glxmodelparm.h>
#include <hbicon.h>
#include <qdatetime.h>
#include <QString>



//#include "glxmlwrapper_p.h"

int main(int argc, char *argv[])
{
    Q_UNUSED(argc);
    HbApplication app(argc, argv);	    

    HbMainWindow *mMainWindow = new HbMainWindow();
    TestGlxMLWrapper tv;

    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\testmlwrapper.txt";

    int res = QTest::qExec(&tv, 3, pass);

    return res;
}

// -----------------------------------------------------------------------------
// initTestCase
// -----------------------------------------------------------------------------
//
void TestGlxMLWrapper::initTestCase()
{
    mTestObject = 0;
}

// -----------------------------------------------------------------------------
// init
// -----------------------------------------------------------------------------
//
void TestGlxMLWrapper::init()
{
   mTestObject = new GlxMLWrapper(KGlxCollectionPluginAllImplementationUid,0,EGlxFilterImage);
   QVERIFY(mTestObject);
}

// -----------------------------------------------------------------------------
// cleanup
// -----------------------------------------------------------------------------
//
void TestGlxMLWrapper::cleanup()
{
    if(mTestObject)
    {
        delete mTestObject;
        mTestObject = 0;
    }  
}

// -----------------------------------------------------------------------------
// cleanupTestCase
// -----------------------------------------------------------------------------
//
void TestGlxMLWrapper::cleanupTestCase()
{

}

// -----------------------------------------------------------------------------
// testGetItemCount
// -----------------------------------------------------------------------------
//
void TestGlxMLWrapper::testGetItemCount()
{
    int count = mTestObject->getItemCount();
    QVERIFY(count != 0);
}


void TestGlxMLWrapper::testGetFocusIndex()
{
    int focusIndex = mTestObject->getFocusIndex();
    QVERIFY(focusIndex == -1);
    mTestObject->setFocusIndex(mTestObject->getItemCount()-1);
    focusIndex = mTestObject->getFocusIndex();
    QVERIFY(focusIndex == mTestObject->getItemCount()-1);
}
void TestGlxMLWrapper::testSetFocusIndex()
{
    mTestObject->setFocusIndex(mTestObject->getItemCount()-1);
    int focusIndex = mTestObject->getFocusIndex();
    QVERIFY(focusIndex == mTestObject->getItemCount()-1);
}
void TestGlxMLWrapper::testSetContextMode()
{
    int itemIndex = mTestObject->getItemCount()-1;
    
    //Retriveing without adding any context .so grid icon should be NULL
    HbIcon* icon = mTestObject->retrieveItemIcon(itemIndex,GlxTBContextGrid);
    QVERIFY(icon == NULL);
    
    // After adding grid context should get grid icon 
    mTestObject->setContextMode(GlxContextGrid);
    QTest::qWait(2000);
    icon = mTestObject->retrieveItemIcon(itemIndex,GlxTBContextGrid);
    QVERIFY(icon != NULL);
}

void TestGlxMLWrapper::testRetrieveItemUri()
{
    qDebug("TestGlxMLWrapper::testRetrieveItemUri enter");
	
	int count = mTestObject->getItemCount();
	mTestObject->setContextMode(GlxContextGrid);
    QTest::qWait(20000);
	QString uri = mTestObject->retrieveItemUri(count-1);
	qDebug("TestGlxMLWrapper::testRetrieveItemUri =%d",uri.isEmpty());
	QVERIFY( uri.isEmpty() == 0 );
}

void TestGlxMLWrapper::testRetrieveItemUriName()
{
    qDebug("TestGlxMLWrapper::testRetrieveItemUriName enter");
	
	int count = mTestObject->getItemCount();
	mTestObject->setContextMode(GlxContextGrid);
    QTest::qWait(20000);
    	
    QString uri = mTestObject->retrieveItemUri(count-1);
	QString imageName = uri.section('\\',-1);
    qDebug("TestGlxMLWrapper::testRetrieveItemUriName =%d",uri.isEmpty());
    QVERIFY( imageName.isEmpty() == 0 );
}

void TestGlxMLWrapper::testRetrieveItemIcon()
{
    int itemIndex = mTestObject->getItemCount()-1;
    
    // grid icon should be NULL
    HbIcon* icon = mTestObject->retrieveItemIcon(itemIndex,GlxTBContextGrid);
    QVERIFY(icon == NULL);
    
    // fullscreen icon should be NULL
    icon = mTestObject->retrieveItemIcon(itemIndex,GlxTBContextLsFs);
    QVERIFY(icon == NULL);
    
    // Should get fullscreen icon 
    mTestObject->setContextMode(GlxContextLsFs);
    QTest::qWait(4000);
    icon = mTestObject->retrieveItemIcon(itemIndex,GlxTBContextLsFs);
    QVERIFY(icon != NULL);
    
    // Should get grid icon
    mTestObject->setContextMode(GlxContextGrid);
    QTest::qWait(2000);
    icon = mTestObject->retrieveItemIcon(itemIndex,GlxTBContextGrid);
    QVERIFY(icon != NULL);
	   
}

void TestGlxMLWrapper::testRetrieveItemDateIsNotNull()
{
    qDebug("TestGlxMLWrapper::testRetrieveItemDateIsNotNull enter");
    int count = mTestObject->getItemCount();
    mTestObject->setContextMode(GlxContextGrid);
    QTest::qWait(20000);
	
	QDate date = mTestObject->retrieveItemDate(count-1);
    qDebug("TestGlxMLWrapper::testRetrieveItemDateIsNotNull =%d",date.isNull());
    QVERIFY( date.isNull() == 0 );
}


void TestGlxMLWrapper::testRetrieveItemDateIsValid()
{
    qDebug("TestGlxMLWrapper::testRetrieveItemDateIsValid enter");
    int count = mTestObject->getItemCount();
    mTestObject->setContextMode(GlxContextGrid);
    QTest::qWait(20000);
	
	QDate date = mTestObject->retrieveItemDate(count-1);
	qDebug("TestGlxMLWrapper::testRetrieveItemDateIsValid IsNull=%d",date.isNull());
    qDebug("TestGlxMLWrapper::testRetrieveItemDateIsValid IsValid=%d",date.isValid() );
	
    QVERIFY( date.isNull() == 0 );
    QVERIFY( date.isValid() == 1 );
}

void TestGlxMLWrapper::testRetrieveItemDateValidate()
{
    qDebug("TestGlxMLWrapper::testRetrieveItemDateValidate enter");
    int count = mTestObject->getItemCount();
	mTestObject->setContextMode(GlxContextGrid);
    QTest::qWait(20000);
	
    QDate date = mTestObject->retrieveItemDate(count-1);
    qDebug("TestGlxMLWrapper::testRetrieveItemDateValidate =%d",date.isValid(date.year(), date.month(), date.day()) );
    QVERIFY( date.isValid(date.year(), date.month(), date.day()) == 1 );
}

void TestGlxMLWrapper::testGetVisibleWindowIndex()
{
    int visibleIndex = mTestObject->getVisibleWindowIndex();
    QVERIFY(visibleIndex == 0);
    
    int itemIndex = mTestObject->getItemCount()-1;
    mTestObject->setVisibleWindowIndex(itemIndex);
    visibleIndex = mTestObject->getVisibleWindowIndex();
    QVERIFY(visibleIndex == itemIndex);
}

void TestGlxMLWrapper::testSetVisibleWindowIndex()
{
    int itemIndex = mTestObject->getItemCount()-1;
    mTestObject->setVisibleWindowIndex(itemIndex);
    int visibleIndex = mTestObject->getVisibleWindowIndex();
    QVERIFY(visibleIndex == itemIndex);
}

void TestGlxMLWrapper::testItemsAdded()
{
    QSignalSpy spysignal(mTestObject, SIGNAL(insertItems(int ,int )));
    QVERIFY(spysignal.count() == 0);
    
    int index = mTestObject->getItemCount();
    spysignal.clear(); 

    mTestObject->itemsAdded(index,index + 5);
    qDebug("Signal Count %d",spysignal.count());
    QVERIFY(spysignal.count() == 1);
    QVERIFY(spysignal.value(0).at(0).toInt() == index);
    QVERIFY(spysignal.value(0).at(1).toInt() == index+5);

    mTestObject->itemsAdded(index + 6,index + 6);
    qDebug("Signal #Count %d",spysignal.count());
    QVERIFY(spysignal.count() == 2);
    QVERIFY(spysignal.value(1).at(0).toInt() == index+6);
    QVERIFY(spysignal.value(1).at(1).toInt() == index+6);
}

void TestGlxMLWrapper::testItemsRemoved()
{
    QSignalSpy spysignal(mTestObject, SIGNAL(removeItems(int ,int )));
    QVERIFY(spysignal.count() == 0);
    
    int index = mTestObject->getItemCount();
    spysignal.clear(); 

    mTestObject->itemsRemoved(index,index + 5);
    qDebug("Signal Count %d",spysignal.count());
    QVERIFY(spysignal.count() == 1);
    QVERIFY(spysignal.value(0).at(0).toInt() == index);
    QVERIFY(spysignal.value(0).at(1).toInt() == index+5);

    mTestObject->itemsRemoved(index + 6,index + 6);
    qDebug("Signal #Count %d",spysignal.count());
    QVERIFY(spysignal.count() == 2);
    QVERIFY(spysignal.value(1).at(0).toInt() == index+6);
    QVERIFY(spysignal.value(1).at(1).toInt() == index+6);
}

void TestGlxMLWrapper::testHandleReceivedIcon()
{
    qRegisterMetaType<GlxTBContextType>("GlxTBContextType");
    QSignalSpy spysignal(mTestObject, SIGNAL(updateItem(int , GlxTBContextType )));
    QVERIFY(spysignal.count() == 0);
    
    int count = mTestObject->getItemCount();
    spysignal.clear(); 

    mTestObject->handleReceivedIcon(count-1,GlxTBContextGrid);
    qDebug("Signal Count %d",spysignal.count());
    QVERIFY(spysignal.count() == 1);
    QVERIFY(spysignal.value(0).at(0).toInt() == count-1);
    QVERIFY(spysignal.value(0).at(1).toInt() == GlxTBContextGrid);
    spysignal.clear(); 

    // mTestObject->handleReceivedIcon(count-1,GlxTBContextLsFs);
    // qDebug("Signal Count %d",spysignal.count());
    // QVERIFY(spysignal.count() == 1);
    // QVERIFY(spysignal.value(0).at(0).toInt() == count-1);
    // QVERIFY(spysignal.value(0).at(1).toInt() == GlxTBContextLsFs);
    // spysignal.clear(); 

    // mTestObject->handleReceivedIcon(count-1,GlxTBContextPtFs);
    // qDebug("Signal Count %d",spysignal.count());
    // QVERIFY(spysignal.count() == 1);
    // QVERIFY(spysignal.value(0).at(0).toInt() == count-1);
    // QVERIFY(const_cast<GlxTBContextType>(spysignal.value(0).at(1)) == GlxTBContextPtFs);

}

void TestGlxMLWrapper::testHandleIconCorrupt()
{
    QSignalSpy spysignal(mTestObject, SIGNAL(itemCorrupted(int )));
    QVERIFY(spysignal.count() == 0);
    
    int index = mTestObject->getItemCount();
    spysignal.clear(); 

    mTestObject->handleIconCorrupt(index-1);
    qDebug("Signal Count %d",spysignal.count());
    QVERIFY(spysignal.count() == 1);
    QVERIFY(spysignal.value(0).at(0).toInt() == index-1);

}

void TestGlxMLWrapper::testHandleListItemAvailable()
{
    qRegisterMetaType<GlxTBContextType>("GlxTBContextType");
    QSignalSpy spysignal(mTestObject, SIGNAL(updateItem(int , GlxTBContextType )));
    QVERIFY(spysignal.count() == 0);
    
    int count = mTestObject->getItemCount();
    spysignal.clear(); 

    mTestObject->handleListItemAvailable(count-1);
    qDebug("Signal Count %d",spysignal.count());
    QVERIFY(spysignal.count() == 1);
    QVERIFY(spysignal.value(0).at(0).toInt() == count-1);
	qDebug("update Item enum %d",spysignal.value(0).at(1).toInt());
    QVERIFY(spysignal.value(0).at(1).toInt() == 3);

}

void TestGlxMLWrapper::testRetrieveItemDateIsNull()
{
    int count = mTestObject->getItemCount();
    QDate date = mTestObject->retrieveItemDate(count-1);
    qDebug("TestGlxMLWrapper::testRetrieveItemDateIsNull =%d",date.isNull());
    QVERIFY( date.isNull() == 1 );
}
