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


/**
 *  CGlxCommandHandlerAiwEdit
 *
 *  Edit AIW command handler
 *
 *  @lib glxcommoncommandhandlers.lib
 */
NONSHARABLE_CLASS (CGlxCommandHandlerAiwEdit)
    : public CGlxCommandHandlerAiwBase
    {
public:
    /**
     * Two-phase constructor
     * @param aMediaListProvider object that provides the media list
     * @param aMenuResource The menu resource that the AIW command will be 
     *          implented in
     * @param aCommandSingleClick Flag to identify single click command
     * @return Fully constructed command handler
     * @warning ConstructL of base class is called. If ConstructL is 
     *          implemented in this class, then care must be taken to call 
     *          CGlxCommandHandlerAiwBase::ConstructL
     */
	IMPORT_C static CGlxCommandHandlerAiwEdit* NewL(
	    MGlxMediaListProvider* aMediaListProvider, TInt aMenuResource,
	    TBool aCommandSingleClick=EFalse);
	    
protected: // From CGlxMediaListCommandHandler
	/**
	 * See @ref CGlxMediaListCommandHandler::DoGetRequiredAttributesL
	 */
    virtual void DoGetRequiredAttributesL(RArray<TMPXAttribute>& aAttributes, TBool aFilterUsingSelection) const;

protected: // From CGlxCommandHandlerAiwBase
    virtual TBool AppendAiwParameterL(const TGlxMedia& aItem, 
                                     CGlxAiwServiceHandler& aAiwServiceHandler);
    virtual TInt CommandId() const;
    virtual TInt AiwCommandId() const ;
    virtual TInt AiwInterestResource() const ;
    virtual TInt CommandSpace() const ;
	//single clk chng-hide edit for 0 or >1 items
    void AiwDoDynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);
private:
    /**
     * Constructor
     */
    CGlxCommandHandlerAiwEdit(MGlxMediaListProvider* aMediaListProvider, 
            TInt aMenuResource);
    
    /** 
     * Second phase constructor
     * @param aCommandSingleClick Flag to identify single click command
     */
    void ConstructL(TBool aCommandSingleClick);
    
private:
    TBool iCommandSingleClick;
    };

#endif // C_GLXCOMMANDHANDLERAIWEDIT_H
