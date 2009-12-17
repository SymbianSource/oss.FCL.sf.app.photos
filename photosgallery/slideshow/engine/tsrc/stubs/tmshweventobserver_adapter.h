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
* Description:   Stub file for observer of event
 *
*/




#ifndef __TMGLXVISUALLIST_ADAPTER_H__
#define __TMGLXVISUALLIST_ADAPTER_H__

//  EXTERNAL INCLUDES
#include "shwevent.h"
#include "shweventobserver.h"

//  INTERNAL INCLUDES

//  FORWARD DECLARATIONS

//	CLASS DEFINITION
class MMShwEventObserver_Observer
	{
	public:		// Enums

		enum TMShwEventObserverMethodId
			{
			E_NotCalled,
			E_void_NotifyL
			};

	public:     // Destructor
		
		/**
		 * Destructor
		 */
		virtual ~MMShwEventObserver_Observer() {};
		
	public:     // Abstract methods
	
		virtual void MShwEventObserver_MethodCalled( TMShwEventObserverMethodId aMethodId, MShwEvent* aEvent ) = 0;
	
	};

//  CLASS DEFINITION
/**
 * Adapter for event controllers
 * for testing purposes
 */
class TMShwEventObserver_Adapter
	: public MShwEventObserver
	{
	public:     // Constructors and destructors

		/**
		 * Construction
		 */
		TMShwEventObserver_Adapter( MMShwEventObserver_Observer* aObserver, MShwEventObserver* aAdaptee );

	public:	// from MShwEventObserver

		void NotifyL( MShwEvent* aEvent );

	private:    // Data

		MMShwEventObserver_Observer* iMMShwEventObserver_Observer;
		MShwEventObserver* iMShwEventObserver;

	};

#endif      //  __TMGLXVISUALLIST_ADAPTER_H__

// End of file
