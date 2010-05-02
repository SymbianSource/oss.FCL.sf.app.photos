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




#ifndef GLXGRIDVIEW_H
#define GLXGRIDVIEW_H

#include "glxview.h"

//Qt/Orbit forward declarations
class HbMainWindow;
class QAbstractItemModel;
class HgWidget;
class GlxModelWrapper;
class HbPushButton;
class HbIconItem;

class GlxGridView : public GlxView
{
    Q_OBJECT

public :
    GlxGridView(HbMainWindow *window);
    ~GlxGridView();
    void activate() ;
    void deActivate();
    void initializeView(QAbstractItemModel *model);
    void setModel(QAbstractItemModel *model);
    void addToolBar( HbToolBar *toolBar );
    void enableMarking() ;
    void disableMarking() ;
    void handleUserAction(qint32 commandId); 
    QItemSelectionModel * getSelectionModel() ;
    QGraphicsItem * getAnimationItem(GlxEffect transitionEffect);

public slots:
    void itemSelected(const QModelIndex &  index);
    void scrollingEnded();
    void scrollingStarted();
    void orientationchanged(Qt::Orientation orient);
    void visibleIndexChanged(const QModelIndex& current, const QModelIndex& previous);

protected :
    QVariant  itemChange (GraphicsItemChange change, const QVariant &value) ;

private slots:
    void indicateLongPress(const QModelIndex& index, QPointF coords);
    void uiButtonClicked(bool checked);
	
private:
    void addViewConnection();
    void removeViewConnection();
    void hideorshowitems(Qt::Orientation orient);
    void scrolltofocus();
    void loadGridView();
  
	HbMainWindow        *mWindow;          // no ownership
	QAbstractItemModel  *mModel ;
	HgWidget            *mWidget;          // HG Grid Widget
	QItemSelectionModel *mSelectionModel;  // Selected items model
    GlxModelWrapper     *mModelWrapper;    // Temp Model Wrapper, so That Role Change not a problem
    HbPushButton        *mUiOnButton;
    bool                 mScrolling;
    HbIconItem          *mIconItem;
};

#endif /* GLXGRIDVIEW_H_ */
