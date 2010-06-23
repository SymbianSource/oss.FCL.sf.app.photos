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

//include
#include <QApplication>
#include <QDebug>
#include <QItemSelectionModel>
#include <hbnotificationdialog.h>
#include <QProcess>
#include <hbinstance.h>

//user includes
#include "glxstatemanager.h"
#include "glxviewmanager.h"
#include "glxviewids.h"
#include "glxmediamodel.h"
#include "glxgridstate.h"
#include "glxliststate.h"
#include "glxfullscreenstate.h"
#include "glxdetailstate.h"
#include "glxslideshowsettingsstate.h"
#include "glxslideshowstate.h"
#include "glxtnobserver.h"
#include "glxmodelparm.h"
#include "glxcollectionpluginall.hrh"
#include "glxcollectionplugincamera.hrh"
#include "glxcollectionpluginalbums.hrh"
#include "glxcollectionpluginimageviewer.hrh"
#include "glxexternalutility.h"
#include "glxalbummodel.h"
#include "glxloggerenabler.h"
#include "glxmediaid.h"
#include "glxactionhandler.h"
#include "glxcommandhandlers.hrh"
#include "glxplugincommandid.hrh"
#include "glxlog.h"
#include "glxtracer.h"


GlxStateManager::GlxStateManager() 
    : mAllMediaModel( NULL ), 
      mAlbumGridMediaModel( NULL ),
      mAlbumMediaModel( NULL ),
      mImageviewerMediaModel( NULL ), 
      mCurrentModel( NULL ), 
      mCurrentState( NULL ), 
      mActionHandler( NULL ),
      mTNObserver ( NULL ),
      isProgressbarRunning ( false )
{
    qDebug("GlxStateManager::GlxStateManager");
    PERFORMANCE_ADV ( d1, "view manager creation time") {
        mViewManager = new GlxViewManager();
    }
    mTNObserver = new GlxTNObserver();
    
    connect ( this, SIGNAL( setupItemsSignal() ), this, SLOT( setupItems() ), Qt::QueuedConnection );
    connect ( mViewManager, SIGNAL(actionTriggered( qint32 )), this, SLOT(actionTriggered( qint32 )), Qt::QueuedConnection );
    connect ( mTNObserver, SIGNAL( leftTNCount( int ) ), this, SLOT( updateTNProgress( int ) ) );
	//TO:DO TBD through exception when it is null
}

void GlxStateManager::enterMarkingMode()
{
    mViewManager->enterMarkingMode(mCurrentState->id());
}

void GlxStateManager::exitMarkingMode()
{
    mViewManager->exitMarkingMode(mCurrentState->id());
}

bool GlxStateManager::executeCommand(qint32 commandId)
{
    qDebug( "GlxStateManager::executeCommand() : command ID %d", commandId);
    QItemSelectionModel * selectionModel = mViewManager->getSelectionModel(mCurrentState->id());
    if ( selectionModel == NULL )
        return FALSE;
    
    QModelIndexList indexList = selectionModel->selectedIndexes();
    //if no item is selected then do nothing
    if (indexList.count() == 0) 
        return FALSE;
    
    for ( int i = 0; i < indexList.count(); i++) {
        qDebug( "GlxGridView::doneAction() : index %d", indexList[i].row());            
        mCurrentModel->setData( indexList[i], indexList[i].row(), GlxSelectedIndexRole );
    }
    mActionHandler->handleAction(commandId, mCollectionId,indexList);
    return TRUE;
}

bool GlxStateManager::eventFilter(QObject *obj, QEvent *event)
{
    TRACER("GlxStateManager::eventFilter() ");
    GLX_LOG_INFO1("GlxStateManager::eventFilter() %d event type", event->type());
    
    if ( event->type() ==  QEvent::ApplicationActivate ) {
        mTNObserver->startTNMDaemon();
    }
    if ( event->type() ==  QEvent::ApplicationDeactivate ) {
        mTNObserver->stopTNMDaemon();
    }
    return QObject::eventFilter(obj, event);
}

void GlxStateManager::launchApplication()
{
    qDebug("GlxStateManager::launchApplication");   
       
    //To:Do use it in future once performance code is removed nextState(GLX_GRIDVIEW_ID, ALL_ITEM_S)
    mCurrentState = createState( GLX_GRIDVIEW_ID );
    mCurrentState->setState( ALL_ITEM_S );
       
    if ( mTNObserver->getTNLeftCount() > 0 ) {
        mViewManager->launchApplication( GLX_GRIDVIEW_ID, mCurrentModel);
        launchProgressDialog();
    }
    else {
        createModel( GLX_GRIDVIEW_ID );
        mViewManager->launchApplication( GLX_GRIDVIEW_ID, mCurrentModel);
    }
    
    mTNObserver->startTNObserving() ; 
}

void GlxStateManager::launchFromExternal()
{
    qDebug("GlxStateManager::launchApplication");
    mCurrentState = createState(GLX_FULLSCREENVIEW_ID);
    mCurrentState->setState(IMAGEVIEWER_S);
    
    createModel(GLX_FULLSCREENVIEW_ID);
    mCurrentModel->setData( mCurrentModel->index(0,0), 0, GlxFocusIndexRole );
    mViewManager->launchApplication( GLX_FULLSCREENVIEW_ID, mCurrentModel);
	setupItems();
}

void GlxStateManager::actionTriggered(qint32 id)
{
    qDebug("GlxStateManager::actionTriggered action ID = %d", id);
    mCurrentState->eventHandler(id);
    if (id != EGlxCmdHandled ) {
        eventHandler(id);
    }
}

void GlxStateManager::setupItems()
{    
    qDebug("GlxStateManager::setupItems()");
    mActionHandler = new GlxActionHandler();
    connect ( mViewManager, SIGNAL(externalCommand(int )), this, SIGNAL(externalCommand(int )) );
    mViewManager->setupItems();
    mViewManager->updateToolBarIcon(GLX_ALL_ACTION_ID);
}

void GlxStateManager::updateTNProgress( int count)
{    
    TRACER("GlxStateManager::updateTNProgress() ");
    // this is case when progress bar is not showing
    // in the case of rename of an image or capture the single item
    // it is also launching the progress bar, to avoid this scenario add the check of count more than 5
    if ( mCurrentModel && count > 5 ) { 
         goBack( GLX_GRIDVIEW_ID, ALL_ITEM_S ) ;
         cleanAllModel();
         launchProgressDialog();
    }
    
    if ( isProgressbarRunning ){
        if ( count == 0 ) {
            createModel( mCurrentState->id() );
            mViewManager->setModel( mCurrentModel );
            vanishProgressDialog();
        }
        else {
            mViewManager->updateProgressDialog( count );
        }
    }
}

void GlxStateManager::nextState(qint32 state, int internalState)
{
    qDebug("GlxStateManager::nextState next state = %u", state);
    GlxEffect effect = NO_EFFECT ;
    GlxViewEffect viewEffect = NO_VIEW ;
    
    PERFORMANCE ( d1, State Creation Time ) {
        mCurrentState = createState(state); //create a new state
        mCurrentState->setState(internalState); 
    }
    
    createModel(state); //model should created after the internal state was set.
    
    PERFORMANCE ( d2, view launch time ) {
        mCurrentState->setTranstionParameter( FORWARD_DIR, effect, viewEffect); // to run the transtion effect
        if ( viewEffect == NO_VIEW ) {
            mViewManager->launchView( state, mCurrentModel );
        }
        else {
            mViewManager->launchView( state, mCurrentModel, effect, viewEffect);
        }
    }
}

void GlxStateManager::previousState()
{
    qDebug("GlxStateManager::previousState");
    GlxEffect effect = NO_EFFECT ;
    GlxViewEffect viewEffect = NO_VIEW ;

    GlxState *state = mCurrentState; // To delete the current state later  

    if ( mCurrentState->previousState() == NULL ) { //In the case only one state in stack then exit from the application
        exitApplication() ;
        mCurrentState = NULL;
    }
    else {
        //effect parameter should be set here other wise there is no way to know from where it come from
        mCurrentState->setTranstionParameter( BACKWARD_DIR, effect, viewEffect);
        mCurrentState = mCurrentState->previousState(); //get the pervious state
        createModel( mCurrentState->id(), BACKWARD_DIR ); 

        if ( viewEffect == NO_VIEW ) {
            mViewManager->launchView( mCurrentState->id(), mCurrentModel );
        }
        else {
            mViewManager->launchView( mCurrentState->id(), mCurrentModel, effect, viewEffect);
        }
    }
    delete state;
}
   
void GlxStateManager::goBack(qint32 stateId, int internalState)
{
    qDebug("GlxStateManager::goBack()");
    
    //if current state and it internal state is same then no need to do any thing
    if ( mCurrentState->id() == stateId  && mCurrentState->state() == internalState ) {
        return ;
    }
    
    GlxState *state = mCurrentState;
    
    while ( mCurrentState ) {
        if ( mCurrentState->id() == stateId  && mCurrentState->state() == internalState ) {
            break ;           
        }
        
        mCurrentState = mCurrentState->previousState(); // set pervious state to the current state
        delete state; //delete the current state
        state = mCurrentState;    
    }
    
    //case when new state is not hierarchy then create a new state
    if ( mCurrentState == NULL ) {
        mCurrentState = createState(stateId);
        mCurrentState->setState( internalState );
    }   
    
    qDebug("GlxStateManager::goBack() state %u", mCurrentState);
    createModel( mCurrentState->id() ); //model should created after the internal state was set.
    mViewManager->launchView( mCurrentState->id(), mCurrentModel );   
}

void GlxStateManager::changeState(qint32 stateId, int internalState)
{
    qDebug("GlxStateManager::changeState %d", stateId);	
    
    GlxState *state = mCurrentState;
    GlxEffect effect = NO_EFFECT ;
    GlxViewEffect viewEffect = NO_VIEW ;

    mCurrentState = mCurrentState->previousState();//pervious state of current state become the pervious state of new state 
    
    mCurrentState = createState(stateId);
    mCurrentState->setState( internalState );
    createModel( stateId ); //model should created after the internal state was set.
    
    mCurrentState->setTranstionParameter( NO_DIR, effect, viewEffect); //set the transition parameter    
    if ( viewEffect == NO_VIEW ) {
        mViewManager->launchView( mCurrentState->id(), mCurrentModel );
    }
    else {
        mViewManager->launchView( mCurrentState->id(), mCurrentModel, effect, viewEffect);
    }
    delete state;
}

void GlxStateManager::removeCurrentModel()
{
    TRACER("GlxStateManager::removeCurrentModel() ");
    if ( mCurrentModel == mAllMediaModel ) {
        qDebug("GlxStateManager::removeCurrentModel() remove all media model");	
        delete mAllMediaModel ;
        mAllMediaModel = NULL ;
        mCurrentModel = NULL ;
    }
    else if ( mCurrentModel == mAlbumGridMediaModel ) {
        qDebug("GlxStateManager::removeCurrentModel() remove album grid media model");	
        delete mAlbumGridMediaModel ;
        mAlbumGridMediaModel = NULL ;
        mCurrentModel = NULL ;
    }
    else if ( mCurrentModel == mAlbumMediaModel ) {
        qDebug("GlxStateManager::removeCurrentModel() remove  model");	
        delete mAlbumMediaModel ;
        mAlbumMediaModel = NULL ;
        mCurrentModel = NULL ;
    }
    else {
    	qDebug("GlxStateManager::removeCurrentModel() do nothing");
    	//do nothing
    }
}

void GlxStateManager::cleanAllModel()
{
    TRACER("GlxStateManager::cleanAllModel() ");
    delete mAllMediaModel ;
    mAllMediaModel = NULL ;
    delete mAlbumMediaModel ; 
    mAlbumMediaModel = NULL ;
    delete mAlbumGridMediaModel ;
    mAlbumGridMediaModel = NULL ;
    delete mImageviewerMediaModel ;
    mImageviewerMediaModel = NULL ;
    mCurrentModel = NULL ;
}

void GlxStateManager::launchProgressDialog()
{
    TRACER("GlxStateManager::launchProgressDialog() ");
    //HbMainWindow *window = hbInstance->allMainWindows().first();
    //window->setAutomaticOrientationEffectEnabled( true );
    
    QCoreApplication::instance()->installEventFilter( this );
    mViewManager->launchProgressDialog( mTNObserver->getTNLeftCount() );
    isProgressbarRunning = true ;
}

void GlxStateManager::vanishProgressDialog()
{
    TRACER("GlxStateManager::vanishProgressDialog() ");
    //HbMainWindow *window = hbInstance->allMainWindows().first();
    //window->setAutomaticOrientationEffectEnabled( false );
    
    QCoreApplication::instance()->removeEventFilter( this );
    isProgressbarRunning = false;
    mViewManager->updateProgressDialog( 0 );    
}

GlxState * GlxStateManager::createState(qint32 stateId)
{
    qDebug("GlxStateManager::createState state id = %d", stateId);
    
    switch( stateId ) {    
    case GLX_GRIDVIEW_ID :
        return new GlxGridState( this, mCurrentState );
        
    case GLX_LISTVIEW_ID :
        return new GlxListState( mCurrentState );    	
    	
    case GLX_FULLSCREENVIEW_ID :
        return new GlxFullScreenState( mCurrentState );
        
    case GLX_DETAILSVIEW_ID:
        return new GlxDetailState( mCurrentState );
        
    case GLX_SLIDESHOWVIEW_ID :
        return new GlxSlideShowState( this, mCurrentState );
    case GLX_SLIDESHOWSETTINGSVIEW_ID :
        return new GlxSlideShowSettingsState(this, mCurrentState );
    	
    default :
        return NULL;		
    } 
}

void GlxStateManager::createModel(qint32 stateId, NavigationDir dir)
{
    switch( stateId ) {
    case GLX_GRIDVIEW_ID :
        createGridModel( mCurrentState->state(), dir );
        break;

    case GLX_LISTVIEW_ID :
    	//To:Do change the code when integrate the album list model
        if (mAlbumMediaModel == NULL) {
            GlxModelParm modelParm (KGlxCollectionPluginAlbumsImplementationUid, 0);
            mAlbumMediaModel = new GlxAlbumModel (modelParm);
        }
        mCurrentModel = mAlbumMediaModel;
        mCollectionId = KGlxCollectionPluginAlbumsImplementationUid;
        mCurrentModel->setData(QModelIndex(), (int)GlxContextPtList, GlxContextRole );
        mViewManager->updateToolBarIcon(GLX_ALBUM_ACTION_ID);
        break;
        
    case GLX_FULLSCREENVIEW_ID :
		if ( mCurrentState->state() == IMAGEVIEWER_S) {
            GlxModelParm modelParm (KGlxCollectionPluginImageViewerImplementationUid, 0);
            mCurrentModel = mImageviewerMediaModel = new GlxMediaModel (modelParm);
            mCollectionId = KGlxCollectionPluginImageViewerImplementationUid;
		}
        else if ( mCurrentState->state() == EXTERNAL_S) {
            if(!mAllMediaModel) {
                GlxModelParm modelParm (KGlxCollectionPluginAllImplementationUid, 0);
                mCurrentModel = mAllMediaModel = new GlxMediaModel (modelParm);
            }
            else {
                mCurrentModel = mAllMediaModel;
            }
            mCollectionId = KGlxCollectionPluginAllImplementationUid;
        }
		setFullScreenContext();
        break;
        
    case GLX_DETAILSVIEW_ID :
        setFullScreenContext();
        break;
        
    case GLX_SLIDESHOWVIEW_ID :
        if ( mCurrentState->state() == SLIDESHOW_ALBUM_ITEM_S ) {
            GlxModelParm modelParm ( KGlxAlbumsMediaId , 0);
            mCurrentModel = mAlbumGridMediaModel = new GlxMediaModel( modelParm );
        }
        setFullScreenContext();
        break;
    	    	
    default :
        break;
    }
    mCurrentModel->setData( QModelIndex(), mCurrentState->state(), GlxSubStateRole );
}

void GlxStateManager::createGridModel(int internalState, NavigationDir dir)
{
    GlxModelParm modelParm;
    GlxContextMode mode;
    
    if ( mViewManager->orientation() == Qt::Horizontal ) {
        mode = GlxContextLsGrid ;
    }
    else {
        mode = GlxContextPtGrid ;
    }
    
    switch( internalState) {
    case ALL_ITEM_S :
        if ( mAllMediaModel == NULL ) {
            modelParm.setCollection( KGlxCollectionPluginAllImplementationUid );
            modelParm.setDepth(0);
            modelParm.setContextMode( mode ) ;
            mAllMediaModel = new GlxMediaModel( modelParm );
        }
        mCollectionId = KGlxCollectionPluginAllImplementationUid;
        mCurrentModel = mAllMediaModel;
        mViewManager->updateToolBarIcon(GLX_ALL_ACTION_ID);
        break;
        
    case ALBUM_ITEM_S :
        if ( dir != BACKWARD_DIR ) { 
            modelParm.setCollection( KGlxAlbumsMediaId );
            modelParm.setDepth(0);
            modelParm.setContextMode( mode ) ;
            mAlbumGridMediaModel = new GlxMediaModel( modelParm );
        }               
        mCollectionId = KGlxAlbumsMediaId;
        mCurrentModel = mAlbumGridMediaModel ;
        mViewManager->updateToolBarIcon(NO_ACTION_ID);
        break;
    
    default :
        break;
    }
    
    mCurrentModel->setData(QModelIndex(), (int)mode, GlxContextRole );
}

void GlxStateManager::setFullScreenContext()
{
    if ( mViewManager->orientation() == Qt::Horizontal || mCurrentState->id() == GLX_SLIDESHOWVIEW_ID ) {
        mCurrentModel->setData(QModelIndex(), (int)GlxContextLsFs, GlxContextRole );
    }
    else {
        mCurrentModel->setData(QModelIndex(), (int)GlxContextPtFs, GlxContextRole );
    }    
}

void GlxStateManager::eventHandler(qint32 &id)
{
    qDebug("GlxStateManager::eventHandler action id = %d", id);
    switch(id) {
    case EGlxCmdAllGridOpen :
        changeState( GLX_GRIDVIEW_ID, ALL_ITEM_S );
        id = EGlxCmdHandled;
        break;
    	
    case EGlxCmdAlbumListOpen:
        changeState( GLX_LISTVIEW_ID, -1 );
        id = EGlxCmdHandled;
        break;
    	
    case EGlxCmdAlbumGridOpen:
        nextState( GLX_GRIDVIEW_ID, ALBUM_ITEM_S );
        id = EGlxCmdHandled;
        break;
      
    case EGlxCmdFirstSlideshow :
        //play the slide show with first item
        mCurrentModel->setData( mCurrentModel->index(0, 0), 0, GlxFocusIndexRole );
        nextState( GLX_SLIDESHOWVIEW_ID, SLIDESHOW_GRID_ITEM_S );
        id = EGlxCmdHandled;
        break;
        
    case EGlxCmdSelectSlideshow:
        //play the slide show with selected item
        nextState( GLX_SLIDESHOWVIEW_ID, SLIDESHOW_GRID_ITEM_S );
        id = EGlxCmdHandled;
        break;
        
    case EGlxCmdAlbumSlideShow:
        //play the slide show for a album
        nextState( GLX_SLIDESHOWVIEW_ID, SLIDESHOW_ALBUM_ITEM_S );
        id = EGlxCmdHandled;
        break;
        
    case EGlxCmdSlideshowSettings:
        nextState(GLX_SLIDESHOWSETTINGSVIEW_ID,-1 );
        id = EGlxCmdHandled;
        break;
        
    case EGlxCmdDetailsOpen:
        qDebug("GlxStateManager::eventHandler EGlxCmdDetailsOpen");
        nextState( GLX_DETAILSVIEW_ID, -1 );
        id = EGlxCmdHandled;
        break;
        
    case EGlxCmdEmptyData : {
        GlxState *tmpState = mCurrentState ;
        while ( tmpState->id() != GLX_GRIDVIEW_ID ) {
            tmpState = tmpState->previousState() ;
        }
        
        goBack( GLX_GRIDVIEW_ID, tmpState->state() );
        id = EGlxCmdHandled;
        break;
    }
    	
    case EGlxCmdBack :
    	previousState();
    	id = EGlxCmdHandled;
    	break;
    	
    case EGlxCmdCameraOpen:
		{
		QProcess::startDetached(QString("cxui.exe"));
		id = EGlxCmdHandled;
		}
        break;	
        
    case EGlxCmdOviOpen:
		{
		HbNotificationDialog::launchDialog("Not Implemented");
        id = EGlxCmdHandled;
		}
        break;	
    
   case EGlxCmdDelete :
		{
        GlxExternalData* externalItems = GlxExternalUtility::instance()->getExternalData();
        if(externalItems){
            QVariant variant = mCurrentModel->data( mCurrentModel->index(0,0), GlxFocusIndexRole );    
            if ( variant.isValid() &&  variant.canConvert<int> () ) {
                int selIndex = variant.value<int>(); 
                int externalDataCount = externalItems->count();
                if(selIndex < externalDataCount){
                    GlxInterfaceParams tmp = externalItems->value(selIndex);
                    tmp.isSelected = true;
                    externalItems->replace(selIndex,tmp);
                    emit externalCommand(EGlxPluginCmdDelete);
                    return;
                }
            }
        }
        mActionHandler->handleAction(id,mCollectionId);
		}
        break;
        
   case EGlxCmdMarkAll:
   case EGlxCmdUnMarkAll:
       mViewManager->handleUserAction(mCurrentState->id(), id);
       id = EGlxCmdHandled;
       break;
       
   case EGlxCmdSetupItem :
       emit setupItemsSignal();
       break;
       
    default :
        mActionHandler->handleAction(id,mCollectionId);
    	break;
    }	
}

void GlxStateManager::exitApplication()
{
    if ( mCurrentState->id( ) == GLX_FULLSCREENVIEW_ID && mCurrentState->state() == EXTERNAL_S ) {
        emit externalCommand(EGlxPluginCmdBack);
    }
    else {
    //To:Do memory cleanup
        QApplication::quit();
    }    
}

GlxStateManager::~GlxStateManager()
{
    qDebug("GlxStateManager::~GlxStateManager");
    cleanAllModel();
    delete mActionHandler;
    qDebug("GlxStateManager::~GlxStateManager delete Model");
    
    disconnect ( mViewManager, SIGNAL(actionTriggered(qint32 )), this, SLOT(actionTriggered(qint32 )) );
    disconnect ( mViewManager, SIGNAL(externalCommand(int )), this, SIGNAL(externalCommand(int )) );
    disconnect ( mTNObserver, SIGNAL( leftTNCount( int ) ), this, SLOT( updateTNProgress( int ) ) );
    disconnect ( this, SIGNAL( setupItemsSignal() ), this, SLOT( setupItems() ) );
    
    delete mTNObserver;
    delete mViewManager; 
    qDebug("GlxStateManager::~GlxStateManager delete view manager");
    
    GlxState *tmp;   
    while (mCurrentState) {
        tmp = mCurrentState;
        mCurrentState = mCurrentState->previousState();
        delete tmp;        
    }
    
    qDebug("GlxStateManager::~GlxStateManager Exit");
}

void GlxStateManager::cleanupExternal()
{
    qDebug("GlxStateManager::cleanupExternal");
    mViewManager->deactivateCurrentView();
    GlxMediaModel *glxModel = dynamic_cast<GlxMediaModel *>(mCurrentModel);
    if(glxModel) {
		glxModel->clearExternalItems();  
	}
}
