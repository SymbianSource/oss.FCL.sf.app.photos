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

//--------------------------------------------------------------------------------------------------------------------------------------------

#include <QModelIndex>
#include <QtDebug>
#include <qdatetime.h>

//--------------------------------------------------------------------------------------------------------------------------------------------

#include <hbdataform.h>
#include <hblabel.h>
#include <hbinstance.h>
#include <hbiconitem.h>
#include <hblineedit.h>
#include <hbdataformmodel.h>
#include <hbdataformmodelitem.h>
#include <hbdataformviewitem.h>

//--------------------------------------------------------------------------------------------------------------------------------------------

#include <glxmediamodel.h>
#include "glxdetailsview.h"
#include "glxmodelparm.h"
#include "glxdetailscustomwidgets.h"
#include "glxviewids.h"
#include <glxcommandhandlers.hrh>



//--------------------------------------------------------------------------------------------------------------------------------------------
//GlxDetailsView
//--------------------------------------------------------------------------------------------------------------------------------------------
GlxDetailsView::GlxDetailsView(HbMainWindow *window) :  GlxView ( GLX_DETAILSVIEW_ID),
    mDetailsPixmap(NULL), 
    mBlackBackgroundItem(NULL), 
    mModel(NULL), 
    mWindow(window), 
    mDataForm(NULL), 
    mDetailModel(NULL),
    mCustomPrototype(NULL),
    mSelIndex (0)
{
    qDebug("@@@GlxDetailsView::GlxDetailsView constructor Enter");
    setContentFullScreen( true );//for smooth transtion between grid to full screen and vice versa
}

//--------------------------------------------------------------------------------------------------------------------------------------------
//~GlxDetailsView
//--------------------------------------------------------------------------------------------------------------------------------------------
GlxDetailsView::~GlxDetailsView()
{
    qDebug("@@@GlxDetailsView::~GlxDetailsView Enter");

    if(mDetailsPixmap) {
        delete mDetailsPixmap;
        mDetailsPixmap = NULL;
    }

    if(mDataForm && mDataForm->model()) {
        delete mDataForm->model();
        mDataForm->setModel(0);
    }

    if(mDataForm) {
        delete mDataForm;
        mDataForm = NULL;
    }


    if(mBlackBackgroundItem) {
        delete mBlackBackgroundItem;
        mBlackBackgroundItem = NULL;
    }
    
    clearCurrentModel();
    disconnect(mWindow, SIGNAL(orientationChanged(Qt::Orientation)), this, SLOT(updateLayout(Qt::Orientation)));
}

//--------------------------------------------------------------------------------------------------------------------------------------------
//activate
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsView::activate()
{
    qDebug("@@@GlxDetailsView::activate Enter");
    setFormData();
    connect(mWindow, SIGNAL(orientationChanged(Qt::Orientation)), this, SLOT(updateLayout(Qt::Orientation)));
}

//--------------------------------------------------------------------------------------------------------------------------------------------
//initializeView
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsView::initializeView(QAbstractItemModel *model)
{   
    qDebug("@@@GlxDetailsView::initializeView Enter");
    
    mBlackBackgroundItem = new HbIconItem(this);
    mBlackBackgroundItem->setBrush(QBrush(Qt::black));
    mBlackBackgroundItem->hide();

    //To show the thumbnail 
    mDetailsPixmap = new QGraphicsPixmapItem(this);

    //Create the form and the model for the data form
    mDataForm = new HbDataForm(this);
    mDetailModel = new HbDataFormModel();

    //custom prototype
    mCustomPrototype = new GlxDetailsCustomWidgets(mDataForm);
    mDataForm->setItemPrototype(mCustomPrototype);

    //Set the Model
    mModel = model;
    initializeNewModel();
    
    //Add the Widgets according to the mime type
    addWidgets();
    
    //Set the Layout Correspondingly.
    updateLayout(mWindow->orientation());
    
    //Shows the Image 
    showImage();
        
}

//--------------------------------------------------------------------------------------------------------------------------------------------
//resetView
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsView::resetView()
{
 //Do Nothing here
}

//--------------------------------------------------------------------------------------------------------------------------------------------
//deActivate
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsView::deActivate()
{ 
    qDebug("@@@GlxDetailsView::deActivate Enter");

    //mWindow->setItemVisible(Hb::AllItems, FALSE) ;

    if(mDetailsPixmap) {
        delete mDetailsPixmap;
        mDetailsPixmap = NULL;
    }

    if(mDataForm && mDataForm->model()) {
        delete mDataForm->model();
        mDataForm->setModel(0);
    }

    if(mDataForm) {
        delete mDataForm;
        mDataForm = NULL;
    }

    if(mBlackBackgroundItem) {
        delete mBlackBackgroundItem;
        mBlackBackgroundItem = NULL;
    }
    
    clearCurrentModel();
    disconnect(mWindow, SIGNAL(orientationChanged(Qt::Orientation)), this, SLOT(updateLayout(Qt::Orientation)));
}

//--------------------------------------------------------------------------------------------------------------------------------------------
//addWidgets
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsView::addWidgets()
    {
    qDebug("@@@GlxDetailsView::addWidgets create the form");

    //----------------------------START OF CREATION OF WIDGETS---------------------------------//
    // To add new widgets in the details view, add it here.

    //---------------------------IMAGE NAME LABEL --------------------------------------------//
    qDebug("@@@GlxDetailsView::addWidgets create Image name Label"); 
    HbDataFormModelItem *imageLabelitem = mDetailModel->appendDataFormItem((HbDataFormModelItem::DataItemType)(ImageNameItem), QString("Name"), mDetailModel->invisibleRootItem());
    imageLabelitem->setData(HbDataFormModelItem::KeyRole,QString(""));

    //---------------------------DATE LABEL --------------------------------------------//
    qDebug("@@@GlxDetailsView::addWidgets create Image name Label"); 
    HbDataFormModelItem *dateLabelItem = mDetailModel->appendDataFormItem((HbDataFormModelItem::DataItemType)(DateLabelItem), QString("Date"), mDetailModel->invisibleRootItem());
    dateLabelItem->setData(HbDataFormModelItem::KeyRole,QString(""));

    //---------------------------LOCATION LABEL--------------------------------------------//
    qDebug("@@@GlxDetailsView::addWidgets create Location Label");
    HbDataFormModelItem *locationLabelItem = mDetailModel->appendDataFormItem((HbDataFormModelItem::DataItemType)(LocationTagItem), QString("Location Tag"), mDetailModel->invisibleRootItem());
    locationLabelItem->setData(HbDataFormModelItem::KeyRole, QString(""));

    //---------------------------DURATION LABEL--------------------------------------------//
    /*
     Need to Add a check here for the type of the item, if it is video or image
     qDebug("@@@GlxDetailsView::addWidgets create Duration Label");
     HbDataFormModelItem *DurationLabelItem = mDetailModel->appendDataFormItem((HbDataFormModelItem::DataItemType)(DurationItem), QString("Duration"), mDetailModel->invisibleRootItem());
     DurationLabelItem->setData(HbDataFormModelItem::KeyRole, QString("")); 
    */

    //----------------------------COMMENTS TEXT ITEM---------------------------------------------//
    qDebug("@@@GlxDetailsView::addWidgets create Comment text edit");
    HbDataFormModelItem *commentsLabelItem = mDetailModel->appendDataFormItem((HbDataFormModelItem::DataItemType)(CommentsItem), QString("Comments"), mDetailModel->invisibleRootItem());
    commentsLabelItem->setData(HbDataFormModelItem::KeyRole, QString(""));

    //----------------------------END OF CREATION OF WIDGETS-------------------------------------//

    //Set the model to the Data Form
     mDataForm->setModel(mDetailModel);
    }


//--------------------------------------------------------------------------------------------------------------------------------------------
//setModel
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsView::setModel(QAbstractItemModel *model)
{
    qDebug("@@@GlxDetailsView::setModel");
    if ( mModel == model ) {
        return ;
    }
    clearCurrentModel();
    mModel = model;
    initializeNewModel();
}

QGraphicsItem * GlxDetailsView::getAnimationItem(GlxEffect transtionEffect)
{
    if ( transtionEffect == FULLSCREEN_TO_DETAIL
            || transtionEffect == DETAIL_TO_FULLSCREEN ) {
        return this;
    }
    
    return NULL;    
}

//--------------------------------------------------------------------------------------------------------------------------------------------
//updateLayout
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsView::updateLayout(Qt::Orientation orient)
    {
    qDebug("GlxDetailsView::updateLayout() %d", orient );

    QRect screen_rect = mWindow->geometry(); 
    setGeometry(screen_rect);

    if(orient == Qt::Horizontal)
        {
        qDebug("GlxDetailsView::updateLayout HORIZONTAL");

        mBlackBackgroundItem->setSize(QSize(280,280));
        mBlackBackgroundItem->setPos(11,71);

        mDetailsPixmap->setPos(15,75);
        mDetailsPixmap->setZValue(mBlackBackgroundItem->zValue() + 1);

       // mDataForm->setPos(301,5);
        mDataForm->setGeometry(301,60,335,300);

        }
    else
        {
        qDebug("GlxDetailsView::updateLayout VERTICAL");

        mBlackBackgroundItem->setSize(QSize(310,260));
        mBlackBackgroundItem->setPos(25,75);

        mDetailsPixmap->setPos(25,75);
        mDetailsPixmap->setZValue(mBlackBackgroundItem->zValue() + 1);

//        mDataForm->setPos(5,291);
        mDataForm->setGeometry(5,351,335,300);
        mDataForm->setMaximumWidth(340);
        }
    }

void GlxDetailsView::rowsRemoved(const QModelIndex &parent, int start, int end)
{
    Q_UNUSED(parent);
    Q_UNUSED(start);
    Q_UNUSED(end);
    qDebug("GlxDetailsView::rowsRemoved");
    
    if ( mModel->rowCount() <= 0 ) {
        return emit actionTriggered( EGlxCmdEmptyData );
    }
    
    if ( start <= mSelIndex && end >= mSelIndex ) {
        return emit actionTriggered( EGlxCmdBack );
    }
}

//--------------------------------------------------------------------------------------------------------------------------------------------
//showImage
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsView::showImage()
    {
    qDebug("GlxDetailsView::showImage() Enter" );
    
    QVariant variant = mModel->data( mModel->index(0,0), GlxFocusIndexRole );    
    if ( variant.isValid() &&  variant.canConvert<int> () ) {
        mSelIndex = variant.value<int>();  
    }

    variant = mModel->data( mModel->index( mSelIndex ,0), GlxFsImageRole);
    if ( variant.isValid() &&  variant.canConvert<HbIcon> () )
        {
        QIcon itemIcon = variant.value<HbIcon>().qicon();
        if(mWindow->orientation() == Qt::Horizontal)
            {
            qDebug("GlxDetailsView::showImage HORIZONTAL");
            QPixmap itemPixmap = itemIcon.pixmap(128,128);
            QSize sz( 270, 270);
            itemPixmap = itemPixmap.scaled(sz, Qt::IgnoreAspectRatio );
            if(itemPixmap.isNull())
                {
                qDebug("@@@GlxDetailsView::showImage Null Pixmap");
                }
            else
                {
                qDebug("@@@GlxDetailsView::showImage NOT --Null Pixmap");
                mDetailsPixmap->setPixmap(itemPixmap);
                }
            }
        else
            {
            qDebug("GlxDetailsView::showImage VERTICAL");

            QPixmap itemPixmap = itemIcon.pixmap(128,128);
            QSize sz( 310, 260);
            itemPixmap = itemPixmap.scaled(sz, Qt::IgnoreAspectRatio );

            if(itemPixmap.isNull())
                {
                qDebug("@@@GlxDetailsView::showImage Null Pixmap");
                }
            else
                {
                qDebug("@@@GlxDetailsView::showImage NOT --Null Pixmap");
                mDetailsPixmap->setPixmap(itemPixmap);
                }
            }
        }
    }

//--------------------------------------------------------------------------------------------------------------------------------------------
//FillData
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsView::setFormData()
    {
    qDebug("GlxDetailsView::FillData Enter");
    //Call to set the Image Name
    setImageName();
    //Call to set the date in the from
    setDate();
    }

//--------------------------------------------------------------------------------------------------------------------------------------------
//setImageName
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsView::setImageName()
    {
    qDebug("GlxDetailsView::setImageName Enter");

    QString imagePath = (mModel->data(mModel->index(mModel->data(mModel->index(0,0),GlxFocusIndexRole).value<int>(),0),GlxUriRole)).value<QString>();
    QString imageName = imagePath.section('\\',-1);

    //Fetch the text edit for displaying the Name from the Form Model
    GlxDetailsCustomWidgets * imageLabel = (GlxDetailsCustomWidgets *)mDataForm->itemByIndex(mDetailModel->index(0,0));

    if(imageLabel)
        {
        qDebug("GlxDetailsView::setImageName  imageLabel!=NULL");
        HbLineEdit* label = static_cast<HbLineEdit*>(imageLabel->dataItemContentWidget());

        if(label) 
            {
            qDebug("GlxDetailsView::setImageName,setText");
            label->setText(imageName);
            }
        }
}

//--------------------------------------------------------------------------------------------------------------------------------------------
//setDate
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsView::setDate()
    {
    qDebug("GlxDetailsView::setDate Enter");

    QString datestring;
    QString str("dd.MM.yyyy");
    QDate date = (mModel->data(mModel->index(mModel->data(mModel->index(0,0),GlxFocusIndexRole).value<int>(),0),GlxDateRole)).value<QDate>();

    if(date.isNull() == FALSE )
        {
        qDebug("GlxDetailsView::setDate date is not Null");
        datestring = date.toString(str);
        }

    //Fetch the Label from the Form Model
    GlxDetailsCustomWidgets * dateLabel = (GlxDetailsCustomWidgets *)mDataForm->itemByIndex(mDetailModel->index(1,0));

    if(dateLabel) 
        {
        qDebug("GlxDetailsView::setDate  dateLabel!=NULL");
        HbLabel* label = static_cast<HbLabel*>(dateLabel->dataItemContentWidget());

        if(label) 
            {
            qDebug("GlxDetailsView::setDate,setText");
            label->setPlainText(datestring);
            }
        }    
    }

void GlxDetailsView::initializeNewModel()
{
    if ( mModel ) {
        connect(mModel, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(rowsRemoved(QModelIndex,int,int)));
    }
}

void GlxDetailsView::clearCurrentModel()
{
    if ( mModel ) {
        disconnect(mModel, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(rowsRemoved(QModelIndex,int,int)));
        mModel = NULL ;
    }    
}
