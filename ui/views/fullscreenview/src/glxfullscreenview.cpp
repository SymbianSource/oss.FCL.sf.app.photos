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


//Includes
#include <QTimer>
#include <hbmenu.h>
#include <HbAction>
#include <HbToolBar>
#include <hbgridview.h>
#include <hbpushbutton.h>
#include <hbmainwindow.h>
#include <shareuidialog.h>
#include <hbdocumentloader.h>
#include <QAbstractItemModel>
#include <hbabstractviewitem.h>
#include <hbiconitem.h>
#include <QCoreApplication>

//User Includes
#include "glxlog.h"
#include "glxtracer.h"
#include "glxviewids.h"  //contains the view ids of all the views in photos app.
#include "glxicondefs.h" //Contains the icon names/Ids
#include "glxmodelparm.h"
#include "glxcoverflow.h"
#include "glxdocloaderdefs.h" //contains the definations of path, widget names and view names in docml
#include "glxloggerenabler.h"
#include "glxtvoutwrapper.h"
#include "glxfullscreenview.h" 
#include "glxcommandhandlers.hrh"
#include "glxzoomwidget.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "glxfullscreenviewTraces.h"
#endif
 

const int KUiOffTime = 3000;

GlxFullScreenView::GlxFullScreenView(HbMainWindow *window,HbDocumentLoader *DocLoader) : 
    GlxView ( GLX_FULLSCREENVIEW_ID), 
    mModel(NULL), 
    mWindow( window), 
    mCoverFlow(NULL) , 
    mImageStrip (NULL), 
    mUiOffTimer(NULL),
    mIconItem(NULL),
    mTvOutWrapper(NULL),
    mFullScreenToolBar(NULL),
	mZoomWidget(NULL)
{
    OstTraceFunctionEntry0( GLXFULLSCREENVIEW_GLXFULLSCREENVIEW_ENTRY );
    
    mDocLoader = DocLoader;
    setContentFullScreen( true );
    HbEffect::add( QString("HbGridView"), QString(":/data/transitionup.fxml"), QString( "TapShow" ));
    HbEffect::add( QString("HbGridView"), QString(":/data/transitiondown.fxml"), QString( "TapHide" ));
    HbEffect::add( QString("HbGridViewItem"), QString(":/data/gridtofullscreenhide.fxml"), QString( "Select" ));
    
    OstTraceFunctionExit0( GLXFULLSCREENVIEW_GLXFULLSCREENVIEW_EXIT );
}

void GlxFullScreenView::initializeView(QAbstractItemModel *model)
{
    OstTraceFunctionEntry0( GLXFULLSCREENVIEW_INITIALIZEVIEW_ENTRY );
    
    // if animations is on, then Set the image to HDMI here
    if (!mTvOutWrapper){
        mTvOutWrapper = new GlxTvOutWrapper();
    }
    setHdmiModel(model);
    loadWidgets();

    // Initialize the coverflow and partially creates the coverflow with one image
    // to make the widget light weight in order to make transition smooth 
    mCoverFlow->partiallyCreate( model, screenSize() );
    
    OstTraceFunctionExit0( GLXFULLSCREENVIEW_INITIALIZEVIEW_EXIT );
}

void GlxFullScreenView::loadWidgets()
{
    OstTraceFunctionEntry0( GLXFULLSCREENVIEW_LOADWIDGETS_ENTRY );
       
    mCoverFlow = qobject_cast<GlxCoverFlow*> (mDocLoader->findWidget(GLXFULLSCREEN_COVERFLOW));
    
    mZoomWidget  =  qobject_cast<GlxZoomWidget*> (mDocLoader->findWidget(GLXFULLSCREENZOOMWIDGET));
    mZoomWidget->connectDecodeRequestToPinchEvent();
    mCoverFlow->setMultitouchFilter(mZoomWidget);
    
    //initialise the cover flow for basic connections and the rest
    mCoverFlow->setCoverFlow();
    mImageStrip = qobject_cast<HbGridView*> (mDocLoader->findWidget(GLXFULLSCREEN_FILMSTRIP));
     
    //When the widget is loaded/retreived the widgets are shown by default.
    //@to do : hide the widgets by default in docml
    mImageStrip->hide();      
	mImageStrip->setLayoutName( QString( "ImageStrip" ) ); // To distinguish in CSS file
	mImageStrip->setEnabledAnimations( HbAbstractItemView::None );
	
    OstTraceFunctionExit0( GLXFULLSCREENVIEW_LOADWIDGETS_EXIT );
}

void GlxFullScreenView::loadFullScreenToolBar()
{
    OstTraceFunctionEntry0( GLXFULLSCREENVIEW_LOADFULLSCREENTOOLBAR_ENTRY );
    
    //The fullscreen tool bar is delted when ever the view is deactivated
    //so load the docml once again and retreive the object
    bool loaded =true;      
    mDocLoader->load(GLXFULLSCREENDOCMLPATH,&loaded);
    loadViewSection();

    mFullScreenToolBar = qobject_cast<HbToolBar *> (mDocLoader->findObject(QString("toolBar")));
    mFullScreenToolBar->clearActions();
    
    addToolBarAction( EGlxCmdDetailsOpen, GLXICON_FLIP, "Flip Action" ); //create  Flip tool bar button action
    addToolBarAction( EGlxCmdSend, GLXICON_SEND, "Send Action" );        //create  Send tool bar button action
    if( getSubState() != IMAGEVIEWER_S ) {        
        addToolBarAction( EGlxCmdDelete, GLXICON_DELETE, "Delete Action" ); //create  Delete tool bar button action
    }
    else {
        addToolBarAction( EGlxCmdHandled, GLXICON_USEIMAGE, "Use Action" ); //create  Use Image tool bar button action
    }
 
    OstTraceFunctionExit0( GLXFULLSCREENVIEW_LOADFULLSCREENTOOLBAR_EXIT );
}

void GlxFullScreenView::addToolBarAction( int commandId, const QString &iconName, const QString &name )
{
    HbAction *action = new HbAction( this ); 
    action->setData( commandId );
    action->setIcon( HbIcon( iconName ) ); 
    action->setObjectName( name );
    mFullScreenToolBar->addAction( action);
    connect(action, SIGNAL(triggered( )), this, SLOT(handleToolBarAction( )) ); 
}

void GlxFullScreenView::activate()
{
    OstTraceFunctionEntry0( GLXFULLSCREENVIEW_ACTIVATE_ENTRY );    
//for zoom might not be required after wk15 release
	mWindow->viewport()->setAttribute(Qt::WA_AcceptTouchEvents,true); 
	mWindow->viewport()->grabGesture(Qt::PinchGesture);
	
	QCoreApplication::instance()->installEventFilter(this);

    if( !mCoverFlow ) {
        loadWidgets();  //retrives the widgets
    }
    //Loads the widgets corresponding to the orientation.
    loadViewSection();
    
    setStatusBarVisible(FALSE);
    setTitleBarVisible(FALSE);
        
    mUiOffTimer = new QTimer();
    mUiOff = true;
    mUiOffTimer->stop();        
    mCoverFlow->setUiOn(FALSE);    
    addConnection(); 
    setLayout();
     
    if (!mTvOutWrapper){
        mTvOutWrapper = new GlxTvOutWrapper();
    }
    OstTraceFunctionExit0( GLXFULLSCREENVIEW_ACTIVATE_EXIT );
}

void GlxFullScreenView::loadViewSection()
{
    OstTraceFunctionEntry0( GLXFULLSCREENVIEW_LOADVIEWSECTION_ENTRY );    

    bool loaded =true;
    if( mWindow->orientation() == Qt::Horizontal ) {
        //Load the Landscape section for Horizontal
        mDocLoader->load(GLXFULLSCREENDOCMLPATH,GLXFULLSCREENLSSECTION,&loaded);
    }
    else{
        //Load the Portrait section for Horizontal
        mDocLoader->load(GLXFULLSCREENDOCMLPATH,GLXFULLSCREENPTSECTION,&loaded);    
    }
    
    OstTraceFunctionExit0( GLXFULLSCREENVIEW_LOADVIEWSECTION_EXIT );
}

void GlxFullScreenView::resetView()
{
    OstTraceFunctionEntry0( GLXFULLSCREENVIEW_RESETVIEW_ENTRY );    

    //Clean up the rest of the resources allocated
    cleanUp(); 
        
    //Clear the 4 icons present in the Coverflow,so that the transition between the views are smooth
    mCoverFlow->partiallyClean();
    
    OstTraceFunctionExit0( GLXFULLSCREENVIEW_RESETVIEW_EXIT );
}

void GlxFullScreenView::deActivate()
{ 
    OstTraceFunctionEntry0( GLXFULLSCREENVIEW_DEACTIVATE_ENTRY );    
    
	//Clean up the rest of the resources allocated
    cleanUp();
    QCoreApplication::instance()->removeEventFilter(this);
    //deletes the iconitems in the coverflow   
    mCoverFlow->ClearCoverFlow();

    //the coverflow is not deleted as it is loaded by document loader
    //the coverflow is initialised to null 
    //to just reset to the initial state
    mCoverFlow = NULL;
    
    OstTraceFunctionExit0( GLXFULLSCREENVIEW_DEACTIVATE_EXIT );
}

void GlxFullScreenView::cleanUp()
{ 
    OstTraceFunctionEntry0( GLXFULLSCREENVIEW_CLEANUP_ENTRY );

    removeConnection();

    if ( mUiOffTimer ) {
        OstTrace0( TRACE_NORMAL, GLXFULLSCREENVIEW_CLEANUP, "GlxFullScreenView::cleanUp() mUiOffTimer" );
        mUiOffTimer->stop();
        delete mUiOffTimer;
        mUiOffTimer = NULL;
    }

    if(mFullScreenToolBar) {
       mFullScreenToolBar->clearActions();
       mFullScreenToolBar = NULL;
    }
    
    if (mTvOutWrapper) {
        delete mTvOutWrapper;
        mTvOutWrapper = NULL;
    }
    
    if(mZoomWidget)
    {
        mZoomWidget->cleanUp();
        mZoomWidget = NULL;
    }
    OstTraceFunctionExit0( GLXFULLSCREENVIEW_CLEANUP_EXIT );
}

QGraphicsItem * GlxFullScreenView::getAnimationItem(GlxEffect transitionEffect)
{
    if ( transitionEffect == GRID_TO_FULLSCREEN 
            || transitionEffect == FULLSCREEN_TO_DETAIL
            || transitionEffect == DETAIL_TO_FULLSCREEN ) {
            return this;
    }

    return NULL;    
}

void GlxFullScreenView::setModel( QAbstractItemModel *model )
{
    OstTraceFunctionEntry0( GLXFULLSCREENVIEW_SETMODEL_ENTRY );
    OstTraceExt2( TRACE_NORMAL, GLXFULLSCREENVIEW_SETMODEL, "GlxFullScreenView::setModel; model=%x; mModel=%u", ( TUint )( model ), ( TUint ) mModel );
    
    mModel = model;     
	setModelContext();  
    setHdmiModel(mModel);
	mZoomWidget->setModel(mModel);  
    mCoverFlow->setModel(mModel);
    setImageStripModel();
    if(getSubState() == IMAGEVIEWER_S) {
        setTitle("Image Viewer");
    }
	else if(getSubState() == FETCHER_S){
		setStatusBarVisible(TRUE);
        setTitleBarVisible(TRUE);
	}
    OstTraceFunctionExit0( GLXFULLSCREENVIEW_SETMODEL_EXIT );
}

void GlxFullScreenView::setHdmiModel(QAbstractItemModel* model)
{
    if (mTvOutWrapper) {
        mTvOutWrapper->setModel(model); 
        mTvOutWrapper->setImagetoHDMI(); // for the first image on screen
    }    
}

void GlxFullScreenView::setModelContext()
{
    OstTraceFunctionEntry0( GLXFULLSCREENVIEW_SETMODELCONTEXT_ENTRY );
    
    if ( mModel && mWindow ) {
        if ( mWindow->orientation() == Qt::Horizontal ) {
            WRITE_TIMESTAMP("set the fullscreen landscape context")
            mModel->setData(QModelIndex(), (int)GlxContextLsFs, GlxContextRole );
        }
        else {
            WRITE_TIMESTAMP("set the fullscreen portrait context")
            mModel->setData(QModelIndex(), (int)GlxContextPtFs, GlxContextRole );
        }
    }
    
    OstTraceFunctionExit0( GLXFULLSCREENVIEW_SETMODELCONTEXT_EXIT );
}

void GlxFullScreenView::orientationChanged(Qt::Orientation orient)
{
    OstTraceFunctionEntry0( GLXFULLSCREENVIEW_ORIENTATIONCHANGED_ENTRY );
    Q_UNUSED(orient)
    
    // fullscreen thumnail size is depends on orientation of phone so change the model context before changing the layout
    if ( mUiOff == FALSE ) {
        mUiOffTimer->start(KUiOffTime);
    }
    setModelContext();
    loadViewSection();
    setLayout();
    
    OstTraceFunctionExit0( GLXFULLSCREENVIEW_ORIENTATIONCHANGED_EXIT );
}

void GlxFullScreenView::activateUI()
{
    OstTraceFunctionEntry0( GLXFULLSCREENVIEW_ACTIVATEUI_ENTRY );
    
    if ( mUiOff && getSubState() != FETCHER_S){      
        if( !mFullScreenToolBar ) {
            loadFullScreenToolBar();
        }
        mUiOff = FALSE;
        
        QVariant variant = mModel->data( mModel->index(0,0), GlxFocusIndexRole );    
        if ( variant.isValid() &&  variant.canConvert<int> ()  ) {
            mImageStrip->setCurrentIndex ( mModel->index( variant.value<int>(), 0) );    
            mImageStrip->scrollTo( mModel->index( variant.value<int>(), 0), HbGridView::PositionAtTop ); 
        }
        
        mFullScreenToolBar->show();
		setStatusBarVisible(TRUE);
        setTitleBarVisible(TRUE);
       
        if ( mImageStrip && getSubState() != IMAGEVIEWER_S) {
            mImageStrip->show(); 
            HbEffect::start(mImageStrip, QString("HbGridView"), QString("TapShow"), this, "effectFinished" );
        }
        else if( getSubState() == IMAGEVIEWER_S){
            setTitle("Image Viewer");
        }
    }
    else {
        hideUi();
    }
    
    OstTraceFunctionExit0( GLXFULLSCREENVIEW_ACTIVATEUI_EXIT );
}

void GlxFullScreenView::hideUi()
{
    OstTraceFunctionEntry0( GLXFULLSCREENVIEW_HIDEUI_ENTRY );
 
    //if option menu is open then no need to hide the status bar and image strip
    HbMenu *menuWidget = menu();
    if ( menuWidget->isVisible() ) { 
        OstTraceFunctionExit0( GLXFULLSCREENVIEW_HIDEUI_EXIT );
        return ;
    }
    
    mUiOff = TRUE;
	if ( getSubState() != FETCHER_S ) {
		setStatusBarVisible(FALSE);
        setTitleBarVisible(FALSE);
	}
    if ( mImageStrip && ( getSubState() != IMAGEVIEWER_S && getSubState() != FETCHER_S )) {
        HbEffect::start(mImageStrip, QString("HbGridView"), QString("TapHide"), this, "effectFinished" );
    }

    if(mFullScreenToolBar) {
       mFullScreenToolBar->hide();
    }
    
    OstTraceFunctionExit0( DUP1_GLXFULLSCREENVIEW_HIDEUI_EXIT );
}

void GlxFullScreenView::changeSelectedIndex(const QModelIndex &index)
{
    OstTraceFunctionEntry0( GLXFULLSCREENVIEW_CHANGESELECTEDINDEX_ENTRY );
    
    QVariant variant = mModel->data( mModel->index(0,0), GlxFocusIndexRole );    
    if ( variant.isValid() &&  variant.canConvert<int> () && ( index.row() == variant.value<int>() ) ) {
        OstTraceFunctionExit0( GLXFULLSCREENVIEW_CHANGESELECTEDINDEX_EXIT );
        if (mTvOutWrapper){
            // for the image changed on deletion
            mTvOutWrapper->setImagetoHDMI();
        }
        if (mZoomWidget){
            // for the image changed on deletion
            mZoomWidget->indexChanged();
        }
        return;
    }
    
    mModel->setData( index, index.row(), GlxFocusIndexRole );
    mModel->setData( index, index.row(), GlxVisualWindowIndex );
	mZoomWidget->indexChanged(index.row());  
    if (mTvOutWrapper){
        // for the image changed on swipe
        mTvOutWrapper->setImagetoHDMI();
    }
    OstTraceFunctionExit0( DUP1_GLXFULLSCREENVIEW_CHANGESELECTEDINDEX_EXIT );
}

void GlxFullScreenView::indexChanged(const QModelIndex &index)
{
    OstTraceFunctionEntry0( GLXFULLSCREENVIEW_INDEXCHANGED_ENTRY );    
    
    QVariant variant = mModel->data( mModel->index(0,0), GlxFocusIndexRole );    
    if ( variant.isValid() &&  variant.canConvert<int> () && ( index.row() == variant.value<int>() ) ) {
       OstTraceFunctionExit0( GLXFULLSCREENVIEW_INDEXCHANGED_EXIT );
       return;
    }         
    mModel->setData( index, index.row(), GlxFocusIndexRole );
	mZoomWidget->indexChanged(index.row());
    mCoverFlow->indexChanged(index.row());
    mImageStrip->scrollTo(index, HbGridView::EnsureVisible );
    if (mTvOutWrapper){
    // for the indexchnaged through filmstrip
    mTvOutWrapper->setImagetoHDMI();
    }
    //disable the animation for the time being
    //imageSelectionAnimation( index );
    
    OstTraceFunctionExit0( DUP1_GLXFULLSCREENVIEW_INDEXCHANGED_EXIT );
}

void GlxFullScreenView::scrollingStarted()
{
    OstTraceFunctionEntry0( GLXFULLSCREENVIEW_SCROLLINGSTARTED_ENTRY );
    
    //no need to hide the ui during image strip scrolling
    if ( mUiOffTimer ) {
        mUiOffTimer->stop();
    }
    
    OstTraceFunctionExit0( GLXFULLSCREENVIEW_SCROLLINGSTARTED_EXIT );
}

void GlxFullScreenView::scrollingEnded()
{
    OstTraceFunctionEntry0( GLXFULLSCREENVIEW_SCROLLINGENDED_ENTRY );
    
    if ( mUiOffTimer ) {
        mUiOffTimer->start(KUiOffTime);
    }
    setVisvalWindowIndex();
    
    OstTraceFunctionExit0( GLXFULLSCREENVIEW_SCROLLINGENDED_EXIT );
}

void GlxFullScreenView::pressed(const QModelIndex &index)
{
    OstTraceFunctionEntry0( GLXFULLSCREENVIEW_PRESSED_ENTRY );
    
    Q_UNUSED(index)
    TRACER("GlxFullScreenView::pressed() ");
    if ( mUiOffTimer ) {
        mUiOffTimer->stop();
    }
    OstTraceFunctionExit0( GLXFULLSCREENVIEW_PRESSED_EXIT );
}

void GlxFullScreenView::released(const QModelIndex &index)
{
    OstTraceFunctionEntry0( GLXFULLSCREENVIEW_RELEASED_ENTRY );
    
    Q_UNUSED(index)
    
    if ( mUiOffTimer && mImageStrip->isScrolling() == false ) {
        mUiOffTimer->start(KUiOffTime);
    }
    
    OstTraceFunctionExit0( GLXFULLSCREENVIEW_RELEASED_EXIT );
}

void GlxFullScreenView::setVisvalWindowIndex()
{
    OstTraceFunctionEntry0( GLXFULLSCREENVIEW_SETVISVALWINDOWINDEX_ENTRY );
    
    QList< HbAbstractViewItem * >  visibleItemList =  mImageStrip->visibleItems();
    if ( visibleItemList.count() <= 0 )
        return ;

    HbAbstractViewItem *item = visibleItemList.at(0);
    if ( item == NULL ) 
        return ;

    if (  item->modelIndex().row() < 0 || item->modelIndex().row() >= mModel->rowCount() )
        return ;

    mModel->setData( item->modelIndex (), item->modelIndex().row(), GlxVisualWindowIndex);
    
    OstTraceFunctionExit0( GLXFULLSCREENVIEW_SETVISVALWINDOWINDEX_EXIT );
}

void GlxFullScreenView::coverFlowEventHandle( GlxCoverFlowEvent e )
{
    OstTraceFunctionEntry0( GLXFULLSCREENVIEW_COVERFLOWEVENTHANDLE_ENTRY );
    
    switch( e ){
    case TAP_EVENT :
        activateUI();
        break ;
        
    case PANNING_START_EVENT :
        hideUi();
        break ;
        
    case EMPTY_ROW_EVENT :
        emit actionTriggered( EGlxCmdEmptyData );
        break ;
        
    default :
        break ;
    } 
    
    OstTraceFunctionExit0( GLXFULLSCREENVIEW_COVERFLOWEVENTHANDLE_EXIT );
}

void GlxFullScreenView::effectFinished( const HbEffect::EffectStatus  )
{
    OstTraceFunctionEntry0( GLXFULLSCREENVIEW_EFFECTFINISHED_ENTRY );
    
    if ( mUiOffTimer == NULL ){ //view is already deactivate so no need to do any thing
       return ;
    }
    
    if ( mUiOff ) {
        mUiOffTimer->stop();        
        mCoverFlow->setUiOn(FALSE);
        mImageStrip->hide();
    }
    else {
        mUiOffTimer->start(KUiOffTime);
        mCoverFlow->setUiOn(TRUE);               
    }
    
    OstTraceFunctionExit0( GLXFULLSCREENVIEW_EFFECTFINISHED_EXIT );
}

void GlxFullScreenView::imageSelectionEffectFinished( const HbEffect::EffectStatus  )
{
    OstTraceFunctionEntry0( GLXFULLSCREENVIEW_IMAGESELECTIONEFFECTFINISHED_ENTRY );
    
    mIconItem->resetTransform();
    mIconItem->setVisible( false );
    
    QVariant variant = mModel->data( mModel->index(0,0), GlxFocusIndexRole );    
    if ( variant.isValid() &&  variant.canConvert<int> ()  ) {
        mCoverFlow->indexChanged( variant.value<int>() ) ;         
    }

    OstTraceFunctionExit0( GLXFULLSCREENVIEW_IMAGESELECTIONEFFECTFINISHED_EXIT );
}

void GlxFullScreenView::setLayout()
{
    OstTraceFunctionEntry0( GLXFULLSCREENVIEW_SETLAYOUT_ENTRY );
    
    QSize sz = screenSize();

    mCoverFlow->setItemSize(sz);
 	mZoomWidget->setWindowSize(sz);

    OstTraceFunctionExit0( GLXFULLSCREENVIEW_SETLAYOUT_EXIT );
}

void GlxFullScreenView::addConnection()
{
    OstTraceFunctionEntry0( GLXFULLSCREENVIEW_ADDCONNECTION_ENTRY );
    
    if ( mCoverFlow ) {    
        connect( mCoverFlow, SIGNAL( coverFlowEvent( GlxCoverFlowEvent ) ), this, SLOT( coverFlowEventHandle( GlxCoverFlowEvent ) ) );
        connect( mCoverFlow, SIGNAL(changeSelectedIndex(const QModelIndex &)), this, SLOT( changeSelectedIndex( const QModelIndex & )));
    }

    if ( mImageStrip ) {
        connect(mImageStrip, SIGNAL( activated(const QModelIndex &) ), this, SLOT( indexChanged(const QModelIndex &) ));
        connect(mImageStrip, SIGNAL( scrollingStarted()),  this, SLOT( scrollingStarted()));
        connect(mImageStrip, SIGNAL( scrollingEnded()),    this, SLOT( scrollingEnded()));
        connect(mImageStrip, SIGNAL( pressed(const QModelIndex &) ),      this, SLOT( pressed(const QModelIndex &) ));
        connect(mImageStrip, SIGNAL( released(const QModelIndex &) ),     this, SLOT( released(const QModelIndex &) ));
    }

    if ( mUiOffTimer ) {
        connect(mUiOffTimer, SIGNAL(timeout()), this, SLOT(hideUi()));
    }
    
    if(mCoverFlow && mZoomWidget) {
		connect(mZoomWidget,SIGNAL( pinchGestureReceived(int) ), mCoverFlow, SLOT( zoomStarted(int) ) );
		connect(mZoomWidget,SIGNAL( zoomWidgetMovedBackground(int) ), mCoverFlow, SLOT( zoomFinished(int) ) );
        connect(mCoverFlow,SIGNAL( doubleTapEventReceived(QPointF) ), mZoomWidget, SLOT( animateZoomIn(QPointF) ) );
	}

    connect(mWindow, SIGNAL(orientationChanged(Qt::Orientation)), this, SLOT(orientationChanged(Qt::Orientation)));

    OstTraceFunctionExit0( GLXFULLSCREENVIEW_ADDCONNECTION_EXIT );
}

void GlxFullScreenView::removeConnection()
{
    OstTraceFunctionEntry0( GLXFULLSCREENVIEW_REMOVECONNECTION_ENTRY );    

    if ( mCoverFlow ) {    
        GLX_LOG_INFO("GlxFullScreenView::removeConnection() mCoverFlow " );
        disconnect( mCoverFlow, SIGNAL( coverFlowEvent( GlxCoverFlowEvent ) ), this, SLOT( coverFlowEventHandle( GlxCoverFlowEvent ) ) );
        disconnect( mCoverFlow, SIGNAL(changeSelectedIndex(const QModelIndex &)), this, SLOT( changeSelectedIndex( const QModelIndex & )));
    }

    //For Image Strip
    if ( mImageStrip ) {
        GLX_LOG_INFO("GlxFullScreenView::removeConnection() mImageStrip " );
        disconnect(mImageStrip, SIGNAL( activated(const QModelIndex &) ), this, SLOT( indexChanged(const QModelIndex &) ));
        disconnect(mImageStrip, SIGNAL( scrollingStarted()), this, SLOT( scrollingStarted()));
        disconnect(mImageStrip, SIGNAL( scrollingEnded()), this, SLOT( scrollingEnded()));
        disconnect(mImageStrip, SIGNAL( pressed(const QModelIndex &) ), this, SLOT( pressed(const QModelIndex &) ));
        disconnect(mImageStrip, SIGNAL( released(const QModelIndex &) ), this, SLOT( released(const QModelIndex &) ));
    }

    if ( mUiOffTimer ) {
        GLX_LOG_INFO("GlxFullScreenView::removeConnection() mUiOffTimer  " );
        disconnect(mUiOffTimer, SIGNAL(timeout()), this, SLOT(hideUi()));
    }
            
	if( mCoverFlow && mZoomWidget ) {
		disconnect(mZoomWidget,SIGNAL( pinchGestureReceived(int) ), mCoverFlow, SLOT( zoomStarted(int) ) );
		disconnect(mZoomWidget,SIGNAL( zoomWidgetMovedBackground(int) ), mCoverFlow, SLOT( zoomFinished(int) ) );
	}
    
	disconnect(mWindow, SIGNAL(orientationChanged(Qt::Orientation)), this, SLOT(orientationChanged(Qt::Orientation)));

    OstTraceFunctionExit0( GLXFULLSCREENVIEW_REMOVECONNECTION_EXIT );
}

void GlxFullScreenView::setImageStripModel()
{
    OstTraceFunctionEntry0( GLXFULLSCREENVIEW_SETIMAGESTRIPMODEL_ENTRY );
    
    mImageStrip->setModel( mModel );
    QVariant variant = mModel->data( mModel->index(0,0), GlxFocusIndexRole );    
    if ( variant.isValid() &&  variant.canConvert<int> () ) {
        mImageStrip->setCurrentIndex  ( mModel->index( variant.value<int>(), 0) );  
    }
    
    OstTraceFunctionExit0( GLXFULLSCREENVIEW_SETIMAGESTRIPMODEL_EXIT );
}

GlxFullScreenView::~GlxFullScreenView()
{
    OstTraceFunctionEntry0( DUP1_GLXFULLSCREENVIEW_GLXFULLSCREENVIEW_ENTRY );
    
	cleanUp();
	
	delete mIconItem ;
    delete mImageStrip;
    delete mFullScreenToolBar;
    delete mCoverFlow;
    delete mZoomWidget;

    if(mDocLoader != NULL) {
        mDocLoader->reset();
        delete mDocLoader;    
    }
    
    HbEffect::remove( QString("HbGridView"), QString(":/data/transitionup.fxml"), QString( "TapShow" ));
    HbEffect::remove( QString("HbGridView"), QString(":/data/transitiondown.fxml"), QString( "TapHide" ));
    HbEffect::remove( QString("HbGridViewItem"), QString(":/data/gridtofullscreenhide.fxml"), QString( "Select" ));

    OstTraceFunctionExit0( DUP1_GLXFULLSCREENVIEW_GLXFULLSCREENVIEW_EXIT );
}

void GlxFullScreenView::imageSelectionAnimation(const QModelIndex &index)
{
    OstTraceFunctionEntry0( GLXFULLSCREENVIEW_IMAGESELECTIONANIMATION_ENTRY );

    if ( mIconItem == NULL ) {
        mIconItem = new HbIconItem( mCoverFlow );
        mIconItem->setBrush( QBrush( Qt::black ) );
        mIconItem->setZValue( mCoverFlow->zValue() );
    }
    
    HbAbstractViewItem *mItem = mImageStrip->itemByIndex( index );    
    mIconItem->setSize( mItem->size() );    
    mIconItem->setPos( mItem->sceneTransform().map( QPoint(0,0)).x() , screenSize().height() - 2 * mItem->size().height() ); 
    mIconItem->setVisible( true );
    
    QVariant variant = mModel->data( index, Qt::DecorationRole );
    if ( variant.isValid() &&  variant.canConvert<HbIcon> () ) {
        mIconItem->setIcon ( variant.value<HbIcon>() ) ; 
    }
    else {
        mIconItem->setIcon( HbIcon() );
    }    
    HbEffect::start( mIconItem, QString("HbGridViewItem"), QString("Select"), this, "imageSelectionEffectFinished" ); 

    OstTraceFunctionExit0( GLXFULLSCREENVIEW_IMAGESELECTIONANIMATION_EXIT );
}
	
void GlxFullScreenView::handleToolBarAction()
{
    OstTraceFunctionEntry0( GLXFULLSCREENVIEW_HANDLETOOLBARACTION_ENTRY );
    
    HbAction *action = qobject_cast<HbAction*>(sender());
    qint32 commandId = action->data().toInt();
    emit actionTriggered( commandId );
    
    OstTraceFunctionExit0( GLXFULLSCREENVIEW_HANDLETOOLBARACTION_EXIT );
}

int GlxFullScreenView::getSubState()
{
	int substate = NO_FULLSCREEN_S;	
	
	if ( mModel  ) {	
        QVariant variant = mModel->data( mModel->index(0,0), GlxSubStateRole );    
        if ( variant.isValid() &&  variant.canConvert<int> ()  ) {
            substate = variant.value<int>();
        }
	}
	return substate;
}

bool GlxFullScreenView::eventFilter(QObject *obj, QEvent *ev)
{
    GLX_LOG_INFO1("GlxFullScreenView::event() %d event type", ev->type());
    if ( ev->type() ==  QEvent::ApplicationActivate && mTvOutWrapper) {
        GLX_LOG_INFO("GlxFullScreenView::event() shift to native - CGlxHdmi");
        mTvOutWrapper->setToNativeMode();    
    }
    if ( ev->type() ==  QEvent::ApplicationDeactivate && mTvOutWrapper) {
        GLX_LOG_INFO("GlxFullScreenView::event() shift to Clone - CGlxHdmi");
        mTvOutWrapper->setToCloningMode();    
    }
    return HbView::eventFilter(obj,ev);
}

