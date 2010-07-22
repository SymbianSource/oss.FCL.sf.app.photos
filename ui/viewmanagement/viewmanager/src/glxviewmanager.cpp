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
#include "glxlocalisationstrings.h"

#include <hbaction.h>
#include <hbtoolbar.h>
#include <hbmenu.h>
#include <QDebug>
#include <hbstyleloader.h>
#include <hbprogressdialog.h>
#include <QItemSelectionModel>
#include <glxmainwindoweventfilter.h>
#include <xqsettingsmanager.h>
#include <xqsettingskey.h>


GlxViewManager::GlxViewManager() 
    : mBackAction( NULL ), 
      mMenuManager( NULL ), 
      mEffectEngine( NULL ), 
      mViewToolBar( NULL ), 
      mMarkingToolBar( NULL ), 
      mSelectionModel ( NULL ),
      mProgressDialog( NULL )
{
    qDebug("GlxViewManager::GlxViewManager() ");
    PERFORMANCE_ADV ( viewMgrD1, "main window creation time" ) {
        //check the case when application launch through some other application (view plugin)
        mMainWindow = GlxExternalUtility::instance()->getMainWindow();
        if(mMainWindow == NULL)	{
            mMainWindow = new HbMainWindow();
            connect(mMainWindow, SIGNAL( viewReady() ), this, SLOT( handleReadyView() ));
        }
		//Without this Zoom Does not work

		mWindowEventFilter = new GlxMainWindowEventFilter;
		mMainWindow->scene()->installEventFilter(mWindowEventFilter);
		mMainWindow->viewport()->setAttribute(Qt::WA_AcceptTouchEvents);
		mMainWindow->viewport()->grabGesture(Qt::PinchGesture);
    }
    HbStyleLoader::registerFilePath(":/data/photos.css");
}
void GlxViewManager::handleReadyView()
{
    emit actionTriggered( EGlxCmdSetupItem );
    emit applicationReady();
    disconnect(mMainWindow, SIGNAL( viewReady() ), this, SLOT( handleReadyView() ));
}

void GlxViewManager::setupItems( )
{
    mMenuManager = new GlxMenuManager(mMainWindow);
    addBackSoftKeyAction();    
    addConnection();    
    mMenuManager->addMenu( mView->viewId(), mView->menu() );
    mMenuManager->setModel( mModel );
}

void GlxViewManager::launchApplication(qint32 id, QAbstractItemModel *model)
{
    mModel = model;
    PERFORMANCE_ADV ( viewMgrD1, "View Creation time" ) {
        mView = resolveView(id);
    }
    createToolBar();
    mView->addToolBar(mViewToolBar);
    
    /* We are showing the toolBar before activating the 
     * view. This is done to avoid the animation effect seen otherwise 
     * when the tool bar comes up.
     * 
     * If animation Effect is not removed, it leads to flickering effect 
     * since we are creating a new tool bar..although a fake tool bar was 
     * already created on the splashscreen
     * 
     */
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
    mBackAction->setObjectName( "App Back" );
    mView->setNavigationAction( mBackAction );
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
        mEffectEngine = new GlxEffectEngine();
        mEffectEngine->registerTransitionEffect();
        connect( mEffectEngine, SIGNAL( effectFinished() ), this, SLOT( effectFinished() ), Qt::QueuedConnection );
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

void GlxViewManager::launchProgressDialog( int maxValue )
{
    if ( maxValue <= 0 ) {
        // TNM return the some error code ( negative value ) until it populated the count
		// To show progress dialog 10 is chossen
	     maxValue = 10;
		          
    }
    if ( mProgressDialog == NULL ) {
        mProgressDialog = new HbProgressDialog( HbProgressDialog::ProgressDialog );
        mProgressDialog->actions().at(0)->disconnect( SIGNAL( triggered() ) );
        connect ( mProgressDialog->actions().at(0), SIGNAL( triggered() ), this, SLOT( hideProgressDialog() ) );
        mProgressDialog->setMinimum( 0 );
    }
    mProgressDialog->setMaximum( maxValue );
    mProgressDialog->setProgressValue( 0 );
    mProgressDialog->setModal( true );
    mProgressDialog->actions().at(0)->setText( GLX_BUTTON_HIDE );
    mProgressDialog->open();
}

void GlxViewManager::hideProgressDialog( )
{
    mMainWindow->lower();
}

void GlxViewManager::updateProgressDialog( int currentValue )
{
    static int i = 0;
    HbIcon icon;
    User::ResetInactivityTime();  

    //To:Do temp code remove later
    if ( mProgressDialog ) {
        i = ++i % 10;
        switch ( i ) {
        case 0 :
            icon = HbIcon( QString(":/data/Image1.jpg") );
            break;
        case 1 :
            icon = HbIcon( QString(":/data/Image2.jpg") );
            break;
        case 2 :
            icon = HbIcon( QString(":/data/Image3.jpg") );
            break;
        case 3 :
            icon = HbIcon( QString(":/data/Image4.jpg") );
            break;
        case 4 :
            icon = HbIcon( QString(":/data/Image5.jpg") );
            break;
        case 5 :
            icon = HbIcon( QString(":/data/Image6.jpg") );
            break;
        case 6 :
            icon = HbIcon( QString(":/data/Image7.jpg") );
            break;
        case 7 :
            icon = HbIcon( QString(":/data/Image8.jpg") );
            break;
        case 8 :
            icon = HbIcon( QString(":/data/Image9.jpg") );
            break;
        case 9 :
            icon = HbIcon( QString(":/data/Image10.jpg") );
            break;            
        }
        
        int max = mProgressDialog->maximum() ;
        if ( currentValue > max ) {
            mProgressDialog->setMaximum( currentValue );
            max = currentValue ;
        }
        
        mProgressDialog->setIcon(icon);
        
        if ( currentValue < 0 ) {
            mProgressDialog->setText( QString( "Refreshing" ) ); //To:Do string will change later
            mProgressDialog->setProgressValue( 0 );
        }
        else {
            int value = max - currentValue;
            mProgressDialog->setProgressValue( value );
            mProgressDialog->setText( QString( " %1 " ).arg( currentValue ) );
        }
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

void GlxViewManager::checkMarked()
{
    qDebug("GlxViewManager::checkMarked");
    QModelIndexList selectedModelIndex = mSelectionModel->selectedIndexes();
    for ( int i = 0 ; i <  mMarkingActionList.count(); i++) {
        if( mMarkingActionList.at(i)->data()==EGlxCmdSelect) {
       	    bool noSelection=selectedModelIndex.empty();
            mMarkingActionList.at(i)->setDisabled(noSelection);
            mMenuManager->disableAction(mView->menu(),noSelection);
            break;
        }
    }
}

void GlxViewManager::enterMarkingMode(qint32 viewId)
{
    GlxView *view = findView ( viewId );
    qDebug("GlxViewManager::enterMarkingMode view ID %d", viewId);
    
    if ( mMarkingToolBar == NULL) {
        createMarkingModeToolBar(); //Marking mode tool bar is different from normal mode tool bar
    }
    
    if ( view ) { 
        view->enableMarking();
        view->takeToolBar();
        view->addToolBar(mMarkingToolBar);
        mSelectionModel = view->getSelectionModel();
        if(mSelectionModel) 
        {
            connect(mSelectionModel, SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection& ) ), this, SLOT(checkMarked()));
            checkMarked();
        }
    }
    qDebug("GlxViewManager::enterMarkingMode view ID %d exit", viewId);
}

void GlxViewManager::exitMarkingMode(qint32 viewId)
{
    GlxView *view = findView ( viewId );
    qDebug("GlxViewManager::exitMarkingMode view ID %d", viewId);
    if ( view ) { 
        view->disableMarking(); 
        view->takeToolBar();
        view->addToolBar(mViewToolBar);
        if(mSelectionModel)
        {
            disconnect(mSelectionModel, SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection& ) ), this, SLOT(checkMarked()));
        }
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

void GlxViewManager::setModel( QAbstractItemModel *model )
{
    if ( mView ) {
        mView->setModel( model ) ;
    }
    
    if ( mMenuManager ) {
        mMenuManager->setModel( model );
    }    
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
        if ( mBackAction ) {
            view->setNavigationAction( mBackAction );
        }
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
    HbAction* allAction = new HbAction( this );
    allAction->setData( EGlxCmdAllGridOpen );
    mActionList.append( allAction );    
    allAction->setIcon( HbIcon( GLXICON_ALL ) ) ;
    allAction->setObjectName( "All Action" );
       
    //create the Album tool bar button action
    HbAction* albumAction = new HbAction( this );
    albumAction->setData( EGlxCmdAlbumListOpen );
    mActionList.append( albumAction );
    albumAction->setIcon( HbIcon( GLXICON_ALBUMS ) ) ;
    albumAction->setObjectName( "Album Action" );
   
    //create the album tool bar button action
    HbAction* cameraAction = new HbAction( this );
    cameraAction->setData( EGlxCmdCameraOpen );
    mActionList.append( cameraAction );  
    cameraAction->setIcon( HbIcon( GLXICON_CAMERA ) ) ;
    cameraAction->setObjectName( "Camera Action" );
    
    //Configure the 4 th Action in the tool bar
    XQSettingsManager *ciSettingsManager = NULL;
    ciSettingsManager = new XQSettingsManager(this);
    
    
    XQSettingsKey* operatorLinkCenrepKey = NULL;
    operatorLinkCenrepKey = new XQSettingsKey(XQSettingsKey::TargetCentralRepository, 
                                            KGlxCi_UidGallery, KGlxOperatorLink);
    QVariant value = ciSettingsManager->readItemValue(*operatorLinkCenrepKey);
    
    
    switch(value.toInt()) {
        case KGlxOvi:
            {
                XQSettingsKey* oviCenrepKey = NULL;
                oviCenrepKey = new XQSettingsKey(XQSettingsKey::TargetCentralRepository, 
                                                        KGlxCi_UidGallery, KGlxOvi);
                QVariant Ovivalue = ciSettingsManager->readItemValue(*oviCenrepKey, XQSettingsManager::TypeString);
                HbAction* configurableAction = new HbAction(this);
                configurableAction->setData(EGlxCmdOviOpen);
                mActionList.append(configurableAction);
            
                if ( Ovivalue.isValid() && Ovivalue.canConvert<QString>() ) {
                    configurableAction->setIcon( HbIcon( Ovivalue.toString() ) );
                }

                delete oviCenrepKey;
            }
            break;
            
        default:
            qDebug("GlxViewManager::Configurable Action is empty " );
            break;
    }
    
    delete operatorLinkCenrepKey;
    delete ciSettingsManager;
}

void GlxViewManager::createMarkingModeActions()
{
    mMarkingActionList.clear();
    
    //create the ok tool bar button action
    HbAction* selectAction = new HbAction(GLX_BUTTON_OK, this);
    selectAction->setData(EGlxCmdSelect);
    selectAction->setObjectName( "Select Action" );
    mMarkingActionList.append(selectAction);
    connect( selectAction, SIGNAL(triggered( )), this, SLOT(handleAction( )), Qt::QueuedConnection );
    mMarkingToolBar->addAction( selectAction );
    
    //create the cancel tool bar button action
    HbAction* cancelAction = new HbAction(GLX_BUTTON_CANCEL, this);
    cancelAction->setData(EGlxCmdCancel);
    cancelAction->setObjectName( "Cancel Action" );
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
        connect( mEffectEngine, SIGNAL( effectFinished() ), this, SLOT( effectFinished() ), Qt::QueuedConnection );
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
    delete mViewToolBar;
    delete mMarkingToolBar;
    qDebug("GlxViewManager::~GlxViewManager deleted toolbar");
    
    while( mViewList.isEmpty( ) == FALSE){
        delete mViewList.takeLast() ;
    }
    qDebug("GlxViewManager::~GlxViewManager view deleted");    
        
    delete mBackAction;
    delete mProgressDialog;
    
    if ( mEffectEngine ) {
        mEffectEngine->deregistertransitionEffect();
        delete mEffectEngine;
    }
 
    if( mMainWindow != GlxExternalUtility::instance()->getMainWindow() ){
        qDebug("GlxViewManager::~GlxViewManager remove view");
        delete mMainWindow;
    }
    delete mWindowEventFilter;
    
    qDebug("GlxViewManager::~GlxViewManager Exit");
}

