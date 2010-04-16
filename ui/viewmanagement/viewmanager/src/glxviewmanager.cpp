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



#include <glxviewmanager.h>
#include <glxview.h>
#include "glxicondefs.h" //Contains the icon names/Ids
#include <glxviewsfactory.h>
#include <glxeffectengine.h>
#include <hbmainwindow.h>
#include <glxexternalutility.h>
#include <glxloggerenabler.h>
#include <glxmenumanager.h>
#include <glxcommandhandlers.hrh>
#include <glxplugincommandid.hrh>

#include <hbaction.h>
#include <hbtoolbar.h>
#include <hbmenu.h>
#include <QDebug>
#include <hbstyleloader.h>


GlxViewManager::GlxViewManager() 
    : mBackAction( NULL ), 
      mMenuManager( NULL ), 
      mEffectEngine( NULL ), 
      mViewToolBar( NULL ), 
      mMarkingToolBar( NULL ), 
      mMenu( NULL )
{
    qDebug("GlxViewManager::GlxViewManager() ");
    PERFORMANCE_ADV ( viewMgrD1, "main window creation time" ) {
        //check the case when application launch through some other application (view plugin)
        mMainWindow = GlxExternalUtility::instance()->getMainWindow();
        if(mMainWindow == NULL)	{
            mMainWindow = new HbMainWindow();
        }
		//Without this Zoom Does not work
		mMainWindow->viewport()->grabGesture(Qt::PinchGesture);
    }
    HbStyleLoader::registerFilePath(":/data/photos.css");
}

void GlxViewManager::setupItems(int subState)
{
    mMenuManager = new GlxMenuManager(mMainWindow);
    addBackSoftKeyAction(); 
    createToolBar();
    addConnection();
    mView->addToolBar(mViewToolBar);
    mMenuManager->addMenu( mView->viewId(), mView->menu() );
    mMenuManager->setModel( mModel );
}

void GlxViewManager::launchApplication(qint32 id, QAbstractItemModel *model)
{
    mModel = model;
    PERFORMANCE_ADV ( viewMgrD1, "View Creation time" ) {
        mView = resolveView(id);
    }
    mView->activate();
    
    PERFORMANCE_ADV ( viewMgrD3, "Set Model time")  
        mView->setModel(mModel);
        
    PERFORMANCE_ADV( viewMgrD4, "View Display time") {
        mMainWindow->setCurrentView(mView, false);
        mMainWindow->showFullScreen();           
    }    
}

void GlxViewManager::handleMenuAction(qint32 commandId)
{
    emit actionTriggered(commandId);
}

void GlxViewManager::handleAction()
{
    HbAction *action = qobject_cast<HbAction*>(sender());
    qint32 commandId = action->data().toInt();
    emit actionTriggered(commandId);
}

void GlxViewManager::addBackSoftKeyAction ( )
{
    qDebug("GlxViewManager::addBackSoftKeyAction ");
    //create the back soft key action and set the data
    mBackAction = new HbAction(Hb::BackNaviAction, this);
    mBackAction->setData(EGlxCmdBack);
    mView->setNavigationAction(mBackAction);
}

Qt::Orientation GlxViewManager::orientation() const
{
    return mMainWindow->orientation();
}

void GlxViewManager::launchView(qint32 id, QAbstractItemModel *model)
{
    qDebug("GlxViewManager::launchView Id = %d ", id);
    mModel = model;
    deActivateView();
    
    PERFORMANCE_ADV ( viewMgrD1, "View Creation time" ) {
        mView = resolveView(id);
    }
    
    activateView();
}

void GlxViewManager::launchView (qint32 id, QAbstractItemModel *model, GlxEffect effect, GlxViewEffect viewEffect)
{
    qDebug("GlxViewManager::launchView Id = %d effect %d view effect %d", id, effect, viewEffect);

    //In the case of no animation is play during the view transition just call launch view and return
    if ( viewEffect == NO_VIEW ) {
        return launchView(id, model);
    }
    
    //create and registered the effect
    if ( mEffectEngine == NULL ) { 
        mEffectEngine = new GlxSlideShowEffectEngine();
        mEffectEngine->registerTransitionEffect();
        connect( mEffectEngine, SIGNAL( effectFinished() ), this, SLOT( effectFinished() ) );
    }
    
    QList< QGraphicsItem * > itemList;
    QGraphicsItem *item = NULL;
    itemList.clear();
    
    //partially clean the view so that animation run smoothly
    GlxView *curr_view = (GlxView *) mMainWindow->currentView();
    curr_view->resetView();
    
    mView = resolveView(id);
    //partially initialise the view so that animation run smoothly
    mView->initializeView( model);
    mModel = model; 

    if ( viewEffect == CURRENT_VIEW || viewEffect == BOTH_VIEW ) { 
        item = curr_view->getAnimationItem(effect);
        if ( item ) {
            itemList.append(item);
            item = NULL;
        }
    }
    
    if ( viewEffect == LAUNCH_VIEW || viewEffect == BOTH_VIEW ) {
        item = mView->getAnimationItem(effect);
        if ( item ) {
            //increase the z value and show the view to shown the view animation
            mView->setZValue(curr_view->zValue() + 2);
            mView->show();
            item->show();        
            itemList.append(item);
        }
    }
    
    //error check
    if ( itemList.count() > 0 ) {
        mEffectEngine->runEffect(itemList, effect);
        mMainWindow->grabMouse();
    }
    else {
        deActivateView();
        activateView();       
    }    
}

//to be called only when the photos plugin was activated by external means
void GlxViewManager::deactivateCurrentView()
{
    GlxView *curr_view = (GlxView *) mMainWindow->currentView();
    if ( curr_view ) {
        curr_view->deActivate() ;
    }
}

void GlxViewManager::updateToolBarIcon(int id)
{
    QString path;
    int toolBarActionId = (int) GLX_ALL_ACTION_ID;
    int count = mActionList.count();
    
    qDebug("GlxViewManager::updateToolBarIcon() action ID list %d count %d", id, count);
    
    for ( int i = 0; i < count ; i++ )
        {
        qDebug("GlxViewManager::updateToolBarIcon() toolBarActionId %d value %d", toolBarActionId, ( id & toolBarActionId ) );
        //check and get the icon path
        if ( ( id & toolBarActionId ) == toolBarActionId )
            {
            mActionList[i]->setCheckable(TRUE);
            mActionList[i]->setChecked(TRUE);                        
            }
        else 
            {
            mActionList[i]->setChecked(FALSE);
            }
        //to get it the next action id to verify it is selecter or not
        toolBarActionId = toolBarActionId << 1; 
        }
}

void GlxViewManager::enterMarkingMode(qint32 viewId)
{
    GlxView *view = findView ( viewId );
    qDebug("GlxViewManager::enterMarkingMode view ID %d", viewId);
    
    //In the case of first time create the marking mode menu( Mark All, Un Mark All )
    if( mMenu == NULL ) {
        mMenu = new HbMenu();
        mMenuManager->createMarkingModeMenu(mMenu);
    }
    
    if ( mMarkingToolBar == NULL) {
        createMarkingModeToolBar(); //Marking mode tool bar is different from normal mode tool bar
    }
    
    if ( view ) { 
        view->enableMarking();
        HbMenu *menu = view->takeMenu(); //Take the owner ship of current menu
        view->setMenu(mMenu); //Set the marking mode menu
        mMenu = menu;
        view->takeToolBar();
        view->addToolBar(mMarkingToolBar);
    }
    qDebug("GlxViewManager::enterMarkingMode view ID %d exit", viewId);
}

void GlxViewManager::exitMarkingMode(qint32 viewId)
{
    GlxView *view = findView ( viewId );
    qDebug("GlxViewManager::exitMarkingMode view ID %d", viewId);
    if ( view ) { 
        view->disableMarking(); 
        HbMenu *menu = view->takeMenu(); //Take the owner ship of current menu
        view->setMenu(mMenu); //Set the view menu option
        mMenu = menu;
        view->takeToolBar();
        view->addToolBar(mViewToolBar);
    }
    qDebug("GlxViewManager::exitMarkingMode view ID %d exit", viewId);
}

void GlxViewManager::handleUserAction(qint32 viewId, qint32 commandId)
{
    GlxView *view = findView ( viewId );
    qDebug("GlxViewManager::handleUserAction view ID %d command id %d", viewId, commandId);
    if ( view ) { 
        view->handleUserAction(commandId);
    }    
}

QItemSelectionModel *  GlxViewManager::getSelectionModel(qint32 viewId) 
{ 
    GlxView *view = findView ( viewId );
    if ( view ) { 
        return view->getSelectionModel();
    }
    return NULL;
}

GlxView * GlxViewManager::resolveView(qint32 id)
{
    qDebug("GlxViewManager::resolveView %d", id);
    GlxView *view = findView ( id );
    if ( view ) {
        return view ;
    }
    
    view = GlxViewsFactory::createView(id, mMainWindow);
    if ( view ) {
        connect ( view, SIGNAL(actionTriggered(qint32 )), this, SLOT(actionProcess(qint32 )), Qt::QueuedConnection );
        connect ( view, SIGNAL(itemSpecificMenuTriggered(qint32,QPointF ) ), this, SLOT( itemSpecificMenuTriggered(qint32,QPointF ) ), Qt::QueuedConnection );
        mViewList.append(view);
        mMainWindow->addView(view);
        if ( mMenuManager ) {
            mMenuManager->addMenu( id, view->menu());
        }
        view->setNavigationAction(mBackAction);
    }
    return view;
}

void GlxViewManager::itemSpecificMenuTriggered(qint32 viewId,QPointF pos)
{
    mMenuManager->ShowItemSpecificMenu(viewId,pos);
}

void GlxViewManager::cancelTimer()
{
    emit externalCommand(EGlxPluginCmdUserActivity);
}

void GlxViewManager::effectFinished( )
{
    qDebug("GlxViewManager::EffectFinished");
    mMainWindow->releaseMouse();
    deActivateView();
    activateView(); 
}

GlxView * GlxViewManager::findView(qint32 id)
{
    qDebug("GlxViewManager::findView Id = %d ", id);
    int count = mViewList.count();
    
    for ( int i = 0 ; i < count; i++) {
        if ( mViewList.at(i)->compare(id) ) {
            return mViewList.at(i);
        }
    }
    return NULL;
}

void GlxViewManager::deActivateView()
{
    qDebug("GlxViewManager::deActivateCurrentView()");
    
    GlxView *view = (GlxView *) mMainWindow->currentView();
    if ( view ){
        view->deActivate() ;
    }
    if (mView && view) {
        mView->setZValue(view->zValue());
    }
}

void GlxViewManager::activateView()
{
    qDebug("GlxViewManager::activateView()");
        
    PERFORMANCE_ADV ( viewMgrD2, "View Activation time") {
        mView->addToolBar(mViewToolBar);
        mView->activate();
        mView->show();
        mMenuManager->setModel( mModel ); //set the model to get the item type info and row count info
    }        
      
    PERFORMANCE_ADV ( viewMgrD3, "Set Model time")  
        mView->setModel(mModel);
        
    PERFORMANCE_ADV( viewMgrD4, "View Display time") {
        mMainWindow->setCurrentView(mView, false);
        mMainWindow->showFullScreen();           
    }
}

void GlxViewManager::createActions()
{
    qDebug("GlxViewManager::createActions() " );
    mActionList.clear();  
    
    //create the All tool bar button action
    HbAction* allAction = new HbAction(this);
    allAction->setData(EGlxCmdAllGridOpen);
    mActionList.append(allAction);    
    allAction->setIcon(HbIcon(GLXICON_ALL)) ;
       
    //create the Album tool bar button action
    HbAction* albumAction = new HbAction(this);
    albumAction->setData(EGlxCmdAlbumListOpen);
    mActionList.append(albumAction);
    albumAction->setIcon(HbIcon(GLXICON_ALBUMS)) ;
   
    //create the album tool bar button action
    HbAction* cameraAction = new HbAction(this);
    cameraAction->setData(EGlxCmdCameraOpen);
    mActionList.append(cameraAction);  
    cameraAction->setIcon(HbIcon(GLXICON_CAMERA)) ;
    
    
    //create the ovi tool bar button action
    HbAction* oviAction = new HbAction(this);
    oviAction->setData(EGlxCmdOviOpen);
    mActionList.append(oviAction);
    oviAction->setIcon(HbIcon(GLXICON_OVI)) ;

}

void GlxViewManager::createMarkingModeActions()
{
    mMarkingActionList.clear();
    
    //create the ok tool bar button action
    HbAction* selectAction = new HbAction("Ok", this);
    selectAction->setData(EGlxCmdSelect);
    mMarkingActionList.append(selectAction);
    connect( selectAction, SIGNAL(triggered( )), this, SLOT(handleAction( )), Qt::QueuedConnection );
    mMarkingToolBar->addAction( selectAction );
    
    //create the cancel tool bar button action
    HbAction* cancelAction = new HbAction("Cancel", this);
    cancelAction->setData(EGlxCmdCancel);
    mMarkingActionList.append(cancelAction); 
    connect( cancelAction, SIGNAL(triggered( )), this, SLOT(handleAction( )), Qt::QueuedConnection ); 
    mMarkingToolBar->addAction( cancelAction );
}


void GlxViewManager::createToolBar()
{
    qDebug("GlxViewManager::createToolBar() " );
    
    mViewToolBar = new HbToolBar();
    mViewToolBar->setOrientation( Qt::Horizontal );
    mViewToolBar->setVisible(true);
    mViewToolBar->clearActions();
    
    createActions();    
    
    qDebug("GlxViewManager::createToolBar() clear the action" );
    int count = mActionList.count();
    for ( int i = 0; i < count; i++ ) {
        connect( mActionList.at(i), SIGNAL(triggered( )), this, SLOT(handleAction( )) );
        mViewToolBar->addAction( mActionList.at(i) );      
    }
    qDebug("GlxViewManager::createToolBar() exit" );
}

void GlxViewManager::createMarkingModeToolBar()
{
    qDebug("GlxViewManager::createMarkingModeToolBar() " );
    mMarkingToolBar = new HbToolBar();
    mMarkingToolBar->setOrientation( Qt::Horizontal );
    mMarkingToolBar->setVisible(true);            
    mMarkingToolBar->clearActions();    
    createMarkingModeActions();
}

void GlxViewManager::addConnection()
{    
    if ( mMenuManager )
        connect(mMenuManager, SIGNAL( commandTriggered(qint32 ) ), this, SLOT( handleMenuAction(qint32 ) ));
    if ( mBackAction )
        connect(mBackAction, SIGNAL( triggered() ), this, SLOT( handleAction() ));
        
    if ( mEffectEngine )  {
        connect( mEffectEngine, SIGNAL( effectFinished() ), this, SLOT( effectFinished() ) );
    }        
}

void GlxViewManager::removeConnection()
{
    int count = mActionList.count();
    for ( int i = 0; i < count; i++ ) {
        disconnect( mActionList.at(i), SIGNAL(triggered( )), this, SLOT(handleAction( )) );  
    }
    
    count = mMarkingActionList.count();
    for ( int i = 0; i < count; i++ ) {
        disconnect( mMarkingActionList.at(i), SIGNAL(triggered( )), this, SLOT(handleAction( )) );  
    }
    
    count = mViewList.count();
    for ( int i = 0; i < count; i++ ) {
        disconnect ( mViewList.at(i), SIGNAL(actionTriggered(qint32 )), this, SLOT(actionProcess(qint32 )) );
        disconnect ( mViewList.at(i), SIGNAL(itemSpecificMenuTriggered(qint32, QPointF ) ), this, SLOT( itemSpecificMenuTriggered(qint32, QPointF ) ));
        mMenuManager->removeMenu( mViewList.at(i)->viewId(), mViewList.at(i)->menu() ) ;
    }
	   
    if ( mMenuManager )
        disconnect(mMenuManager, SIGNAL( commandTriggered(qint32 ) ), this, SLOT( handleMenuAction(qint32 ) ));
    
    if ( mBackAction )
        disconnect(mBackAction, SIGNAL( triggered() ), this, SLOT( handleAction() ));
        
    if ( mEffectEngine )  {
        disconnect( mEffectEngine, SIGNAL( effectFinished() ), this, SLOT( effectFinished() ) );
    }
}

void GlxViewManager::destroyView(qint32 id)
{
    qDebug("GlxViewManager::destroyView ");
    GlxView *view = findView ( id );
    mViewList.removeOne(view);
    delete view;
}

void GlxViewManager::actionProcess(qint32 id)
{
    qDebug("GlxViewManager::actionProcess action Id = %d ", id);
    emit actionTriggered(id);
}

GlxViewManager::~GlxViewManager()
{
    qDebug("GlxViewManager::~GlxViewManager");
    HbStyleLoader::unregisterFilePath(":/data/photos.css");
	
    removeConnection();
    delete mMenuManager;
    qDebug("GlxViewManager::~GlxViewManager deleted menu manager");
    
    while( mViewList.isEmpty( ) == FALSE){
        delete mViewList.takeLast() ;
    }
    qDebug("GlxViewManager::~GlxViewManager view deleted");
    
        
    delete mBackAction;
    delete mViewToolBar;
    delete mMarkingToolBar;
    delete mMenu;
             
    if ( mEffectEngine ) {
        mEffectEngine->deregistertransitionEffect();
        delete mEffectEngine;
    }
 
    if( mMainWindow != GlxExternalUtility::instance()->getMainWindow() ){
        qDebug("GlxViewManager::~GlxViewManager remove view");
        delete mMainWindow;
    }
    
    qDebug("GlxViewManager::~GlxViewManager Exit");
}

