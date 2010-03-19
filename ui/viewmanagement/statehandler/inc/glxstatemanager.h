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




#ifndef STATEHANDLER_H
#define STATEHANDLER_H

#include <QObject>
#include <QList>
#include <glxbasestate.h>

class GlxState;
class GlxViewManager;
class GlxMediaModel;
class GlxAlbumModel;
class QAbstractItemModel;
class GlxActionHandler;

#ifdef BUILD_STATEMANAGER
#define GLX_STATEMANAGER_EXPORT Q_DECL_EXPORT
#else
#define GLX_STATEMANAGER_EXPORT Q_DECL_IMPORT
#endif

class GLX_STATEMANAGER_EXPORT GlxStateManager : public QObject
{
    Q_OBJECT
	friend class TestGlxStateManager;
	
public :
    GlxStateManager();
    virtual ~GlxStateManager();
    void launchFromExternal();
    void cleanupExternal();
    
    //internal exposed function
    void enterMarkingMode();
    void exitMarkingMode();
    bool executeCommand(qint32 commandId);

signals :    
    void externalCommand(int cmdId);
    void setupItemsSignal();
    
public slots:
    void launchApplication();
    void actionTriggered(qint32 id);
    void setupItems();

public :
/*
 * It will create a new state and replace the current state with new state in the stack.
 * It will use in the case of state switching.
 * use -1 if state does not have internal state
 */    
    void changeState(qint32 newStateId, int internalState );
/*
 * Go back to previous state 
 */    
    void previousState();

/*
 * Go back to a state in hierarchy and pop all the state upto that level.
 * if state is not found, then all the state from the hierachy is removed and create a new state on level 0
 * This function will be use in the case when more then one back is required in single event.
 */
    void goBack(qint32 stateId, int internalState);
       
/*
 * Create a new state and current state should be pushed into stack.
 * use -1 if state does not have internal state
 */  
    void nextState(qint32 newStateId, int internalState );

/*
 * It will delete the current model
 */      
    void removeCurrentModel();
    

private:
/*
 * Factory function to create the state.
 */  
    GlxState * createState(qint32 state);
/*
 * Factory function to create the model.
 */  	
    void createModel(qint32 stateId, NavigationDir dir = NO_DIR);
/*
 * Factory function to create the grid model.
 */  	
    void createGridModel(int internalState, NavigationDir dir = NO_DIR);
    void eventHandler(qint32 &id);
/*
 * A function to care the exit for application, in the case when application launch from internal and external
 */        
    void exitApplication();

private:
    GlxViewManager      *mViewManager;
    GlxMediaModel       *mAllMediaModel;        // for all grid
    GlxMediaModel       *mAlbumGridMediaModel;  // for album grid 
    GlxAlbumModel       *mAlbumMediaModel;      // for album list
	GlxMediaModel       *mImageviewerMediaModel;// for image viewer 
    QAbstractItemModel  *mCurrentModel;         // no owner ship
    GlxState            *mCurrentState;		
    GlxActionHandler    *mActionHandler;
    int                 mCollectionId;
};


#endif /* STATEHANDLER_H_ */
