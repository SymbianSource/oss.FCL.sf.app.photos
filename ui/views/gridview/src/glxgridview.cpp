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
#include <hbaction.h>
#include <hbtoolbar.h>
#include <hbgridview.h>
#include <hbmainwindow.h>
#include <shareuidialog.h>
#include <hbdocumentloader.h>
#include <QAbstractItemModel>
#include <hbabstractviewitem.h>
#include <xqserviceutil.h>

//User Includes
#include "glxuistd.h"
#include "glxviewids.h"
#include "glxgridview.h"
#include "glxmodelparm.h"
#include "glxloggerenabler.h"
#include "glxdocloaderdefs.h"
#include "glxcommandhandlers.hrh"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "glxgridviewTraces.h"
#endif

GlxGridView::GlxGridView(HbMainWindow *window) : GlxView ( GLX_GRIDVIEW_ID ), 
             mGridView(NULL), mView(NULL), mWindow(window), mModel ( NULL), mVisualIndex(0),
             mItem(NULL)
    {
    OstTraceFunctionEntry0( GLXGRIDVIEW_GLXGRIDVIEW_ENTRY );
    mDocLoader = new HbDocumentLoader();
    OstTraceFunctionExit0( GLXGRIDVIEW_GLXGRIDVIEW_EXIT );
	setContentFullScreen( true );
    }

void GlxGridView::activate()
    {
    OstTraceFunctionEntry0( GLXGRIDVIEW_ACTIVATE_ENTRY );

    loadGridView(mWindow->orientation());
    addViewConnection();
    //   mVisualIndex = 0; //To:Do remove later once we get visual index change notification from grid view



    mGridView->resetTransform(); //to reset the transition effect (reset transform matrix)
    mGridView->setOpacity( 1);

    if ( mItem ) {
    mItem->resetTransform(); //to reset the transition effect (reset transform matrix)
    mItem->setOpacity( 1);
    mItem->setZValue( mItem->zValue() - 20);
    disconnect( mItem, SIGNAL( destroyed() ), this, SLOT( itemDestroyed() ) );
    mItem = NULL;
    }
    OstTraceFunctionExit0( GLXGRIDVIEW_ACTIVATE_EXIT );
    }

void GlxGridView::deActivate()
    {
    OstTraceFunctionEntry0( GLXGRIDVIEW_DEACTIVATE_ENTRY );
    removeViewConnection();


    /*if ( mItem ) {
        mItem->resetTransform(); //to reset the transition effect
        mItem->setOpacity( 1);
        disconnect( mItem, SIGNAL( destroyed() ), this, SLOT( itemDestroyed() ) );
        mItem = NULL; 
    }*/
    takeToolBar(); //To:Do improved later
    emit toolBarChanged();
    OstTraceFunctionExit0( GLXGRIDVIEW_DEACTIVATE_EXIT );
    }

void GlxGridView::setModel(QAbstractItemModel *model) 
    {
    OstTraceFunctionEntry0( GLXGRIDVIEW_SETMODEL_ENTRY );

    mModel =  model ;
    mGridView->setModel(mModel);
    QVariant variant = mModel->data( mModel->index(0,0), GlxFocusIndexRole );    
    if ( variant.isValid() &&  variant.canConvert<int> () ) {
    mGridView->scrollTo( mModel->index( variant.value<int>(),0), HbGridView::PositionAtCenter );
    }  
    
    OstTraceFunctionExit0( GLXGRIDVIEW_SETMODEL_EXIT );
    }

void GlxGridView::addToolBar( HbToolBar *toolBar ) 
    {
    OstTraceFunctionEntry0( GLXGRIDVIEW_ADDTOOLBAR_ENTRY );
    //toolBar->setParent(this); 
    if ( mGridView ) {
        toolBar->setZValue(mGridView->zValue());
    }
    setToolBar(toolBar) ;
    OstTraceFunctionExit0( GLXGRIDVIEW_ADDTOOLBAR_EXIT );
    }

void GlxGridView::enableMarking()
    {
    OstTrace0( TRACE_NORMAL, GLXGRIDVIEW_ENABLEMARKING, "GlxGridView::enableMarking" );
    mGridView->setSelectionMode(HbGridView::MultiSelection);
    }

void GlxGridView::disableMarking() 
    {
    OstTrace0( TRACE_NORMAL, GLXGRIDVIEW_DISABLEMARKING, "GlxGridView::disableMarking" );
    mGridView->setSelectionMode(HbGridView::NoSelection);
    }

void GlxGridView::handleUserAction(qint32 commandId)
    {
    OstTrace0( TRACE_NORMAL, GLXGRIDVIEW_HANDLEUSERACTION, "GlxGridView::handleUserAction" );
    switch( commandId ){
        case EGlxCmdMarkAll :
            mGridView->selectAll();
            break;

        case EGlxCmdUnMarkAll :
            mGridView->clearSelection();
            break;

        case EGlxCmdSend:
            {
            OstTrace0( TRACE_NORMAL, DUP1_GLXGRIDVIEW_HANDLEUSERACTION, "GlxGridView::handleUserAction-SendUI" );
            
            QString imagePath = (mModel->data(mModel->index(mModel->data(mModel->index(0,0),GlxFocusIndexRole).value<int>()
                    ,0),GlxUriRole)).value<QString>();

            if(imagePath.isNull())
                {
                OstTrace0( TRACE_NORMAL, DUP2_GLXGRIDVIEW_HANDLEUSERACTION, "GlxGridView::SendUi path is NULL" );
                }
            qDebug() << "GlxGridView::SendUi() imagePath= " << imagePath;

            ShareUi dialog;
            QList <QVariant> fileList;
            fileList.append(QVariant(imagePath));
            dialog.init(fileList,true);
            }
            break;

        default :
            break;
    }    
    }

QItemSelectionModel * GlxGridView::getSelectionModel()
    {
    OstTrace0( TRACE_NORMAL, GLXGRIDVIEW_GETSELECTIONMODEL, "GlxGridView::getSelectionModel" );
    return mGridView->selectionModel();    
    }

QGraphicsItem * GlxGridView::getAnimationItem(GlxEffect transitionEffect)
    {
    OstTraceFunctionEntry0( GLXGRIDVIEW_GETANIMATIONITEM_ENTRY );
    int selIndex = -1;

    if ( transitionEffect == FULLSCREEN_TO_GRID ) {
    return mGridView;
    }

    if ( transitionEffect == GRID_TO_FULLSCREEN ) {
    QVariant variant = mModel->data( mModel->index(0,0), GlxFocusIndexRole );    
    if ( variant.isValid() &&  variant.canConvert<int> () ) {
    selIndex = variant.value<int>();  
    }  

    mItem = mGridView->itemByIndex( mModel->index(selIndex,0) );
    connect(mItem, SIGNAL(destroyed()), this, SLOT( itemDestroyed()));
    mItem->setZValue( mItem->zValue() + 20); 
    return mItem;
    }

    if ( transitionEffect == GRID_TO_ALBUMLIST  || transitionEffect == ALBUMLIST_TO_GRID ){
    return mGridView;
    }
    return NULL;
    OstTraceFunctionExit0( GLXGRIDVIEW_GETANIMATIONITEM_EXIT );
    }

void GlxGridView::loadGridView(Qt::Orientation orient)
    {
    OstTraceFunctionEntry0( GLXGRIDVIEW_LOADGRIDVIEW_ENTRY );
    bool loaded = true;
    QString section;
    //Load the widgets accroding to the current Orientation
    if(orient == Qt::Horizontal)
        {
        section = GLX_GRIDVIEW_LSSECTION ;
        }
    else
        {
        section = GLX_GRIDVIEW_PTSECTION ;
        }

    if (mGridView == NULL )
        {       
        mDocLoader->load(GLX_GRIDVIEW_DOCMLPATH,&loaded);
        if(loaded)
            {
            //retrieve the widgets            
            mView = static_cast<HbView*>(mDocLoader->findWidget(QString(GLX_GRIDVIEW_VIEW)));
            mGridView = static_cast<HbGridView*>(mDocLoader->findWidget(GLX_GRIDVIEW_GRID)); 
            setWidget( mView );
            }
        }
    //Load the Sections
    mDocLoader->load(GLX_GRIDVIEW_DOCMLPATH,section,&loaded); 
    OstTraceFunctionExit0( GLXGRIDVIEW_LOADGRIDVIEW_EXIT );
    }

void GlxGridView::itemDestroyed()
    {
    OstTrace0( TRACE_NORMAL, GLXGRIDVIEW_ITEMDESTROYED, "GlxGridView::itemDestroyed" );
    disconnect( mItem, SIGNAL( destroyed() ), this, SLOT( itemDestroyed() ) );
    mItem = NULL;    
    }

QVariant  GlxGridView::itemChange (GraphicsItemChange change, const QVariant &value) 
    {
    OstTrace0( TRACE_NORMAL, GLXGRIDVIEW_ITEMCHANGE, "GlxGridView::itemChange" );
    
    static bool isEmit = true;

    if ( isEmit && change == QGraphicsItem::ItemVisibleHasChanged && value.toBool()  ) {
    emit actionTriggered( EGlxCmdSetupItem );
    isEmit = false;
    }
    return HbWidget::itemChange(change, value);
    }

void GlxGridView::addViewConnection ()
    {
    OstTrace0( TRACE_NORMAL, GLXGRIDVIEW_ADDVIEWCONNECTION, "GlxGridView::addViewConnection" );
    connect(mWindow, SIGNAL(orientationChanged(Qt::Orientation)), this, SLOT(loadGridView(Qt::Orientation)));
    connect(mGridView, SIGNAL(activated(const QModelIndex &)), this, SLOT( itemSelected(const QModelIndex &)));
    connect( mGridView, SIGNAL( scrollingEnded() ), this, SLOT( setVisvalWindowIndex() ) );
    if(XQServiceUtil::isService()){
    connect(mGridView, SIGNAL(activated(const QModelIndex &)), this, SIGNAL( gridItemSelected(const QModelIndex &)));
    }
    //    connect(mGridView, SIGNAL(scrollPositionChange(QPointF , Qt::Orientations)), this, SLOT( scrollPositionChange(QPointF, Qt::Orientations)));
    connect(mGridView, SIGNAL(longPressed( HbAbstractViewItem*, QPointF )),this, SLOT( indicateLongPress( HbAbstractViewItem*, QPointF ) ) );
    }

void GlxGridView::removeViewConnection ()
    {
    OstTrace0( TRACE_NORMAL, GLXGRIDVIEW_REMOVEVIEWCONNECTION, "GlxGridView::removeViewConnection" );
    disconnect(mWindow, SIGNAL(orientationChanged(Qt::Orientation)), this, SLOT(orientationChanged(Qt::Orientation)));
    disconnect(mGridView, SIGNAL(activated(const QModelIndex &)), this, SLOT( itemSelected(const QModelIndex &)));
    disconnect(mGridView, SIGNAL(activated(const QModelIndex &)), this, SIGNAL( gridItemSelected(const QModelIndex &)));
    disconnect( mGridView, SIGNAL( scrollingEnded() ), this, SLOT( setVisvalWindowIndex() ) );
    disconnect(mGridView, SIGNAL(longPressed( HbAbstractViewItem*, QPointF )),this, SLOT( indicateLongPress( HbAbstractViewItem*, QPointF ) ) );
    }

void GlxGridView::itemSelected(const QModelIndex &  index)
    {
    OstTrace1( TRACE_NORMAL, GLXGRIDVIEW_ITEMSELECTED, "GlxGridView::itemSelected;index=%d", index.row() );

    if ( mGridView->selectionMode() == HbGridView::MultiSelection ){ //in multi selection mode no need to open the full screen
    return ;
    }
    OstTraceEventStart0( EVENT_DUP1_GLXGRIDVIEW_ITEMSELECTED_START, "Fullscreen Launch Time" );
    
    if ( mModel ) 
        {
        mModel->setData( index, index.row(), GlxFocusIndexRole );
        }
    emit actionTriggered( EGlxCmdFullScreenOpen );
    OstTraceEventStop( EVENT_DUP1_GLXGRIDVIEW_ITEMSELECTED_STOP, "Fullscreen Launch Time", EVENT_DUP1_GLXGRIDVIEW_ITEMSELECTED_START );
    }

void GlxGridView::setVisvalWindowIndex()
    {
    OstTrace0( TRACE_IMPORTANT, GLXGRIDVIEW_SETVISVALWINDOWINDEX, "GlxGridView::setVisvalWindowIndex" );
    QList< HbAbstractViewItem * >  visibleItemList =  mGridView->visibleItems();
    qDebug("GlxGridView::setVisvalWindowIndex() %d", visibleItemList.count());    
    OstTrace1( TRACE_IMPORTANT, DUP1_GLXGRIDVIEW_SETVISVALWINDOWINDEX, "GlxGridView::setVisvalWindowIndex;visibleitemindex=%d", 
            visibleItemList.count() );
    
    if ( visibleItemList.count() <= 0 )
        return ;

    HbAbstractViewItem *item = visibleItemList.at(0);    
    if ( item == NULL ) 
        return ;
        
    OstTrace1( TRACE_IMPORTANT, DUP2_GLXGRIDVIEW_SETVISVALWINDOWINDEX, "GlxGridView::setVisvalWindowIndex item=%d", item );
        OstTrace1( TRACE_IMPORTANT, DUP3_GLXGRIDVIEW_SETVISVALWINDOWINDEX, "GlxGridView::setVisvalWindowIndex;visual index=%d",
                item->modelIndex().row() );
        
    if (  item->modelIndex().row() < 0 || item->modelIndex().row() >= mModel->rowCount() )
        return ;
    
    mModel->setData( item->modelIndex (), item->modelIndex().row(), GlxVisualWindowIndex);
    }

//To:Do remove later once we get visual index change notification from grid view
void GlxGridView::scrollPositionChange (QPointF newPosition, Qt::Orientations importantDimensions)
    {
    OstTrace0( TRACE_IMPORTANT, DUP1_GLXGRIDVIEW_SCROLLPOSITIONCHANGE, "GlxGridView::scrollPositionChange" );
    Q_UNUSED(importantDimensions);
    qreal x = newPosition.x();
    qreal y = newPosition.y();  
    int index = 0;
    OstTraceExt2( TRACE_IMPORTANT, GLXGRIDVIEW_SCROLLPOSITIONCHANGE, "GlxGridView::scrollPositionChange;x=%f;y=%f", x, y );

    if( mWindow->orientation() == Qt::Vertical ) {
    index = y / 100;
    index = index * NBR_COL;
    }
    else {
    index = y / 98;
    index = index * NBR_ROW;
    }

    OstTraceExt2( TRACE_IMPORTANT, DUP2_GLXGRIDVIEW_SCROLLPOSITIONCHANGE, 
            "GlxGridView::scrollPositionChange;index=%d;visualindex=%d", index, mVisualIndex );

    if ( qAbs (index - mVisualIndex) >= NBR_PAGE &&  index >=0 && index < mModel->rowCount() ) {
    mVisualIndex = index;
    OstTrace0( TRACE_IMPORTANT, DUP3_GLXGRIDVIEW_SCROLLPOSITIONCHANGE, 
            "GlxGridView::scrollPositionChange visual index changed" );
    //To:Do call back of model for visual index change
    //mModel->setVisibleWindowIndex( mModel->index(mVisualIndex, 0 ));
    }    
    }


GlxGridView::~GlxGridView()
    {
    OstTraceFunctionEntry0( DUP1_GLXGRIDVIEW_GLXGRIDVIEW_ENTRY );
    removeViewConnection();
    delete mGridView;
    mGridView = NULL;

    delete mDocLoader;
    mDocLoader = NULL;

    OstTraceFunctionExit0( DUP1_GLXGRIDVIEW_GLXGRIDVIEW_EXIT );
    }

void GlxGridView::indicateLongPress(HbAbstractViewItem *item, QPointF coords)
    {
    OstTrace0( TRACE_NORMAL, GLXGRIDVIEW_INDICATELONGPRESS, "GlxGridView::indicateLongPress" );
    qDebug() << "GlxGridView:indicateLongPress Item " << item->modelIndex() << "long pressed at "
    << coords;

    if ( mGridView->selectionMode() == HbGridView::MultiSelection ){ //in multi selection mode no need to open the context menu
    return ;
    }

    if ( mModel ) {
    mModel->setData( item->modelIndex(), item->modelIndex().row(), GlxFocusIndexRole );
    }
    emit itemSpecificMenuTriggered(viewId(),coords);
    }
