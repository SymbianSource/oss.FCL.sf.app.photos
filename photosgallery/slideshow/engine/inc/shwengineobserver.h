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
* Description:    The event observer interface
 *
*/




#ifndef __MSHWENGINEOBSERVER_H__
#define __MSHWENGINEOBSERVER_H__

// INCLUDES
#include <e32std.h>
#include <e32base.h>

// CLASS DECLARATION

/**
 *  MShwEventObserver
 */
class MShwEngineObserver
	{
	protected:
	
		/**
		 * Destructor. Dont allow deleting objects through this interface.
		 */
		virtual ~MShwEngineObserver() {};

	public: // the API

		/**
		 * This notification is called once the engine is started,
		 * meaning that the slideshow is running.
		 */
		virtual void EngineStartedL() = 0;

		/**
		 * This notification is called once the engine is paused,
		 * meaning that the slideshow is frozen.
		 */
		virtual void EnginePausedL() = 0;

		/**
		 * This notification is called once the engine is resumed,
		 * meaning that the slideshow is re-running after a pause
		 */
		virtual void EngineResumedL() = 0;
		
		/**
         * This notification is called once the selection key, EngineLSKPressedL
         * pressed.
         */
        virtual void EngineToggleUiStateL() = 0;
        
        /**
         * LSk pressed.
         */
        virtual void EngineLSKPressedL() = 0;

        /**
         * This notification is called when there is such an error during 
         * slideshow that it cannot continue, for example all items got removed.
         * This method is allowed to leave, in which case slideshow engine panics
         * with EEngineFatalError. This is ok as in case of error all we can do is
         * try to exit slideshow, either easy way or hard way :)
         * @exception EEngineFatalError if this method does leave.
         */
        virtual void ErrorDuringSlideshowL() = 0;

	};

#endif // __MSHWENGINEOBSERVER_H__
