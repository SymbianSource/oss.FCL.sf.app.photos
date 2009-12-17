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
* Description:    Media List View Base
*
*/




#ifndef C_GLXMEDIALISTVIEW_H
#define C_GLXMEDIALISTVIEW_H

#include "glxtoolbarcontroller.h"
#include "glxmskcontroller.h"
#include "glxviewbase.h"
#include "mglxmedialistprovider.h"
#include "mglxtitlefetcherobserver.h"
#include <glxmedialistiterator.h>

class MGlxMediaListFactory;
class MMPXCollectionUtility;
class CGlxTitleFetcher;
class CGlxAttributeContext;

/**
 * Struct to hold resource ids
 * 
 */
struct TViewWidgetIds
	{	
	//Instance Id for the View Widget
	const char* iViewWidgetId;
	//Instance Id for the Widget	
	const char* iWidgetId;
	// Control Group Id for View Widget
	TInt iControlGroup;
	};

/**
 * Media Liost View Base
 */
class CGlxMediaListViewBase : public CGlxViewBase, 
                              public MGlxMediaListProvider,
                              public MGlxTitleFetcherObserver

	{
public:
	/**
	 * Constructor
	 */
	IMPORT_C CGlxMediaListViewBase();
	
	/**
	 * Destructor
	 */
	IMPORT_C virtual ~CGlxMediaListViewBase();
	
public: // From MGlxMediaListProvider
	IMPORT_C virtual MGlxMediaList& MediaList();


    // From MGlxTitleFetcherObserver
    IMPORT_C void HandleTitleAvailableL(const TDesC& aTitle);



protected:
	/**
	 * This will be called on derived classes when the view is activated
     * @param aPrevViewId Specifies the view previously active.
     * @param aCustomMessageId Specifies the message type.
     * @param aCustomMessage The activation message.
	 */
	virtual void DoMLViewActivateL(const TVwsViewId& aPrevViewId, TUid aCustomMessageId, const TDesC8& aCustomMessage) = 0;

	/**
	 * This will be called on derived classes when the view is deactivated
	 */
    virtual void DoMLViewDeactivate() = 0;
    
	/**
	 * Derived classes should use this to deal with any view-specific commands
	 * @param aCommand The command to respond to
	 * @return ETrue iff the command has been handled.
	 */
	IMPORT_C virtual TBool HandleViewCommandL(TInt aCommand);

    /**
     * Handles commands
	 * @param aCommand The command to respond to
	 * @return ETrue iff the command has been handled.
     */
     
    IMPORT_C virtual TInt DoHandleCommandL(TInt aCommand);

protected:
	/**
	 * Media List View Base Constructor
     * @param aMediaListFactory A media list provider.
     * @param aTitle If specified, title to be displayed instead of that from media list
     * @param aCustomMessage The activation message.
     * @param aEnableMiddleSoftkey MSK Enabler
	 */
	IMPORT_C void MLViewBaseConstructL(
	                 MGlxMediaListFactory* aMediaListFactory,
	                 const TDesC& aTitle = KNullDesC(),TBool aEnableMiddleSoftkey = ETrue );
	
private: // From CGlxViewBase
	IMPORT_C virtual void DoViewActivateL(const TVwsViewId& aPrevViewId, TUid aCustomMessageId, const TDesC8& aCustomMessage);
    IMPORT_C virtual void DoViewDeactivate();

    /**
     * See @ref CGlxViewBase::FetchAttributesL()
     */
    IMPORT_C virtual void FetchAttributesL();
    
    /**
     * See @ref CGlxViewBase::FetchAttributesForCommandL()
     */
    IMPORT_C virtual void FetchAttributesForCommandL(TInt aCommand);
    
    /**
     * See @ref CGlxViewBase::SetToolbarStateL()
     */
    IMPORT_C void SetToolbarStateL();
    
protected: // from CGlxViewBase
	/**
	 * See @ref CGlxViewBase::DoPrepareCommandHandlerL
	 */
	IMPORT_C void DoPrepareCommandHandlerL(CGlxCommandHandler* aCommandHandler);
	

private:
	/**
	 * Removes the attribute context (iPreloadContextForCommandHandlers) 
	 * from the media list and closes the media list.
	 */
	void CloseMediaList();
	
	/**
	 * Helper method to add attributes to an attribute context.
	 * @param aAttributeContext context to add attributes to.
	 * @param aCommandHandler command handler to request attributes from.
	 * @param aFilterUsingSelection If ETrue, only attributes relevant
	 * to the current selection will be appended.
	 * @param aFilterUsingCommandId If ETrue, only attributes relevant
	 * to the command id specified by aCommandId will be appended
	 * @param aCommandId if aFilterUsingCommandId is ETrue, only
	 * attributes relevant to aCommandId will be appened.
	 */
	void AddAttributesToContextL(CGlxAttributeContext& aAttributeContext, 
			                     CGlxCommandHandler* aCommandHandler, 
			                     TBool aFilterUsingSelection, 
			                     TBool aFilterUsingCommandId, 
			                     TInt aCommandId = 0);
	
	/**
	 * Helper method to fetch attributes
	 * @param aFilterUsingCommandId If ETrue, only attributes relevant
	 * to the command id specified by aCommandId will be appended
	 * @param aCommandId if aFilterUsingCommandId is ETrue, only
	 * attributes relevant to aCommandId will be appened.
	 */
	void FetchAttributesL(TBool aFilterUsingCommandId, TInt aCommandId = 0);
	
protected:
	MGlxMediaList* iMediaList;
    MMPXCollectionUtility* iCollectionUtility;
 	HBufC*    iFixedTitle;
 	
private:
	MGlxMediaListFactory* iMediaListFactory; 
	CGlxTitleFetcher* iTitleFetcher;
	
	/**
	 * Controlls the ui states of toolbar for all the views
	 * (Owned)
	 */
	CGlxToolbarController* iToolbarControl; 
	
	/**
	 * Controlls the Msk for all the views
	 * (Owned)
	 */
	CGlxMSKController* iCbaControl;
	
    /**
     * Attribute context used to preload attributes
     * required by command handlers.
     * (Owned)
     */
    CGlxAttributeContext* iPreloadContextForCommandHandlers;
    
    /**
     * Selection iterator used by iPreloadContextForCommandHandlers above.
     */
    TGlxSelectionIterator iSelectionIterator;
    
    /**
     * MSK Enabler used by FullScreen to disable the MSK
     */
    TBool iEnableMidddleSoftkey;
	};


#endif // C_GLXMEDIALISTVIEW_H
