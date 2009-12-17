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
* Description:    Delete command handler
*
*/




#ifndef __C_GLXCOMMANDHANDLERREMOVEFROM_H__
#define __C_GLXCOMMANDHANDLERREMOVEFROM_H__

#include <e32base.h>
#include <glxmedia.h>
#include <glxmpxcommandcommandhandler.h>
#include <mpxmediageneraldefs.h>

class MGlxMediaListProvider;

/**
 * @class CGlxCommandHandlerRemoveFrom
 *
 * Command handler that removes relationships between media objects.
 *
 */

NONSHARABLE_CLASS( CGlxCommandHandlerRemoveFrom )
    : public CGlxMpxCommandCommandHandler
	{
public:
    /**
     * Two-phase constructor
     * @param aMediaListProvider object that provides the media list
     * @param aContainerType specifies the container type. (Used to determine
     * the correct confirmation note.) Only confirmation notes for EMPXAlbum
     * and EMPXTag are supported. A confirmation note will not be displayed for
     * unsupported types.
     */
	IMPORT_C static CGlxCommandHandlerRemoveFrom* NewL(
	    MGlxMediaListProvider* aMediaListProvider, 
            TMPXGeneralCategory aContainerType, TInt aCommandId = NULL);
	    
    /** 
    * Create remove (from) Favourites command handler 
    * @param aMediaListProvider object that provides the media list.
    */
   IMPORT_C static CGlxCommandHandlerRemoveFrom* NewRemFromFavCommandHandlerL(
       MGlxMediaListProvider* aMediaListProvider);


	/** Destructor */
	IMPORT_C ~CGlxCommandHandlerRemoveFrom();
	
	/**
	 * Set the container from which the items will be removed.
	 * If this is not set, the items will be removed from their container (their
	 * parent) on the path owned by the media list passed as a parameter to 
	 * CreateCommandL().
	 * @param aContainerId. The container from which the items will be removed.
	 */
	IMPORT_C void SetContainerId(TGlxMediaId aContainerId);
    
protected: // From CGlxMpxCommandCommandHandler
    /** See @ref CGlxMpxCommandCommandHandler::CreateCommandL */
	virtual CMPXCommand* CreateCommandL(TInt aCommandId, MGlxMediaList& aList,
	    TBool& aConsume) const;    

    /** See CGlxMpxCommandCommandHandler::ConfirmationTextL */
    virtual HBufC* ConfirmationTextL(TInt aCommandId, TBool aMultiSelection) const;

    virtual HBufC* CompletionTextL() const;
    
protected: // from CGlxCommandHandler
    /** See @ref CGlxCommandHandler::DoActivateL */
    void DoActivateL(TInt /*aViewId*/);
    
private:
    /** Second phase constructor */
	void ConstructL(TInt aCommandId);

    /** Constructor */
	CGlxCommandHandlerRemoveFrom(MGlxMediaListProvider* aMediaListProvider, TMPXGeneralCategory aContainerType);
	
private:
    /// Resource file offset
	TInt iResourceOffset;
	TGlxMediaId iRemoveFromContainerId;
	TMPXGeneralCategory iContainerType;
	TInt iCommandId;
	};

#endif // __C_GLXCOMMANDHANDLERREMOVEFROM_H__
