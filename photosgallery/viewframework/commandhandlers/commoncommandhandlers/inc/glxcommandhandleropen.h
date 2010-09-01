/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    Rename command handler
*
*/




#ifndef __C_GLXCOMMANDHANDLERREOPEN_H__
#define __C_GLXCOMMANDHANDLERREOPEN_H__

#include <e32base.h>
#include <glxmedialistcommandhandler.h>
#include <glxuiutility.h>

class MGlxMediaListProvider;

/**
 * @class CGlxCommandHandlerOpen
 * Command handler for Open Option.
 */
NONSHARABLE_CLASS(CGlxCommandHandlerOpen) :public CGlxMediaListCommandHandler
	{
public:
    /**
     * Two-phase constructor
     */
	IMPORT_C static CGlxCommandHandlerOpen* NewL(MGlxMediaListProvider* aMediaListProvider);
	    
	/** Destructor */
	IMPORT_C ~CGlxCommandHandlerOpen();
    

private:
    /** Second phase constructor */
	void ConstructL();
	 /**
     *  Constructor
     * @param aMediaListProvider object that provides the media list
     */
	CGlxCommandHandlerOpen( MGlxMediaListProvider* aMediaListProvider);
 
private:
	    virtual void DoActivateL(TInt aViewId);
	    
		virtual TBool DoExecuteL( TInt aCommandId , MGlxMediaList& aList);
		
	    TBool DoIsDisabled(TInt aCommandId, MGlxMediaList& aList) const;
	    
private:
    CGlxUiUtility* iUiUtility;
	};

#endif // __C_GLXCOMMANDHANDLEROPEN_H__
