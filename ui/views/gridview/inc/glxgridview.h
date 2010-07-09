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
class HgGrid;
class GlxModelWrapper;
class HbPushButton;
class HbIconItem;
class HbCheckBox;
class HbLabel;
class GlxSettingInterface;

class GlxGridView : public GlxView
{
    Q_OBJECT

public :
    GlxGridView(HbMainWindow *window);
    ~GlxGridView();
    void activate() ;
    void deActivate();
    void initializeView( QAbstractItemModel *model, GlxView *preView );
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
    void stateChanged(int state);
    //show the marked item count
    void showMarkedItemCount();
    //show item count in the grid
    void showItemCount();
    //clear all the model connection
    void clearCurrentModel();
    //add the connection to the model
    void initializeNewModel();
    void showAlbumTitle(QString aTitle);
    void populated();

protected :
   

private slots:
    void indicateLongPress(const QModelIndex& index, QPointF coords);
    void uiButtonClicked(bool checked);
    void cameraButtonClicked(bool checked);
	
private:
    void addViewConnection();
    void removeViewConnection();
    void hideorshowitems(Qt::Orientation orient);
    void scrolltofocus();
    void loadGridView();
    void showHbItems();
    int getSubState();
    void showNoImageString();

	HbMainWindow        *mWindow;          // no ownership
	QAbstractItemModel  *mModel ;
	HgGrid              *mWidget;          // HG Grid Widget
	QItemSelectionModel *mSelectionModel;  // Selected items model
    GlxModelWrapper     *mModelWrapper;    // Temp Model Wrapper, so That Role Change not a problem
    HbPushButton        *mUiOnButton;
    HbPushButton        *mCameraButton;    // Camera Button, when item count is zero
    bool                 mScrolling;
    HbIconItem          *mIconItem;
    HbCheckBox          *mMarkCheckBox;    // Mark All checkbox 
    HbLabel             *mCountItem;       // Item count of the grid
    HbLabel             *mMainLabel;       
    HbLabel             *mCountLabel;      // Marked item count
    HbLabel             *mZeroItemLabel;   // zero itemcount
    HbLabel             *mAlbumName;
    GlxSettingInterface *mSettings;      
};

#endif /* GLXGRIDVIEW_H_ */
