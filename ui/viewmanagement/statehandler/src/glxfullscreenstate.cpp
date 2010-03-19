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



#include <glxfullscreenstate.h>

GlxFullScreenState::GlxFullScreenState(GlxState *preState) : GlxState(GLX_FULLSCREENVIEW_ID, preState)
{
	
}

void GlxFullScreenState::eventHandler(qint32 &id)
{
    Q_UNUSED(id);	
}

void GlxFullScreenState::setTranstionParameter(NavigationDir dir, GlxEffect &effect, GlxViewEffect &viewEffect)
{
    GlxState *preState = previousState();
    effect = NO_EFFECT ;
    viewEffect = NO_VIEW ;
    
    if ( preState == NULL ) {
        return ;        
    }
    
    if ( dir == FORWARD_DIR ){
         if ( preState->compare( GLX_GRIDVIEW_ID ) ) {
             effect = GRID_TO_FULLSCREEN ;
             viewEffect = BOTH_VIEW ;
         }
    }    
    else if ( dir == BACKWARD_DIR ) {
        if ( preState->compare( GLX_GRIDVIEW_ID ) ) {
            effect = FULLSCREEN_TO_GRID;
            viewEffect = LAUNCH_VIEW;
        }       
    }    
}

