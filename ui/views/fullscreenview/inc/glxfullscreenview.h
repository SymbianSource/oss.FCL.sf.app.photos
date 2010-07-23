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

#define NBR_ANIM_ITEM 2

//Orbit/Qt forward declartion
class QTimer;
class HbAction;
class HbGridView;
class HbMainWindow;
class HbPushButton;
class HbDocumentLoader;
class QAbstractItemModel;
class HbIconItem;

//User Defined forward declartion
class GlxZoomWidget;
class GlxTvOutWrapper;

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
    void initializeView( QAbstractItemModel *model, GlxView *preView );
    
   /*
     * resets the view, with just one icon being present in the widget
     * to make the widget light weight in order to make
     * transition smooth
     */
    void resetView();
    void setModel(QAbstractItemModel *model);
    void setModelContext ( );
    void cleanUp();
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
	void setVisvalWindowIndex();
    void coverFlowEventHandle( GlxCoverFlowEvent e);
    void effectFinished( const HbEffect::EffectStatus  );
    void imageSelectionEffectFinished( const HbEffect::EffectStatus  );
    void handleToolBarAction();

protected :
    bool eventFilter(QObject *obj, QEvent *ev);
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
    
    void loadFullScreenToolBar();
    void addToolBarAction( int commandId, const QString &iconName, const QString &name) ;
    void imageSelectionAnimation( const QModelIndex &index );
    void cancelSelectionAnimation( );
    int getSubState();
    void setHdmiModel( QAbstractItemModel *model );
    
private:
    QAbstractItemModel  *mModel;   //no ownership
    HbMainWindow        *mWindow;  //no ownership
    GlxCoverFlow        *mCoverFlow;
    HbGridView          *mImageStrip;
    QTimer              *mUiOffTimer;  //use for ui off after 30 sec
    HbIconItem          *mIconItems[ NBR_ANIM_ITEM ] ;   //temporary item for play the image strip select animation
    GlxTvOutWrapper     *mTvOutWrapper;
    HbToolBar           *mFullScreenToolBar; //Fullscreen Toolbar
    //for Zoom
	GlxZoomWidget		*mZoomWidget;
    HbDocumentLoader    *mDocLoader;
  
    bool                mUiOff;        // to check the current status of ui on / off
};

#endif /* GLXFULLSCREENVIEW_H_ */
