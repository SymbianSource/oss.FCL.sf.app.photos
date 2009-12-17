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
* Description:    Metadata dialog implementation
*
*/


#ifndef GLXMETADATACOMMANDHANDLER_H_
#define GLXMETADATACOMMANDHANDLER_H_


#include <mglxmedialistprovider.h>
#include <glxmedialistcommandhandler.h>

//Forward Decleration
class CGlxCommandHandlerAiwShowMap;
class CGlxMediaListCommandHandler;
/**
*  CGlxMetadataCommandHandler
* 
*/
class CGlxMetadataCommandHandler :public CGlxMediaListCommandHandler
    {
public:
    /**
    * Two-phase constructor NewL
    *
    * @param aEnv , Hui Environment 
    */
    static CGlxMetadataCommandHandler* NewL(MGlxMediaListProvider *aMediaListProvider );

    /** Destructor 
    *
    */
    ~CGlxMetadataCommandHandler();

    /** OfferKeyEventL 
    *
    */
    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);


public: // From CGlxMediaListCommandHandler
    /// See @ref CGlxMediaListCommandHandler::ExecuteL */
    TBool DoExecuteL(TInt aCommandId, MGlxMediaList& aList);
    
    void PreDynInitMenuPaneL( TInt aResourceId );
    
    void DynInitMenuPaneL( TInt aMenuId, CEikMenuPane* aMenuPane );
    
private:
    /** 
    *  2nd phase Constructor
    */
    void ConstructL();

    /** 
    *  Constructor
    */
    CGlxMetadataCommandHandler(MGlxMediaListProvider* aMediaListProvider);

private:
    CGlxCommandHandlerAiwShowMap * iShowOnMapCmdHandler;	
    MGlxMediaListProvider* iMediaListProvider;

    };

#endif /*GLXMETADATACOMMANDHANDLER_H_*/
