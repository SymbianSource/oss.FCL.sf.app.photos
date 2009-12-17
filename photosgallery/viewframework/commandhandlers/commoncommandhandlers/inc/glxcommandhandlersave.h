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




#ifndef __C_GLXCOMMANDHANDLERSAVE_H__
#define __C_GLXCOMMANDHANDLERSAVE_H__

#include <e32base.h>
#include <glxmpxcommandcommandhandler.h>
#include <glximageviewermanager.h>

class MGlxMediaListProvider;
class CGlxUiUtility;

/**
 * @class CGlxCommandHandlerSave
 *
 * Command handler that deletes selected items from a media list.
 *
 */

NONSHARABLE_CLASS( CGlxCommandHandlerSave)
    : public CGlxMpxCommandCommandHandler
    {
public:
    /**
     * Two-phase constructor
     * @param aMediaListProvider object that provides the media list
     * @param aHasToolbarItem Whether Command Should be toolbar item (ETrue - if toolbar item)
     * @return pointer to CGlxCommandHandlerSave object
     */
    IMPORT_C static CGlxCommandHandlerSave* NewL(
        MGlxMediaListProvider* aMediaListProvider, TBool aHasToolbarItem);
        
    /** Destructor */
    IMPORT_C ~CGlxCommandHandlerSave();
    
public: // From CGlxMpxCommandCommandHandler
    /** See @ref CGlxMpxCommandCommandHandler::CreateCommandL */
    virtual CMPXCommand* CreateCommandL(TInt aCommandId, MGlxMediaList& aList,
        TBool& aConsume) const;    

public: // From CGlxCommandHandler
    /** See @ref CGlxCommandHandler::DoActivateL */
    void DoActivateL(TInt aViewId);
    virtual void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);
     /** See @ref CGlxCommandHandler::OfferKeyEventL  */
    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);

protected:
     /** See @ref CGlxCommandHandler::PopulateToolbar*/
    void PopulateToolbarL();

private:
    /** Second phase constructor */
    void ConstructL();

    /** Constructor */
    CGlxCommandHandlerSave(MGlxMediaListProvider* aMediaListProvider, TBool aHasToolbarItem);    

    
private:
    /// Resource file offset
    TInt iResourceOffset;
    
 
    CGlxUiUtility* iUiUtility;
    // Not own
    CGlxImageViewerManager* iImageViewerInstance;
    };
    

#endif // __C_GLXCOMMANDHANDLERSAVE_H__
