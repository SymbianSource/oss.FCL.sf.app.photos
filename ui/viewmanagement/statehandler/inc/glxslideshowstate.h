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

#ifndef GLXSLIDESHOWSTATE_H
#define GLXSLIDESHOWSTATE_H

#include <glxbasestate.h>

typedef enum 
{
    NO_SLIDESHOW_S,
    SLIDESHOW_GRID_ITEM_S,
    SLIDESHOW_ALBUM_ITEM_S,
} SlideShowState;

class GlxStateManager;

class GlxSlideShowState : public GlxState
{
public :
	GlxSlideShowState(GlxStateManager *stateManager, GlxState *preState = NULL);
	int state() const { return (int) mState; }
	void setState(int internalState) { mState = (SlideShowState) internalState; }
	void eventHandler(qint32 &id);
	void albumItemEventHandler( qint32 &id );
	~GlxSlideShowState() {}	
	
private:
    //slide show internal state
    SlideShowState mState;
    GlxStateManager *mStateManager;
};

#endif /* GLXSLIDESHOWSTATE_H */
