/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    The view key handling control for the slideshow
 *
*/




#ifndef __CSHWSLIDESHOWKEYHANDLER_H__
#define __CSHWSLIDESHOWKEYHANDLER_H__

// INCLUDES
#include <coecntrl.h>
#include <glxuiutility.h>

// FORWARD DECLARATIONS
class CShwSlideshowEngine;
class CShwSlideShowPauseHandler;

// CLASS DECLARATION

/**
 *  CShwSlideShowKeyHandler
 * @internal reviewed 07/06/2007 by Kimmo Hoikka
 */
NONSHARABLE_CLASS( CShwSlideShowKeyHandler ) : public CCoeControl
    {
    public: // Constructors and destructor

        /**
         * Constructor.
         * @param reference to the slide show engine
         * @param reference to the pause handler
         * @param reference to the screen furniture
         * @param the slideshow view's id
         */
         static CShwSlideShowKeyHandler* NewL( 
            CShwSlideshowEngine& aEngine,
            CShwSlideShowPauseHandler& aPauseHandler,
            TInt& aState  );

        /**
         * Destructor.
         */
        ~CShwSlideShowKeyHandler();

    private:

        /**
         * Constructor
         * @ref CShwSlideShowKeyHandler::NewL
         */
         CShwSlideShowKeyHandler(
            CShwSlideshowEngine& aEngine,
            CShwSlideShowPauseHandler& aPauseHandler,
            TInt& aState  );
         /**
          * ConstructL
          * @ref CShwSlideShowKeyHandler::ConstructL
          */
         void ConstructL();

    public: // From CCoeControl

        /**
         * @ref CCoeControl::OfferKeyEventL
         */	
        TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);

    private: // Implementation

        // Ref: The slideshow engine
        CShwSlideshowEngine& iEngine;
        
        /// Ref: the slideshow pause handler
        CShwSlideShowPauseHandler& iPauseHandler;
        
        CGlxUiUtility* iUiUtility;
        
        /// The slideshow view's id
        TInt& iState ;
    };

#endif // __CSHWSLIDESHOWKEYHANDLER_H__

