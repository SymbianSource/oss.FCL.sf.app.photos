/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    Telephone call handler 
*
*/




#ifndef C_SHWTELEPHONEHANDLER_H
#define C_SHWTELEPHONEHANDLER_H

// INCLUDES
#include <e32base.h>
#include <etel3rdparty.h>

// FORWARD DECLARATIONS
class CShwSlideshowEngine;
class CShwSlideShowPauseHandler;

/**
 *  CShwTelephoneHandler
 *
 *  Telphone call handler
 *
 *  @lib shwslideshowviewplugin.lib
 */
NONSHARABLE_CLASS( CShwTelephoneHandler ) : public CActive
    {
public:
	/**
	 * Factory function constructor
	 * @param aEngine to pause and resume the slideshow
	 */
	static CShwTelephoneHandler* NewL(
	    CShwSlideshowEngine& aEngine, 
	    CShwSlideShowPauseHandler& aPauseHandler );
	
	/**
	 * Destructor
	 */
    ~CShwTelephoneHandler();
 
public:
	/**
	 * Start the handler
	 */
    void StartL();
    
private:
	/**
	 * Constructor
	 * @param aEngine to pause and resume the slideshow
	 */
    CShwTelephoneHandler(
        CShwSlideshowEngine& aEngine,
        CShwSlideShowPauseHandler& aPauseHandler );
    
	/**
	 * 2nd phase construction
	 */
    void ConstructL();

private:
    // from CActive
    void RunL();
    void DoCancel();
    
private:

    /// Ref: slide show engine
    CShwSlideshowEngine& iEngine;
    /// Ref: slide show pause handler
    CShwSlideShowPauseHandler& iPauseHandler;

    CTelephony* iTelephony; // owned
    
    // line status    
    CTelephony::TCallStatusV1 iLineStatus; // owned
    CTelephony::TCallStatusV1Pckg iLineStatusPckg; // owned
    };

#endif // C_SHWTELEPHONEHANDLER_H
