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
* Description:    Handles & propogates UI state change notifications.
*
*/


#ifndef CGLXACTIVE_CALLBACK_H
#define CGLXACTIVE_CALLBACK_H

// INCLUDES
#include <aknapp.h>

// CLASS DECLARATION

class CGlxActiveCallBack : public CActive
    { 
    public:
        CGlxActiveCallBack( TCallBack aCallBack, TInt aPriority): CActive(aPriority), 
                                                                  iCallBack(aCallBack) {};
        CGlxActiveCallBack();
    	
    public: // Functions from base classes
		void SetActive() { CActive::SetActive();}; 
		void RunL() { iCallBack.CallBack();};
 		void DoCancel() {};
 		
   	private: // Data
		TCallBack iCallBack;
    };

#endif // CGLXACTIVE_CALLBACK_H

// End of File

