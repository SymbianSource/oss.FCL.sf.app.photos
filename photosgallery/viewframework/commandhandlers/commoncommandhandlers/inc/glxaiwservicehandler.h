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
* Description:    Photos AIW Service handler Proxy class for the CAiwServiceHandler
*
*/




#ifndef _GLXAIWSERVICEHANDLER_H
#define _GLXAIWSERVICEHANDLER_H

/**
 * CGlxAiwServiceHandler
 * Proxy class for the CAiwServiceHandler
 *
 * @author Rowland Cook
 * @internal reviewed Aki Vanhatalo 20/08/2007
 */

#include <e32base.h>
#include <eikmenup.h>

class TAiwGenericParam;
class CAiwServiceHandler;
class CAiwGenericParamList;
class MAiwNotifyCallback;
class CGlxAiwMedia;
class TGlxMediaId;
class t_CGlxAiwServiceHandler;

/**
 *  CGlxAiwServiceHandler class
 *  This class is a proxy for the CAiwServiceHandler.
 *  It is Singleton.
 */
NONSHARABLE_CLASS(CGlxAiwServiceHandler) : public CBase 
    {
    friend class t_CGlxAiwServiceHandler;
public:
    /**
     * Obtain the instance of the Singleton
     */ 
    static CGlxAiwServiceHandler* InstanceL();
    
    /**
     * Close
     */
    void Close();

    /**
     *  Destructor
     */
    ~CGlxAiwServiceHandler();
    
    /**
     *  Adds a TAiwGenericParam to the InParams
     * @param aParam The parameter to append to this list. This object takes
     *               an own copy of the data in aParam.
     */
    void AddParamL(const TGlxMediaId& aId, const TAiwGenericParam& aParam );

    /**
     *  Initialises the Meu Pane
     * @param aMenuPane Handle of the menu pane to initialise.
     * @param aMenuResourceId The menu to be attached.
     * @param aBaseMenuCmdId Base ID for the Service Handler to generate 
     *                       menu IDs for placeholders.
     * @leave KErrNotSupported CCoeEnv is not accessible.
     * @leave KErrOverflow Consumer application has too many AIW placeholders in its menu. 
     *                     Currently, maximum 16 is supported.
     *  
     */
    void InitializeMenuPaneL(CEikMenuPane& aMenuPane, TInt aResourceId, TInt aBaseMenuCmdId);
    
    /**
     * Attach menu related criteria items to the given menu.
     * If a provider leaves during initialization, it is trapped by the Service Handler. 
     *
     * @param aMenuResourceId      Menu to be attached.
     * @param aInterestResourceId  Resource id for the interest list.
     * @leave KErrNotSupported     CCoeEnv is not accessible.
     */
    void AttachMenuL(TInt aMenuResource, TInt aAiwInterestResource);
    
    /**
     * Identifies is a menu command Id belongs to an Aiw Menu
     * @param aCommand The command to handle
     * @param aAiwCommandId The AIW command.
     * @return ETrue if they are the same, otherwise EFalse.
     */
    TBool IsSameCommand(TInt aCommandId, TInt aAiwCommandId);
    
    /**
     * Executes a menu command.
     * Not supported if calling outside UI framework. 
     *
     * @param aMenuCmdId The menu command to be executed.
     * @param aCmdOptions Options for the command, see TAiwServiceCmdOptions in AiwCommon.hrh.
     * @param aCallback Callback for asynchronous command handling, parameter checking, etc.
     * @leave KErrArgument Callback is missing when required.
     * @leave KErrNotSupported No cmd matches given menu command or CCoeEnv is not accessible.
     */
    void ExecuteMenuCmdL(TInt aMenuCmdId, TUint aCmdOptions = 0, MAiwNotifyCallback* aCallback = NULL);
    
    /**
     * Handles AIW submenus. This method should be called from consumer application's 
     * DynInitMenuPaneL.
     *
     * @param  aPane  Menu pane to be handled.
     * @return ETrue  if aPane was an AIW submenu and it was handled. 
     *                Consumer's DynInitMenuPaneL pane may now return.
     *         EFalse if aPane was not an AIW submenu and DynInitMenuPaneL should
     *                continue normally.
     */
    TBool HandleSubmenuL(CEikMenuPane& aPane);

    /**
     * Returns boolean value indicating whether the given menu contains
     * currently attached placeholders.
     *
     * @param   aMenuResourceId  Resource id of the menu to be queried.
     * @return  ETrue  if aMenuResource contains currently attached placeholders.
     *          EFalse otherwise. 
     */
    TBool IsAiwMenu(TInt aMenuResourceId);
    
    /**
     * Iterates through the Id Array and appends any AIW parameters
     * associated with that Id to the InParams list.
     */
    void AppendInParamsL();
    
    /**
    * Reset menu initialisation flag
    */
    void ResetMenuInitialisedFlag();

private:
    /**
     *  Instantiate the class.
     *  This must be private.
     *  To obtain an Instance on this class call the static method InstanceL().
     *  @return pointer to the CGlxAiwServiceHandler
     */
    static CGlxAiwServiceHandler* NewL();
    
    /**
     *  Constructor
     */
    CGlxAiwServiceHandler();
    
    /**
     * 2nd phase constructor
     */
    void ConstructL();
    
    /**
     * Finds the Index for a given Id
     * @param TGlxMediaId of the index to find.
     * @return index of aId or KErrNotFound.
     */
    TInt FindId(const TGlxMediaId& aId);
    
    /**
     * Function for comparing two CGlxAiwMedia classes 
     * Used from ordering iAiwMediaArray by media id
     * @param aAiwMedia1 The CGlxAiwMedia to compare against aAiwMedia2
     * @param aAiwMedia2 The second item to compare
     * @return -1 if aAiwMedia1.Id() < aAiwMedia2.Id(); 1 if aAiwMedia1.Id() > aAiwMedia2.Id(); 0 if aAiwMedia1.Id() == aAiwMedia2.Id()
     */
    static TInt AiwMediaOrderById( const CGlxAiwMedia& aAiwMedia1, const CGlxAiwMedia& aAiwMedia2 );

    /**
     * Function for comparing a TGlxMediaId with a CGlxAiwMedia class 
     * Used from ordering iAiwMediaArray by media id
     * @param aMediaId The TGlxMediaId to compare against aAiwMedia2
     * @param aAiwMedia2 The second item to compare
     * @return -1 if aMediaId < aAiwMedia2.Id(); 1 if aMediaId > aAiwMedia2.Id(); 0 if aMediaId == aAiwMedia2.Id()
     */
    static TInt AiwMediaOrderByMediaId( const TGlxMediaId* aMediaId, const CGlxAiwMedia& aAiwMedia );
    
    /**
     * Gets the InParameters. This method is for testing only
     * @return a reference to the CAiwGenericParamList pointer to by iInParams
     */
     CAiwGenericParamList& GetInParams();

private:
    // Pointer to the real AiwServiceHandler (Owned).
    CAiwServiceHandler*             iAiwServiceHandler;

	//Enum for menu state    
    enum TMenuState 
    	{
    	ENotInitialised,
    	ESubMenuInitialised,
    	EMainMenuInitialised
    	};
    // Keeps track of the menu being initialised and inParams being 
    // deleted by the AiwServiceHandler
    TMenuState iInitialisedMenu;
    
    // The inParams generated to initialise the menu with
    CAiwGenericParamList*           iInParams;
    
    // Array of CGlxAiwMedia
    RPointerArray<CGlxAiwMedia>     iAiwMediaArray;
    };

#endif

