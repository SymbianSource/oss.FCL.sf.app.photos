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
#ifndef GLXZOOMCONTROL_H
#define GLXZOOMCONTROL_H
#include <hbscrollarea.h>

class GlxMediaModel;
class QAbstractItemModel;
class HbIconItem;
class GlxImageDecoderWrapper;
class GlxZoomControl : public HbScrollArea
{
Q_OBJECT

public:
	GlxZoomControl(QGraphicsItem *parent = NULL);
	~GlxZoomControl();
	void setModel (QAbstractItemModel *model);
    void indexChanged (int index);
	void setWindowSize (QSize windowSize);
signals:
	void hideFullScreenUi();
public slots:
	void zoomImage(int zoomFactor);
	void decodedImageAvailable();
	void initialZoomFactor(int initZoomFactor);
private:
	void initializeZoomControl(int zoomFactor);
	void resetZoomControl();
	void updateItemPosition();
	void calculatePanOffset();
	void checkandAdjustImageBoundaries(QPointF &finalPos, QPointF undefferedPos);

private:
	QGraphicsPixmapItem *mZoomItem;
	QGraphicsWidget *mZoomWidget;
	GlxImageDecoderWrapper* mImageDecoder;
	HbIconItem *mBlackBackgroundItem;
	QSizeF mCurrentSize;
	QSizeF mItemSize;
	GlxMediaModel *mModel; //not owned
	int mInitialZoomFactor;
	int mCurrentZoomFactor;
	bool mIsControlActivated;
	bool mIsItemChanged;
	QSize mWindowSize;
	QPointF mPanOffset;
};
#endif //GLXZOOMCONTROL_h
