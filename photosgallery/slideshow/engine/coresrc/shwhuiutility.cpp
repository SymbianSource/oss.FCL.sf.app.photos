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
* Description:    Utility that contains HUI related slideshow code
 *
*/




// INCLUDES
#include "shwhuiutility.h"
#include <alf/alfroster.h>

// DEPENDENCIES
#include <alf/alfdisplay.h>
#include <alf/alfcontrolgroup.h>
#include <mglxvisuallist.h>
#include <glxtracer.h>
#include <glxlog.h>
#include "shwslideshowenginepanic.h"

// -----------------------------------------------------------------------------
// ShowVisualListL.
// -----------------------------------------------------------------------------
void ShwUiUtility::ShowVisualListL( CAlfDisplay* aDisplay, MGlxVisualList* aVisualList )
	{
	TRACER("ShwUiUtility::ShowVisualListL");
	GLX_LOG_INFO("ShwUiUtility::ShowVisualListL");
	// make sure we always got HUI display and the visual list
	__ASSERT_ALWAYS( aDisplay, NShwEngine::Panic( NShwEngine::ENullHuiDisplay ) );
	__ASSERT_ALWAYS( aVisualList, NShwEngine::Panic( NShwEngine::ENullVisualList ) );
	// get the control group
	CAlfControlGroup* controlGroup = aVisualList->ControlGroup();
	// and show it using the displays roster
	aDisplay->Roster().ShowL( *controlGroup ,KAlfRosterShowAtBottom);
	}

