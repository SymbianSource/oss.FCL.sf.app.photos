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


#ifndef __GLXCOLLECTIONMANAGERAO_H__
#define __GLXCOLLECTIONMANAGERAO_H__

#include <e32base.h>
#include <glxcollectionmanager.h>
#include <StifLogger.h>
#include <StifItemParser.h>

#include <e32def.h>
#include <glxcollectioninfo.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <TestclassAssert.h>

// @todo: Documentation to be done

// Observer CLASS DECLARATION
class MObserver 
    {
public : 
    virtual void RequestComplete(TInt aError) = 0 ;
    };

class TElement
    {
public :
    TElement();
public :
    TBuf<256> iData;
    };



class CGlxCollectionManagerAO : public CActive
    {
public:
    enum TRequestType {EAddToCollectionL,EAddToCollectionL2,ECollectionInfo,ECancel} ;
    
public:
    
    static CGlxCollectionManagerAO* NewL(MObserver& aObserver);
    ~CGlxCollectionManagerAO();
    void TestingFunc(CStifItemParser& aItem);
    void AddToCollectionTestL();
    void TestCollectionInfoL();
	void TestCancel();
	
public: // from CActive
    
    void RunL();
    void DoCancel();
    TInt RunError(TInt aError) ;


private:
    CGlxCollectionManagerAO(MObserver& aObserver);
    void ConstructL();
    void ParseUserInput(CArrayFixFlat<TElement>* aArray);
    
private:

    MObserver&  iObserver;
    CGlxCollectionManager* iCollectionManager;
    CArrayFixFlat<TElement>* fixflat;
    CGlxCollectionInfo* iCollectionInfo;
    CGlxCollectionManager::TCollection iCollection;
    TUint32 iCollectionId;
    };


#endif // __GLXCOLLECTIONMANAGERAO_H__
