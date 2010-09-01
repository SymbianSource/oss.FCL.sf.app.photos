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
* Description:    SortOrder command handler
*
*/




#ifndef C_GLXCOMMANDHANDLERSORTORDER_H
#define C_GLXCOMMANDHANDLERSORTORDER_H

// INCLUDES
#include <e32base.h>
#include <glxmedialistcommandhandler.h>
#include <glxfilterfactory.h>		// For TGlxFilterFactory


//FORWARD DECLARATION
class MGlxMediaListProvider;

/**
 * @class CGlxCommandHandlerSortOrder
 *
 * Command handler that changes the sorting criteria of the MediaList
 *
 */
NONSHARABLE_CLASS (CGlxCommandHandlerSortOrder)
    : public CGlxMediaListCommandHandler
	{
	    
	    friend class ut_cglxcommandhandlersortorder;
public:

    /**
     * Two-phase constructor
     * @param aMediaListProvider object that provides the media list
     * @param aCRKey The settings key which used by the commandhandler 
     *								set/get the sort order setting
     */
	IMPORT_C static CGlxCommandHandlerSortOrder* NewL(
							MGlxMediaListProvider* aMediaListProvider,
							const TUint32 aCRKey);


	/** Destructor */
	IMPORT_C ~CGlxCommandHandlerSortOrder();
    
protected:
	
	/** see CGlxMediaListCommandHandler::DoExcecuteL */
    TBool DoExecuteL(TInt aCommandId, MGlxMediaList& aList);
    
    /** see CGlxMediaListCommandHandler::DoIsDisabled */
    TBool DoIsDisabled(TInt aCommandId, MGlxMediaList& aList) const;
    	     
private:

    /** Second phase constructor */
	void ConstructL();

    /** 
     * Constructor 
     * @param aMediaListProvider object that provides the media list
     * @param aCRKey The settings key which used by the commandhandler 
     *								set/get the sort order setting
     */
	CGlxCommandHandlerSortOrder(MGlxMediaListProvider* aMediaListProvider,
												const TUint32 aCRKey);
						
    /**
     *  Finds out the current sort order applied to the media list
     *  @param aList reference to a media list
     *  @returns The current sort order					
     */
    TGlxFilterSortOrder	CurrentSortOrder(MGlxMediaList& aList) const;
	
private: // Data

	/** Holds the id of the cenrep key which needs to be queried/changed */
	TUint32	iKey;

	};

#endif // C_GLXCOMMANDHANDLERSORTORDER_H
