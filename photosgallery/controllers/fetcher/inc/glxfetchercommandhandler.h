/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    Commandhandler that requests the marking commandhandler to 
*                mark items onto the medialist during multiple selection
*
*/




#ifndef GLXFETCHERCOMMANDHANDLER_H_
#define GLXFETCHERCOMMANDHANDLER_H_

#include <glxmedialistcommandhandler.h>
#include <glxcommandhandlermarking.h>

//Forward Decleration
class MGlxTitleObserver;

/**
*  CGlxFetcherCommandHandler
* 
*/
class CGlxFetcherCommandHandler : public CGlxMediaListCommandHandler
                         
{
public:
    /**
     * Two-phase constructor NewL
     * @param aMediaListProvider Medialist provider
     * @param aTitleObserver Title observer 
     */
    static CGlxFetcherCommandHandler* NewL(MGlxMediaListProvider 
            *aMediaListProvider ,MGlxTitleObserver* aTitleObserver);

    /**
     * Destructor 
     */
    ~CGlxFetcherCommandHandler(); 

public: 
    /// From CGlxMediaListCommandHandler
    /// See @ref CGlxMediaListCommandHandler::ExecuteL */
    TBool DoExecuteL(TInt aCommandId, MGlxMediaList& aList);

protected:  
    /** See @ref CGlxMediaListCommandHandler */
    TBool DoIsDisabled(TInt aCommandId, MGlxMediaList& aList) const;
    
private:
    /** 
     *  ConstructorL
     */
    void ConstructL();
    
    /** 
     *  Default Constructor
     */
    CGlxFetcherCommandHandler(MGlxMediaListProvider* aMediaListProvider,
        MGlxTitleObserver* aTitleObserver );

private:    
    // (Not owned)
    MGlxMediaListProvider* iMediaListProvider;
    MGlxTitleObserver* iTitleObserver;
    
    // (Owned)
    CGlxCommandHandlerMarking* iCommandhandlerMarking;         
    };
#endif /*GLXFETCHERCOMMANDHANDLER_H_*/
