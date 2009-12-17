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

//  INTERNAL INCLUDES
#include "glxmedialistcommandhandler.h"         // for MediaListCommandHandler
#include <glxuiutility.h>                       // for UiUtility

//  FORWARD DECLARATIONS
class MGlxMediaListProvider;
class CGlxDefaultAttributeContext;

/**
 * Command handler that launches Upload AIW ShareOnline
 * @class CGlxCommandHandlerUpload
 * @author Abhijit S 
 */
 
// CLASS DECLARATION

NONSHARABLE_CLASS (CGlxCommandHandlerUpload) : public CGlxMediaListCommandHandler
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
    void GetToolTipL(HBufC*& aToolTipText);

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
	
private: // data

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
	};

#endif //__GLXCOMMANDHANDLERUPLOAD_H__
