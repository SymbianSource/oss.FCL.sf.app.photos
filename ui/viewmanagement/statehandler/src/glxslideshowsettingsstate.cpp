/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   ?Description
*
*/


#include <glxslideshowsettingsstate.h>
#include <glxstatemanager.h>
#include <glxcommandhandlers.hrh>


GlxSlideShowSettingsState::GlxSlideShowSettingsState(GlxStateManager *stateManager, GlxState *preState ) : GlxState(GLX_SLIDESHOWSETTINGSVIEW_ID, preState)
{
     if ( preState)
         qDebug("GlxSlideShowSettingsState::GlxSlideShowSettingsState() state id = %d", preState->id());
     mStateManager = stateManager ;
}

void GlxSlideShowSettingsState::eventHandler(qint32 &id)
{      
    qDebug("GlxSlideShowSettingsState::eventHandler() action id = %d", id);
		return;
}
