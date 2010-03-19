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



#ifndef GLXCOVERFLOW_H
#define GLXCOVERFLOW_H

#define NBR_ICON_ITEM 5

#include <hbscrollarea.h>
#include <hbeffect.h>

//forward declaration
class HbIconItem;
class HbMainWindow;
class QAbstractItemModel;

typedef enum
{
    NO_MOVE,
    TAP_MOVE,
    LEFT_MOVE,
    RIGHT_MOVE,
    LONGPRESS_MOVE,
} GlxUserMove;

typedef enum
{
    TAP_EVENT, //send the signal when user tap on full screen
    PANNING_START_EVENT, //send the signal when panning of full screen start
    EMPTY_ROW_EVENT //send the signal when model have no data
} GlxCoverFlowEvent;

class GlxCoverFlow : public HbScrollArea
{
Q_OBJECT

public :
	GlxCoverFlow (QGraphicsItem *parent = NULL);
	~GlxCoverFlow ();
    void setItemSize (QSize &size);
    void setModel (QAbstractItemModel *model);
    void indexChanged (int index);
    void setUiOn(bool uiOn) { mUiOn = uiOn; }
    void partiallyClean();
    void partiallyCreate(QAbstractItemModel *model, QSize itemSize);
	void rotateImage();
    void setCoverFlow();
    void ClearCoverFlow();
		    
public slots:
    void rotationEffectFinished( const HbEffect::EffectStatus &status );

signals :
    void coverFlowEvent(GlxCoverFlowEvent e);
    void changeSelectedIndex(const QModelIndex &index);
    void autoLeftMoveSignal();
    void autoRightMoveSignal();

protected slots:
    void panGesture ( const QPointF & delta )  ;
    void leftGesture (int value);
    void rightGesture (int value) ;
    void longPressGesture(const QPointF &point) ;
    void dataChanged(QModelIndex startIndex, QModelIndex endIndex);
    void rowsInserted(const QModelIndex &parent, int start, int end);
    void rowsRemoved(const QModelIndex &parent, int start, int end);
    void autoLeftMove();
    void autoRightMove();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);	
    void mouseReleaseEvent (QGraphicsSceneMouseEvent *event) ;

    void move(int value);

    void setRows() ;
    void setStripLen();
    int calculateIndex(int index);
    
    void loadIconItems (); 
    void updateIconItem (qint16 selIndex, qint16 selIconIndex, qint16 deltaX);
    
    //clear all the model connection
    void clearCurrentModel();
    //add the connection to the model
    void initializeNewModel();
    //reset all the data of cover flow
    void resetCoverFlow();
	
private slots:

private:
	HbIconItem *mIconItem[NBR_ICON_ITEM];      //at most contain only five item
    qint16 mSelItemIndex;                    // current full screen index
    qint16 mRows;                        // total number of item  
    //QModelIndex mIndex;                  //current model index of seleced item
    qint16 mSelIndex;                    // remove once model concept is integreted
    qint32 mStripLen;                    //virtual strip lenght
    qint32 mCurrentPos;                   //current postion in the virtual strip
    QSize mItemSize;                     //hb icon item size
    bool mUiOn;
    int mBounceBackDeltaX;
    QAbstractItemModel *mModel;
    GlxUserMove mMoveDir;                       //arun_goel3 may be remove later or change later
	int rotAngle;
        
};

#endif /* GLXCOVERFLOW_H_ */
