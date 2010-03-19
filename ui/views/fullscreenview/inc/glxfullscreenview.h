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



#ifndef GLXFULLSCREENVIEW_H
#define GLXFULLSCREENVIEW_H

//Includes
#include <hbeffect.h>

//User Defined Includes
#include <glxview.h>
#include <glxcoverflow.h>
//Orbit/Qt forward declartion
class QTimer;
class HbGridView;
class HbMainWindow;
class HbPushButton;
class HbDocumentLoader;
class QAbstractItemModel;

//User Defined forward declartion
class GlxZoomSlider;
class GlxZoomControl;
class CGlxHdmiController;

class GlxFullScreenView : public GlxView
{
    Q_OBJECT
public :
    GlxFullScreenView(HbMainWindow *window,HbDocumentLoader *DocLoader);
    ~GlxFullScreenView();
    void activate() ;
    void deActivate();

    /*
     * Initialize the coverflow and partially creates the coverflow with one image
     * to make the widget light weight in order to make transition smooth
     * and also loads the widgets. 
     */
    void initializeView(QAbstractItemModel *model);
    
   /*
     * resets the view, with just one icon being present in the widget
     * to make the widget light weight in order to make
     * transition smooth
     */
    void resetView();
    void setModel(QAbstractItemModel *model);
    void setModelContext ( );
    void cleanUp();
    void handleUserAction(qint32 commandId);
    QGraphicsItem * getAnimationItem(GlxEffect transitionEffect);
    
    
public slots:
    void orientationChanged(Qt::Orientation);
    void activateUI();
    void hideUi();
    void changeSelectedIndex(const QModelIndex &index);
    void indexChanged(const QModelIndex &index );
    void scrollingStarted();
    void scrollingEnded();
    void pressed(const QModelIndex &index );
    void released(const QModelIndex &index );
	void showdetailsview();
    void setVisvalWindowIndex();
    void coverFlowEventHandle( GlxCoverFlowEvent e);
 
signals :
    void cancelTimer();

private:
    
    /*
     * The widgets are retrieved from the docml
     */
    void loadWidgets();

    /*
     * Loads the corresponding sections in the docml while the orentation is changed.
     */
    void loadViewSection();
    
    void setLayout();
    void addConnection();
    void addImageStripConnection();
    void removeConnection();
    void createUiControl();
    void setImageStripModel();
    
    /*
     * Called when an item is highlighted.
     */
    void SetImageToHdmiL();
private:
    QAbstractItemModel  *mModel;   //no ownership
    HbMainWindow        *mWindow;  //no ownership
    GlxCoverFlow        *mCoverFlow;
    HbGridView          *mImageStrip;
    bool                mUiOff;        // to check the current status of ui on / off
    QTimer              *mUiOffTimer;  //use for ui off after 30 sec
	  //for Zoom
    HbPushButton        *mZmPushButton;
    GlxZoomSlider       *mZoomSlider;
    GlxZoomControl      *mZoomControl;
    bool                mSendUserActivityEvent;
	//for the  FLIP CASE
    HbPushButton        *mFlipPushButton;
    int nbrCol; //to store the number of column in the image strip
    HbDocumentLoader    *mDocLoader;
    CGlxHdmiController* iHdmiController;
};

#endif /* GLXFULLSCREENVIEW_H_ */
