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
#ifndef GLXZOOMSLIDER_H
#define GLXZOOMSLIDER_H
#include <QObject>
#include <QSize>
class HbSlider;
class GlxMediaModel;
class QAbstractItemModel;
class QGraphicsItem;
class GlxZoomSlider : public QObject
{
Q_OBJECT

public:
	GlxZoomSlider (QGraphicsItem *parent = NULL);
	~GlxZoomSlider ();
	void setModel (QAbstractItemModel *model);
    void indexChanged (int index);
signals:
	void valueChanged(int newValue);
	void initialZoomFactor(int initZoomFactor);
public slots:
	void toggleSliderVisibility();
	void filterandEmitSliderValueChanges(int newValue);
	void sliderThumbPressed();
	void sliderThumbReleased();
private:
	void retrieveActualAndDisplayedSize(QSize& itemsize, QSize& displayedSize);
	int calculateZoomFactor(QSize& itemsize, QSize& displayedSize);
	void calculateAndInitializeFSZoomFactor();
	HbSlider* mZoomSlider;
	bool isSliderVisible; 
	bool mSliderThumbPressed;
	GlxMediaModel *mModel; //not owned
};
#endif //GLXZOOMSLIDER_H
