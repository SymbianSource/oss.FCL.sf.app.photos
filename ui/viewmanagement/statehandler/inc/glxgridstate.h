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



#ifndef GLXGRIDSTATE_H
#define GLXGRIDSTATE_H

#include <glxbasestate.h>

class GlxStateManager;

typedef enum 
{
    NO_GRID_S,
    ALL_ITEM_S,
    ALBUM_ITEM_S,
} GridState;

class GlxGridState : public GlxState
{
public :
	GlxGridState(GlxStateManager *stateManager, GlxState *preState = NULL);
	int state() const { return (int) mState; }
	void setState(int internalState) { mState = (GridState) internalState; }
	void eventHandler(qint32 &id);
	void setTranstionParameter(NavigationDir dir, GlxEffect &effect, GlxViewEffect &viewEffect);
	
signals :    

public slots:

protected:
	
private slots:

private:
	void defaultEventHandler ( qint32 &id );
	void allItemEventHandler ( qint32 &id );
	void albumItemEventHandler ( qint32 &id );

private:
	//grid internal state
	GridState mState; 	
	GlxStateManager *mStateManager;
	bool mIsMarkingMode;
	qint32 mCommandId;  //commandID, save the command before entering into the marking mode for next user interaction
};


#endif /* GLXGRIDSTATE_H_ */
