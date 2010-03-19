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
#include <QDebug>
#include <QTimer>
#include <hbmenu.h>
#include <hbgridview.h>
#include <hbpushbutton.h>
#include <hbmainwindow.h>
#include <shareuidialog.h>
#include <hbdocumentloader.h>
#include <QAbstractItemModel>
#include <hbabstractviewitem.h>


//User Includes
#include "glxviewids.h"  //contains the view ids of all the views in photos app.
#include "glxmodelparm.h"
#include "glxcoverflow.h"
#include "glxzoomslider.h"
#include "glxzoomcontrol.h"
#include "glxdocloaderdefs.h" //contains the definations of path, widget names and view names in docml
#include "glxloggerenabler.h"
#include "glxfullscreenview.h" 
#include "glxcommandhandlers.hrh"
#include <glxhdmicontroller.h>
#include <glxlog.h>
#include <glxtracer.h>


const int KUiOffTime = 3000;

GlxFullScreenView::GlxFullScreenView(HbMainWindow *window,HbDocumentLoader *DocLoader) : GlxView ( GLX_FULLSCREENVIEW_ID),
                   mModel(NULL), mWindow( window), mCoverFlow(NULL) , mImageStrip (NULL), mUiOffTimer(NULL),
                   mZmPushButton(NULL), mZoomSlider(NULL), mZoomControl(NULL),mSendUserActivityEvent(false),
                   mFlipPushButton(NULL), nbrCol(0),iHdmiController(NULL)
{
    TRACER("GlxFullScreenView::GlxFullScreenView()" );
    mDocLoader = DocLoader;
    setContentFullScreen( true );
    HbEffect::add( QString("HbGridView"), QString(":/data/transitionup.fxml"), QString( "TapShow" ));
    HbEffect::add( QString("HbGridView"), QString(":/data/transitiondown.fxml"), QString( "TapHide" ));
}

void GlxFullScreenView::initializeView(QAbstractItemModel *model)
{
    TRACER("GlxFullScreenView::initializeView()" );

    //Load/Retrieve the Widgets
    loadWidgets();

    //Initialize the coverflow and partially creates the coverflow with one image
    // to make the widget light weight in order to make
    // transition smooth 
    QRect screen_rect = mWindow->geometry(); 
    QSize sz(screen_rect.width(), screen_rect.height());
    mCoverFlow->partiallyCreate( model, sz);     
}

void GlxFullScreenView::loadWidgets()
    {
    TRACER("GlxFullScreenView::loadWidgets()");
    //Load the View and Widget   
    GlxFullScreenView *view  =   static_cast<GlxFullScreenView*> (mDocLoader->findWidget(GLXFULLSCREEN_VIEW));
    mCoverFlow               =   static_cast<GlxCoverFlow*> (mDocLoader->findWidget(GLXFULLSCREEN_COVERFLOW));

    //initialise the cover flow for basic connections and the rest
    mCoverFlow->setCoverFlow();

    //Load the IMAGE STRIP & FULLSCREEN TOOLBAR[PUSH BUTTON]
    mImageStrip              =   static_cast<HbGridView*> (mDocLoader->findWidget(GLXFULLSCREEN_FILMSTRIP));
    mFlipPushButton          =   static_cast<HbPushButton*> (mDocLoader->findWidget(GLXFULLSCREEN_FLIP));

    //When the widget is loaded/retreived the widgets are shown by default.
    //@to do : hide the widgets by default in docml
    mImageStrip->hide();
    mFlipPushButton->hide();        
    }

void GlxFullScreenView::activate()
    {
    TRACER("GlxFullScreenView::activate()" );

    mWindow->setItemVisible(Hb::AllItems, FALSE) ;

    if(!mCoverFlow)
        {
        //retrives the widgets
        loadWidgets();
        }

    loadViewSection();

    if(mZmPushButton == NULL) {
    mZmPushButton = new HbPushButton("ZM",this);
    }

    if(NULL == mZoomSlider) {
    mZoomSlider = new GlxZoomSlider(this);
    }

    if(NULL == mZoomControl) {
    mZoomControl = new GlxZoomControl(this);
    }

    addConnection(); 
    hideUi();
    setLayout();
    mSendUserActivityEvent = true;  
    if (!iHdmiController)    
        {
        GLX_LOG_INFO("GlxFullScreenView::activate() - CGlxHdmi" );
        iHdmiController = CGlxHdmiController::NewL();
        }   
    }

void GlxFullScreenView::loadViewSection()
    {
    TRACER("GlxFullScreenView::loadViewSection()" );
    bool loaded =true;
    if( mWindow->orientation() == Qt::Horizontal )
        {
        //Load the Landscape section for Horizontal
        mDocLoader->load(GLXFULLSCREENDOCMLPATH,GLXFULLSCREENLSSECTION,&loaded);
        }
    else
        {
        //Load the Portrait section for Horizontal
        mDocLoader->load(GLXFULLSCREENDOCMLPATH,GLXFULLSCREENPTSECTION,&loaded);    
        }
    }


void GlxFullScreenView::resetView()
    {
    TRACER("GlxFullScreenView::resetView()" );

    //Clear the 4 icons present in the Coverflow,so that the transition between the views are smooth
    mCoverFlow->partiallyClean();

    //Clean up the rest of the resources allocated
    cleanUp();    
    }

void GlxFullScreenView::deActivate()
    { 
    TRACER("GlxFullScreenView::deActivate()" );

    mWindow->setItemVisible(Hb::AllItems , TRUE);

    //Clean up the rest of the resources allocated
    cleanUp();

    //deletes the iconitems in the coverflow   
    mCoverFlow->ClearCoverFlow();

    //the coverflow is not deleted as it is loaded by document loader
    //the coverflow is initialised to null 
    //to just reset to the initial state
    mCoverFlow = NULL;
    }

void GlxFullScreenView::cleanUp()
    { 
    TRACER("GlxFullScreenView::cleanUp()" );

    removeConnection();

    if ( mUiOffTimer ) {
    GLX_LOG_INFO("GlxFullScreenView::cleanUp() mUiOffTimer" );
    mUiOffTimer->stop();
    delete mUiOffTimer;
    mUiOffTimer = NULL;
    }

    mSendUserActivityEvent = false;

    if(mZmPushButton) {
    GLX_LOG_INFO("GlxFullScreenView::cleanUp() delete mZmPushButton " );
    delete mZmPushButton;
    mZmPushButton = NULL;
    }

    if(mZoomSlider) {
    GLX_LOG_INFO("GlxFullScreenView::cleanUp() delete mZoomSlider " );
    delete mZoomSlider;        
    mZoomSlider = NULL;
    }

    if(mZoomControl) {
    GLX_LOG_INFO("GlxFullScreenView::cleanUp() delete mZoomControl " );
    delete mZoomControl;
    mZoomControl = NULL;
    }
    if (iHdmiController) {
    GLX_LOG_INFO("GlxFullScreenView::cleanUp() delete iHdmiController " );
    delete iHdmiController;
    iHdmiController = NULL;
    }

    }

QGraphicsItem * GlxFullScreenView::getAnimationItem(GlxEffect transitionEffect)
    {
    TRACER("GlxFullScreenView::getAnimationItem(GlxEffect transitionEffect)");
    if ( transitionEffect == GRID_TO_FULLSCREEN 
            || transitionEffect == FULLSCREEN_TO_DETAIL
            || transitionEffect == DETAIL_TO_FULLSCREEN ) {
            return this;
    }

    return NULL;    
    }

void GlxFullScreenView::setModel( QAbstractItemModel *model )
    {
    TRACER("GlxFullScreenView::setModel( QAbstractItemModel *model )");
    GLX_LOG_INFO2("GlxFullScreenView::setModel() model %u  mModel %u", model, mModel );

    mModel = model;     
    mCoverFlow->setModel(mModel);
    mZoomSlider->setModel(mModel);
    mZoomControl->setModel(mModel);    
    setImageStripModel();
    SetImageToHdmiL(); // for the first image on screen
    }

void GlxFullScreenView::setModelContext()
    {
    TRACER("GlxFullScreenView::setModelContext()");
    if ( mModel && mWindow ) {
    GLX_LOG_INFO1("GlxFullScreenView::setModelContext %d", mWindow->orientation() );

    if ( mWindow->orientation() == Qt::Horizontal ) {
    WRITE_TIMESTAMP("set the fullscreen landscape context")
    mModel->setData(QModelIndex(), (int)GlxContextLsFs, GlxContextRole );
    }
    else {
    WRITE_TIMESTAMP("set the fullscreen portrait context")
    mModel->setData(QModelIndex(), (int)GlxContextPtFs, GlxContextRole );
    }
    }
    }

void GlxFullScreenView::orientationChanged(Qt::Orientation orient)
    {
    TRACER("GlxFullScreenView::orientationChanged()");
    // fullscreen thumnail size is depends on orientation of phone so change the model context before changing the layout
    GLX_LOG_INFO1("GlxFullScreenView::orientationChanged() %d", orient );
    setModelContext();
    loadViewSection();
    setLayout();
    }

void GlxFullScreenView::activateUI()
    {
    TRACER("GlxFullScreenView::activateUI()");
    GLX_LOG_INFO1("GlxFullScreenView::activateUI() %d", mUiOff );
    if(mSendUserActivityEvent){
    mSendUserActivityEvent = false;
    emit cancelTimer();
    }
    if ( mUiOff ){

    if ( mUiOffTimer == NULL ) {
    mUiOffTimer = new QTimer();
    connect(mUiOffTimer, SIGNAL(timeout()), this, SLOT(hideUi()));
    }

    //set and scrool the current index to the center
    QVariant variant = mModel->data( mModel->index(0,0), GlxFocusIndexRole );    
    if ( variant.isValid() &&  variant.canConvert<int> ()  ) {
    mImageStrip->setCurrentIndex ( mModel->index( variant.value<int>(), 0) );    
    mImageStrip->scrollTo( mModel->index( variant.value<int>(), 0), HbGridView::PositionAtCenter ); 
    } 
    mImageStrip->show();

    //mZmPushButton->show();
    mFlipPushButton->setZValue( 6 + mCoverFlow->zValue() ); 
    mFlipPushButton->show();
    mZmPushButton->hide();
    mUiOff = FALSE;
    mUiOffTimer->start(KUiOffTime);
    mCoverFlow->setUiOn(TRUE);
    mWindow->setItemVisible(Hb::AllItems, TRUE) ;
    HbEffect::start(mImageStrip, QString("HbGridView"), QString("TapShow"), this, "effectFinished" );        
    }
    else {
    hideUi();
    }	
    }

void GlxFullScreenView::hideUi()
    {
    TRACER("GlxFullScreenView::hideUi()");
    GLX_LOG_INFO1("GlxFullScreenView::hideUi() %d", mUiOff );
    if(mSendUserActivityEvent){
    mSendUserActivityEvent = false;
    emit cancelTimer();
    }
    HbMenu *menuWidget = menu();

    //if option menu is open then no need to hide the status bar and image strip
    if ( menuWidget->isVisible() ) { 
    return ;
    }

    mWindow->setItemVisible(Hb::AllItems, FALSE) ;
    if ( mImageStrip ) {
    HbEffect::start(mImageStrip, QString("HbGridView"), QString("TapHide"), this, "effectFinished" );
    //mImageStrip->hide();
    }
    mZmPushButton->hide();  

    if(mFlipPushButton){
    mFlipPushButton->hide();
    }

    if ( mUiOffTimer ) {
    mUiOffTimer->stop();
    }
    mUiOff = TRUE;
    mCoverFlow->setUiOn(FALSE);
    }

void GlxFullScreenView::changeSelectedIndex(const QModelIndex &index)
    {
    TRACER("GlxFullScreenView::changeSelectedIndex()");
    GLX_LOG_INFO1("GlxFullScreenView::changeSelectedIndex() %d", index.row() );
    QVariant variant = mModel->data( mModel->index(0,0), GlxFocusIndexRole );    
    if ( variant.isValid() &&  variant.canConvert<int> () && ( index.row() == variant.value<int>() ) ) {
    return;
    }         
    mModel->setData( index, index.row(), GlxFocusIndexRole );    
    mZoomSlider->indexChanged(index.row());
    // mZoomControl->indexChanged(index.row());
    SetImageToHdmiL(); // for the image changed on swipe
    }

void GlxFullScreenView::indexChanged(const QModelIndex &index)
    {
    TRACER("GlxFullScreenView::indexChanged()");
    GLX_LOG_INFO1("GlxFullScreenView::indexChanged() %d", index.row() );
    QVariant variant = mModel->data( mModel->index(0,0), GlxFocusIndexRole );    
    if ( variant.isValid() &&  variant.canConvert<int> () && ( index.row() == variant.value<int>() ) ) {
    return;
    }         
    mModel->setData( index, index.row(), GlxFocusIndexRole );
    mCoverFlow->indexChanged(index.row());
    mZoomSlider->indexChanged(index.row());
    mImageStrip->scrollTo(index, HbGridView::PositionAtCenter );
    //mZoomControl->indexChanged(index.row());
    SetImageToHdmiL();  // for the indexchnaged through filmstrip
    }

void GlxFullScreenView::scrollingStarted()
    {
    TRACER("GlxFullScreenView::scrollingStarted() ");
    //no need to hide the ui during image strip scrolling
    if ( mUiOffTimer ) {
    mUiOffTimer->stop();
    }
    }

void GlxFullScreenView::scrollingEnded()
    {
    TRACER("GlxFullScreenView::scrollingEnded() ");
    if ( mUiOffTimer ) {
    mUiOffTimer->start(KUiOffTime);
    }
    setVisvalWindowIndex();
    }

void GlxFullScreenView::pressed(const QModelIndex &index)
    {
    Q_UNUSED(index)
    TRACER("GlxFullScreenView::pressed() ");
    if ( mUiOffTimer ) {
    mUiOffTimer->stop();
    }
    }

void GlxFullScreenView::released(const QModelIndex &index)
    {
    Q_UNUSED(index)
    TRACER("GlxFullScreenView::released() ");

    if ( mUiOffTimer && mImageStrip->isScrolling() == false ) {
    mUiOffTimer->start(KUiOffTime);
    }
    }

void GlxFullScreenView::setVisvalWindowIndex()
    {
    TRACER("GlxFullScreenView::setVisvalWindowIndex()");
    QList< HbAbstractViewItem * >  visibleItemList =  mImageStrip->visibleItems();

    GLX_LOG_INFO1("GlxFullScreenView::setVisvalWindowIndex() %d", visibleItemList.count());    
    if ( visibleItemList.count() <= 0 )
        return ;

    HbAbstractViewItem *item = visibleItemList.at(0);
    GLX_LOG_INFO1("GlxFullScreenView::setVisvalWindowIndex()1 item %u", item);
    if ( item == NULL ) 
        return ;

    GLX_LOG_INFO1("GlxFullScreenView::setVisvalWindowIndex() item %u", item);

    GLX_LOG_INFO1("GlxFullScreenView::setVisvalWindowIndex() visual index %d", item->modelIndex().row() );    
    if (  item->modelIndex().row() < 0 || item->modelIndex().row() >= mModel->rowCount() )
        return ;

    mModel->setData( item->modelIndex (), item->modelIndex().row(), GlxVisualWindowIndex);
    }

void GlxFullScreenView::coverFlowEventHandle( GlxCoverFlowEvent e )
    {
    TRACER("GlxFullScreenView::coverFlowEventHandle()");
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
}

void GlxFullScreenView::setLayout()
    {
    TRACER("GlxFullScreenView::setLayout()");
    //TO:DO read form layout doc
    GLX_LOG_INFO1("GlxFullScreenView::setLayout() %d", mWindow->orientation() ); 

    QRect screen_rect = mWindow->geometry(); 
    QSize sz(screen_rect.width(), screen_rect.height());


    mCoverFlow->setItemSize(sz);
    mZoomControl->setGeometry(screen_rect);
    mZoomControl->setWindowSize(sz);

    mZmPushButton->setZValue(6);
    mZmPushButton->setGeometry(QRectF(0,50,50,50));    
    }

void GlxFullScreenView::addConnection()
    {
    TRACER("GlxFullScreenView::addConnection() " );
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
    //for Zoom
    if(mZmPushButton &&mZoomSlider) {
    connect(mZmPushButton, SIGNAL(clicked()), mZoomSlider, SLOT(toggleSliderVisibility()));
    }

    if(mFlipPushButton)
        {
        connect(mFlipPushButton, SIGNAL(clicked()), this, SLOT(showdetailsview()), Qt::QueuedConnection);
        }


    if(mZoomControl && mZoomSlider) {
    connect(mZoomSlider, SIGNAL(initialZoomFactor(int)), mZoomControl, SLOT(initialZoomFactor(int)));
    connect(mZoomSlider, SIGNAL(valueChanged(int)), mZoomControl, SLOT(zoomImage(int)));
    connect(mZoomControl, SIGNAL(hideFullScreenUi()), this, SLOT(hideUi()));
    }

    connect(mWindow, SIGNAL(orientationChanged(Qt::Orientation)), this, SLOT(orientationChanged(Qt::Orientation)));
    }

void GlxFullScreenView::removeConnection()
    {
    TRACER("GlxFullScreenView::removeConnection() " );

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

    //for Zoom
    if(mZmPushButton &&mZoomSlider) {
    GLX_LOG_INFO("GlxFullScreenView::removeConnection() mZoomSlider " );
    disconnect(mZmPushButton, SIGNAL(clicked()), mZoomSlider, SLOT(toggleSliderVisibility()));
    }

    if(mZoomControl &&mZoomSlider) {
    GLX_LOG_INFO("GlxFullScreenView::removeConnection() mZoomSlider  " );
    disconnect(mZoomSlider, SIGNAL(initialZoomFactor(int)), mZoomControl, SLOT(initialZoomFactor(int)));
    disconnect(mZoomSlider, SIGNAL(valueChanged(int)), mZoomControl, SLOT(zoomImage(int)));
    disconnect(mZoomControl, SIGNAL(hideFullScreenUi()), this, SLOT(hideUi()));
    }

    //for Details view launching
    if(mFlipPushButton) {
    GLX_LOG_INFO("GlxFullScreenView::removeConnection() mFlipPushButton  " );
    disconnect(mFlipPushButton, SIGNAL(clicked()), this, SLOT(showdetailsview()));
    }

    disconnect(mWindow, SIGNAL(orientationChanged(Qt::Orientation)), this, SLOT(orientationChanged(Qt::Orientation)));
    }

void GlxFullScreenView::setImageStripModel()
    {
    TRACER("GlxFullScreenView::setImageStripModel()" );
    if ( mModel && mImageStrip ) {
    mImageStrip->setModel( mModel );
    QVariant variant = mModel->data( mModel->index(0,0), GlxFocusIndexRole );    
    if ( variant.isValid() &&  variant.canConvert<int> () ) {
    mImageStrip->setCurrentIndex  ( mModel->index( variant.value<int>(), 0) );  
    } 
    }
    }

GlxFullScreenView::~GlxFullScreenView()
    {
    TRACER("GlxFullScreenView::~GlxFullScreenView()" );
	cleanUp();
    
	if(mImageStrip) {
    GLX_LOG_INFO("GlxFullScreenView::cleanUp() delete mImageStrip " );
    delete mImageStrip;
    mImageStrip = NULL;
    }

    if(mFlipPushButton) {
    GLX_LOG_INFO("GlxFullScreenView::cleanUp() delete mFlipPushButton " );
    delete mFlipPushButton;
    mFlipPushButton = NULL;
    }

    if(mCoverFlow) {
    GLX_LOG_INFO("GlxFullScreenView::~GlxFullScreenView() delete mCoverFlow " );
    delete mCoverFlow;
    mCoverFlow = NULL;
    }

    if(mDocLoader != NULL)
        {
        mDocLoader->reset();
        delete mDocLoader;    
        }
    HbEffect::remove( QString("HbGridView"), QString(":/data/transitionup.fxml"), QString( "TapShow" ));
    HbEffect::remove( QString("HbGridView"), QString(":/data/transitiondown.fxml"), QString( "TapHide" ));  
    }

void GlxFullScreenView::showdetailsview()
    {
    TRACER("GlxFullScreenView::showdetailsview()" );
    //trigger the details view to be shown
    emit actionTriggered( EGlxCmdDetailsOpen );
    }

void GlxFullScreenView::handleUserAction(qint32 commandId)
    {
    TRACER("GlxFullScreenView::handleUserAction()" );
    switch( commandId )
        {
        case EGlxCmdRotate :
            //trigger the rotate Effect in CoverFlow
            mCoverFlow->rotateImage();
            break;
        case EGlxCmdSend:
            {
            QString imagePath = (mModel->data(mModel->index(mModel->data(mModel->index(0,0),GlxFocusIndexRole).value<int>(),0),GlxUriRole)).value<QString>();
            if(imagePath.isNull())
                {
                GLX_LOG_INFO("GlxFullScreenView::SendUi() path is null" );
                }
            qDebug() << "GlxFullScreenView::SendUi() imagePath= " << imagePath;

            ShareUi dialog;
            QList <QVariant> fileList;
            fileList.append(QVariant(imagePath));
            dialog.init(fileList,true);
            }
            break;
        default :
            break;
        }
    }

void GlxFullScreenView::SetImageToHdmiL()
    {
    TRACER("GlxFullScreenView::SetImageToHdmiL() - CGlxHdmi 1" );
    if (iHdmiController)
        {
        GLX_LOG_INFO("GlxFullScreenView::SetImageToHdmiL() - CGlxHdmi 2" );
        // Get the image uri
        QString imagePath = (mModel->data(mModel->index(mModel->data(mModel->index(0,0),GlxFocusIndexRole).value<int>(),0),GlxUriRole)).value<QString>();
        if(imagePath.isNull())
            {
            GLX_LOG_INFO("GlxFullScreenView::SetImageToHdmiL() path is null" );
            }
        qDebug() << "GlxFullScreenView::SetImageToHdmiL() imagePath= " << imagePath;
        TPtrC aPtr = reinterpret_cast<const TUint16*>(imagePath.utf16());

        // Get the image Dimensions
        QSize imageDimension = (mModel->data(mModel->index(mModel->data(mModel->index(0,0),GlxFocusIndexRole).value<int>(),0),GlxDimensionsRole)).value<QSize>();
        TSize imageSize(imageDimension.width(),imageDimension.height());
        
        // Get the framecount
        int frameCount = (mModel->data(mModel->index(mModel->data(mModel->index(0,0),GlxFocusIndexRole).value<int>(),0),GlxFrameCount)).value<int>();
        iHdmiController->SetImageL(aPtr,imageSize, frameCount);
        }
    }
