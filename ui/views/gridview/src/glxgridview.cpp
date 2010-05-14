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
#include <hbmainwindow.h>
#include <hggrid.h>
#include <glxmodelwrapper.h>
#include <hbpushbutton.h>
#include <HbToolBar> // Temp
#include <hbiconitem.h>
#include <hbicon.h>
#include <xqserviceutil.h>

//User Includes
#include "glxviewids.h"
#include "glxgridview.h"
#include "glxmodelparm.h"
#include "glxcommandhandlers.hrh"


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "glxgridviewTraces.h"
#endif

GlxGridView::GlxGridView(HbMainWindow *window) 
    : GlxView ( GLX_GRIDVIEW_ID ), 
      mWindow(window), 
      mModel ( NULL), 
      mWidget(NULL),
      mSelectionModel(NULL),
      mModelWrapper(NULL),
      mUiOnButton(NULL),
      mScrolling(FALSE),
      mIconItem(NULL)
{
    OstTraceFunctionEntry0( GLXGRIDVIEW_GLXGRIDVIEW_ENTRY );
    mModelWrapper = new GlxModelWrapper();
    mModelWrapper->setRoles(GlxQImageSmall);
    mIconItem = new HbIconItem(this);
    OstTraceFunctionExit0( GLXGRIDVIEW_GLXGRIDVIEW_EXIT );
}

void GlxGridView::activate()
{
    OstTraceFunctionEntry0( GLXGRIDVIEW_ACTIVATE_ENTRY );
    if(mUiOnButton == NULL) {
        mUiOnButton = new HbPushButton("UI",this);
        connect(mUiOnButton, SIGNAL(clicked(bool)), this, SLOT(uiButtonClicked(bool)));
        mUiOnButton->setGeometry(QRectF(610,0,15,15));
        mUiOnButton->setZValue(1);
        mUiOnButton->hide();
    }
    loadGridView();
    OstTraceFunctionExit0( GLXGRIDVIEW_ACTIVATE_EXIT );
}

void GlxGridView::deActivate()
{
    OstTraceFunctionEntry0( GLXGRIDVIEW_DEACTIVATE_ENTRY );
    if (mUiOnButton && mUiOnButton->isVisible())
        {
        mUiOnButton->hide();
        }    
    if(mIconItem)
        {
        mIconItem->hide();
        mIconItem->resetTransform();
        mIconItem->setOpacity(0);
        mIconItem->setZValue(mIconItem->zValue()-20);
        }
    OstTraceFunctionExit0( GLXGRIDVIEW_DEACTIVATE_EXIT );
}

void GlxGridView::initializeView(QAbstractItemModel *model)
{
    activate();
    setModel(model);
}

void GlxGridView::setModel(QAbstractItemModel *model) 
{
    OstTraceFunctionEntry0( GLXGRIDVIEW_SETMODEL_ENTRY );
    if(model)
        {
        mModel =  model;
        QVariant variantimage = mModel->data(mModel->index(0,0),GlxDefaultImage);
        if (mWidget && variantimage.isValid() &&  variantimage.canConvert<QImage> () )
            {
            mWidget->setDefaultImage(variantimage.value<QImage>());
            }
        mModelWrapper->setModel(mModel);
        mWidget->setModel(mModelWrapper);  
        if(!mSelectionModel)
            {
            mSelectionModel = new QItemSelectionModel(mModelWrapper, this);
            connect(mSelectionModel, SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)), this, SLOT(visibleIndexChanged(const QModelIndex &, const QModelIndex &)));
            mWidget->setSelectionModel(mSelectionModel);
            }
        scrolltofocus();  // Need to do it here ?
        }
    OstTraceFunctionExit0( GLXGRIDVIEW_SETMODEL_EXIT );
}

void GlxGridView::visibleIndexChanged(const QModelIndex& current, const QModelIndex& previous)
{
    if(mScrolling)
        {
        Q_UNUSED(previous);
        if(current.row() >= 0 || current.row() < mModel->rowCount())
            {
            mModel->setData(mModel->index(0,0),current.row(),GlxVisualWindowIndex);
            }
        }
}

void GlxGridView::addToolBar( HbToolBar *toolBar ) 
{
    OstTraceFunctionEntry0( GLXGRIDVIEW_ADDTOOLBAR_ENTRY ); 
    setToolBar(toolBar);  
    hideorshowitems(mWindow->orientation());
    OstTraceFunctionExit0( GLXGRIDVIEW_ADDTOOLBAR_EXIT );
}

void GlxGridView::enableMarking()
{
    OstTrace0( TRACE_NORMAL, GLXGRIDVIEW_ENABLEMARKING, "GlxGridView::enableMarking" );
    mWidget->setSelectionMode(HgWidget::MultiSelection);
}

void GlxGridView::disableMarking() 
{
    OstTrace0( TRACE_NORMAL, GLXGRIDVIEW_DISABLEMARKING, "GlxGridView::disableMarking" );
    mWidget->setSelectionMode(HgWidget::NoSelection);
}

void GlxGridView::handleUserAction(qint32 commandId)
{
    OstTrace0( TRACE_NORMAL, GLXGRIDVIEW_HANDLEUSERACTION, "GlxGridView::handleUserAction" );
    switch( commandId ) {
        case EGlxCmdMarkAll :
            mWidget->selectAll();
            break;

        case EGlxCmdUnMarkAll :
            mWidget->clearSelection();
            break;        

        default :
            break;
    }    
}

QItemSelectionModel * GlxGridView::getSelectionModel()
{
    OstTrace0( TRACE_NORMAL, GLXGRIDVIEW_GETSELECTIONMODEL, "GlxGridView::getSelectionModel" );
    return mWidget->selectionModel();
}

QGraphicsItem * GlxGridView::getAnimationItem(GlxEffect transitionEffect)
{
    OstTraceFunctionEntry0( GLXGRIDVIEW_GETANIMATIONITEM_ENTRY );
    int selIndex = -1;

    if ( transitionEffect == FULLSCREEN_TO_GRID ) {
        return mWidget;
    }

    if ( transitionEffect == GRID_TO_FULLSCREEN )
        {
        QVariant variant = mModel->data( mModel->index(0,0), GlxFocusIndexRole );    
        if ( variant.isValid() &&  variant.canConvert<int> () ) 
            {
            selIndex = variant.value<int>();  
            } 
        else 
            {
            return mWidget;
            }
        QVariant variantimage = mModel->data(mModel->index(selIndex,0),Qt::DecorationRole);
        if ( variantimage.isValid() &&  variantimage.canConvert<HbIcon> () )
            {
            QPolygonF poly;
            if (mModelWrapper && !mWidget->getItemOutline(mModelWrapper->index(selIndex,0), poly))
                {
                return mWidget;
                }
            QRectF itemRect = poly.boundingRect();

            mIconItem->setSize(QSize(120,120));
            HbIcon tempIcon =  variantimage.value<HbIcon>();
            QPixmap tempPixmap = tempIcon.qicon().pixmap(120, 120);
            QSize sz = QSize ( 120, 120);
            tempPixmap = tempPixmap.scaled(sz, Qt::IgnoreAspectRatio );
            HbIcon tmp = HbIcon( QIcon(tempPixmap)) ;
            mIconItem->setIcon(tmp);
            mIconItem->setPos(itemRect.topLeft());
            mIconItem->setZValue(mIconItem->zValue() + 20);
            mIconItem->show();
            return mIconItem;   
            }
        else
            {
            return mWidget;
            }
        }

    if ( transitionEffect == GRID_TO_ALBUMLIST  || transitionEffect == ALBUMLIST_TO_GRID ){
        return mWidget;
    }
    OstTraceFunctionExit0( GLXGRIDVIEW_GETANIMATIONITEM_EXIT );
    return NULL;    
}

void GlxGridView::loadGridView()
{
    OstTraceFunctionEntry0( GLXGRIDVIEW_LOADGRIDVIEW_ENTRY );
    if(mWidget == NULL) {
        Qt::Orientation orient = mWindow->orientation();
        mWindow->viewport()->grabGesture(Qt::PanGesture);
        mWindow->viewport()->grabGesture(Qt::TapGesture);
        mWindow->viewport()->grabGesture(Qt::TapAndHoldGesture);
        mWidget = new HgGrid(orient);
        mWidget->setLongPressEnabled(true);
        mWidget->setScrollBarPolicy(HgWidget::ScrollBarAutoHide);  
        //mWidget->setItemSize(QSizeF(120,120));
        setWidget( mWidget );  
        addViewConnection();
        //hideorshowitems(orient);
    }
    OstTraceFunctionExit0( GLXGRIDVIEW_LOADGRIDVIEW_EXIT );
}

void GlxGridView::orientationchanged(Qt::Orientation orient)
{
    hideorshowitems(orient);
}
void GlxGridView::hideorshowitems(Qt::Orientation orient)
{
    if(orient == Qt::Horizontal) 
        {
        if (mUiOnButton)
            {
            mUiOnButton->show();
            }
        setItemVisible(Hb::AllItems, FALSE) ;
        }
    else
        {
        if (mUiOnButton)
            {
            mUiOnButton->hide();
            }
        setItemVisible(Hb::AllItems, TRUE) ;
        }
}

void GlxGridView::scrolltofocus()
{
    if(mModelWrapper && mWidget)
        {
        QVariant variant = mModelWrapper->data( mModelWrapper->index(0,0), GlxFocusIndexRole );    
        if ( variant.isValid() &&  variant.canConvert<int> () ) 
            {
            mWidget->scrollTo( mModelWrapper->index( variant.value<int>(),0) );
            mModel->setData(mModel->index(0,0),variant.value<int>(),GlxVisualWindowIndex);
            }
        }
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

void GlxGridView::addViewConnection()
{
    OstTrace0( TRACE_NORMAL, GLXGRIDVIEW_ADDVIEWCONNECTION, "GlxGridView::addViewConnection" );
    connect(mWindow, SIGNAL(orientationChanged(Qt::Orientation)), this, SLOT(orientationchanged(Qt::Orientation)));
    //connect(mWindow, SIGNAL(aboutToChangeOrientation()), mWidget, SLOT(aboutToChangeOrientation()));
    connect(mWindow, SIGNAL(orientationChanged(Qt::Orientation)), mWidget, SLOT(orientationChanged(Qt::Orientation)));
    connect(mWidget, SIGNAL(activated(const QModelIndex &)), SLOT( itemSelected(const QModelIndex &)));
    connect(mWidget, SIGNAL( scrollingStarted() ), this, SLOT( scrollingStarted() ) );
    connect(mWidget, SIGNAL( scrollingEnded() ), this, SLOT( scrollingEnded() ) );
    connect(mWidget, SIGNAL(longPressed(const QModelIndex &, QPointF)), SLOT( indicateLongPress(const QModelIndex &, QPointF) ) );
}

void GlxGridView::removeViewConnection ()
{
    OstTrace0( TRACE_NORMAL, GLXGRIDVIEW_REMOVEVIEWCONNECTION, "GlxGridView::removeViewConnection" );
    if(mWidget)
        {
        disconnect(mWindow, SIGNAL(orientationChanged(Qt::Orientation)), this, SLOT(orientationchanged(Qt::Orientation)));
        //disconnect(mWindow, SIGNAL(aboutToChangeOrientation()), mWidget, SLOT(aboutToChangeOrientation()));
        disconnect(mWindow, SIGNAL(orientationChanged(Qt::Orientation)), mWidget, SLOT(orientationChanged(Qt::Orientation)));
        disconnect(mWidget, SIGNAL(activated(const QModelIndex &)),this, SLOT( itemSelected(const QModelIndex &)));
        disconnect(mWidget, SIGNAL( scrollingStarted() ), this, SLOT( scrollingStarted() ) );
        disconnect(mWidget, SIGNAL( scrollingEnded() ), this, SLOT( scrollingEnded() ) );
        disconnect(mWidget,  SIGNAL(longPressed(const QModelIndex &, QPointF)),this, SLOT( indicateLongPress(const QModelIndex &, QPointF) ) );
        }
}


void GlxGridView::itemSelected(const QModelIndex &  index)
{
    OstTrace1( TRACE_NORMAL, GLXGRIDVIEW_ITEMSELECTED, "GlxGridView::itemSelected;index=%d", index.row() );
    if ( mWidget->selectionMode() == HgWidget::MultiSelection )
        { 
        return ;
        }    
   if(XQServiceUtil::isService()){
        emit gridItemSelected(index);
        return;
    }    
    OstTraceEventStart0( EVENT_DUP1_GLXGRIDVIEW_ITEMSELECTED_START, "Fullscreen Launch Time" );    
    if ( mModel ) 
        {
        mModel->setData( index, index.row(), GlxFocusIndexRole );
        }
    setItemVisible(Hb::AllItems, FALSE);
    emit actionTriggered( EGlxCmdFullScreenOpen ); 
    OstTraceEventStop( EVENT_DUP1_GLXGRIDVIEW_ITEMSELECTED_STOP, "Fullscreen Launch Time", EVENT_DUP1_GLXGRIDVIEW_ITEMSELECTED_START );
}

void GlxGridView::scrollingStarted()
{
    if ((mWindow->orientation() == Qt::Horizontal))
        {
        setItemVisible(Hb::AllItems, FALSE) ;
        }    
    mScrolling = TRUE;
}

void GlxGridView::scrollingEnded()
{
    mScrolling = FALSE;
    QList<QModelIndex> visibleIndex = mWidget->getVisibleItemIndices();
    if (visibleIndex.count() <= 0)
        {
        return;
        }
    QModelIndex index = visibleIndex.at(0);
    if (  index.row() < 0 || index.row() >= mModel->rowCount() )
        {
        return;
        }    
    if(mModel)
        {
        mModel->setData( index, index.row(), GlxVisualWindowIndex);
        }
}

GlxGridView::~GlxGridView()
{
    OstTraceFunctionEntry0( DUP1_GLXGRIDVIEW_GLXGRIDVIEW_ENTRY );
    removeViewConnection();
    if(mSelectionModel)
        {
        disconnect(mSelectionModel, SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)), this, SLOT(visibleIndexChanged(const QModelIndex &, const QModelIndex &)));
        delete mSelectionModel;
        }
    delete mWidget;
    delete mModelWrapper;
    if(mUiOnButton) {
        disconnect(mUiOnButton, SIGNAL(clicked(bool)), this, SLOT(uiButtonClicked(bool)));
        delete mUiOnButton;
    }
    delete mIconItem;
    OstTraceFunctionExit0( DUP1_GLXGRIDVIEW_GLXGRIDVIEW_EXIT );
}

void GlxGridView::indicateLongPress(const QModelIndex& index, QPointF coords)
{
    OstTrace0( TRACE_NORMAL, GLXGRIDVIEW_INDICATELONGPRESS, "GlxGridView::indicateLongPress" );
     
     if ( mWidget->selectionMode() == HgWidget::MultiSelection )
         { 
         return;
         }     
     if ( mModel ) 
         {
         mModel->setData( index, index.row(), GlxFocusIndexRole );
         }     
     emit itemSpecificMenuTriggered(viewId(),coords);
}

void GlxGridView::uiButtonClicked(bool /*checked*/)
{
    if (isItemVisible(Hb::TitleBarItem))  // W16  All item is Not Working , So Temp Fix
        {
        setItemVisible(Hb::AllItems, FALSE) ;
        }
    else
        {
        setItemVisible(Hb::AllItems, TRUE) ;
        }
}

