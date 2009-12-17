/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    Application's adapter class to Map and Navigation use cases
*
*/




#ifndef C_GLXCOMMANDHANDLERAIWSHOWMAPHARDKEY_H
#define C_GLXCOMMANDHANDLERAIWSHOWMAPHARDKEY_H

// INCLUDES
#include <AiwCommon.h>
#include <e32std.h>
//  EXTERNAL INCLUDES
#include "AiwServiceHandler.h"                  // AIW service handler

//  INTERNAL INCLUDES
#include "glxmedialistcommandhandler.h"         // for MediaListCommandHandler

class CAiwServiceHandler;
class TCoordinate;

/**
* The class CGlxCommandHandlerAiwShowMapHardKey contains the implemenation 
* for only show on map Functionality:
*   To check map service providers
*   To show location on map
*   To navigate to the location
*/

NONSHARABLE_CLASS( CGlxCommandHandlerAiwShowMapHardKey )
                    : public CGlxMediaListCommandHandler
	{

public: // Constructors and destructor
    /**
    * Symbian 1st phase static constructor. 
    *       
    */        
    IMPORT_C static CGlxCommandHandlerAiwShowMapHardKey* NewL(MGlxMediaListProvider* 
            aMediaListProvider,
            TBool aHasToolbarItem);
     
    /**
    * Destructor.
    */
    ~CGlxCommandHandlerAiwShowMapHardKey();		

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
// Below methods are commented as they hold implemenatation of dofferent commandhandler
//  To check map service providers
//  To navigate to the location
//public: // New methods          
    
    /**
     * It will return total map service providers.
     * @since S60 v9.1
     * @ret total number of map service providers
     */
//    TInt MapServiceProvidersL();
    
    /**
     * It will show location on map
     * @since S60 v9.1
     */
//   IMPORT_C void ShowOnMapL( const TCoordinate& aCoordinate );
    
    /**
     * It will navigate to location
     * @since S60 v9.1
     */
//    void NavigateToPlaceL( const TCoordinate& aCoordinate );

private:
    /**
    * C++ default constructor.                 
    */
    CGlxCommandHandlerAiwShowMapHardKey(MGlxMediaListProvider* 
            aMediaListProvider,
            TBool aHasToolbarItem); 
    
    /**
    * Symbian 2nd phase constructor. 
    */
    void ConstructL();
            
private: // Data    	
    
    // ID of owning view 
    TInt iViewId;
    
    // Owns : Pointer to AIW service handler to send service commands     	
    CAiwServiceHandler*     iAiwServiceHandler;
    
    // Owns : Generic parameter input list     
    CAiwGenericParamList*   iInList;
    
    // Owns : Generic parameter output list    	     	
    CAiwGenericParamList*   iOutList;      
	};

#endif //C_GLXCOMMANDHANDLERAIWSHOWMAPHARDKEY_H

// End of file

