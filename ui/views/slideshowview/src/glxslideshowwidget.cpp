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
#include <e32base.h>
#include <hbiconitem.h>
#include <hbpushbutton.h>
#include <hbmainwindow.h>
#include <hbdocumentloader.h>
#include <QAbstractItemModel>


//User Includes
#include "glxicondefs.h" //Contains the icon names/Ids
#include "glxmodelparm.h"
#include "glxeffectengine.h"
#include "glxdocloaderdefs.h"
#include "glxslideshowwidget.h"
#include <glxlog.h>
#include <glxtracer.h>


GlxSlideShowWidget::GlxSlideShowWidget( QGraphicsItem *parent ) : HbScrollArea(parent), 
                      mEffectEngine(NULL), mContinueButton(NULL), mItemIndex(1), 
                      mSelIndex(0), mSlideTimer(NULL), mModel(NULL)
    {
    TRACER("GlxSlideShowWidget::GlxSlideShowWidget()");
    }

void GlxSlideShowWidget::setSlideShowWidget(HbDocumentLoader *DocLoader)
    {
    //To:Do error handling
    TRACER("GlxSlideShowWidget::setSlideShowWidget()");   
    
    //create the effect engine
    mEffectEngine = new GlxSlideShowEffectEngine();

    // Now load the view and the contents.
    // and then set the play icon to the button
    mContinueButton = static_cast<HbPushButton*>(DocLoader->findWidget(GLXSLIDESHOW_PB));
    mContinueButton->setIcon(HbIcon(GLXICON_PLAY));
    mContinueButton->hide();
    mIsPause = false;

    for ( int i = 0; i < NBR_ITEM ; i++) {
    mIconItems[i] = new HbIconItem(this);
    mIconItems[i]->setBrush(QBrush(Qt::black));
    }

    mSlideTimer = new QTimer();
    mItemList.clear();    

    //Add the signal-slot for this widget.
    addConnections();

    //Create the settings for the effects 
    mEffectEngine->readSetting();
    mEffectEngine->registerEffect(QString("HbIconItem"));

    //provide the xml info for the effect to take place
    HbEffect::add( QString("HbIconItem"), QString(":/data/transition.fxml"), QString( "Move" ));
    HbEffect::add( QString("HbIconItem"), QString(":/data/transitionleft.fxml"), QString( "LeftMove" ));
    HbEffect::add( QString("HbIconItem"), QString(":/data/transitionright.fxml"), QString( "RightMove" ));
    }

GlxSlideShowWidget::~GlxSlideShowWidget()
    {
    TRACER("GlxSlideShowWidget::~GlxSlideShowWidget()");

    //Delete the resources allocated
    cleanUp();

    if(mContinueButton)
        {
        delete mContinueButton;
        mContinueButton = NULL;
        }
    }


void GlxSlideShowWidget::cleanUp()
    {
    TRACER("GlxSlideShowWidget::cleanUp()");
    removeConnections();

    if(mEffectEngine)
        {
        mEffectEngine->deRegisterEffect( QString("HbIconItem") );    
        delete mEffectEngine;
        mEffectEngine = NULL;
        }
     
    
    for ( int i = 0; i < NBR_ITEM ; i++) 
        {
        delete mIconItems[i] ;
        mIconItems[i] = NULL;
        }

    if(mSlideTimer)
        {
        delete mSlideTimer;
        mSlideTimer = NULL;
        }

    clearCurrentModel();
    HbEffect::remove( QString("HbIconItem"), QString(":/data/transition.fxml"), QString( "Move" ));
    HbEffect::remove( QString("HbIconItem"), QString(":/data/transitionleft.fxml"), QString( "LeftMove" ));
    HbEffect::remove( QString("HbIconItem"), QString(":/data/transitionright.fxml"), QString( "RightMove" ));
    }

void GlxSlideShowWidget::setModel (QAbstractItemModel *model)
    {
    TRACER("GlxSlideShowWidget::setModel()");
    if ( model == mModel ) {
    return ;
    }
    clearCurrentModel();   
    mModel = model;
    initializeNewModel();
    resetSlideShow();
    }

void GlxSlideShowWidget::setItemGeometry(QRect screenRect)
    {
    TRACER("GlxSlideShowWidget::setItemGeometry()");
    int index = mItemIndex;
    mScreenRect = screenRect;   
    mIconItems[index]->setGeometry(mScreenRect);
    index = ( mItemIndex + 1) % NBR_ITEM;
    mIconItems[index]->setGeometry( QRect( mScreenRect.width(), mScreenRect.top(), mScreenRect.width(), mScreenRect.height() ) );
    index = mItemIndex ? mItemIndex - 1 : NBR_ITEM - 1;    
    mIconItems[index]->setGeometry( QRect( -mScreenRect.width(), mScreenRect.top(), mScreenRect.width(), mScreenRect.height() ) );     
    }

void GlxSlideShowWidget::triggeredEffect()
    { 
    TRACER("GlxSlideShowWidget::triggeredEffect()"); 
    int index = mItemIndex;
    mSlideTimer->stop();
    mItemList.clear();
    User::ResetInactivityTime();

    mItemList.append( mIconItems[index] );
    if ( mEffectEngine->slideShowMoveDir() == MOVE_FORWARD ) {
    index = ( mItemIndex + 1) % NBR_ITEM;
    }
    else {
    index = mItemIndex ? mItemIndex - 1 : NBR_ITEM - 1;
    }
    mItemList.append( mIconItems[index] );

    GLX_LOG_INFO3("GlxSlideShowWidget::triggeredEffect() image selected index %d array index %d index %d", mSelIndex, mItemIndex, index);  
    mEffectEngine->runEffect( mItemList, QString("HbIconItem") );
    }


void GlxSlideShowWidget::effectFinshed()
    {
    TRACER("GlxSlideShowWidget::effectFinshed()");
    //To:Do boundery condition or last item check implemented after behaviour of slide show clear
    int rowCount = mModel->rowCount();
    GLX_LOG_INFO2("GlxSlideShowWidget::effectFinshed() before image selected index %d array index %d", mSelIndex, mItemIndex); 

    if ( mEffectEngine->slideShowMoveDir() == MOVE_FORWARD ) {
    mSelIndex = ( ++mSelIndex ) % rowCount;
    mItemIndex = ( ++mItemIndex ) % NBR_ITEM;
    }
    else {
    mSelIndex = mSelIndex ? --mSelIndex : rowCount - 1;
    mItemIndex = mItemIndex ? mItemIndex - 1 : NBR_ITEM - 1;
    }

    mModel->setData( mModel->index(mSelIndex, 0), mSelIndex, GlxFocusIndexRole );
    setIconItems(mEffectEngine->slideShowMoveDir());
    //setItemPos(mEffectEngine->slideShowMoveDir());
    GLX_LOG_INFO2("GlxSlideShowWidget::effectFinshed() after image selected index %d array index %d ", mSelIndex, mItemIndex);
    if ( mIsPause == false ) {
    mSlideTimer->start( mEffectEngine->slideDelayTime() );  
    } 
    mItemList.clear();
    emit indexchanged(); // on each item change
    }

void GlxSlideShowWidget::cancelEffect()
    {
    TRACER("GlxSlideShowWidget::cancelEffect()");
    mEffectEngine->cancelEffect( mItemList );
    }

void GlxSlideShowWidget::pauseSlideShow()
    {
    TRACER("GlxSlideShowWidget::pauseSlideShow()");
    GLX_LOG_INFO1("GlxSlideShowWidget::pauseSlideShow() %d", this->zValue());
    mIsPause = true;
    mSlideTimer->stop();
    cancelEffect();
    mContinueButton->setZValue( this->zValue() + 2);
    mContinueButton->show() ;
    emit slideShowEvent(UI_ON_EVENT);
    }

void GlxSlideShowWidget::continueSlideShow(bool checked)
    {
    Q_UNUSED( checked )
    TRACER("GlxSlideShowWidget::continueSlideShow()");
    mIsPause = false;
    if ( mModel &&  mModel->rowCount() > 1 ) {
    mSlideTimer->start( mEffectEngine->slideDelayTime() ); 
    }
    mContinueButton->hide(); 
    emit slideShowEvent(UI_OFF_EVENT);
    }

void GlxSlideShowWidget::dataChanged(QModelIndex startIndex, QModelIndex endIndex)
    {
    Q_UNUSED( endIndex )
    TRACER("GlxSlideShowWidget::dataChanged()");
    GLX_LOG_INFO2("GlxSlideShowWidget::dataChanged startIndex = %d mSelIndex = %d ", startIndex.row(), mSelIndex  );
    int deltaIndex = startIndex.row() - mSelIndex;

    if ( deltaIndex <= 1 && deltaIndex >= -1 ) {
    int index = ( mItemIndex + deltaIndex + NBR_ITEM ) % NBR_ITEM; //calculated the array index in which data sould be updated
    GLX_LOG_INFO2("GlxSlideShowWidget::dataChanged index = %d mSelItemIndex = %d ", index, mItemIndex );

    QVariant variant = mModel->data( startIndex, GlxFsImageRole );
    if ( variant.isValid() &&  variant.canConvert<HbIcon> () ) {
    mIconItems[index]->setIcon ( variant.value<HbIcon>() ) ;
    }
    else {
    mIconItems[index]->setIcon ( HbIcon() ) ; 
    }
    }	
    }

void GlxSlideShowWidget::rowsInserted(const QModelIndex &parent, int start, int end)
    {
    TRACER("GlxSlideShowWidget::rowsInserted()");
    Q_UNUSED(parent);
    Q_UNUSED(start);
    Q_UNUSED(end);
    resetSlideShow();  
    }

void GlxSlideShowWidget::rowsRemoved(const QModelIndex &parent, int start, int end)
    {
    TRACER("GlxSlideShowWidget::rowsRemoved()");
    Q_UNUSED(parent);
    Q_UNUSED(start);
    Q_UNUSED(end);

    GLX_LOG_INFO1( "GlxSlideShowWidget::rowsRemoved row count = %d ", mModel->rowCount()  );

    if ( mModel->rowCount() <= 0 ) {
    clearCurrentModel();
    emit slideShowEvent( EMPTY_DATA_EVENT );
    }
    else {
    resetSlideShow();
    }
    }

void GlxSlideShowWidget::modelDestroyed()
    {
    TRACER("GlxSlideShowWidget::modelDestroyed()");
    clearCurrentModel();
    }


void GlxSlideShowWidget::orientationChanged(QRect screenRect)
    {
    TRACER("GlxSlideShowWidget::orientationChanged()");
    cancelEffect();
    setItemGeometry( screenRect);
    resetSlideShow();
    }


void GlxSlideShowWidget::leftGesture(int value)
    {
    Q_UNUSED(value)
    TRACER("GlxSlideShowWidget::leftGesture()");

    int index = ( mItemIndex + 1 ) % NBR_ITEM;
    moveImage( index, mScreenRect.width(), QString("LeftMove"), "leftMoveEffectFinished");
    }

void GlxSlideShowWidget::rightGesture(int value)
    {
    Q_UNUSED(value)
    TRACER ("GlxSlideShowWidget::rightGesture()");

    int index = mItemIndex ? mItemIndex - 1 : NBR_ITEM - 1;
    moveImage( index, -mScreenRect.width(), QString("RightMove"), "rightMoveEffectFinished"); 
    }

void GlxSlideShowWidget::leftMoveEffectFinished( const HbEffect::EffectStatus &status )
    {
    Q_UNUSED(status)
    TRACER("GlxSlideShowWidget::leftMoveEffectFinished()");
    GLX_LOG_INFO1("GlxSlideShowWidget::leftMoveEffectFinished() %d status", status.reason);

    int rowCount = mModel->rowCount();
    mSelIndex = ( ++mSelIndex ) % rowCount;
    mItemIndex = ( ++mItemIndex ) % NBR_ITEM;
    mModel->setData( mModel->index(mSelIndex, 0), mSelIndex, GlxFocusIndexRole );

    setIconItems(MOVE_FORWARD);
    startSlideShow();
    emit indexchanged(); // on left swipe
    }

void GlxSlideShowWidget::rightMoveEffectFinished( const HbEffect::EffectStatus &status )
    {
    Q_UNUSED(status)
    TRACER ( "GlxSlideShowWidget::rightMoveEffectFinished( ) ");
    GLX_LOG_INFO1("GlxSlideShowWidget::rightMoveEffectFinished() %d status", status.reason);

    int rowCount = mModel->rowCount();
    mSelIndex = mSelIndex ? --mSelIndex : rowCount - 1;
    mItemIndex = mItemIndex ? mItemIndex - 1 : NBR_ITEM - 1;
    mModel->setData( mModel->index(mSelIndex, 0), mSelIndex, GlxFocusIndexRole );

    setIconItems(MOVE_BACKWARD);
    startSlideShow();
    emit indexchanged(); // on right swipe
    } 

void GlxSlideShowWidget::mouseReleaseEvent( QGraphicsSceneMouseEvent *event)
    {
    Q_UNUSED( event )
    TRACER ( "GlxSlideShowWidget::mouseReleaseEvent( ) ");
    GLX_LOG_INFO1 ( "GlxSlideShowWidget::mouseReleaseEvent( ) is pause %d", mIsPause);
    if ( mIsPause == false ) {
    pauseSlideShow();
    }
    }

void GlxSlideShowWidget::mousePressEvent ( QGraphicsSceneMouseEvent * event )
    {
    Q_UNUSED( event )
    TRACER ( "GlxSlideShowWidget::mousePressEvent( ) ");
    GLX_LOG_INFO1 ( "GlxSlideShowWidget::mousePressEvent( ) is pause %d", mIsPause);
    }

void GlxSlideShowWidget::startSlideShow ( )
    {
    TRACER ( "GlxSlideShowWidget::startSlideShow( ) ");
    GLX_LOG_INFO1 ( "GlxSlideShowWidget::startSlideShow( ) is pause %d", mIsPause);    
    if ( mIsPause == false && mModel &&  mModel->rowCount() > 1 ) {
    mSlideTimer->start( mEffectEngine->slideDelayTime() );  
    }    
    }

void GlxSlideShowWidget::stopSlideShow (  )
    {
    TRACER ( "GlxSlideShowWidget::stopSlideShow( ) ");
    cancelEffect();
    mSlideTimer->stop();                
    }

void GlxSlideShowWidget::clearCurrentModel()
    {
    TRACER ( "GlxSlideShowWidget::clearCurrentModel( ) ");
    if ( mModel ) {
    disconnect( mModel, SIGNAL( dataChanged(QModelIndex,QModelIndex) ), this, SLOT( dataChanged(QModelIndex,QModelIndex) ) );
    disconnect(mModel, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(rowsInserted(QModelIndex,int,int)));
    disconnect(mModel, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(rowsRemoved(QModelIndex,int,int)));
    disconnect(mModel, SIGNAL(destroyed()), this, SLOT( modelDestroyed()));
    mModel = NULL ;
    }
/*
    disconnect(mModel, SIGNAL(destroyed()), this, SLOT(_q_modelDestroyed()));
    disconnect(mModel, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)), this, SLOT(rowsAboutToBeRemoved(QModelIndex,int,int)));
 */	
    }

void GlxSlideShowWidget::initializeNewModel()
    {
    TRACER("GlxSlideShowWidget::initializeNewModel" );
    if ( mModel ) {
    connect( mModel, SIGNAL( dataChanged(QModelIndex,QModelIndex) ), this, SLOT( dataChanged(QModelIndex,QModelIndex) ) );
    connect(mModel, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(rowsInserted(QModelIndex,int,int)));
    connect(mModel, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(rowsRemoved(QModelIndex,int,int)));
    connect(mModel, SIGNAL(destroyed()), this, SLOT( modelDestroyed()));
    }	
    }


void GlxSlideShowWidget::resetSlideShow()
    {
    TRACER("GlxSlideShowWidget::resetSlideShow()" );
	if(! mModel) {
		return;
	}
    QVariant variant = mModel->data( mModel->index( mSelIndex, 0 ), GlxFocusIndexRole );
    if ( variant.isValid() &&  variant.canConvert<int> () ) {
    mSelIndex = variant.value<int>() ;
    GLX_LOG_INFO1("GlxSlideShowWidget::resetSlideShow() selected index %d", mSelIndex ); 
    }

    variant = mModel->data( mModel->index( mSelIndex, 0 ), GlxFsImageRole );
    if ( variant.isValid() &&  variant.canConvert<HbIcon> () ) {
    mIconItems[mItemIndex]->setIcon ( variant.value<HbIcon>() ) ; 
    }
    else {
    mIconItems[mItemIndex]->setIcon ( HbIcon() ) ; 
    }
    setIconItems(MOVE_FORWARD);
    setIconItems(MOVE_BACKWARD);
    if ( mIsPause == false && mModel &&  mModel->rowCount() > 1 ) {
    mSlideTimer->start( mEffectEngine->slideDelayTime() );  
    }  
    }

void GlxSlideShowWidget::setIconItems(int moveDir)
    {
    TRACER("GlxSlideShowWidget::setIconItems()");
    int index = 0, itemIndex = 0;
    int rowCount = mModel->rowCount();
    GLX_LOG_INFO1("GlxSlideShowWidget::setIconItems() rowcount %d ", rowCount);

    if ( rowCount == 0 ) {
    return ;
    }

    if (moveDir == MOVE_FORWARD) {
    index = ( mSelIndex + 1) % rowCount;
    itemIndex = ( mItemIndex + 1) % NBR_ITEM;
    }else {
    index = mSelIndex ? mSelIndex - 1 : rowCount - 1;  
    itemIndex = mItemIndex ? mItemIndex - 1 : NBR_ITEM - 1; 
    }

    GLX_LOG_INFO4("GlxSlideShowWidget::setIconItems() image selected index %d array index %d index %d icon index %d", mSelIndex, mItemIndex, index, itemIndex);

    QVariant variant = mModel->data( mModel->index( index, 0 ), GlxFsImageRole );
    if ( variant.isValid() &&  variant.canConvert<HbIcon> () ) {
    mIconItems[itemIndex]->setIcon ( variant.value<HbIcon>() ) ; 
    }
    else {
    mIconItems[itemIndex]->setIcon ( HbIcon() ) ;
    } 
    }

//To:DO it is not used so may be remove later
void GlxSlideShowWidget::setItemPos(int moveDir)
    {
    Q_UNUSED( moveDir) 
    TRACER("GlxSlideShowWidget::setItemPos()");
    GLX_LOG_INFO1("GlxSlideShowWidget::setItemPos() array index %d", mItemIndex );

    if (moveDir == MOVE_FORWARD) {
    int index = ( mItemIndex + 1) % NBR_ITEM;
    mIconItems[index]->setPos( mScreenRect.width(), mScreenRect.top() );
    }
    else {
    int index = mItemIndex ? mItemIndex - 1 : NBR_ITEM - 1;    
    mIconItems[index]->setPos( -mScreenRect.width(), mScreenRect.top() ); 
    }   
    }

void GlxSlideShowWidget::moveImage(int nextIndex, int posX, const QString & move, char * callBack)
    {
    TRACER("GlxSlideShowWidget::MoveImage()");

    if ( mModel->rowCount() <= 1 || mEffectEngine->isEffectRuning( mItemList ) ) {
    return ;
    }

    mSlideTimer->stop();
    HbEffect::start(mIconItems[mItemIndex], QString("HbIconItem"), move );
    mIconItems[nextIndex]->setPos( posX, mScreenRect.top());
    HbEffect::start(mIconItems[nextIndex], QString("HbIconItem"), QString("Move"), this, callBack );    
    }

void GlxSlideShowWidget::addConnections()
    {
    TRACER("GlxSlideShowWidget::addConnections()");
    if ( mEffectEngine )  {
    connect( mEffectEngine, SIGNAL( effectFinished() ), this, SLOT( effectFinshed() ) );
    }
    if ( mSlideTimer ) {
    connect( mSlideTimer, SIGNAL(timeout()), this, SLOT( triggeredEffect() ) );
    }
    if ( mContinueButton ) {
    connect( mContinueButton, SIGNAL( clicked(bool) ), this, SLOT( continueSlideShow(bool) ) );
    }
    }

void GlxSlideShowWidget::removeConnections()
    {
    TRACER("GlxSlideShowWidget::removeConnections()");
    if ( mEffectEngine )  {
    disconnect( mEffectEngine, SIGNAL( effectFinished() ), this, SLOT( effectFinshed() ) );
    }
    if ( mSlideTimer ) {
    disconnect( mSlideTimer, SIGNAL(timeout()), this, SLOT( triggeredEffect() ) );
    }
    if ( mContinueButton ) {
    disconnect( mContinueButton, SIGNAL( clicked(bool) ), this, SLOT( continueSlideShow(bool) ) );
    }
    }
