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
* Description:    CommandHandlerMoreInfo - Handler for DRM related commands.
*
*/





/**
 * @internal reviewed 03/07/2007 by Rowland Cook
 */

#ifndef C_GLXCOMMANDHANDLERMOREINFO_H
#define C_GLXCOMMANDHANDLERMOREINFO_H

// INCLUDES
#include <e32base.h>
#include <glxmedialistcommandhandler.h>

// FORWARD DECLARATIONS
class CGlxDRMUtility;
class CGlxDefaultAttributeContext;
// CLASS DECLARATION

//class

/**
 *  CGlxCommandHandlerMoreInfo class 
 *  Handler for DRM related commands
 *  @glxdrmcommandhandler.lib
 *  @author M Byrne
 */
NONSHARABLE_CLASS( CGlxCommandHandlerMoreInfo ): public CGlxMediaListCommandHandler                                                     
    {
public : //constructor and desctructor
    /**
     * Two-phase constructor
     * @param aMediaListProvider object that provides the media list
     * @param aIsContainerList 
     */
IMPORT_C static CGlxCommandHandlerMoreInfo* NewL(
        MGlxMediaListProvider* aMediaListProvider, TBool aIsContainerList);
        
    /** Destructor */
    IMPORT_C ~CGlxCommandHandlerMoreInfo();
    
    /** Activate command handler
      * @param aViewId id of view activating cmd handler
      */
    void DoActivateL(TInt aViewId);
    
    /**
      * Deactivate command handler
      */
    void Deactivate();
    
protected:
    /** see @ref CGlxMediaListCommandHandler::DoExecuteL */
    TBool DoExecuteL(TInt aCommandId, MGlxMediaList& aList);

    /** see @ref CGlxMediaListCommandHandler::DoIsDisabled */
    TBool DoIsDisabled(TInt aCommandId, MGlxMediaList& aList) const;
         
private:
    /** Second phase constructor */
    void ConstructL(TBool aIsContainerList);

    /** Constructor */
    CGlxCommandHandlerMoreInfo(MGlxMediaListProvider* aMediaListProvider);
    
    /**
     * Check if 'more info' option is supported 
     * (i.e. URL for information can be returned)
     * @param aList reference to media list
     * @return ETrue if 'more info' URL is available
     */
    TBool CanGetInfoURLL(MGlxMediaList& aList) const;
     
private:
    
    // Does the list contain containers or items
    TInt iIsContainerList;  
    
    /** Pointer to DRM utility class (owned) */
    CGlxDRMUtility* iDrmUtility;    
    
    /** Fetch context for required attributes (owned) */
    CGlxDefaultAttributeContext* iFetchContext;
    
    /** URI for current media items (owned) */
    mutable HBufC*  iUrl;
    };
    
#endif // C_GLXCOMMANDHANDLERMOREINFO_H



