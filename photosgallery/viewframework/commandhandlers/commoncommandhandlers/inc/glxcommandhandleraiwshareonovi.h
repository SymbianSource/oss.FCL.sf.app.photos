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
* Description:    AIW Share On Ovi command handler ( Works only with ShareOnline application version 4.3 and above )
*
*/


#ifndef GLXCOMMANDHANDLERAIWSHAREONOVI_H_
#define GLXCOMMANDHANDLERAIWSHAREONOVI_H_

// INTERNAL INCLUDES
#include "glxcommandhandleraiwbase.h"

// FORWARD REFERENCES
class MGlxMediaListProvider;

/**
 *  CGlxCommandHandlerAiwShareOnOvi
 *
 *  Share on Ovi AIW command handler
 *
 *  @lib glxcommoncommandhandlers.lib
 */
NONSHARABLE_CLASS (CGlxCommandHandlerAiwShareOnOvi)
    : public CGlxCommandHandlerAiwBase
    {
public:
    /**
	 * Two-phase constructor
	 * @param aMediaListProvider object that provides the media list
	 * @param aMenuResource The menu resource that the AIW command will be 
	 *          implented in
	 * @param aFileName resource file
	 * @return Fully constructed command handler
	 * @warning ConstructL of base class is called. If ConstructL is 
	 *          implemented in this class, then care must be taken to call 
	 *          CGlxCommandHandlerAiwBase::ConstructL
	 */
	IMPORT_C static CGlxCommandHandlerAiwShareOnOvi* NewL(
			MGlxMediaListProvider* aMediaListProvider, TInt aMenuResource,
			const TDesC& aFileName);
        
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

private:
    /**
     * Constructor
     */
    CGlxCommandHandlerAiwShareOnOvi(MGlxMediaListProvider* aMediaListProvider, 
            TInt aMenuResource);
    };


#endif /*GLXCOMMANDHANDLERAIWSHAREONOVI_H_*/
