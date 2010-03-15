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
* Description:    AIW assign command handler
*
*/




#ifndef C_GLXCOMMANDHANDLERAIWASSIGN_H
#define C_GLXCOMMANDHANDLERAIWASSIGN_H

#include "glxcommandhandleraiwbase.h"

class CGlxAttributeContext;
class CAiwGenericParamList;
class CGlxMedia;
class MGlxMediaListProvider;
class CFeatureDiscovery;

/**
 *  CGlxCommandHandlerAiwAssign
 *
 *  Assign AIW command handler
 *
 *  @lib glxcommoncommandhandlers.lib
 */
NONSHARABLE_CLASS (CGlxCommandHandlerAiwAssign)
	: public CGlxCommandHandlerAiwBase
    {
public:
    /**
     * Two-phase constructor
     * @param aMediaListProvider object that provides the media list
     * @param aMenuResource The menu resource that the AIW command will be 
     *          implented in
     * @return Fully constructed command handler
     * @warning ConstructL of base class is called. If ConstructL is 
     *          implemented in this class, then care must be taken to call 
     *          CGlxCommandHandlerAiwBase::ConstructL
     */
	IMPORT_C static CGlxCommandHandlerAiwAssign* NewL(
	    MGlxMediaListProvider* aMediaListProvider, TInt aMenuResource);
	
	~CGlxCommandHandlerAiwAssign();
	
protected: // From CGlxMediaListCommandHandler
	/**
	 * See @ref CGlxMediaListCommandHandler::DoGetRequiredAttributesL
	 */
	virtual void DoGetRequiredAttributesL(RArray<TMPXAttribute>& aAttributes, TBool aFilterUsingSelection) const;

protected: // From CGlxCommandHandlerAiwBase
    TBool AppendAiwParameterL(const TGlxMedia& aItem, 
                             CGlxAiwServiceHandler& aAiwServiceHandler);
    TInt CommandId() const;
    TInt AiwCommandId() const;
    TInt AiwInterestResource() const;
    void AiwDoDynInitMenuPaneL(TInt aResourceId, 
        CEikMenuPane* aMenuPane);
    TInt CommandSpace() const;

private:
    /**
     * Constructor
     */
    CGlxCommandHandlerAiwAssign(MGlxMediaListProvider* aMediaListProvider, 
        TInt aMenuResource);
    
    void ConstructL();

private: // Data Members
    
    CFeatureDiscovery* iFeatManager;
    };

#endif // C_GLXCOMMANDHANDLERAIWASSIGN_H
