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
* Description:    AIW command handler base class
*
*/



#ifndef C_GLXCOMMANDHANDLERAIWBASE_H
#define C_GLXCOMMANDHANDLERAIWBASE_H

#include <AiwCommon.h>
#include <glxmedialistcommandhandler.h>
#include <glxmedialistiterator.h>

class CGlxAttributeContext;
class CGlxAiwServiceHandler;
class CGlxMedia;
class TGlxMedia;
class MGlxMediaList;
class CGlxAttributeRetriever;
class CAknAppUi;

/**
 *  CGlxCommandHandlerAiwBase
 *
 *  Base class for AIW command handlers
 *
 *  @lib glxcommoncommandhandlers.lib
 */
NONSHARABLE_CLASS (CGlxCommandHandlerAiwBase)
    : public CGlxMediaListCommandHandler, 
      public MAiwNotifyCallback
    {
public:
	/** Destructor */
	virtual ~CGlxCommandHandlerAiwBase();

public: // From MAiwNotifyCallback
    virtual TInt HandleNotifyL(
        TInt aCmdId,
        TInt aEventId,
        CAiwGenericParamList& aEventParamList,
        const CAiwGenericParamList& aInParamList);
            
protected: // From CGlxMediaListCommandHandler
    virtual TBool BypassFiltersForExecute() const;
    virtual TBool BypassFiltersForMenu() const;
    virtual TBool DoExecuteL(TInt aCommandId, MGlxMediaList& aList);
    virtual void DoDynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);
	virtual void PreDynInitMenuPaneL( TInt aResourceId );
    virtual void DoActivateL(TInt aViewId);
    virtual void Deactivate();
    
protected: // New functions
    /** 
     * Constructor 
     * @param aMediaListProvider The media list owner, used by 
     *          CGlxMediaListCommandHandler
     * @param aMenuResource The menu resource that the AIW command will be 
     *          implented in
     */
	CGlxCommandHandlerAiwBase(MGlxMediaListProvider* aMediaListProvider, 
	                          TInt aMenuResource);

    /** Second phase constructor */
	void ConstructL();
	
    /**
     *  This appends a Uri and/or a mimetype parameter to the InParams list (see Aiw Service Handler)
     *  @param aItem - The data source. Used to extract the Uri/mimetype
     *  @param aAiwServiceHandler - Reference to the CGlxAiwServiceHandler
     *  @param aAddUri - ETrue to add the Uri, EFalse if not.
     *  @param aAddMimeType - Etrue to add the mimetype, EFalse if not.
     */
    TBool AppendDefaultAiwParameterL(const TGlxMedia& aItem, 
                                    CGlxAiwServiceHandler& aAiwServiceHandler,
                                    TBool aAddUri,
                                    TBool aAddMimeType);

	
protected: // New functions to be overridden by implementing class
    /**
     * Appends one or more parameters to aParamList, to represent aItem. 
     * aParamList will eventually be passed to Aiw, so the format of these 
     * parameters should be agreed with AIW.
     * @param aItem Item to examine
     * @param aParamList The parameter list to append the new parameter(s) to
     * @return True iff the parameter could be appended; False otherwise
     */
    virtual TBool AppendAiwParameterL(const TGlxMedia& aItem, 
                                     CGlxAiwServiceHandler& aAiwServiceHandler) = 0;
    
    /**
     * @return The command ID in the menu that AIW will replace
     */
    virtual TInt CommandId() const = 0;
    
    /**
     * @return The AIW command implemented
     */
    virtual TInt AiwCommandId() const = 0;
    
    /**
     * @return The AIW interest resource for the subclass
     */
    virtual TInt AiwInterestResource() const = 0;
    
    /**
     * @return The maximum number of selected items supported
     */
    virtual TInt MaxSelectedItems() const;

    /**
     * Allow the concrete implementation to do some work on the menu before
     * passing it to the AIW service handler
	 * @param aResourceId The resource ID of the menu
	 * @param aMenuPane The in-memory representation of the menu pane
	 * @see CGlxMediaListCommandHandler::DoDynInitMenuPaneL
	 */
    virtual void AiwDoDynInitMenuPaneL(TInt aResourceId, 
        CEikMenuPane* aMenuPane);

    /**
     * @return The command space to use for the command
     */    
    virtual TInt CommandSpace() const = 0;
    
    /**
     * @param aMenuItemText itemText for menu Item
     * @param aMenuPane menuPane in which itemText to be searched
     * @return The AIW commandId for menuItemText
     */
    TInt AiwMenuCmdIdL( const TDesC& aMenuItemText, CEikMenuPane* aMenuPane );
        
private:
    /**
     * Collates all Aiw parameters needed by iterating through the selected 
     * items. adding them to an internal array in the aAiwServiceHandler
     * @param aAiwServiceHandler a reference to the AiwServiceHandler
     * @return True iff all parameters were available, False otherwise
     * @see AppendAiwParameterL
     */
    TBool GatherAiwParametersL(CGlxAiwServiceHandler& aAiwServiceHandler);

    /**
     * Collates all Aiw parameters needed by iterating through the selected 
     * items adding them directly to the AIW InParams
     * @param aAiwServiceHandler a reference to the AiwServiceHandler
     * @return True iff all parameters were available, False otherwise
     * @see AppendAiwParameterL
     */
    TBool RegatherAiwParametersL(CGlxAiwServiceHandler& aAiwServiceHandler);
    
    /**
     * Check whether the AIW item is enabled
     * @return Whether the AIW item is enabled
     */
    TBool AiwItemEnabledL();

    TBool AppendTextParameterL(const TGlxMedia& aItem,
                              CGlxAiwServiceHandler& aAiwServiceHandler,
                              const TDesC& aText,
                              TGenericParamIdValue aGenericParamIdValue);
                              
    /*
    TBool AppendTextParameterL(CGlxAiwServiceHandler& aAiwServiceHandler,
                              const TDesC& aText,
                              TGenericParamIdValue aGenericParamIdValue);
    */

private:
    /// Menu resource containing iCommandId
    TInt iMenuResource;
        
    /// The AppUI, not owned.
    CAknAppUi* iAppUi;
    
    /// Pointer to the AiwServiceHandler. This is the real CGlxAiwServiceHandler, of which there can only be one instance, Hence the need to wrap
    /// in this singleton class.
    CGlxAiwServiceHandler* iAiwServiceHandler;
    TInt iResourceOffset;
    };

#endif // C_GLXCOMMANDHANDLERAIWBASE_H
