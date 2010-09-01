/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    The event publisher interface
 *
*/




#ifndef __MSHWEVENTPUBLISHER_H__
#define __MSHWEVENTPUBLISHER_H__

// INCLUDES
#include <e32std.h>
#include <e32base.h>

// Forward declarations
class MShwEventQueue;

// CLASS DECLARATION

/**
 * MShwEventPublisher
 * @author Kimmo Hoikka
 */
class MShwEventPublisher
    {
    protected:

        /**
         * Destructor. Dont allow deleting objects through this interface.
         */
        virtual ~MShwEventPublisher() {};

    public: // the API

        /**
         * Set the event queue.
         * @param aQueue, the event queue where to send the events.
         */
        virtual void SetEventQueue( MShwEventQueue* aQueue ) = 0;

    };

#endif // __MSHWEVENTPUBLISHER_H__
