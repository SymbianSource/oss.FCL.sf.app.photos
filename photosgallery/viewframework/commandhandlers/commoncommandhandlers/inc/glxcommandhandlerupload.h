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
* Description:    Upload commmand handler
*
*/

#ifndef __GLXCOMMANDHANDLERUPLOAD_H__
#define __GLXCOMMANDHANDLERUPLOAD_H__


//  EXTERNAL INCLUDES
#include "AiwServiceHandler.h"                  // AIW service handler
#include <centralrepository.h>              // for checking the ShareOnline version

//  INTERNAL INCLUDES
#include "glxmedialistcommandhandler.h"         // for MediaListCommandHandler
#include <glxuiutility.h>                       // for UiUtility

//  FORWARD DECLARATIONS
class MGlxMediaListProvider;
class CGlxDefaultAttributeContext;
class CGlxUploadCenRepWatcher;
class CSvgEngineInterfaceImpl;

// For upload icon change notification
class MGlxUploadIconObserver
    {
public:
    /**
     * Handle upload icon changes
     */
    virtual void HandleUploadIconChangedL( ) = 0;
    };

/**
 * Command handler that launches Upload AIW ShareOnline
 * @class CGlxCommandHandlerUpload
 * @author Abhijit S 
 */
 
// CLASS DECLARATION

NONSHARABLE_CLASS (CGlxCommandHandlerUpload) : public CGlxMediaListCommandHandler,
public MGlxMediaListObserver, public MGlxUploadIconObserver
	{
public:  // Constructors and destructor
	/**
	* Two-phased constructor.
    * @param aMediaListProvider pointer to media list owner
    * @param aHasToolbarItem Whether Command Should be toolbar item (ETrue - if toolbar item)
    * @return pointer to CGlxCommandHandlerUpload object
    */
	IMPORT_C static CGlxCommandHandlerUpload* NewL(MGlxMediaListProvider* 
	                                                    aMediaListProvider, TBool aHasToolbarItem);
    
    /**
    * Destructor.
    */
    IMPORT_C ~CGlxCommandHandlerUpload();
    
    /**
     * To set the Help Text for Upload button on toolbar
     */
    void SetToolTipL();


public: // From MGlxMediaListObserver
    void HandleItemAddedL(TInt aStartIndex, TInt aEndIndex, 
                                            MGlxMediaList* aList);
    
    void HandleMediaL(TInt aListIndex, MGlxMediaList* aList);
    
    void HandleItemRemovedL(TInt aStartIndex, TInt aEndIndex, 
                                        MGlxMediaList* aList);
    void HandleItemModifiedL(const RArray<TInt>& aItemIndexes, 
                                        MGlxMediaList* aList);
    void HandleAttributesAvailableL(TInt aItemIndex,     
        const RArray<TMPXAttribute>& aAttributes, MGlxMediaList* aList);
    void HandleFocusChangedL(NGlxListDefs::TFocusChangeType aType, 
                        TInt aNewIndex, TInt aOldIndex, MGlxMediaList* aList);
    void HandleItemSelectedL(TInt aIndex, TBool aSelected, MGlxMediaList* aList);
    void HandleMessageL(const CMPXMessage& aMessage, MGlxMediaList* aList);
    void HandlePopulatedL(MGlxMediaList* aList);
    
public: // from MGlxUploadIconObserver
    /**
     * @ref MGlxUploadIconObserver::HandleUploadIconChangedL
     */
    void HandleUploadIconChangedL( );    
    
protected:
	//@ref From CGlxCommandHandler
	void PopulateToolbarL();

private:
	/**
    * Symbian 2nd phase constructor
	*/
    void ConstructL();

    /**
    * C++ default constructor.
    * @param aMediaListProvider pointer to media list owner
    * @param aHasToolbarItem Whether Command Should be toolbar item (ETrue - if toolbar item)
    * @ref CGlxMediaListCommandHandler
    */
    CGlxCommandHandlerUpload(MGlxMediaListProvider* aMediaListProvider, TBool aHasToolbarItem);

    /**
     * Gets the tooltip text or the Help Text from AIWServiceHandler
     */
    void GetToolTipL();

private: // From CGlxMediaListCommandHandler
	/**
    * Execute the command, if applicable. 
	* @param aCommand The command to handle
	* @param aList List of media items
    * @return ETrue if command was handled, EFalse otherwise
    * @ref CGlxMediaListCommandHandler	
	*/
	TBool DoExecuteL (TInt aCommandId, MGlxMediaList& aList);	
	
	/**
	* Called when the owning view is activated
	* @param aViewId The ID of the view
    * @ref CGlxMediaListCommandHandler	
	*/
	void DoActivateL (TInt aViewId);
	
	/**
	* Called when the owning view is deactivated
	*/
	void Deactivate();	

private:
	/**
	* Retrieve the currently selected items file name and path
	* @return The currently selected items filename and path
	*/	
	
	void AppendSelectedFilesL(CAiwGenericParamList& aInputParams);
	
	/**
	 * To check whether OneClickUpload is supported by the Shareonline 
	 * application. 
	 * NOTE: OneClickUpload is a feature of ShareOnline 4.3 and above. Hence this function
	 * ensures that the upload command works only if this version of ShareOnline is 
	 * available in the build. Else, Upload will not work! 
	 */
	void CheckVersionL();
	
	/**
	 * To attach the aiw share interest resource to the servicehandler
	 */
	void InitializeOneClickUploadL();
	
	/**
	 * To check whether its is fullscreen view
	 */
	TBool IsFullScreenViewL();

	/**
	 * Disable/Enable the upload toolbar item
	 */	
	void DisableUploadToolbarItem(TBool aDimmed);

	/**
	 * Updates the selection count based on the mimetypes
	 */		
	void UpdateSelectionCount(TInt aIndex, TBool aSelected, MGlxMediaList* aList);

	/**
	 * Gets the Icon path and fileName from the cenrep
	 */	
	void GetIconNameL(TDes& aUplaodIconName);

	/**
	 * Decodes the Icon using the SVG Engine
	 */	
	void DecodeIconL(const TDes& aUplaodIconName);


	/**
	 * Updates the Toolbar upload icon based on the current selection
	 */	
	void UpdateFSUploadIconL();
	
private: // data

    //Cenrep watcher for monitoring the icon changes
    CGlxUploadCenRepWatcher* iUploadCenRepWatcher;
    
    // ID of owning view 
    TInt iViewId;
    
    // True if ShareOnline versioned 4.3 and above is present in the build
    TBool iUploadSupported;
    
    // Not owned - Provider of media list
    MGlxMediaListProvider* iMediaListProvider;
    
    // Not owned - UI utility 
    CGlxUiUtility* iUiUtility;

    // Owned - AIW Service Handler
    CAiwServiceHandler* iServiceHandler;
    
    //Keep the count of no. of images/videos selected
    TInt iSelectedImageCount;
    TInt iSelectedVideoCount;
    
	//keep track of current icon used
    enum TCenRepMonitors
        {
            EMonitorNone,
            EImageMonitor,
            EVideoMonitor,
            EImageVideoMonitor
        };
    
    TCenRepMonitors iCurrentCenRepMonitor;
    
	//Toolbar instance of current view
    CAknToolbar* iToolbar;

    //To check if we are in fullscreen
    TBool iIsFullScreenView;
    
    CSvgEngineInterfaceImpl* iSvgEngine;
    
    // Tooltip text
    HBufC* iTooltipText;
	};

#endif //__GLXCOMMANDHANDLERUPLOAD_H__
