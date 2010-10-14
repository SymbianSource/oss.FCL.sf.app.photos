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



#ifndef GLXVIEWMANAGER_H
#define GLXVIEWMANAGER_H

#include <QObject>
#include <QList>
#include <hbeffect.h>
#include "glxuistd.h"
#include <QModelIndex>

class GlxView;
class HbMainWindow;
class QAbstractItemModel;
class HbAction;
class GlxMenuManager;
class HbToolBar;
class HbAction;
class QItemSelectionModel;
class HbMenu;
class GlxEffectEngine;
class HbProgressDialog;
class GlxMainWindowEventFilter;

#ifdef BUILD_VIEWMANAGER
#define GLX_VIEWMANAGER_EXPORT Q_DECL_EXPORT
#else
#define GLX_VIEWMANAGER_EXPORT Q_DECL_IMPORT
#endif

typedef enum
{
    NO_ACTION_ID           = 0x00,
    GLX_ALL_ACTION_ID      = 0x01,
    GLX_ALBUM_ACTION_ID    = 0x02,
    GLX_CAMERA_ACTION_ID   = 0x04,
    GLX_OVI_ACTION_ID      = 0x08,
    GLX_ALL_ID             = 0xFF
} glxToolBarActionIds;

/**
 * view manager class
 */
class GLX_VIEWMANAGER_EXPORT GlxViewManager : public QObject
{
Q_OBJECT

public :
    /**
     * Constructor
     */
    GlxViewManager();
    
    /**
     * Destructor
     */
    ~GlxViewManager();

    /**
     * setupItems() - For initial setup of application after view is ready for drawing
     */
    void setupItems( );

    /**
     * launchApplication() - Api to launch the photos application
     * @param id viewId
     * @param model model to be used for the view
     */
    void launchApplication( qint32 id, QAbstractItemModel *model );

    /**
     * addBackSoftKeyAction() - add back key action
     */
    void addBackSoftKeyAction();

    /**
     * orientation() - This will return the current orientation of device
     */    
    Qt::Orientation orientation() const;
    
    /**
     * This function is used to launch the view
     * @param view id
     * @param pointer of data model
     */
    void launchView ( qint32 id, QAbstractItemModel *model );

    /**
     * It is over load slot and used to run the animation for view transition and launch the view
     * @param view id
     * @param pointer of data model
     * @param 
     * @param View transtion id
     * @param to play the animation on which views
     */    
    void launchView ( qint32 id, QAbstractItemModel *model, GlxEffect effect, GlxViewEffect viewEffect );
    
    /**
     * launchProgressDialog() - To launch the refreshing media progress dialog
     * @param - number of item left to populate
     */
    void launchProgressDialog( int maxValue );
    
    /**
     * updateProgressDialog() - To update the refreshing media progress dialog value
     * @param - number of item left to populate
     */
    void updateProgressDialog( int currentValue );

    /**
     * deactivateCurrentView() - This will deactivate the current function
     * to be used only in cases where External launch was done
     */     
    void deactivateCurrentView();

    /**
     *  updateToolBarIcon() -To update the tool bar enable and disable icon
     *  @parma This should be selected toolbar tab id
     */    
    void updateToolBarIcon( int id );
	
    /**
     *  updateToolBarActionState() - To update the tool bar action check state
     *  @param This should be selected toolbar tab id
     *  @param action check state
     */        
    void updateToolBarActionState( int id, bool isChecked );	

    /**
     * enterMarkingMode() - Enable the marking mode of the view to select multiple item
     * @param view id
     * @parma command id
     */    
    void enterMarkingMode( qint32 viewId, qint32 commandId );

    /**
     * exitMarkingMode() - Enable the normal mode of the view
     * @param view id
     */    
    void exitMarkingMode( qint32 viewId );

    /**
     * handleUserAction() - Pass the user action to the view to process action
     * @param view id
     * @parma user action id ( command id )
     */    
    void handleUserAction( qint32 viewId, qint32 commandId );

    /**
     *  getSelectionModel() Return the selection model to the user
     *  @param - view id of the current view
     *  @return - selection model of the view 
     */    
    QItemSelectionModel * getSelectionModel( qint32 viewId );

    /**
     * setModel() - To set the model of current view
     * @param model pointer
     */
    void setModel( QAbstractItemModel *model );
    
    /**
     * cancelViewTransitionEffect() - To cancel the current runing view transition
     */
    void cancelViewTransitionEffect();
	    
signals :
    /**
     * actionTriggered() - emit the signal of user action for state manager
     * @param action or command id
     */
    void actionTriggered( qint32 id );

public slots:
    
    /**
     * destroyView() - It will removed and deleted the view form the view list.
     * Currently It is not used so may be in future, It will be removed.
     * @param - view id of view which is suppose to destroy
     */    
    void destroyView ( qint32 id );

    /**
     * actionProcess() - It will pass the user action to the state manager
     * @param action id to process
     */    
    void actionProcess( qint32 id );

    /**
     *  handleAction() - It will pass the user action ( tool bar + back ) to state manager
     */    
    void handleAction();

    /**
     *  effectFinished() -This is slot used for the transition animation finished call back
     */    
    void effectFinished( );

    /**
     *  itemSpecificMenuTriggered() - This will open the item specifc Menu
     *  @parma - view id 
     *  @param - postion of the context menu
     */    
    void itemSpecificMenuTriggered( qint32, QPointF );
    
    void handleReadyView();
	
private slots:
   /**
    * checkMarked() -  cheche the some item is marked or no item is marked
    */
   void checkMarked();
   
   /**
    * hideProgressDialog() - put the application into background
    */
   void hideProgressDialog();
   
private:
    /**
     * resolveView() - It will create and return the view
     * @parma - view id
     * @return the object of desired view or create the view if it is not created and return
     */
    GlxView * resolveView ( qint32 id );

    /**
     * findView() - It will find a view from the view list and return it
     * @parma - view id
     * @return the object of desired view or null
     */    
    GlxView * findView ( qint32 id );

    /**
     * deActivateView() - It will deativate the current view
     */    
    void deActivateView();

    /**
     *  It will activate and show the view
     */
    void activateView();

    /**
     * activateView() - It will create the grid and list view tool bar action
     */    
    void createActions();

    /**
     * createMarkingModeActions() - It will create the marking mode toll bar action
     */    
    void createMarkingModeActions(); 

    /**
     * createToolBar() - It will create the grid and list view tool bar
     */    
    void createToolBar();

    /**
     * createMarkingModeToolBar() - It will create the marking mode tool bar
     */    
    void createMarkingModeToolBar();
    
    /**
     * setMarkingToolBarAction() - set the toolbar action text
     * @parma command id
     */
    void setMarkingToolBarAction( qint32 commandId );

    /**
     * addConnection() -  It will add all the view manager related connection
     */    
    void addConnection();

    /**
     * removeConnection() - It will remove all the view manager releted connection
     */    
    void removeConnection();
    
    /**
     * getSubState() - return the substate of state
     */    
    int getSubState();

private:
    QList<GlxView *>            mViewList;  //It contains all the view created by it self.
    HbMainWindow                *mMainWindow; //main window pointer, It have ownership only if 
    HbAction                    *mBackAction; // For back soft key
    GlxMenuManager              *mMenuManager; //Pointer of menu manger to add the menu in the view 
    GlxEffectEngine             *mEffectEngine; //To run the animation in between view transition
    QList<HbAction *>           mActionList; //Tool bar action list
    QList<HbAction *>           mMarkingActionList; //marking mode tool bar action list
    HbToolBar                   *mViewToolBar; //view tool bar
    HbToolBar                   *mMarkingToolBar; //marking mode tool bar
    GlxView                     *mView;
    QAbstractItemModel          *mModel; //no ownership
    QItemSelectionModel         *mSelectionModel;
    HbProgressDialog            *mProgressDialog;
    GlxMainWindowEventFilter    *mWindowEventFilter;
    bool                        mIsViewTransitionRunning;
    GlxEffect                   mEffect;
    QList< QGraphicsItem * >    mItemList;
};


#endif /* GLXVIEWMANAGER_H_ */
