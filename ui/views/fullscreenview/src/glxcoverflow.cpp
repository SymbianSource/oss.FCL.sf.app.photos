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
#include <hbiconitem.h>
#include <QAbstractItemModel>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>

//User Includes
#include <glxmodelparm.h>
#include <glxcoverflow.h>
#include "glxviewids.h"

const int KMoveX = 60;  //coverflow auto move speed

GlxCoverFlow::GlxCoverFlow(QGraphicsItem *parent ) : HbScrollArea(parent), mSelItemIndex (0),
              mRows(0), mSelIndex (0), mStripLen (0), mCurrentPos(0), 
              mItemSize (QSize(0,0)), mModel ( NULL), mMoveDir(NO_MOVE), rotAngle(0) 
{
//TO:DO through exception
   qDebug("GlxCoverFlow::GlxCoverFlow");
   HbEffect::add( QString("HbIconItem"), QString(":/data/transitionrotate0.fxml"), QString( "RotateImage0" ));
   HbEffect::add( QString("HbIconItem"), QString(":/data/transitionrotate90.fxml"), QString( "RotateImage90" ));
   HbEffect::add( QString("HbIconItem"), QString(":/data/transitionrotate180.fxml"), QString( "RotateImage180" ));
   HbEffect::add( QString("HbIconItem"), QString(":/data/transitionrotate270.fxml"), QString( "RotateImage270" ));
   connect( this, SIGNAL( autoLeftMoveSignal() ), this, SLOT( autoLeftMove() ), Qt::QueuedConnection );
   connect( this, SIGNAL( autoRightMoveSignal() ), this, SLOT( autoRightMove() ), Qt::QueuedConnection );   
}

void GlxCoverFlow::setCoverFlow()
{
    qDebug("GlxCoverFlow::setCoverFlow");
    for ( qint8 i = 0; i < NBR_ICON_ITEM ; i++ ) {
        mIconItem[i] = new HbIconItem(this);
        mIconItem[i]->setBrush(QBrush(Qt::black));
        mIconItem[i]->setSize(QSize(0,0));
    }
    mUiOn = FALSE;
    mBounceBackDeltaX = 10;
}

void GlxCoverFlow::setItemSize(QSize &size)
{
    qDebug("GlxCoverFlow::setSize width = %d",size.width() );
    if( mItemSize != size) {
        mItemSize = size;
        resetCoverFlow();
    }    
}

void GlxCoverFlow::setModel(QAbstractItemModel *model)
{
    qDebug("GlxCoverFlow::setModel model change = %d", model  );
    
    if ( model == mModel) {
        return ;
    }
    
    clearCurrentModel();
    mModel = model;
    initializeNewModel();
    resetCoverFlow();
}

void GlxCoverFlow::indexChanged( int index )
{
    qDebug("GlxCoverFlow::indexChanged index = %d mSelIndex = %d ",index, mSelIndex );
    if ( index != mSelIndex && mModel) {
        loadIconItems();
		if(rotAngle) {
            rotAngle = 0;
            //HbEffect::start(mIconItem[mSelItemIndex], QString("HbIconItem"), QString("RotateImage270"), this, "rotationEffectFinished" );
            mIconItem[mSelItemIndex]->resetTransform();
            mIconItem[mSelItemIndex]->resize(mItemSize);
        }
    }
}

void GlxCoverFlow::rotateImage ()  
{
    qDebug("GlxCoverFlow::rotateImage ");  
	if(rotAngle == 0)
	{
		HbEffect::start(mIconItem[mSelItemIndex], QString("HbIconItem"), QString("RotateImage0"), this, "rotationEffectFinished" );
	}
	if(rotAngle == 90)
	{
		HbEffect::start(mIconItem[mSelItemIndex], QString("HbIconItem"), QString("RotateImage90"), this, "rotationEffectFinished" );
	}
	if(rotAngle == 180)
	{
		HbEffect::start(mIconItem[mSelItemIndex], QString("HbIconItem"), QString("RotateImage180"), this, "rotationEffectFinished" );
	}
	if(rotAngle == 270)
	{
		HbEffect::start(mIconItem[mSelItemIndex], QString("HbIconItem"), QString("RotateImage270"), this, "rotationEffectFinished" );
	}	
	rotAngle += 90;
	rotAngle %= 360;	
}

void GlxCoverFlow::rotationEffectFinished (const HbEffect::EffectStatus &status)  
{
    Q_UNUSED(status)
    
    qDebug("GlxCoverFlow::rotationEffectFinished");  
	QSize itemSize = mItemSize;
	if((rotAngle == 90) || (rotAngle == 270)) {
		itemSize.transpose();
	}
	mIconItem[mSelItemIndex]->resetTransform();
	mIconItem[mSelItemIndex]->resize(itemSize);
	mIconItem[mSelItemIndex]->setPos((mItemSize.width() - itemSize.width())/2, (mItemSize.height() - itemSize.height())/2 );
	QTransform rotateTransform = mIconItem[mSelItemIndex]->transform();
	rotateTransform.translate((mItemSize.width()/2)-((mItemSize.width() - itemSize.width())/2),(mItemSize.height()/2) - ((mItemSize.height() - itemSize.height())/2));
	rotateTransform.rotate(rotAngle);
	rotateTransform.translate(-((mItemSize.width()/2)-((mItemSize.width() - itemSize.width())/2)),-((mItemSize.height()/2) - ((mItemSize.height() - itemSize.height())/2)));
	mIconItem[mSelItemIndex]->setTransform(rotateTransform);

}

void GlxCoverFlow::panGesture ( const QPointF & delta )  
{
    qDebug("GlxCoverFlow::panGesture deltaX= %d", (int)delta.x());  
    if(getSubState() == IMAGEVIEWER_S || getSubState() == FETCHER_S )
        {
        return;
        }
	move((int) delta.x());    
    if( delta.x() > 0 ) {     
        mMoveDir = RIGHT_MOVE;
    }
    else if ( delta.x() < 0) {
        mMoveDir = LEFT_MOVE;
    }
    
    if ( mUiOn == TRUE  ) {
        emit coverFlowEvent( PANNING_START_EVENT );
        mUiOn = FALSE;
    }
}

void GlxCoverFlow::leftGesture(int value)
{
	Q_UNUSED(value);
    qDebug("GlxCoverFlow::leftGesture CurrentPos= %d value %d", mCurrentPos, value); 
    if(getSubState() == IMAGEVIEWER_S || getSubState() == FETCHER_S )
        {
        return;
        }
    mMoveDir = NO_MOVE;
    mBounceBackDeltaX = mItemSize.width() >> 2;
    emit autoLeftMoveSignal();
    if ( mUiOn == TRUE ) {
        mUiOn = FALSE;
        emit coverFlowEvent( PANNING_START_EVENT );        
    }
}

void GlxCoverFlow::rightGesture(int value)
{
	Q_UNUSED(value);
    qDebug("GlxCoverFlow::rightGesture CurrentPos= %d value %d ", mCurrentPos, value);
    if(getSubState() == IMAGEVIEWER_S || getSubState() == FETCHER_S )
        {
        return;
        }
    mMoveDir = NO_MOVE;
    mBounceBackDeltaX = mItemSize.width() >> 2;
    emit autoRightMoveSignal();
    if ( mUiOn == TRUE  ) {
        mUiOn = FALSE;
        emit coverFlowEvent( PANNING_START_EVENT );
    }     
}

void GlxCoverFlow::longPressGesture(const QPointF &point)
{
     qDebug("GlxCoverFlow::longPressGesture x = %d  y = %d", point.x(), point.y());
     mMoveDir = LONGPRESS_MOVE;
}

void GlxCoverFlow::dataChanged(QModelIndex startIndex, QModelIndex endIndex)
{
    Q_UNUSED(endIndex);
    qDebug("GlxCoverFlow::dataChanged startIndex = %d mSelIndex = %d ", startIndex.row(), mSelIndex  );
    
    int index = 0;
    for (int i = 0; i < NBR_ICON_ITEM ; i++) {
        index = calculateIndex( mSelIndex + i - 2);
        if ( index == startIndex.row() ) {
            index = ( mSelItemIndex + i - 2 + NBR_ICON_ITEM ) % NBR_ICON_ITEM;
            qDebug("GlxCoverFlow::dataChanged index = %d mSelItemIndex = %d ", index, mSelItemIndex );
            
            QVariant variant = mModel->data( startIndex, GlxFsImageRole );
            if ( variant.isValid() &&  variant.canConvert<HbIcon> () ) {
                mIconItem[index]->setIcon ( variant.value<HbIcon>() ) ; 
            }
            else {
                mIconItem[index]->setIcon( HbIcon() );
            }
        }
    }
}

void GlxCoverFlow::rowsInserted(const QModelIndex &parent, int start, int end)
{
    qDebug("GlxCoverFlow::rowsInserted");
    Q_UNUSED(parent);
    Q_UNUSED(start);
    Q_UNUSED(end);
    resetCoverFlow();  
    emit changeSelectedIndex ( mModel->index ( mSelIndex, 0 ) ) ;
}

void GlxCoverFlow::rowsRemoved(const QModelIndex &parent, int start, int end)
{
    Q_UNUSED(parent);
    Q_UNUSED(start);
    Q_UNUSED(end);
    qDebug("GlxCoverFlow::rowsRemoved model %d", mModel);
    
    if ( mModel->rowCount() <= 0 ) {
        emit coverFlowEvent( EMPTY_ROW_EVENT );
    }
    else {
        resetCoverFlow();   
        emit changeSelectedIndex ( mModel->index ( mSelIndex, 0 ) ) ;
    }
}

void GlxCoverFlow::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);
    qDebug("GlxCoverFlow::mousePressEvent");
    mMoveDir = TAP_MOVE;
}

void GlxCoverFlow::mouseReleaseEvent (QGraphicsSceneMouseEvent *event) 
{
    Q_UNUSED(event);
    qDebug("GlxCoverFlow::mouseReleaseEvent move dir %d", mMoveDir);
    
    switch( mMoveDir ) {
    case NO_MOVE :
    	break;
    	
    case TAP_MOVE :
	    mMoveDir = NO_MOVE;
	    emit coverFlowEvent( TAP_EVENT );
    	break;
    	
    case LEFT_MOVE: 
        mMoveDir = NO_MOVE;
        emit autoLeftMoveSignal();
        break ;
        
    case RIGHT_MOVE :
        mMoveDir = NO_MOVE;
        emit autoRightMoveSignal();
        break;
        
    default:
        break;
    } 
}

void GlxCoverFlow::autoLeftMove()
{
    qDebug("GlxCoverFlow::autoLeftMove ");
    int width = mItemSize.width() ;
    
    qDebug("GlxCoverFlow::autoLeftMove current pos = %d mBounceBackDeltaX x = %d", mCurrentPos, mBounceBackDeltaX);
    //for bounce back effect for last image ( it will do the back)
    if ( ( mCurrentPos + width ) > ( mStripLen + mBounceBackDeltaX ) && mMoveDir == NO_MOVE ) {
        mMoveDir = LEFT_MOVE;
        mBounceBackDeltaX = 10;
        autoRightMoveSignal();
        return ;
    }
    
    int deltaX = width - mCurrentPos %  width ; 
    int moveX = deltaX > KMoveX ? KMoveX : deltaX;
    
    qDebug("GlxCoverFlow::autoLeftMove delta x = %d current pos = %d move x = %d", deltaX, mCurrentPos, moveX);
    
    move (-moveX);//move the image
    deltaX -= moveX; //To know the auto move is required or not
    
    if ( deltaX ) {
        emit autoLeftMoveSignal();
    }
    else {
        //for bounce back of first image
        if ( mMoveDir == RIGHT_MOVE ) {
            emit autoRightMoveSignal();
            return;
        }
        int selIndex = mCurrentPos / width ;
        if ( mRows == 1 || selIndex != mSelIndex ) {
			if(rotAngle) {
				rotAngle = 0;
				//HbEffect::start(mIconItem[mSelItemIndex], QString("HbIconItem"), QString("RotateImage270"), this, "rotationEffectFinished" );
				mIconItem[mSelItemIndex]->resetTransform();
				mIconItem[mSelItemIndex]->resize(mItemSize);
				mIconItem[mSelItemIndex]->setPos(- mItemSize.width(), 0);
			}
            mSelIndex = selIndex;
            mSelItemIndex = ( ++mSelItemIndex ) % NBR_ICON_ITEM;
            selIndex = ( mSelItemIndex + 2 ) % NBR_ICON_ITEM;
            updateIconItem( mSelIndex + 2, selIndex, width * 2 ) ;
            emit changeSelectedIndex ( mModel->index ( mSelIndex, 0 ) ) ;
        }
        mMoveDir = NO_MOVE;
		mBounceBackDeltaX = 10;
    }	
}

void GlxCoverFlow::autoRightMove()
{
    qDebug("GlxCoverFlow::autoRightMove ");
    int width = mItemSize.width()  ;
    int diffX = mStripLen - mCurrentPos ;
    
    //for bounce back effect for back image ( it will do the back)
    qDebug("GlxCoverFlow::autoRightMove diffX x = %d current pos = %d mBounceBackDeltaX x = %d", diffX, mCurrentPos, mBounceBackDeltaX);
    if ( diffX > mBounceBackDeltaX && diffX < width && mMoveDir == NO_MOVE ){
        mMoveDir = RIGHT_MOVE;
        mBounceBackDeltaX = 10;
        autoLeftMoveSignal();        
        return ;
    }
    
    int deltaX = mCurrentPos %  width ;	
    //in the case of deltaX == 0 ( right flick case ) complete image should move
    deltaX = deltaX ? deltaX : width ; 
    int moveX = deltaX > KMoveX ? KMoveX : deltaX;
    
    qDebug("GlxCoverFlow::autoRightMove delta x = %d current pos = %d move x = %d", deltaX, mCurrentPos, moveX);
    
    move (moveX);
    deltaX -= moveX;

    if ( deltaX ) {
        emit autoRightMoveSignal();
    }
    else {
        //for bounce back of last image
        if ( mMoveDir == LEFT_MOVE ) {
            emit autoLeftMoveSignal();
            return;
        }
        int selIndex = mCurrentPos / width ;
        if ( mRows == 1 || selIndex != mSelIndex ) {
		    if(rotAngle) {
				rotAngle = 0;
				//HbEffect::start(mIconItem[mSelItemIndex], QString("HbIconItem"), QString("RotateImage270"), this, "rotationEffectFinished" );
				mIconItem[mSelItemIndex]->resetTransform();
				mIconItem[mSelItemIndex]->resize(mItemSize);
				mIconItem[mSelItemIndex]->setPos(mItemSize.width(), 0);

		    }
            mSelIndex = selIndex;
            mSelItemIndex = ( mSelItemIndex == 0 ) ?  NBR_ICON_ITEM -1 : --mSelItemIndex;
            selIndex = ( mSelItemIndex + 3 ) % NBR_ICON_ITEM;
            updateIconItem( mSelIndex - 2, selIndex, - width * 2 ) ;
            emit changeSelectedIndex ( mModel->index ( mSelIndex, 0 ) ) ;
        }
        mMoveDir = NO_MOVE;
		mBounceBackDeltaX = 10;
    }
}

void GlxCoverFlow::move(int value)
{
    qDebug("GlxCoverFlow::move ");
    QPointF pos(0,0);

    for ( qint8 i = 0; i < NBR_ICON_ITEM ; i++ ) {
        pos.setX( mIconItem[i]->pos().x() + value);
		pos.setY(mIconItem[i]->pos().y());
        mIconItem[i]->setPos(pos);
    }
    
    mCurrentPos -= value;
    if ( mCurrentPos < 0 ) {
        mCurrentPos += mStripLen;
    }
    else if ( mCurrentPos >= mStripLen ) {
        mCurrentPos -= mStripLen;
    }
    qDebug("GlxCoverFlow::Move value = %d current pos = %d", value, mCurrentPos); 
}

void GlxCoverFlow::setRows()
{
    qDebug("GlxCoverFlow::setRows ");
	if (mModel) {
	    mRows = mModel->rowCount();
	}
	else {
	    mRows = 0;
	}
	qDebug("GlxCoverFlow::setRows number of rows = %d", mRows);
}

void GlxCoverFlow::setStripLen()
{  
    qDebug("GlxCoverFlow::setStripLen ");
    mStripLen = mRows * mItemSize.width();  
    qDebug("GlxCoverFlow::setStripLen rows = %d, striplen %d", mRows, mStripLen);
}

int GlxCoverFlow::calculateIndex(int index)
{
    qDebug("GlxCoverFlow::calculateIndex index = %d, mRows = %d", index, mRows);
    
    if ( mRows == 1 )
        return 0;
    
    if ( index < 0 ) 
        return ( index + mRows );
   
    if ( index >= mRows ) 
        return ( index - mRows ) ;
    
    return index;        
}

void GlxCoverFlow::loadIconItems()
{  
    qDebug("GlxCoverFlow::loadIconItems ");
    int index = 0;
    QVariant variant = mModel->data( mModel->index(0,0), GlxFocusIndexRole );    
    if ( variant.isValid() &&  variant.canConvert<int> () ) {
        mSelIndex = variant.value<int>();  
    }  

    qDebug("GlxCoverFlow::loadIconItems index = %d, width = %d", mSelIndex, size().width() );    
    
    for ( qint8 i = 0; i < NBR_ICON_ITEM ; i++ ) {
        index = calculateIndex ( mSelIndex - 2 + i) ;           
        QVariant variant = mModel->data( mModel->index(index, 0), GlxFsImageRole );
        if ( variant.isValid() &&  variant.canConvert<HbIcon> () ) {
            mIconItem[i]->setIcon ( variant.value<HbIcon>() ) ;       
        }
        else {
            mIconItem[i]->setIcon( HbIcon() );
        }            
        mIconItem[i]->setSize ( mItemSize );
        mIconItem[i]->setPos ( QPointF ( (i - 2) * mItemSize.width(), 0) );   
    }
    
    mSelItemIndex = 2;
    mCurrentPos = mItemSize.width() * mSelIndex;
}

void GlxCoverFlow::updateIconItem (qint16 selIndex, qint16 selItemIndex, qint16 posX)
{
    qDebug("GlxCoverFlow::updateIconItem selIndex = %d, selIconIndex = %d posX = %d", selIndex, selItemIndex, posX );
    mIconItem[selItemIndex]->setPos(QPointF(posX, 0));
    
    selIndex = calculateIndex( selIndex );
    
    QVariant variant = mModel->data( mModel->index(selIndex, 0), GlxFsImageRole );
    if ( variant.isValid() &&  variant.canConvert<HbIcon> () ) {
        mIconItem[selItemIndex]->setIcon ( variant.value<HbIcon>() ) ;       
    }
    else {
        mIconItem[selItemIndex]->setIcon( HbIcon() );
    }
    mIconItem[selItemIndex]->setSize ( mItemSize );    
}


void GlxCoverFlow::clearCurrentModel()
{
    qDebug("GlxCoverFlow::clearCurrentModel ");
    if ( mModel ) {
        disconnect( mModel, SIGNAL( dataChanged(QModelIndex,QModelIndex) ), this, SLOT( dataChanged(QModelIndex,QModelIndex) ) );
        disconnect(mModel, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(rowsInserted(QModelIndex,int,int)));
        disconnect(mModel, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(rowsRemoved(QModelIndex,int,int)));
        mModel = NULL ;
    }
/*
    disconnect(mModel, SIGNAL(destroyed()), this, SLOT(_q_modelDestroyed()));
    disconnect(mModel, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)), this, SLOT(rowsAboutToBeRemoved(QModelIndex,int,int)));
*/	
}

void GlxCoverFlow::initializeNewModel()
{
    qDebug("GlxCoverFlow::initializeNewModel" );
    if ( mModel ) {
        connect( mModel, SIGNAL( dataChanged(QModelIndex,QModelIndex) ), this, SLOT( dataChanged(QModelIndex,QModelIndex) ) );
        connect(mModel, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(rowsInserted(QModelIndex,int,int)));
        connect(mModel, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(rowsRemoved(QModelIndex,int,int)));
    }	
}

void GlxCoverFlow::resetCoverFlow()
{
    qDebug("GlxCoverFlow::resetCoverFlow model %u", mModel );
    setRows();
    setStripLen();
    if ( mModel ) {
        loadIconItems( ) ;  
    }
}

void GlxCoverFlow::partiallyClean()
{
    qDebug("GlxCoverFlow::partiallyClean Enter");
    clearCurrentModel(); //during the animation data update will not cause the crash
    for ( qint8 i = 0; i < NBR_ICON_ITEM ; i++ ) {
        if ( mSelItemIndex != i){
            delete mIconItem[i] ;
            mIconItem[i] = NULL;
        }
    }      
}

void GlxCoverFlow::partiallyCreate(QAbstractItemModel *model, QSize itemSize)
{
    qDebug("GlxCoverFlow::resetpartiallyCreated");
    mIconItem[2]->setSize ( itemSize );
    mIconItem[2]->setPos ( QPointF ( 0, 0) );  
    
    QVariant variant = model->data( model->index(0,0), GlxFocusIndexRole );    
    if ( variant.isValid() &&  variant.canConvert<int> () ) {
        mSelIndex = variant.value<int>();  
       qDebug("GlxCoverFlow::partiallyCreated index mSelIndex=%d",mSelIndex);
    }
    
    variant = model->data( model->index(mSelIndex, 0), GlxFsImageRole );
    if ( variant.isValid() &&  variant.canConvert<HbIcon> () ) {
        mIconItem[2]->setIcon ( variant.value<HbIcon>() ) ;    
        qDebug("#########################GlxCoverFlow::partiallyCreated,ICON PRES##################");
    }
}

GlxCoverFlow::~GlxCoverFlow()
{
    qDebug("GlxCoverFlow::~GlxCoverFlow model " );
    disconnect( this, SIGNAL( autoLeftMoveSignal() ), this, SLOT( autoLeftMove() ) );
    disconnect( this, SIGNAL( autoRightMoveSignal() ), this, SLOT( autoRightMove() ) );
    HbEffect::remove( QString("HbIconItem"), QString(":/data/transitionrotate0.fxml"), QString( "RotateImage0" ));
    HbEffect::remove( QString("HbIconItem"), QString(":/data/transitionrotate90.fxml"), QString( "RotateImage90" ));
    HbEffect::remove( QString("HbIconItem"), QString(":/data/transitionrotate180.fxml"), QString( "RotateImage180" ));
    HbEffect::remove( QString("HbIconItem"), QString(":/data/transitionrotate270.fxml"), QString( "RotateImage270" ));
}


void GlxCoverFlow::ClearCoverFlow()
{
    qDebug("GlxCoverFlow::ClearCoverFlow  " );
    clearCurrentModel();    
    for ( qint8 i = 0; i < NBR_ICON_ITEM ; i++ ) {
        if(mIconItem[i] != NULL ) {
            delete mIconItem[i] ;
            mIconItem[i] = NULL;
        }
    }	
}

int GlxCoverFlow::getSubState()
{
    int substate = NO_FULLSCREEN_S;
    QVariant variant = mModel->data( mModel->index(0,0), GlxSubStateRole );    
    if ( variant.isValid() &&  variant.canConvert<int> ()  ) {
        substate = variant.value<int>();
    }
    return substate;
}
