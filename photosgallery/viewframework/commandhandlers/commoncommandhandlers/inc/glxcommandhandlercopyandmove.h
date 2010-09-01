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
* Description:    Move and Copy commmand handler
*
*/




#ifndef GLXCOMMANDHANDLERCOPYANDMOVE_H_
#define GLXCOMMANDHANDLERCOPYANDMOVE_H_

#include <e32base.h>
#include <glxmediaid.h>
#include <glxmpxcommandcommandhandler.h>

/**
 *  CGlxCommandHandlerCopyAndMove 
 *
 *  Copy and Move command handler.
 *
 *  @lib glxcommoncommandhandlers.lib
 *
 * @internal reviewed 07/06/2007 by M. Byrne
 */
NONSHARABLE_CLASS( CGlxCommandHandlerCopyAndMove )
: public CGlxMpxCommandCommandHandler
    {

public:
    /** 
	 * Two-phase constructor: 
	 */
	IMPORT_C static CGlxCommandHandlerCopyAndMove* NewL(
			MGlxMediaListProvider* aMediaListProvider, TInt aMenuResource,
			const TDesC& aFileName);
    
    /** Destructor */
    IMPORT_C ~CGlxCommandHandlerCopyAndMove();

public: // from CGlxMpCommandCommandHandler
    /** See @ref CGlxMpxCommandCommandHandler::CreateCommandL */
    virtual CMPXCommand* CreateCommandL(TInt aCommandId, MGlxMediaList& aList,
            TBool& aConsume) const;  

public: // from CGlxMediaListCommandHandler
    /** See @ref CGlxMediaListCommandHandler::DoDynInitMenuPaneL */
    void DoDynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);

private:
    /** Second phase constructor
	 * @param aResourceFile resource file
	 */
	void ConstructL(const TDesC& aFileName);

    /** First phase constructor */
    CGlxCommandHandlerCopyAndMove(MGlxMediaListProvider* aMediaListProvider, TInt aMenuResource);
    
    /**
     * Appends a drive letter to a descriptor for a given drive number.
     * @param aDriveNumber a drive number that is required to be represented as a descriptor.
     * @param aDriveLetter descriptor to which the drive letter (including ':\') should be appended
     */
    static void DriveLetterFromNumber(TDriveNumber aDriveNumber, TDes& aDriveLetter);
    
private:    
    /** ETrue if the command is visible 
     * in the menu else EFalse*/
    mutable TBool iVisible;
    
    /// Menu resource containing iCommandId
    TInt iMenuResource;
    
    /// Resource file offset
    TInt iResourceOffset;
    };


#endif /*GLXCOMMANDHANDLERCOPYANDMOVE_H_*/
