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
     * @return Fully constructed command handler
     * @warning ConstructL of base class is called. If ConstructL is 
     *          implemented in this class, then care must be taken to call 
     *          CGlxCommandHandlerAiwBase::ConstructL
     */
    IMPORT_C static CGlxCommandHandlerAiwEdit* NewL(MGlxMediaListProvider* aMediaListProvider);        

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
     */
    CGlxCommandHandlerAiwEdit( MGlxMediaListProvider* aMediaListProvider);
    
    /*
     * return if the viewing mode is fullscreen or grid
     */
    TBool IsInFullScreenViewingModeL();    
    
private: // From CGlxMediaListCommandHandler

    void DoActivateL(TInt aViewId);
    TBool DoExecuteL( TInt aCommandId , MGlxMediaList& aList);
    void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane, 
							TBool aIsBrowseMode);
    
private: // Data Members
    
     // Owned - AIW Service Handler
    CAiwServiceHandler* iServiceHandler;    
    
    //Is ImageEdit supported
    TBool iIsImageEditSupported;
    
    //Is VideoEdit supported
    TBool iIsVideoEditSupported;
       
    // Not own
    CGlxImageViewerManager* iImageViewerInstance;
    
	//If in fullscreen mode
    TBool iIsFullScreenMode;
    
    };

#endif // C_GLXCOMMANDHANDLERAIWEDIT_H
