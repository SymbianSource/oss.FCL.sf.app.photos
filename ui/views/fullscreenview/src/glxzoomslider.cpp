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

#include "glxzoomslider.h"
#include <hbslider.h>
#include <glxmediamodel.h>
#include <QAbstractItemModel>
#include <QGraphicsItem>
#include <QtDebug>
#include "glxmodelparm.h"
#include <QDebug>

GlxZoomSlider::GlxZoomSlider(QGraphicsItem *parent)
{
	mZoomSlider = new HbSlider(Qt::Horizontal,parent);
	mZoomSlider->hide();
	mZoomSlider->setRange(0, 400);
	mZoomSlider->setSingleStep(25);
	mZoomSlider->setSliderPosition(100);
	mZoomSlider->setZValue(7);
	mZoomSlider->setPos(50,100);
    mZoomSlider->resize(300.0, 0.0 );
	isSliderVisible = false;
	mSliderThumbPressed = false;
	connect(mZoomSlider, SIGNAL(valueChanged(int)), this, SLOT(filterandEmitSliderValueChanges(int)));
	connect(mZoomSlider, SIGNAL(sliderPressed()), this, SLOT(sliderThumbPressed()));
	connect(mZoomSlider, SIGNAL(sliderReleased()), this, SLOT(sliderThumbReleased()));
}

GlxZoomSlider::~GlxZoomSlider()
{
	disconnect(mZoomSlider, SIGNAL(valueChanged(int)), this, SLOT(filterandEmitSliderValueChanges(int)));
	disconnect(mZoomSlider, SIGNAL(sliderPressed()), this, SLOT(sliderThumbPressed()));
	disconnect(mZoomSlider, SIGNAL(sliderReleased()), this, SLOT(sliderThumbReleased()));
	delete mZoomSlider;
	isSliderVisible = false;
}

void GlxZoomSlider::setModel (QAbstractItemModel *model)
{
	GlxMediaModel *glxModel = dynamic_cast<GlxMediaModel *>(model);
    if ( glxModel ==NULL ||  glxModel == mModel) {
        return ;
    }
	mModel = glxModel;

}

void GlxZoomSlider::indexChanged (int index)
{
	Q_UNUSED(index);
	mZoomSlider->hide();
	isSliderVisible = false;
}

void GlxZoomSlider::toggleSliderVisibility()
{
	if(isSliderVisible) {
		mZoomSlider->hide();
		isSliderVisible = false;
	}
	else {
		calculateAndInitializeFSZoomFactor();
		mZoomSlider->show();
		isSliderVisible = true;
	}
}

void GlxZoomSlider::retrieveActualAndDisplayedSize(QSize& itemSize, QSize& displayedSize)
{
	itemSize = (mModel->data(mModel->index(mModel->data(mModel->index(0,0),GlxFocusIndexRole).value<int>(),0),GlxDimensionsRole)).value<QSize>();
	qDebug()<<"GlxZoomSlider::retrieveActualAndDisplayedSize"<<itemSize.width()<<itemSize.height();
	 QVariant variant = mModel->data(mModel->index(mModel->data(mModel->index(0,0),GlxFocusIndexRole).value<int>(),0),GlxFsImageRole);
	 if ( variant.isValid() &&  variant.canConvert<HbIcon> () ) {
		 QIcon itemIcon = variant.value<HbIcon>().qicon();
		 QSize windowSize(360,640);
		 QSize itemSize = itemIcon.actualSize(windowSize);
		 QPixmap itemPixmap = itemIcon.pixmap(itemSize);
		 displayedSize = itemPixmap.size();
		 qDebug()<<"GlxZoomSlider::retrieveActualAndDisplayedSize Display"<<displayedSize.width()<<displayedSize.height();
		 }

}
int GlxZoomSlider::calculateZoomFactor(QSize& itemSize, QSize& displayedSize)
{
	int zoomFactor = 100 * (displayedSize.width())/(itemSize.width());
	return zoomFactor;
}

void GlxZoomSlider::calculateAndInitializeFSZoomFactor()
{
	QSize itemSize,displaySize;
	retrieveActualAndDisplayedSize(itemSize, displaySize);
	int sliderPosition = calculateZoomFactor(itemSize,displaySize);
	emit initialZoomFactor(sliderPosition);
	mZoomSlider->setSliderPosition(sliderPosition);
}

void GlxZoomSlider::filterandEmitSliderValueChanges(int newValue)
{
	if (mSliderThumbPressed){
		emit valueChanged(newValue);
	}
	//else {
		qDebug()<<"GlxZoomSlider::filterandEmitSliderValueChanges stray signal"<<newValue;
	//}
}

void GlxZoomSlider::sliderThumbPressed()
{
	qDebug()<<"GlxZoomSlider::sliderThumbPressed";
	mSliderThumbPressed = true;
}

void GlxZoomSlider::sliderThumbReleased()
{
	qDebug()<<"GlxZoomSlider::sliderThumbReleased";
	mSliderThumbPressed = false;
}
