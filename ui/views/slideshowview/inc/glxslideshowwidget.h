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



#ifndef GLXSLIDESHOWWIDGET_H
#define GLXSLIDESHOWWIDGET_H

//Includes
#include <hbwidget.h>
#include <hbeffect.h>

//Qt/Orbit forward declarations
class QTimer;
class QGestureEvent;
class HbIconItem;
class HbMainWindow;
class HbPushButton;
class HbMainWindow;
class QGraphicsItem;
class HbDocumentLoader;
class HbAbstractDataModel;
class HbLabel;

//User Forward Declarations
class GlxEffectEngine;
class GlxSettingInterface;

#define NBR_ITEM 3
typedef enum
{
    UI_ON_EVENT, //send the signal when user tap on screen ( on the UI )
    UI_OFF_EVENT, //send the signal when user press continous button ( off the UI)
    EMPTY_DATA_EVENT, //send the signal when model have no data
    EFFECT_STARTED  // sends the signal when effect is started.
} GlxSlideShowEvent;

class GlxSlideShowWidget : public HbWidget
{
Q_OBJECT

public :
    GlxSlideShowWidget (QGraphicsItem *parent = NULL);
    ~GlxSlideShowWidget ();
    void setModel (QAbstractItemModel *model);
    void setItemGeometry(QRect screenRect);
    void startSlideShow();
    void stopSlideShow();
    /*
     * Initialise the slideshow widget
     * creation of the icons are done here
     */
    void setSlideShowWidget(HbDocumentLoader *DocLoader);
    
    /*
     * Cleans up the slide show widget
     */
    void cleanUp();
    
signals:
    void slideShowEvent(GlxSlideShowEvent e);
    void indexchanged();
    
public slots :
    void triggeredEffect();
    void effectFinshed();
    void cancelEffect();
    void pauseSlideShow();
    void continueSlideShow(bool check);
    void dataChanged(QModelIndex startIndex, QModelIndex endIndex);
    void rowsInserted(const QModelIndex &parent, int start, int end);
    void rowsRemoved(const QModelIndex &parent, int start, int end);
    void modelDestroyed();
    void orientationChanged(QRect screenRect);

    void leftMoveEffectFinished( const HbEffect::EffectStatus &status );
    void rightMoveEffectFinished( const HbEffect::EffectStatus &status );
    
protected slots :
    void leftGesture (int value);
    void rightGesture (int value);
    
protected :
    void gestureEvent(QGestureEvent *event);
    
private :
    //clear all the model connection
    void clearCurrentModel();
    //add the connection to the model
    void initializeNewModel();
    void resetSlideShow();
    void moveImage( int nextIndex, int posX, const QString & move, char * callBack );
    void addConnections();
    void removeConnections();
    
    /*
     * To get the focus index
     */
    int getFocusIndex( );

    /*
     * To get the full screen icon of the image
     */
    HbIcon getIcon( int index );
    
    /*
     * To check the itemis corrupted or not
     */
    bool isCorrupt( int index );
    
    /*
     * To set the current ( focus ) item icon
     */
    bool setFocusItemIcon();
    
    /*
     * To set the next itme icon in the list
     */
    bool setNextItemIcon();
    
    /*
     * To set the previous icon in the list
     */
    bool setPreItemIcon();
    
    /*
     * In the case of all the image are corrupted then show the error notes
     */
    void showErrorNote();
    
    /*
     * It will hide the corrupted images note
     */
    void hideErrorNote();    

private:
    GlxEffectEngine          *mEffectEngine;
    GlxSettingInterface      *mSettings;               //no ownership
    HbIconItem               *mIconItems[ NBR_ITEM ]; 
    HbIconItem               *mBackGroundItem;
    HbPushButton             *mContinueButton;
    HbLabel                  *mErrorNote ;               //when all the image are corrupted then show the no image label
    int                      mItemIndex;
    int                      mSelIndex[ NBR_ITEM ];
    QTimer                   *mSlideTimer;
    QAbstractItemModel       *mModel;
    QRect                    mScreenRect;
    QList <QGraphicsItem *>  mItemList;
    bool                     mIsPause;
    int                      mSlideShowItemCount;
};

#endif /* GLXSLIDESHOWWIDGET_H */
