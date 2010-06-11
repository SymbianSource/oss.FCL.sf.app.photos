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
* Description:   glxzoomwidget.h
*               definition of the class glxzoomwidget which controls the Zoom behavior of coverflow.
*
*/
#ifndef GLXZOOMWIDGET_H
#define GLXZOOMWIDGET_H

#include <QObject>
#include <QGraphicsWidget>
#include <QPixmap>
#include <hbscrollarea.h>

#include <glxzoomwidget_global.h>

class QGestureEvent;
class QPinchGesture;
class HbIconItem;
class GlxImageDecoderWrapper;
class QTimeLine;

const int MAXZVALUE = 100;
const int MINZVALUE = 0;

class GLXZOOMWIDGETSHARED_EXPORT GlxZoomWidget : public HbScrollArea
{
    Q_OBJECT

    public:
    GlxZoomWidget (QGraphicsItem *parent = NULL);
    ~GlxZoomWidget ();
    void setModel (QAbstractItemModel *model) ;
    void indexChanged(int index);
    void setWindowSize(QSize windowSize);
    void cleanUp();
    void activate();
    void setMinMaxZValue(int minZvalue, int maxZvalue);
    void connectDecodeRequestToPinchEvent();

    signals:
    void pinchGestureReceived(int index);
    void zoomWidgetMovedBackground(int index);

    public slots:
    //for Decoder support
    void decodedImageAvailable();
    void sendDecodeRequest(int index);

	//for animation effects
	void animateZoomIn(QPointF animRefPoint);
	void animateZoomOut(QPointF animRefPoint);
	void animationFrameChanged(int frameNumber);
	void animationTimeLineFinished();

    protected:
    bool sceneEvent(QEvent *event);
    bool sceneEventFilter(QGraphicsItem *watched,QEvent *event);
    protected slots:
    void dataChanged(QModelIndex startIndex, QModelIndex endIndex);


    private:
    bool executeGestureEvent(QGraphicsItem *source,QGestureEvent *event);
    //Responsible for changing the transforms of the widget wrt the ZF and center
	//Also adjusts the ZF, center and final size so as to respect the boundaries
    void zoomImage(qreal zoomFactor, QPointF center);
    //This API will adjust the gesture center to maintain boundaries
	//called from GlxZoomWidget::zoomImage
    void adjustGestureCenter(QPointF & gestureCenter, qreal& zoomFactor);
    //this API will limit the max/min Zoom Size possible
	//called from GlxZoomWidget::zoomImage
    void limitRequiredSize(QSizeF &requiredSize);
	//gets the latest focussed image from the model
    void retreiveFocusedImage();
    //finalize the transform and update the sizehint of mZoomWidget
    void finalizeWidgetTransform();
    //get the focused image from the model
    QPixmap getFocusedImage();

    //data members
    private:
    //view widgets
    QGraphicsPixmapItem *mZoomItem;   //Item containing the pixmap
    QGraphicsWidget *mZoomWidget;     //container :all scaling and transforms would be done on this widget
    HbIconItem* mBlackBackgroundItem; //for setting black background

    //view Z values 
    //might push to layouts later

    int mMinZValue ;
    int mMaxZValue ;


    //to be in sync with the model
    QAbstractItemModel *mModel;
    int mFocusIndex;

    //size parameter
    QSizeF mItemSize;        // the actual pixmap size. Keep it updated with latest pixmap size
    QSizeF mStepCurrentSize; //to save the size before gesture started
    QSizeF mCurrentSize;     //save sizes scaled by gesture
    QSize mWindowSize;      //the window size
    QSizeF mMinScaleSize;   //minimum posible size
    QSizeF mMinDecScaleSize;   //the Minimum scale limit after which the image scaling down should be decelerated
    QSizeF mMaxScaleSize;   // the maximum scale limit
    QSizeF mMaxScaleDecSize;   // the Maximum scale limit after which the image scaling should be decelerated
	//for animation effect
	qreal m_FinalAnimatedScaleFactor;
	QPointF m_AnimRefPoint;
	QTimeLine *m_AnimTimeLine;
    //for Decoder support
    GlxImageDecoderWrapper* mImageDecoder;

    //status flags
	//To check if the decode request has already been send or not
    bool mImageDecodeRequestSend; 
	//To check if the pinch gesture is in progress to block any events to HbScrollArea
    bool mPinchGestureOngoing; 
    //to check if decoded image is available
    bool mDecodedImageAvailable;

};
#endif  //GLXZOOMWIDGET_H
