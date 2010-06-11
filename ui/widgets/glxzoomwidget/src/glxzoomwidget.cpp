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
* Description:   glxzoomwidget.cpp
*               description of the class GlxGlxZoomWidget which controls the Zoom behavior of coverflow.
*
*/
#include <QPinchGesture>
#include <hbiconitem.h>
#include <QTimeLine>
#include <QGesture>
#include "glximagedecoderwrapper.h"
#include "glxmodelparm.h"
#include "glxzoomwidget.h"

GlxZoomWidget::GlxZoomWidget(QGraphicsItem *parent):HbScrollArea(parent), mModel(NULL), mMinZValue(MINZVALUE), mMaxZValue(MAXZVALUE), mImageDecodeRequestSend(false), mPinchGestureOngoing(false), mDecodedImageAvailable(false)
{
    grabGesture(Qt::PinchGesture);
    setAcceptTouchEvents(true) ;
    setFrictionEnabled(false);
    setZValue(mMinZValue);
    //create the child items and background
    mZoomWidget = new QGraphicsWidget(this);
    mZoomItem = new QGraphicsPixmapItem(mZoomWidget);
    mZoomItem->setTransformationMode(Qt::SmoothTransformation);
    //the black background
    //replace when a proper substitute for setting backgrounds is known
    mBlackBackgroundItem = new HbIconItem(this);
    mBlackBackgroundItem->setBrush(QBrush(Qt::black));
    mBlackBackgroundItem->hide();
    //does not work so is commented
    //setBackgroundItem(mBlackBackgroundItem);

    //initializing the image decoder
    mImageDecoder = new GlxImageDecoderWrapper;

	//inititalizing the timer for animation
	m_AnimTimeLine = new QTimeLine(1000, this);
	m_AnimTimeLine->setFrameRange(0, 100);
	connect(m_AnimTimeLine, SIGNAL(frameChanged(int)), this, SLOT(animationFrameChanged(int)));
	connect(m_AnimTimeLine, SIGNAL(finished()), this, SLOT(animationTimeLineFinished()));
}

GlxZoomWidget::~GlxZoomWidget()
{
    //disconnect all existing signals
    disconnect(this,SIGNAL( pinchGestureReceived(int) ), this, SLOT( sendDecodeRequest(int) ) );
    //no Null checks required
    delete mZoomItem;
//    delete mZoomWidget; //as this is a content widegt it will automatically be deleted
    delete mBlackBackgroundItem;
    //reset the decoder to cancel pending tasks
    if(mImageDecoder) {
        mImageDecoder->resetDecoder();
        delete mImageDecoder;
    }
}

void GlxZoomWidget::setModel (QAbstractItemModel *model)
{
    if(model)
    {
        mModel = model;
        retreiveFocusedImage(); //Update mZoomItem with focused Image
        connect( mModel, SIGNAL( dataChanged(QModelIndex,QModelIndex) ), this, SLOT( dataChanged(QModelIndex,QModelIndex) ) );
    }
}

void GlxZoomWidget::setWindowSize(QSize windowSize)
{
    mWindowSize = windowSize;
    mBlackBackgroundItem->setGeometry(QRectF(QPointF(0,0), mWindowSize));
    //try to reset the max and min zoomed size here
}

void GlxZoomWidget::indexChanged(int index)
{
    Q_UNUSED(index);
    if(mFocusIndex != index) {
        mImageDecoder->resetDecoder();//reset the decoder first to cancel pending tasks
        mImageDecodeRequestSend = false;
        mDecodedImageAvailable = false;
        retreiveFocusedImage();  //Update mZoomItem with focused Image
    }
}

void GlxZoomWidget::cleanUp()
{
//    disconnect( mModel, SIGNAL( dataChanged(QModelIndex,QModelIndex) ), this, SLOT( dataChanged(QModelIndex,QModelIndex) ) );
    if(mImageDecoder) {
        mImageDecoder->resetDecoder();
    }
    mZoomItem->setPixmap(QPixmap());
}

void GlxZoomWidget::activate()
{
}

void GlxZoomWidget::setMinMaxZValue(int minZvalue, int maxZvalue)
{
    mMinZValue = minZvalue;
    mMaxZValue = maxZvalue;
}

void GlxZoomWidget::connectDecodeRequestToPinchEvent()
{
    connect(this,SIGNAL( pinchGestureReceived(int) ), this, SLOT( sendDecodeRequest(int) ), Qt::QueuedConnection );
}

bool GlxZoomWidget::sceneEvent(QEvent *event)
{
    bool consume(false);
    if (event->type() == QEvent::Gesture) {
        consume = executeGestureEvent(this, static_cast<QGestureEvent*>(event));
    }
    if(!consume)
    {
        consume = HbScrollArea::sceneEvent(event);
    }
    return consume;
}

bool GlxZoomWidget::sceneEventFilter(QGraphicsItem *watched,QEvent *event)
{
     qDebug("GlxCoverFlow::eventFilter " );
    bool consume = false;
    if (event->type() == QEvent::Gesture) {
        consume = executeGestureEvent(watched, static_cast<QGestureEvent*>(event));
    }

    if(!consume) {
        consume = HbScrollArea::sceneEventFilter(watched,event);
    }
    return consume;

}

bool GlxZoomWidget::executeGestureEvent(QGraphicsItem *source,QGestureEvent *event)
{
     if (QGesture *pinch = event->gesture(Qt::PinchGesture))  {
       QPinchGesture* pinchG = static_cast<QPinchGesture *>(pinch);
       QPinchGesture::ChangeFlags changeFlags = pinchG->changeFlags();
       if (changeFlags & QPinchGesture::ScaleFactorChanged) {
            mPinchGestureOngoing = true;
            //bring the zoom widget to foreground
            setZValue(mMaxZValue);
            //show the black background
            mBlackBackgroundItem->setParentItem(parentItem());
            mBlackBackgroundItem->setZValue(mMaxZValue - 1);
            mBlackBackgroundItem->show();

            //retreive the gesture values
            qreal value = pinchG->scaleFactor() / pinchG->lastScaleFactor();
            QPointF center = pinchG->property("centerPoint").toPointF();
            //set the gesture center to the scene coordinates
            QPointF sceneGestureCenter = source->sceneTransform().map(center);
            zoomImage(value, sceneGestureCenter);

        }
       if (pinchG->state() == Qt::GestureStarted) {
           emit pinchGestureReceived(mFocusIndex);
       }

       if (pinchG->state() == Qt::GestureFinished) {
           if(mStepCurrentSize != mCurrentSize) {
               //For giving a spring effect when user has zoomed more than normal.
               if(mStepCurrentSize.width() > mMaxScaleDecSize.width())   {
                   //scale the image to limited size
                   qreal value = mMaxScaleDecSize.width()/mCurrentSize.width();
                   QPointF center(mWindowSize.width()/2, mWindowSize.height()/2);
                   QPointF sceneGestureCenter = source->sceneTransform().map(center);
                   zoomImage(value, sceneGestureCenter);
               }
               mPinchGestureOngoing = false;
                //finalize the transforms to the geometry else panning will not work
                finalizeWidgetTransform();
           }
//push the Zoom widget to background when zoomed image size nears FS image
           if(mStepCurrentSize.width() <= mMinDecScaleSize.width()*1.3)  {
               mBlackBackgroundItem->hide();
               //push the widget back to background
               setZValue(mMinZValue);
               emit zoomWidgetMovedBackground(mFocusIndex);
               //do not reset the transform here as it will then zoom-in the widget to decoded image size
           }
       }
       //gesture accepted
       return true;
     }
     //gesture rejected
     if(!mPinchGestureOngoing) {
         return false; 
     }
     return true;

}

void GlxZoomWidget::zoomImage(qreal zoomFactor, QPointF center)
{
    adjustGestureCenter(center, zoomFactor);
    QSizeF requiredSize(mCurrentSize.width()*zoomFactor, mCurrentSize.height()*zoomFactor);
    limitRequiredSize(requiredSize);
    if(requiredSize != mCurrentSize) {
        QTransform zoomTransform = mZoomWidget->transform();
        QPointF transformedCenter = mZoomWidget->sceneTransform().inverted().map(center);
        zoomTransform.translate(transformedCenter.x(),transformedCenter.y());
        zoomTransform.scale(requiredSize.width()/mCurrentSize.width(), requiredSize.height()/mCurrentSize.height());
        zoomTransform.translate(-transformedCenter.x(),-transformedCenter.y());
        mZoomWidget->setTransform(zoomTransform);
        mCurrentSize = requiredSize;
    }

}


void GlxZoomWidget::limitRequiredSize(QSizeF &requiredSize)
{
    if(requiredSize.width() > mMaxScaleSize.width() ) {
        requiredSize = mMaxScaleSize ;
    }
    else if(requiredSize.width() < mMinDecScaleSize.width() ) {
        requiredSize = mMinDecScaleSize ;
    }


}

//makes sure that the gesture is on the screen center if the image is smaller than the screen
void GlxZoomWidget::adjustGestureCenter(QPointF & gestureCenter, qreal& zoomFactor)
{
    if(zoomFactor > 1 &&zoomFactor > 1.2 )  {
        zoomFactor = 1.2;
    }

    if(zoomFactor < 1 &&zoomFactor < 0.8 )   {
        zoomFactor = 0.8;
    }
    QSizeF requiredSize(mCurrentSize.width()*zoomFactor, mCurrentSize.height()*zoomFactor);
    //keep smaller image centered
    if(mCurrentSize.width() <= mWindowSize.width() )
    {
        gestureCenter.setX(mWindowSize.width()/2);

    }
    if(mCurrentSize.height() <= mWindowSize.height())
    {
        gestureCenter.setY(mWindowSize.height()/2);

    }
    //maintains the boundary of the edges for zoom out conditions
    if(zoomFactor < 1)
    {
        QPointF itemOriginPos = mZoomWidget->sceneTransform().map(QPointF(0,0));
        bool hasWidthExceededWindow = mCurrentSize.width() > mWindowSize.width();
        bool hasHeightExceededWindow = mCurrentSize.height() > mWindowSize.height();
        if(itemOriginPos.x() >= 0)  {
        //image has crossed left boundry leaving blank space
            if(hasWidthExceededWindow) {
                //stick the gesture to the left corner
                gestureCenter.setX(itemOriginPos.x());
            }
        }
        //Check if the right boundry can be adjusted
        if(itemOriginPos.x()+ mCurrentSize.width() <= mWindowSize.width()) {
                //Image is before the right boundry leaving blank space
                if(hasWidthExceededWindow) {
                    //stick the gesture to the right corner
                    gestureCenter.setX(itemOriginPos.x()+ mCurrentSize.width());
                }
        }
        //check if the upper boundry could be adjusted
        if(itemOriginPos.y() >= 0) {
                //image has crossed the upper boundry leaving blank space
                if(hasHeightExceededWindow) {
                    //stick the image to the upper boundry
                    gestureCenter.setY(itemOriginPos.y());
                }
        }
        //check if the lower boundry could be adjusted
        if(itemOriginPos.y()+ mCurrentSize.height() <= mWindowSize.height()) {
        //Image is before the right boundry leaving blank space
            if(hasHeightExceededWindow) {
                //stick the image to the right corner
                gestureCenter.setY(itemOriginPos.y()+ mCurrentSize.height());
            }

        }
    }
    //control the zoom Factor to boundaries
    if(mCurrentSize.width() > mWindowSize.width() && requiredSize.width() <= mWindowSize.width())
    {
        zoomFactor =  mWindowSize.width()/mCurrentSize.width();

    }
    else if(mCurrentSize.height() > mWindowSize.height() && requiredSize.height() <= mWindowSize.height())
    {
        zoomFactor =  mWindowSize.height()/mCurrentSize.height();

    }

    //reduce the ZF so as to show a decelerated effect at max/min levels

    if(mCurrentSize.width() > mMaxScaleDecSize.width() && zoomFactor > 1 ) {
        zoomFactor = 1.0 + ((zoomFactor-1.0)/6) ;
    }
        if(mCurrentSize.width() < mMinDecScaleSize.width() && zoomFactor < 1 ) {
        zoomFactor = 1.0 - ((1.0-zoomFactor)/6) ;
    }


}

//get the latest focused image and set it to mZoomItem
void GlxZoomWidget::retreiveFocusedImage()
{

    QPixmap targetPixmap(getFocusedImage());
    //initialize all the variables wrt the focussed pixmap
    mZoomWidget->resetTransform();
    mItemSize = targetPixmap.size();
    mMaxScaleSize = mItemSize;
    mMaxScaleSize.scale(mWindowSize*13, Qt::KeepAspectRatio);
    mMaxScaleDecSize = mItemSize;
    mMaxScaleDecSize.scale(mWindowSize*7, Qt::KeepAspectRatio);
    mMinScaleSize = mItemSize* 0.7;
    mMinDecScaleSize = mItemSize;
    QPointF originPos = sceneTransform().map(QPointF(0,0));
    mZoomWidget->setGeometry(QRectF(QPointF(mWindowSize.width()/2 - mItemSize.width()/2,mWindowSize.height()/2 - mItemSize.height()/2),mItemSize )); //chk this
    mZoomWidget->setPreferredSize(mItemSize);
    mZoomItem->setPixmap(targetPixmap);
    mCurrentSize = mItemSize;
    mStepCurrentSize = mItemSize;
    setContentWidget(mZoomWidget);
    show();
}


void GlxZoomWidget::dataChanged(QModelIndex startIndex, QModelIndex endIndex)
{
    if(mFocusIndex >= startIndex.row() && mFocusIndex <= endIndex.row()) {
        //get the latest image from the model
        //will replace a decoded image if callback is received after decoded image is received so a fix is required
        //retreiveFocusedImage();
        if(!mDecodedImageAvailable)  {
        QPixmap targetPixmap(getFocusedImage());
        mZoomItem->setPixmap(targetPixmap);
        finalizeWidgetTransform();
        }
    }
}

void GlxZoomWidget::decodedImageAvailable()
{
    //new bitmap with better resolution is available
    //so set it to the item
    QPixmap decodedPixmap = mImageDecoder->getPixmap();
    disconnect(mImageDecoder, SIGNAL(pixmapDecoded()), this, SLOT(decodedImageAvailable()));
    if(decodedPixmap.isNull()){
        return;
    }
    mDecodedImageAvailable = true;
    mZoomItem->setPixmap(decodedPixmap);
    mItemSize = decodedPixmap.size();
    //this is important if not done then old transforms will be applied on the new image
    finalizeWidgetTransform();
}

void GlxZoomWidget::sendDecodeRequest(int index)
{
    if(!mImageDecodeRequestSend) {
        QString imagePath = (mModel->data(mModel->index(index,0),GlxUriRole)).value<QString>();
        mImageDecoder->decodeImage(imagePath);
        connect(mImageDecoder, SIGNAL(pixmapDecoded()), this, SLOT(decodedImageAvailable()));
        mImageDecodeRequestSend = true;
    }
}


void GlxZoomWidget::finalizeWidgetTransform()
{
    QPointF widgetPos = mZoomWidget->sceneTransform().map(QPointF(0,0)); //Map the origin wrt scene
    mZoomWidget->resetTransform();
    mZoomWidget->scale(mCurrentSize.width()/mItemSize.width(), mCurrentSize.height()/mItemSize.height());
    mZoomWidget->setGeometry(QRectF(widgetPos , mCurrentSize));
    // this updates HbScrollArea on the sizeHint of ZoomWidget
    mZoomWidget->setPreferredSize(mCurrentSize);
    mStepCurrentSize = mCurrentSize;
}

QPixmap GlxZoomWidget::getFocusedImage()
{
    mFocusIndex = mModel->data(mModel->index(0,0),GlxFocusIndexRole).value<int>();
    QVariant iconVariant = mModel->data(mModel->index(mFocusIndex,0),GlxFsImageRole);
    QVariant sizeVariant = mModel->data(mModel->index(mFocusIndex,0),GlxDimensionsRole);
    QPixmap targetPixmap;
    //retreive pixmap from the HbIcon received from model
    //should change the model to return and save pixmaps and convert to HbIcons Instead
    if ( iconVariant.isValid() &&  iconVariant.canConvert<HbIcon> () ) {
         QIcon itemIcon = iconVariant.value<HbIcon>().qicon();
         QSize itemSize = itemIcon.actualSize(mWindowSize);
         QSize scaleSize;
         if(sizeVariant.isValid() &&  sizeVariant.canConvert<QSize> ()) {
             scaleSize = sizeVariant.toSize();
             if(!(scaleSize.width() < mWindowSize.width() && scaleSize.height() < mWindowSize.height()))  {
                 scaleSize = mWindowSize;
             }
         }
         targetPixmap = itemIcon.pixmap(itemSize).scaled(scaleSize, Qt::KeepAspectRatio);
         mItemSize = targetPixmap.size();
    }
    return targetPixmap;

}





void GlxZoomWidget::animateZoomIn(QPointF animRefPoint)
{
      emit pinchGestureReceived(mFocusIndex);
            //bring the zoom widget to foreground
            setZValue(mMaxZValue);
            //show the black background
            mBlackBackgroundItem->setParentItem(parentItem());
            mBlackBackgroundItem->setZValue(mMaxZValue - 1);
            mBlackBackgroundItem->show();
	m_AnimRefPoint = animRefPoint;
    QSizeF requiredSize = mItemSize;
    requiredSize.scale(mWindowSize*3.5, Qt::KeepAspectRatio);
	m_FinalAnimatedScaleFactor = requiredSize.width()*3.5/mMinScaleSize.width();
	m_AnimTimeLine->setDirection(QTimeLine::Forward);
	m_AnimTimeLine->start();
  //  zoomImage(5, m_AnimRefPoint);

}
void GlxZoomWidget::animateZoomOut(QPointF animRefPoint)
{
	m_AnimRefPoint = animRefPoint;
	m_FinalAnimatedScaleFactor = mMinScaleSize.width()/mCurrentSize.width();
	//m_AnimTimeLine->setDirection(QTimeLine::Backward);
	m_AnimTimeLine->start();
}
void GlxZoomWidget::animationFrameChanged(int frameNumber)
{
qreal scaleFactor = 1;
	if(m_FinalAnimatedScaleFactor > 1) {
	//	qreal scaleFactor = (100+ ((m_FinalAnimatedScaleFactor*100 - 100)/100)*frameNumber)/100;
		scaleFactor = 1.0 + (((m_FinalAnimatedScaleFactor - 1)/100)*frameNumber);
	}
	if(m_FinalAnimatedScaleFactor < 1) {
		scaleFactor = (m_FinalAnimatedScaleFactor*100+ ((100 - m_FinalAnimatedScaleFactor*100 )/100)*frameNumber)/100;
	//	qreal scaleFactor = 1.0 + (((m_FinalAnimatedScaleFactor - 1)/100)*frameNumber)
	}

	zoomImage(scaleFactor, m_AnimRefPoint);

}
void GlxZoomWidget::animationTimeLineFinished()
{
	finalizeWidgetTransform();
//push the Zoom widget to background when zoomed image size nears FS image
           if(mStepCurrentSize.width() <= mMinDecScaleSize.width()*1.3)  {
               mBlackBackgroundItem->hide();
               //push the widget back to background
               setZValue(mMinZValue);
               emit zoomWidgetMovedBackground(mFocusIndex);
               //do not reset the transform here as it will then zoom-in the widget to decoded image size
           }
}

