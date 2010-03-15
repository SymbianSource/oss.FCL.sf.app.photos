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
* Description:    Image Editor command handler
*
*/




#ifndef C_GLXCOMMANDHANDLERAIWEDIT_H
#define C_GLXCOMMANDHANDLERAIWEDIT_H

#include "glxcommandhandleraiwbase.h"

class CGlxAttributeContext;
class CAiwGenericParamList;
class CGlxMedia;
class MGlxMediaListProvider;
class CAiwServiceHandler;
class CGlxImageViewerManager;
class CFeatureDiscovery;
/**
 *  CGlxCommandHandlerAiwEdit
 *
 *  Edit AIW command handler
 *
 *  @lib glxcommoncommandhandlers.lib
 */

NONSHARABLE_CLASS (CGlxCommandHandlerAiwEdit)
    : public CGlxMediaListCommandHandler
    {
public:     
    /**
     * Two-phase constructor
     * @param aMediaListProvider object that provides the media list
     * @param aCommandSingleClick Flag to identify single click command
     * @return Fully constructed command handler
     * @warning ConstructL of base class is called. If ConstructL is 
     *          implemented in this class, then care must be taken to call 
     *          CGlxCommandHandlerAiwBase::ConstructL
     */
    IMPORT_C static CGlxCommandHandlerAiwEdit* NewL(MGlxMediaListProvider* aMediaListProvider, TBool aCommandSingleClick=EFalse);        

    /**
     * Destructor
     */
    IMPORT_C ~CGlxCommandHandlerAiwEdit();
    
private:
    /** 
     * Second phase constructor
     */
    void ConstructL();
    
    /**
     * Constructor
     * @param aMediaListProvider object that provides the media list
     * @param aCommandSingleClick Flag to identify single click command
     */
    CGlxCommandHandlerAiwEdit( MGlxMediaListProvider* aMediaListProvider, TBool aCommandSingleClick);
    
    /*
     * return if the viewing mode is fullscreen or grid
     */
    TBool IsInFullScreenViewingModeL();    
    
private: // From CGlxMediaListCommandHandler

    void DoActivateL(TInt aViewId);
    TBool DoExecuteL( TInt aCommandId , MGlxMediaList& aList);
    void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);
    
private: // Data Members
    
     // Owned - AIW Service Handler
    CAiwServiceHandler* iServiceHandler;    
    
     //Edit Supported
    TBool iEditSupported;
    
    TBool iCommandSingleClick;
    
    // Not own
    CGlxImageViewerManager* iImageViewerInstance;
    
    CFeatureDiscovery* iFeatManager;
    };

#endif // C_GLXCOMMANDHANDLERAIWEDIT_H
