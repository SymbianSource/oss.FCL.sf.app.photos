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

#include "glxzoomcontrol.h"
#include "glximagedecoderwrapper.h"
#include "glxmodelparm.h"
#include <hbicon.h>
#include <glxmediamodel.h>
#include <QtDebug>
#include <hbinstance.h>
#include <hbiconitem.h>
#include <QDebug>

GlxZoomControl::GlxZoomControl(QGraphicsItem *parent) : HbScrollArea(parent),mIsControlActivated(false), mIsItemChanged(true)
{
	setScrollDirections(Qt::Horizontal | Qt::Vertical);
	hide();
	setZValue(6);
	setScrollingStyle(PanOrFlick);
	mBlackBackgroundItem = new HbIconItem(parent);
	mBlackBackgroundItem->setBrush(QBrush(Qt::black));
	mBlackBackgroundItem->hide();
	mZoomWidget = new QGraphicsWidget(this);
	mZoomItem = new QGraphicsPixmapItem(mZoomWidget);
	mImageDecoder = new GlxImageDecoderWrapper;	

}

GlxZoomControl::~GlxZoomControl()
{
	resetZoomControl();
	delete mZoomItem;
	delete mZoomWidget;
	mImageDecoder->resetDecoder();
	delete mImageDecoder;
	delete mBlackBackgroundItem;

}

void GlxZoomControl::zoomImage(int zoomFactor)
{
	qDebug("@@@GlxZoomControl::zoomImage  ZF=%d, InitialZf=%d", zoomFactor, mInitialZoomFactor);
	if(mIsItemChanged)	{
		initializeZoomControl(zoomFactor);
	}
	if(zoomFactor > mInitialZoomFactor) {

		qreal zoomPercent = qreal(zoomFactor)/100.00;
		QSizeF requiredSize = QSize(qreal(mItemSize.width() * zoomPercent), qreal(mItemSize.height() * zoomPercent ));
		calculatePanOffset();
		mZoomWidget->scale(requiredSize.width()/mCurrentSize.width(), requiredSize.height()/mCurrentSize.height());
		qDebug()<<"GlxZoomControl::zoomImage scaling factor"<<requiredSize.width()/mCurrentSize.width()<<requiredSize.height()/mCurrentSize.height();
		qDebug()<<"GlxZoomControl::zoomImage required Size"<<requiredSize.width()<<requiredSize.height();
		qDebug()<<"GlxZoomControl::zoomImage current Size"<<mCurrentSize.width()<<mCurrentSize.height();
		qDebug()<<"GlxZoomControl::zoomImage item Size"<<mItemSize.width()<<mItemSize.height();

		//mZoomWidget->setGeometry(QRectF(-0.5*QPointF(requiredSize.width(),requiredSize.height()),requiredSize ));
		//mZoomWidget->setPos(mWindowSize.width()/2 - requiredSize.width()/2,mWindowSize.height()/2 - requiredSize.height()/2);
		mCurrentSize = requiredSize;	
		mCurrentZoomFactor = zoomFactor;
		updateItemPosition();
		if(!mIsControlActivated) {
			show();
			mBlackBackgroundItem->show();
			mIsControlActivated = true;
			emit hideFullScreenUi();
		}
	}
	if(zoomFactor < mInitialZoomFactor)
	{
		mBlackBackgroundItem->hide();
		hide();
		mPanOffset.setX(0);
		mPanOffset.setY(0);
		updateItemPosition();
		mIsControlActivated = false;
		//mZoomWidget->resetTransform();
	}
}

void GlxZoomControl::initializeZoomControl(int zoomFactor)
{
	 qDebug("@@@GlxZoomControl::initializeZoomControl Zoom Factor %d", zoomFactor);
	 if(mIsControlActivated || zoomFactor < mInitialZoomFactor)
	 {
		return;
	 }
	 qDebug("@@@GlxZoomControl::initializeZoomControl Context not activated yet");
	 //get the image path and request for a decoded pixmap first
	 QString imagePath = (mModel->data(mModel->index(mModel->data(mModel->index(0,0),GlxFocusIndexRole).value<int>(),0),GlxUriRole)).value<QString>();
	 mImageDecoder->decodeImage(imagePath);
	 connect(mImageDecoder, SIGNAL(pixmapDecoded()), this, SLOT(decodedImageAvailable()));
	 QVariant variant = mModel->data(mModel->index(mModel->data(mModel->index(0,0),GlxFocusIndexRole).value<int>(),0),GlxFsImageRole);
	 if ( variant.isValid() &&  variant.canConvert<HbIcon> () ) {
		 qDebug("@@@GlxZoomControl::initializeZoomControl valid icon");
		 QIcon itemIcon = variant.value<HbIcon>().qicon();
		 //QSize windowSize(360,640);
		 QSize itemSize = itemIcon.actualSize(mWindowSize);
		 QPixmap itemPixmap = itemIcon.pixmap(itemSize);
		 //if(itemPixmap.isNull())
		 qDebug("@@@GlxZoomControl::initializeZoomControl Null Pixmap");
		 //set item size to the actual size
		 mItemSize = (mModel->data(mModel->index(mModel->data(mModel->index(0,0),GlxFocusIndexRole).value<int>(),0),GlxDimensionsRole)).value<QSize>();
		 mCurrentSize = itemPixmap.size();
		 mZoomItem->setPixmap(itemPixmap);
		 mZoomWidget->setGeometry(QRectF(-0.5*QPointF(mCurrentSize.width(),mCurrentSize.height()),mItemSize ));
		 mZoomWidget->setPos(mWindowSize.width()/2 - mCurrentSize.width()/2,mWindowSize.height()/2 - mCurrentSize.height()/2);
		 //show();
         //mIsControlActivated = true;
		 mIsItemChanged = false;
		 setContentWidget(mZoomWidget);
	 }
}

void GlxZoomControl::setModel(QAbstractItemModel *model)
{
	GlxMediaModel *glxModel = dynamic_cast<GlxMediaModel *>(model);
    if ( glxModel ==NULL ||  glxModel == mModel) {
        return ;
    }
	mModel = glxModel;
}

void GlxZoomControl::resetZoomControl()
{
	qDebug("@@@GlxZoomControl::resetZoomControl");
	mBlackBackgroundItem->hide();
	hide();
	mZoomWidget->resetTransform();
	mZoomItem->setPixmap(QPixmap());
	mItemSize = QSizeF(0,0);
	mCurrentSize = mItemSize;
	mIsControlActivated = false;
	mIsItemChanged = true;
	mInitialZoomFactor = 0;
	mPanOffset.setX(0);
	mPanOffset.	setY(0);
	mImageDecoder->resetDecoder();		
}

void GlxZoomControl::indexChanged(int index)
{
	Q_UNUSED(index);
	qDebug("@@@GlxZoomControl::indexChanged");
	resetZoomControl();

}

void GlxZoomControl::decodedImageAvailable()
{
	//new bitmap with better resolution is available
	//so set it to the item
	QPixmap decodedPixmap = mImageDecoder->getPixmap();
	if(decodedPixmap.isNull()){
		return;
	}
	mCurrentSize = decodedPixmap.size();
	//this is important if not done then old transforms will be applied on the new image
	mZoomWidget->resetTransform();
	mZoomItem->setPixmap(decodedPixmap);
	QPointF undefferedPos;
	undefferedPos.setX(mWindowSize.width()/2 - mCurrentSize.width()/2);
	undefferedPos.setY(mWindowSize.height()/2 - mCurrentSize.height()/2);
	mZoomWidget->setGeometry(QRectF(undefferedPos,mCurrentSize )); //temporarily setting it to an undeffered position
	//update the image transforms
	zoomImage(mCurrentZoomFactor);
}

void GlxZoomControl::initialZoomFactor(int initZoomFactor)
{
	qDebug("@@@GlxZoomControl::initialZoomFactor initZoomFactor=%d", initZoomFactor );
	resetZoomControl();
	mInitialZoomFactor = initZoomFactor;
}

void GlxZoomControl::setWindowSize (QSize windowSize)
{
	if(mWindowSize != windowSize) {
		mWindowSize = windowSize;
		mBlackBackgroundItem->setSize(mWindowSize);
		mBlackBackgroundItem->setPos(0,0);
		if(!mIsItemChanged) {
			//first calculate offset because after orientation change this value will be lost
			calculatePanOffset();
			//update the item position WRT the new window size
			updateItemPosition();

		}
	}
}
void GlxZoomControl::updateItemPosition()
{
	qreal zoomPercent = qreal(mCurrentZoomFactor)/100.00;
	QPointF undefferedPos;
	//calculate position if no panning was done
	undefferedPos.setX(mWindowSize.width()/2 - mCurrentSize.width()/2);
	undefferedPos.setY(mWindowSize.height()/2 - mCurrentSize.height()/2);
	QPointF offset = mPanOffset;
	//convert offset value WRT current zoomFactor
	offset *= zoomPercent;
	QPointF finalPos = undefferedPos + offset;
	//check for boundry conditions 
	checkandAdjustImageBoundaries(finalPos, undefferedPos);
	qDebug()<<"GlxZoomControl::updateItemPosition offset x%d, y%d "<<offset.x()<< offset.y();
	//mZoomWidget->setPos(undefferedPos + offset);
	mZoomWidget->setGeometry(QRectF(finalPos,mCurrentSize ));
}

void GlxZoomControl::calculatePanOffset()
{
	qreal zoomPercent = qreal(mCurrentZoomFactor)/100.00;
	QPointF currentPos = mZoomWidget->pos();
	//calculate position if no panning was done
	QPointF undefferedPos;
	undefferedPos.setX(mWindowSize.width()/2 - mCurrentSize.width()/2);
	undefferedPos.setY(mWindowSize.height()/2 - mCurrentSize.height()/2);
	//get the offset with the current zoomFactor which is the difference between currentPos and undefferedPos
	QPointF offset = currentPos - undefferedPos ;
	qDebug()<<"GlxZoomControl::calculatePanOffset current Pos x%d, y%d undeffered x %d, y %d"<<currentPos.x()<< currentPos.y()<< undefferedPos.x()<< undefferedPos.y();
    mPanOffset = offset;
	//convert the offset value WRT a 100% zoomed image as that will be used for reference 
	mPanOffset /= zoomPercent;
	qDebug()<<"GlxZoomControl::calculatePanOffset mPanOffset x%d, y%d "<<mPanOffset.x()<< mPanOffset.y();
}

void GlxZoomControl::checkandAdjustImageBoundaries(QPointF &finalPos, QPointF undefferedPos)
{
	//check for boundary conditions in X Axis

	//for Left side
	//since the image is positioned WRT to screens top left corner(0,0) so if the images top left corner's X (finalPos.x()) is positive
	//then boundry conditions for image to be on the leftmost side is met
	if(finalPos.x() > 0) {
		//chk if the original image position without any offset has also reached boundry conditions
		if(undefferedPos.x() < 0) {
		//set the image to the leftmost axis on the screen
		finalPos.setX(0);
		}
		//if original image position without any offset has also reached boundry conditions then use the undefferedPos 
		else {
			finalPos.setX(undefferedPos.x());
			//set offset to 0 as it is of no use anymore
			mPanOffset.setX(0);
		}
	}

	//Right side
	//since the image is positioned WRT to screens top left corner(0,0) so if image Position added to the image dimensions is 
	//greater than the screen dimensions then boundfy conditions for rightmost and lowest points are met.
	//for X axis width will be of our concern
	if((finalPos.x() + mCurrentSize.width()) <  mWindowSize.width()){
		//if original image position without any offset has also reached boundry conditions then use the undefferedPos 
		if ((undefferedPos.x() + mCurrentSize.width()) <  mWindowSize.width()) 	{
			finalPos.setX(undefferedPos.x());
			//set offset to 0 as it is of no use anymore
			mPanOffset.setX(0);			
		}
		else {
			//set the image to the rightmost axis on the screen
			finalPos.setX(mWindowSize.width()- mCurrentSize.width());
		}
	}


	//check for boundary conditions in Y Axis

	//top
	//since the image is positioned WRT to screens top left corner(0,0) so if the images top left corner's Y (finalPos.y()) is positive
	//then boundry conditions for image to be on the topmost side is met
	if(finalPos.y() > 0) {
		//chk if the original image position without any offset has also reached boundry conditions
		if(undefferedPos.y() < 0) {
		//set the image to the topmost axis on the screen
		finalPos.setY(0);
		}
		//if original image position without any offset has also reached boundry conditions then use the undefferedPos 
		else {
			finalPos.setY(undefferedPos.y());
			//set offset to 0 as it is of no use anymore
			mPanOffset.setY(0);
		}
	}

	//bottom
	//since the image is positioned WRT to screens top left corner(0,0) so if image Position added to the image dimensions is 
	//greater than the screen dimensions then boundfy conditions for rightmost and lowest points are met.
	//for Y axis height will be of our concern

	if((finalPos.y() + mCurrentSize.height()) <  mWindowSize.height()){
		//if original image position without any offset has also reached boundry conditions then use the undefferedPos 
		if ((undefferedPos.y() + mCurrentSize.height()) <  mWindowSize.height()) 	{
			finalPos.setY(undefferedPos.y());
			//set offset to 0 as it is of no use anymore
			mPanOffset.setY(0);			
		}
		else {
			//set the image to the lowest axis on the screen
			finalPos.setY(mWindowSize.height()- mCurrentSize.height());
		}
	}

}
