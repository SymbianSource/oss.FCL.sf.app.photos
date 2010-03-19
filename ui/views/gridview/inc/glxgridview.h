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
class HbGridView;
class HbMainWindow;
class HbDocumentLoader;
class QAbstractItemModel;
class HbAbstractViewItem;

#define NBR_ROW 5
#define NBR_COL 3
#define NBR_PAGE NBR_ROW * NBR_COL

class GlxGridView : public GlxView
{
    Q_OBJECT

public :
    GlxGridView(HbMainWindow *window);
    ~GlxGridView();
    void activate() ;
    void deActivate();
    void setModel(QAbstractItemModel *model);
    void addToolBar( HbToolBar *toolBar );
    void enableMarking() ;
    void disableMarking() ;
    void handleUserAction(qint32 commandId); 
    QItemSelectionModel * getSelectionModel() ;
    QGraphicsItem * getAnimationItem(GlxEffect transitionEffect);

public slots:
    void itemSelected(const QModelIndex &  index);
    void setVisvalWindowIndex();
    void scrollPositionChange (QPointF newPosition, Qt::Orientations importantDimensions);
    /*
     * This loads the docml and retrives the widgets from the 
     * docml corresponding to the present orentation 
     */
     void loadGridView(Qt::Orientation orient);
    void itemDestroyed();
        
protected :
    QVariant  itemChange (GraphicsItemChange change, const QVariant &value) ;

private slots:
    void indicateLongPress( HbAbstractViewItem *item, QPointF coords );
	
private:
    void addViewConnection();
    void removeViewConnection();
  
private:
	HbGridView          *mGridView; 
	HbView              *mView;
	HbMainWindow        *mWindow;  //no ownership
	QAbstractItemModel  *mModel ;
	int mVisualIndex;             //first item index of the page //To:Do remove later
	HbAbstractViewItem   *mItem;
    HbDocumentLoader     *mDocLoader; //Docml loader to load the widgets from docml    
};

#endif /* GLXGRIDVIEW_H_ */
