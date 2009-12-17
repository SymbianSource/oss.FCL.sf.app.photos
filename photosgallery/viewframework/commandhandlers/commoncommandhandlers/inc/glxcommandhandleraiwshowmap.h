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
* Description:    Show on map command handler
*
*/




#ifndef C_GLXCOMMANDHANDLERAIWSHOWMAP_H
#define C_GLXCOMMANDHANDLERAIWSHOWMAP_H

#include "glxcommandhandleraiwbase.h"

class CGlxAttributeContext;
class CAiwGenericParamList;
class CGlxMedia;
class MGlxMediaListProvider;
class TCoordinate;

/**
 *  CGlxCommandHandlerAiwShowMap
 *
 *  Show map AIW command handler
 *
 *  @author Rhodri Byles
 *  @lib glxcommoncommandhandlers.lib
 */
NONSHARABLE_CLASS (CGlxCommandHandlerAiwShowMap)
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
	IMPORT_C static CGlxCommandHandlerAiwShowMap* NewL(
	    MGlxMediaListProvider* aMediaListProvider, TInt aMenuResource);
	
    /**
     * Perform any actions needed before the options menu is displayed.
     * The view calling this must first arrange a wait dialog to be displayed, as this operation may take
     * a long time to complete
     */
	void PreDynInitMenuPaneL(TInt aResourceId);
	
	/**
	 * Destructor
	 */
	~CGlxCommandHandlerAiwShowMap();

protected: // From CGlxMediaListCommandHandler
	/**
	 * See @ref CGlxMediaListCommandHandler::DoGetRequiredAttributesL
	 */
    virtual void DoGetRequiredAttributesL(RArray<TMPXAttribute>& aAttributes, TBool aFilterUsingSelection) const;

protected: // From CGlxCommandHandlerAiwBase
    TBool AppendAiwParameterL(const TGlxMedia& aItem, 
                                     CGlxAiwServiceHandler& aAiwServiceHandler);
    TInt CommandId() const;
    TInt AiwCommandId() const ;
    TInt AiwInterestResource() const ;
    TInt CommandSpace() const ;
    void AiwDoDynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);


    
private:
    /**
     * Constructor
     * @param aMediaListProvider object that provides the media list
     * @param aMenuResource The menu resource that the AIW command will be 
     *          implented in
     */
    CGlxCommandHandlerAiwShowMap(MGlxMediaListProvider* aMediaListProvider, 
            TInt aMenuResource);
    
    /**
     * Check if a selected item in a media list has location information.
     * @param MediaList to check
     * @return ETrue if location information is there. EFalse if not
     */
    TBool IsItemWithLocationInfoSelected(MGlxMediaList& aList);
            
private:
    RPointerArray<HBufC8> iBufferArray;  // owned
    };

#endif // C_GLXCOMMANDHANDLERAIWSHOWMAP_H
