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
* Description:    The base class for event publishers
 *
*/




#include "shweventpublisherbase.h"
#include "shweventqueue.h"

#include <glxlog.h>
#include <glxtracer.h>

// -----------------------------------------------------------------------------
// C++ Constructor.
// -----------------------------------------------------------------------------
CShwEventPublisherBase::CShwEventPublisherBase()
	{
	// No implementation required
	}

// -----------------------------------------------------------------------------
// Destructor.
// -----------------------------------------------------------------------------
CShwEventPublisherBase::~CShwEventPublisherBase()
	{
	// No implementation needed
	}

// -----------------------------------------------------------------------------
// SendEventL.
// -----------------------------------------------------------------------------
void CShwEventPublisherBase::SendEventL( MShwEvent* aEvent )
	{
	__ASSERT_DEBUG( iQueue, User::Panic( _L("Null event Queue"), 1 ) );
	this->iQueue->SendEventL( aEvent );
	}

// -----------------------------------------------------------------------------
// SetEventQueue.
// -----------------------------------------------------------------------------
void CShwEventPublisherBase::SetEventQueue( MShwEventQueue* aQueue )
	{
	TRACER("CShwEventPublisherBase::SetEventQueue");
	GLX_LOG_INFO("CShwEventPublisherBase::SetEventQueue");
	this->iQueue = aQueue;
	}


