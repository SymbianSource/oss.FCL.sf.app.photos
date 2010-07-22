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
#include <qmap.h>

class GlxState;
class GlxViewManager;
class GlxMediaModel;
class GlxAlbumModel;
class QAbstractItemModel;
class GlxActionHandler;
class GlxTNObserver;

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
    /*
     * Constructor
     */
    GlxStateManager();
    
    /*
     * Destructor
     */
    virtual ~GlxStateManager();
    
    /*
     * Fuction to launch the application from some external world
     */
    void launchFromExternal();
    
    /*
     * Clean the all externel data
     */
    void cleanupExternal();
    
    /*
     *  Move the views to the multiple item selection state
     */
    void enterMarkingMode();
    
    /*
     * Exit from the multiselection state
     */
    void exitMarkingMode();
    
    /*
     * Execute the commant on multiple selected item
     */
    bool executeCommand(qint32 commandId);
    
    /*
     *  when application goes into background or come back to foreground
     *  set and reset the background thumbnail generation property
     */
    bool eventFilter(QObject *obj, QEvent *ev);    

signals :
    /*
     * Send the user activities ( command ) to the external world
     */
    void externalCommand(int cmdId);
    
    /*
     * TO send the signal to initialise the rest of items
     * which is not created in the launch sequence
     */
    void setupItemsSignal();
    
public slots:
    /*
     * To launch the application
     */
    void launchApplication();
    
    /*
     * To handle the user action, view switching etc
     */
    void actionTriggered(qint32 id);
    
    /*
     * To create the items which is not created in the aluch sequence
     */
    void setupItems();
    
    /*
     * call back function to monitor the change in thumbnail manager
     */
    void updateTNProgress( int count);
    void saveData();

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
    
    /*
     * It will delete the all model used by state manager
     */
    void cleanAllModel();
    

private:

    /*
     *  Launch the harvesting and TN generation progress bar dialog
     */
    void launchProgressDialog();
    
    /*
     * remove the harvesting and TN generation progress bar dialog
     */
    void vanishProgressDialog();
    
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

    /*
     * To set the fullscreen context based on the currrent orientation
     */    
    void setFullScreenContext();
    
    /*
     * Apllication event handler function
     */
    void eventHandler(qint32 &id);
    
    /*
     * A function to care the exit for application, in the case when application launch from internal and external
     */        
    void exitApplication();

    /*Launch Application as an acitivyt.
     * Return Value @0 : If launching an activity fails
     *              @1 : If launch activity passes
     */
     bool launchActivity();
private:
    GlxViewManager      *mViewManager;
    GlxMediaModel       *mAllMediaModel;        // for all grid
    GlxMediaModel       *mAlbumGridMediaModel;  // for album grid 
    GlxAlbumModel       *mAlbumMediaModel;      // for album list
	GlxMediaModel       *mImageviewerMediaModel;// for image viewer 
    QAbstractItemModel  *mCurrentModel;         // no owner ship
    GlxState            *mCurrentState;		
    GlxActionHandler    *mActionHandler;
    GlxTNObserver       *mTNObserver;
    int                 mCollectionId;
    bool                isProgressbarRunning;
    QMap<QString, qint32> mSaveActivity;
};


#endif /* STATEHANDLER_H_ */
