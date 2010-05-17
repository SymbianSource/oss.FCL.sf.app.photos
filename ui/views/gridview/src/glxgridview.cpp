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
#include <hbcheckbox.h>
#include <hblabel.h>
#include <QString>
#include <hbframeitem.h>

//User Includes
#include "glxviewids.h"
#include "glxgridview.h"
#include "glxmodelparm.h"
#include "glxcommandhandlers.hrh"
#include "glxicondefs.h"
#include "glxlocalisationstrings.h"

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
      mCameraButton(NULL),
      mScrolling(FALSE),
      mIconItem(NULL),
      mMarkCheckBox(NULL),
      mCountItem(NULL),
      mMainLabel(NULL),
      mCountLabel(NULL),
      mZeroItemLabel(NULL),
      mAlbumName(NULL)
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
        mUiOnButton = new HbPushButton(this);
        connect(mUiOnButton, SIGNAL(clicked(bool)), this, SLOT(uiButtonClicked(bool)));
        mUiOnButton->setGeometry(QRectF(590,0,40,40));
        mUiOnButton->setZValue(1);
        mUiOnButton->setIcon(HbIcon(GLXICON_WALL_UI_ON));
        mUiOnButton->hide();
    }
    loadGridView();
	connect(mWindow, SIGNAL(orientationChanged(Qt::Orientation)), this, SLOT(orientationchanged(Qt::Orientation)),Qt::UniqueConnection);
    if(mCountItem == NULL) {
        mCountItem = new HbLabel(this);        
        HbFrameItem *frame = new HbFrameItem(this); //graphics for mCountItem
        frame->frameDrawer().setFrameType(HbFrameDrawer::NinePieces);
        frame->frameDrawer().setFrameGraphicsName("qtg_fr_multimedia_trans");
        frame->graphicsItem()->setOpacity(1);
        mCountItem->setBackgroundItem(frame->graphicsItem(),-1);
    }
    OstTraceFunctionExit0( GLXGRIDVIEW_ACTIVATE_EXIT );
}

void GlxGridView::deActivate()
{
    OstTraceFunctionEntry0( GLXGRIDVIEW_DEACTIVATE_ENTRY );
    mScrolling = FALSE;
    if (mUiOnButton)
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
    if (mCountItem) 
        {
        mCountItem->hide();
        }
    if (mAlbumName) 
        {
        mAlbumName->hide();
        }
    if(mZeroItemLabel) 
        {
        mZeroItemLabel->hide();
        }
    if(mCameraButton) 
        {
        mCameraButton->hide();
        }
    disconnect(mWindow, SIGNAL(orientationChanged(Qt::Orientation)), this, SLOT(orientationchanged(Qt::Orientation)));
    OstTraceFunctionExit0( GLXGRIDVIEW_DEACTIVATE_EXIT );
}

void GlxGridView::initializeView(QAbstractItemModel *model)
{
    activate();
    setModel(model);
}

void GlxGridView::clearCurrentModel()
{
    if ( mModel ) {
        disconnect(mModel, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(showItemCount()));
        disconnect(mModel, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(showItemCount()));
        disconnect(mModel, SIGNAL(destroyed()), this, SLOT( clearCurrentModel()));
        disconnect(mModel, SIGNAL(albumTitleAvailable(QString)), this, SLOT(showAlbumTitle(QString)));
        disconnect(mModel, SIGNAL(populated()), this, SLOT( populated()));
        mModel = NULL ;
    }
}

void GlxGridView::initializeNewModel()
{
    if ( mModel ) {
        connect(mModel, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(showItemCount()));
        connect(mModel, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(showItemCount()));
        connect(mModel, SIGNAL(destroyed()), this, SLOT( clearCurrentModel()));
        connect(mModel, SIGNAL(albumTitleAvailable(QString)), this, SLOT(showAlbumTitle(QString)));
        connect(mModel, SIGNAL(populated()), this, SLOT( populated()));
    }
}

void GlxGridView::setModel(QAbstractItemModel *model)
{
    OstTraceFunctionEntry0( GLXGRIDVIEW_SETMODEL_ENTRY );
    if(model)
        {
        clearCurrentModel();
        mModel = model;
        initializeNewModel();
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
        showItemCount();
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
    if (mMainLabel == NULL) {
        mMainLabel = new HbLabel("Select Photos", this);        
        HbFrameItem *frame1 = new HbFrameItem(this);    //graphics for mMainLabel
        frame1->frameDrawer().setFrameType(HbFrameDrawer::NinePieces);
        frame1->frameDrawer().setFrameGraphicsName("qtg_fr_multimedia_trans");
        frame1->graphicsItem()->setOpacity(1);
        mMainLabel->setBackgroundItem(frame1->graphicsItem(),-1);
    }
    if (mMarkCheckBox == NULL) {
        mMarkCheckBox = new HbCheckBox(GLX_OPTION_MARK_ALL, this);        
        HbFrameItem *frame2 = new HbFrameItem(this);    //graphics for mMarkCheckBox
        frame2->frameDrawer().setFrameType(HbFrameDrawer::NinePieces);
        frame2->frameDrawer().setFrameGraphicsName("qtg_fr_multimedia_trans");
        frame2->graphicsItem()->setOpacity(1);
        mMarkCheckBox->setBackgroundItem(frame2->graphicsItem(),-1);
    }
    if (mCountLabel == NULL) {
        mCountLabel = new HbLabel(this);        
        HbFrameItem *frame3 = new HbFrameItem(this);    //graphics for mCountLabel
        frame3->frameDrawer().setFrameType(HbFrameDrawer::NinePieces);
        frame3->frameDrawer().setFrameGraphicsName("qtg_fr_multimedia_trans");
        frame3->graphicsItem()->setOpacity(1);
        mCountLabel->setBackgroundItem(frame3->graphicsItem(),-1);
    }

    hideorshowitems(mWindow->orientation());

    connect( mWidget->selectionModel() , SIGNAL( selectionChanged(const QItemSelection &, const QItemSelection& ) ), this, SLOT( showMarkedItemCount() ) );
    connect(mMarkCheckBox, SIGNAL( stateChanged(int) ), this, SLOT( stateChanged(int)));

}

void GlxGridView::disableMarking() 
{
    OstTrace0( TRACE_NORMAL, GLXGRIDVIEW_DISABLEMARKING, "GlxGridView::disableMarking" );
    mWidget->setSelectionMode(HgWidget::NoSelection);
    disconnect( mWidget->selectionModel() , SIGNAL( selectionChanged(const QItemSelection &, const QItemSelection& ) ), this, SLOT( showMarkedItemCount() ) );
    disconnect(mMarkCheckBox, SIGNAL( stateChanged(int) ), this, SLOT( stateChanged(int)));
    if (mMainLabel) {
        mMainLabel->hide();
    }
    if (mMarkCheckBox) {
        mMarkCheckBox->setCheckState(Qt::Unchecked);
        mMarkCheckBox->hide();
    }
    if (mCountLabel) {
        mCountLabel->hide();
    }

    hideorshowitems(mWindow->orientation());
}

void GlxGridView::stateChanged(int state)
{
    if(state)
        handleUserAction(EGlxCmdMarkAll);
    else
        handleUserAction(EGlxCmdUnMarkAll);
}

void GlxGridView::showMarkedItemCount()
{
    int count = mModel->rowCount();
    QModelIndexList indexList = mWidget->selectionModel()->selectedIndexes();
    int markItemCount = indexList.count();

    QString text= QString("%1 / %2").arg( markItemCount ).arg( count );
    mCountLabel->setPlainText( text );
}

void GlxGridView::showItemCount()
{
    int count = 0;
    if(mModel) {
        count = mModel->rowCount();
        QSize deviceSize = HbDeviceProfile::current().logicalSize();
        QSize screenSize = ( mWindow->orientation() == Qt::Vertical ) ? QSize( deviceSize.width(), deviceSize.height() )
                                                                       : QSize( deviceSize.height(), deviceSize.width() )  ;
        if(count) {
            if(mZeroItemLabel) {
                mZeroItemLabel->hide();
            }
            if(mCameraButton) {
                mCameraButton->hide();
            }
            if(isItemVisible(Hb::TitleBarItem)) {
                QString text;
                if (getSubState() == ALL_ITEM_S) {
					if (mAlbumName) {
                    	mAlbumName->hide();
					}
                    mCountItem->setGeometry(QRectF(0,0,screenSize.width(),deviceSize.height()/24));
                    text = QString("%1 Items").arg( count );
                    mCountItem->setPlainText( text );
                    mCountItem->setAlignment(Qt::AlignLeft);
                    mCountItem->show();
                }
                else if (getSubState() == ALBUM_ITEM_S) {
                    mCountItem->hide();
                    QVariant variant = mModel->data(mModel->index(0,0),GlxViewTitle);
                    if (variant.toString() != NULL) {
                        showAlbumTitle(variant.toString());
                    }
                }
            }
            else {
                if (mCountItem) {
                    mCountItem->hide();
                }
                if (mAlbumName) {
                    mAlbumName->hide();
                }
            }

        }
        else {
            bool populated = FALSE;
            QVariant variant = mModel->data(mModel->index(0,0),GlxPopulated);
            if (variant.isValid() &&  variant.canConvert<bool>() )
                {
                populated = variant.value<bool>();
                }
            if(populated) {
                if (mCountItem) {
                    mCountItem->hide();
                }
                if (mAlbumName) {
                    mAlbumName->hide();
                }
                
                showNoImageString();            
                
                if (getSubState() == ALBUM_ITEM_S) {                
                    QVariant variant = mModel->data(mModel->index(0,0),GlxViewTitle);
                    if (variant.toString() != NULL) {
                        showAlbumTitle(variant.toString());
                    }
                }
            }
        }
    }
}

void GlxGridView::showAlbumTitle(QString aTitle)
{
    int count = mModel->rowCount();
    QSize deviceSize = HbDeviceProfile::current().logicalSize();
    QSize screenSize = ( mWindow->orientation() == Qt::Vertical ) ? QSize( deviceSize.width(), deviceSize.height() )
                                                                   : QSize( deviceSize.height(), deviceSize.width() )  ;
    if(mAlbumName == NULL) {
        mAlbumName = new HbLabel(this);            
        HbFrameItem *frame = new HbFrameItem(this); //graphics for mAlbumName
        frame->frameDrawer().setFrameType(HbFrameDrawer::NinePieces);
        frame->frameDrawer().setFrameGraphicsName("qtg_fr_multimedia_trans");
        frame->graphicsItem()->setOpacity(1);
        mAlbumName->setBackgroundItem(frame->graphicsItem(),-1);
    }
    
    if(count && isItemVisible(Hb::TitleBarItem)) {        
        mAlbumName->setGeometry(QRectF(0,0,screenSize.width()/2,deviceSize.height()/24));
        QString text = QString(aTitle);
        mAlbumName->setPlainText( text );
        mAlbumName->show();
        mCountItem->setGeometry(QRectF(screenSize.width()/2,0,screenSize.width()/2,deviceSize.height()/24));
        text = QString("(%1)").arg(count);
        mCountItem->setPlainText( text );
        mCountItem->setAlignment(Qt::AlignRight);
        mCountItem->show();
    }    
    else if((!count) && isItemVisible(Hb::TitleBarItem)) {
        mAlbumName->setGeometry(QRectF(0,0,screenSize.width(),deviceSize.height()/24));
        QString text = QString(aTitle);
        mAlbumName->setPlainText( text );
        mAlbumName->show();
    }
}

void GlxGridView::showNoImageString()
{
    qreal chromeHeight = 0.0;
    QSize deviceSize = HbDeviceProfile::current().logicalSize();
    QSize screenSize = ( mWindow->orientation() == Qt::Vertical ) ? QSize( deviceSize.width(), deviceSize.height() )
                                                                   : QSize( deviceSize.height(), deviceSize.width() )  ;
    qreal midHeight = screenSize.height()/2;
    if (isItemVisible(Hb::TitleBarItem)) {
        style()->parameter("hb-param-widget-chrome-height", chromeHeight);
        midHeight -= chromeHeight;
    }
    if (mZeroItemLabel == NULL) {                
        mZeroItemLabel = new HbLabel("(No Images)\n To capture images Open", this);
    }
    mZeroItemLabel->setGeometry(QRectF(0, midHeight - deviceSize.height()/16, screenSize.width(), 3*deviceSize.height()/32));
    mZeroItemLabel->setAlignment(Qt::AlignHCenter);
    mZeroItemLabel->show();
    if (mCameraButton == NULL) {
        mCameraButton = new HbPushButton(this);
        mCameraButton->setIcon(HbIcon(GLXICON_CAMERA));
        connect(mCameraButton, SIGNAL(clicked(bool)), this, SLOT(cameraButtonClicked(bool)));
    }
    mCameraButton->setGeometry(QRectF(screenSize.width()/2 - 3*deviceSize.height()/64, midHeight + deviceSize.height()/32, deviceSize.height()/32, deviceSize.height()/32));
    mCameraButton->show();
}

void GlxGridView::populated()
{
    showItemCount();
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
        setWidget( mWidget );  
        addViewConnection();
        hideorshowitems(orient);
    }
    OstTraceFunctionExit0( GLXGRIDVIEW_LOADGRIDVIEW_EXIT );
}

void GlxGridView::orientationchanged(Qt::Orientation orient)
{
    hideorshowitems(orient);
}
void GlxGridView::hideorshowitems(Qt::Orientation orient)
{
    if (mWidget->selectionMode() == HgWidget::NoSelection) {
        if(orient == Qt::Horizontal)
            {
			setItemVisible(Hb::AllItems, FALSE) ;
        	setViewFlags(viewFlags() | HbView::ViewTitleBarHidden | HbView::ViewStatusBarHidden);
        	showItemCount();
            if (mUiOnButton)
                {
                mUiOnButton->show();
                }
            }
        else
            {
            showHbItems();
            }
    }

    if (mWidget->selectionMode() == HgWidget::MultiSelection) {
        setItemVisible(Hb::TitleBarItem, FALSE) ;
        if (mUiOnButton) {
            mUiOnButton->hide();
        }
        if (mCountItem) {
            mCountItem->hide();
        }
        if (mAlbumName) {
            mAlbumName->hide();
        }
        QSize deviceSize = HbDeviceProfile::current().logicalSize();
        QSize screenSize = ( mWindow->orientation() == Qt::Vertical ) ? QSize( deviceSize.width(), deviceSize.height() )
                                                                       : QSize( deviceSize.height(), deviceSize.width() )  ;
        mMainLabel->setGeometry(QRectF(0,0,screenSize.width(),deviceSize.height()/24));
        mMarkCheckBox->setGeometry(QRectF(0,deviceSize.height()/24,screenSize.width()/2,deviceSize.height()/72));
        mCountLabel->setGeometry(QRectF(screenSize.width()/2,deviceSize.height()/24,screenSize.width()/2,deviceSize.height()/12 - 3));
        mCountLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        mMainLabel->show();
        mMarkCheckBox->show();
        mCountLabel->show();
        showMarkedItemCount();
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
    if ((mWindow->orientation() == Qt::Horizontal) && mWidget->selectionMode() == HgWidget::NoSelection)
        {
        setItemVisible(Hb::AllItems, FALSE) ;
        setViewFlags(viewFlags() | HbView::ViewTitleBarHidden | HbView::ViewStatusBarHidden);
        if (mUiOnButton)
            {
            mUiOnButton->hide();
            }
		if (mCountItem) {
			mCountItem->hide();
		}
		if (mAlbumName) {
            mAlbumName->hide();
		}
    }

    mScrolling = TRUE;
}

void GlxGridView::scrollingEnded()
{
    mScrolling = FALSE;
    if (mUiOnButton && (mWindow->orientation() == Qt::Horizontal))
        {
        mUiOnButton->show();
        }
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
        mModel->setData( index, index.row(), GlxFocusIndexRole );
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
    if(mCameraButton) {
        disconnect(mCameraButton, SIGNAL(clicked()), this, SLOT(cameraButtonClicked()));
        delete mCameraButton;
    }
    delete mIconItem;
    delete mCountItem;
    delete mAlbumName;
    delete mMainLabel;
    delete mMarkCheckBox;
    delete mCountLabel;
    delete mZeroItemLabel;
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
    showHbItems();
}

void GlxGridView::showHbItems()
{
        setItemVisible(Hb::AllItems, TRUE) ;
    setViewFlags(viewFlags() &~ HbView::ViewTitleBarHidden &~ HbView::ViewStatusBarHidden);
        showItemCount();
		toolBar()->resetTransform(); // Temp, this is for HbToolbar issue to get fixed
        toolBar()->show();
    if (mUiOnButton)
        {
        mUiOnButton->hide();
        }
}

void GlxGridView::cameraButtonClicked(bool /*checked*/)
{
    emit actionTriggered(EGlxCmdCameraOpen);
}

int GlxGridView::getSubState()
{
    int substate = NO_GRID_S;
    QVariant variant = mModel->data( mModel->index(0,0), GlxSubStateRole );
    if ( variant.isValid() &&  variant.canConvert<int> ()  ) {
        substate = variant.value<int>();
    }
    return substate;
}

