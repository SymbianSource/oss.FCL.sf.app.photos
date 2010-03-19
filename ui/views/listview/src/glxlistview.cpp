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


//Includes
#include <QDebug>
#include <hbmenu.h>
#include <hbtoolbar.h>
#include <hblistview.h>
#include <hbmainwindow.h>
#include <hbdocumentloader.h>
#include <hbabstractviewitem.h>

//User Includes
#include "glxviewids.h"
#include "glxlistview.h"
#include "glxmodelparm.h"
#include "glxdocloaderdefs.h"
#include "glxcommandhandlers.hrh"


GlxListView::GlxListView(HbMainWindow *window) : GlxView ( GLX_LISTVIEW_ID ), 
             mListView(NULL), mView(NULL), mWindow(window), mModel ( NULL)
{
    qDebug("GlxListView::GlxListView()");
    mDocLoader = new HbDocumentLoader();
    setContentFullScreen( true );
}

void GlxListView::activate()
{
    qDebug("GlxListView::activate() %d", mWindow->orientation() );    

    if (mListView == NULL) {
        createListView();
    }
 }

void GlxListView::deActivate()
{
    qDebug("GlxListView::deActivate()");
    disconnect(mWindow, SIGNAL(orientationChanged(Qt::Orientation)), this, SLOT(orientationChanged(Qt::Orientation)));
    takeToolBar(); //To:Do improved later
    emit toolBarChanged();
}

void GlxListView::setModel(QAbstractItemModel *model) 
{
    qDebug("GlxListView::setModel()");
    mModel =  model ;
    mListView->setModel(mModel);
}

void GlxListView::addToolBar( HbToolBar *toolBar ) 
{
    //toolBar->setParent(this);
    if ( mListView ) {
        toolBar->setZValue(mListView->zValue());
    }
    setToolBar(toolBar) ;
}

void GlxListView::initializeView(QAbstractItemModel *model)
{
    qDebug("GlxListView::initializeView()");
    if (mListView == NULL) {
        createListView();
    }
    setModel(model);
}

QGraphicsItem * GlxListView::getAnimationItem( GlxEffect transtionEffect )
{
    if ( transtionEffect == GRID_TO_ALBUMLIST  || transtionEffect == ALBUMLIST_TO_GRID ) {
        return mListView;
    }
    
    return NULL;
}

void GlxListView::addViewConnection ()
{
    qDebug("GlxListView::addViewConnection()");
    connect(mListView, SIGNAL(activated(const QModelIndex &)), this, SLOT( itemSelected(const QModelIndex &)));
    connect(mListView, SIGNAL(longPressed( HbAbstractViewItem*, QPointF )),this, SLOT( indicateLongPress( HbAbstractViewItem*, QPointF ) ) );
}

void GlxListView::removeViewConnection()
{
    qDebug("GlxListView::removeViewConnection()");
    disconnect(mListView, SIGNAL(activated(const QModelIndex &)), this, SLOT( itemSelected(const QModelIndex &)));
    disconnect(mListView, SIGNAL(longPressed( HbAbstractViewItem*, QPointF )),this, SLOT( indicateLongPress( HbAbstractViewItem*, QPointF ) ) );
}

void GlxListView::loadListView()
{
    qDebug("GlxListView::loadListView()");
    bool loaded = true;
    //Load the widgets accroding to the current Orientation
    if (mListView == NULL )
        {
        mDocLoader->load(GLX_LISTVIEW_DOCMLPATH,&loaded);
        if(loaded)
            {
            //retrieve the widgets
            mView = static_cast<HbView*>(mDocLoader->findWidget(QString(GLX_LISTVIEW_VIEW)));
            mListView = static_cast<HbListView*>(mDocLoader->findWidget(QString(GLX_LISTVIEW_LIST)));

            if(mListView)
                { 
                //sets the widget
                setWidget((QGraphicsWidget*) mView);
                }
            }   
        }
}

void GlxListView::createListView()
{
    qDebug("GlxListView::createListView()");
    loadListView(); 
    addViewConnection();  //only one time add the connection and remove in side the destructor    
}

GlxListView::~GlxListView()
{
    qDebug("GlxListView::~GlxListView()");

    if(widget())
        {
        qDebug("GlxListView::~GlxListView() takeWidget");
        takeWidget();    
        }       

    removeViewConnection();

    if(mListView)
        {
        delete mListView;
        mListView = NULL;
        }

    if(mView)
        {
        delete mView ;
        mView = NULL;
        }
    if(mDocLoader)
        {
        delete mDocLoader;
        mDocLoader = NULL;
        }    
}

void GlxListView::itemSelected(const QModelIndex &  index)
{
    qDebug("GlxListView::itemSelected() index = %d", index.row() );
    if ( mModel ) {
        mModel->setData( index, index.row(), GlxFocusIndexRole );
    }
    emit actionTriggered( EGlxCmdAlbumGridOpen );
}


void GlxListView::indicateLongPress(HbAbstractViewItem *item, QPointF coords)
{
    qDebug() << "GlxListView:indicateLongPress Item " << item->modelIndex() << "long pressed at " << coords;
    if ( mModel ) {
        mModel->setData( item->modelIndex(), item->modelIndex().row(), GlxFocusIndexRole );
    }      
    emit itemSpecificMenuTriggered(viewId(),coords);
}

