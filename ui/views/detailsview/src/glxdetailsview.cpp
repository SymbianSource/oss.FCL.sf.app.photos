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
#include <qdatetime.h>
#include <QDebug>
#include <HbAnchorLayout.h>
//--------------------------------------------------------------------------------------------------------------------------------------------

#include <hblabel.h>
#include <hbdataform.h>
#include <hbinstance.h>
#include <hbiconitem.h>
#include <hbframeitem.h>
#include <hbpushbutton.h>
#include <hbdataformmodel.h>
#include <hbdocumentloader.h>
#include <hbdataformmodelitem.h>
#include <hbdataformviewitem.h>

//--------------------------------------------------------------------------------------------------------------------------------------------
#include "glxviewids.h"
#include "glxicondefs.h" //Contains the icon names/Ids
#include "glxmodelparm.h"
#include "glxmediamodel.h"
#include "glxdetailsview.h"
#include "glxfavmediamodel.h"
#include "glxdocloaderdefs.h"
#include <glxcommandhandlers.hrh>
#include "glxdetailsdescriptionedit.h"
#include "glxdetailsnamelabel.h"


#include "glxviewdocloader.h"
#include <glxcollectionpluginalbums.hrh>

#include "glxlog.h"
#include "glxloggerenabler.h"


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "glxdetailsviewTraces.h"
#endif

//SIZE OF THE IMAGE , LAYOUTS TEAM NEED TO GIVER THE SIZE IN UNITS
#define GLX_IMAGE_SIZE 215 
const int KBytesInKB = 1024;
const int KBytesInMB = 1024 * 1024;
const int KBytesInGB = 1024 * 1024 * 1024;

//--------------------------------------------------------------------------------------------------------------------------------------------
//GlxDetailsView
//--------------------------------------------------------------------------------------------------------------------------------------------
GlxDetailsView::GlxDetailsView(HbMainWindow *window) :
GlxView(GLX_DETAILSVIEW_ID), mDetailsIcon(NULL), mFavIcon(NULL), mModel(
        NULL), mFavModel(NULL), mWindow(window),
        mSelIndex(0),mDocLoader(NULL),mImageName(NULL),mDescriptions(NULL),mDateLabel(NULL),mSizeLabel(NULL),mTimeLabel(NULL)
   {
    GLX_LOG_INFO("GlxDetailsView::GlxDetailsView");
    OstTraceFunctionEntry0( GLXDETAILSVIEW_GLXDETAILSVIEW_ENTRY );
    setContentFullScreen(true);//for smooth transtion between grid to full screen and vice versa
    OstTraceFunctionExit0( GLXDETAILSVIEW_GLXDETAILSVIEW_EXIT );
   }

//--------------------------------------------------------------------------------------------------------------------------------------------
//~GlxDetailsView
//--------------------------------------------------------------------------------------------------------------------------------------------
GlxDetailsView::~GlxDetailsView()
    {
    OstTrace0( TRACE_IMPORTANT, GLXDETAILSVIEW_GLXDETAILSVIEW, "GlxDetailsView::~GlxDetailsView" );
    
    delete mDocLoader;
    mDocLoader = NULL;
    
    cleanUp();
    }

//--------------------------------------------------------------------------------------------------------------------------------------------
//activate
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsView::activate()
    {
    OstTraceFunctionEntry0( GLXDETAILSVIEW_ACTIVATE_ENTRY );
    //create and set the Favourite Model
    setFavModel();
    
    //fill the data
    FillDetails();
    
    //Initialze the Model
    initializeNewModel();
    
    //make the connections
    setConnections();
        
    //Set context mode to fetch descriptions
    mModel->setData(QModelIndex(), (int) GlxContextComment, GlxContextRole);    
    
    OstTraceFunctionExit0( GLXDETAILSVIEW_ACTIVATE_EXIT );
    }

//--------------------------------------------------------------------------------------------------------------------------------------------
//initializeView
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsView::initializeView(QAbstractItemModel *model)
    {
    OstTraceFunctionEntry0( GLXDETAILSVIEW_INITIALIZEVIEW_ENTRY );
    bool loaded = false;
    
    if(!mDocLoader)
        {
         mDocLoader = new GlxDetailsViewDocLoader();
        }
    
    //Load the docml
    mDocLoader->load(GLX_DETAILSVIEW_DOCMLPATH, &loaded);     
    
    HbView *mView = static_cast<HbView*> (mDocLoader->findWidget(
            GLX_DETAILSVIEW_VIEW));
    
    HbWidget *mwidget = static_cast<HbWidget*> (mDocLoader->findWidget(
            "MainWidget"));

    mDetailsIcon = static_cast<HbLabel*> (mDocLoader->findWidget(
            GLX_DETAILSVIEW_IMAGE));
    mFavIcon = static_cast<HbPushButton*> (mDocLoader->findWidget(
            GLX_DETAILSVIEW_FAVICON));

    mDescriptions = static_cast<GlxDetailsDescriptionEdit*> (mDocLoader->findWidget(
            GLX_DETAILSVIEW_DESCRPTIONTEXT));
    
    mImageName = static_cast<GlxDetailsNameLabel*> (mDocLoader->findWidget(
            GLX_DETAILSVIEW_IMGNAME));
    
    mDateLabel = static_cast<HbLabel*> (mDocLoader->findWidget(
            GLX_DETAILSVIEW_DATETEXT));
    
    mTimeLabel = static_cast<HbLabel*> (mDocLoader->findWidget(
            GLX_DETAILSVIEW_TIMETEXT));
    
    mSizeLabel = static_cast<HbLabel*> (mDocLoader->findWidget(
            GLX_DETAILSVIEW_SIZETEXT));

    //set the frame graphics to the background of the fav icon
    HbFrameItem* frame = new HbFrameItem(this);
    frame->frameDrawer().setFrameType(HbFrameDrawer::NinePieces);
    frame->frameDrawer().setFrameGraphicsName("qtg_fr_multimedia_trans");
    frame->graphicsItem()->setOpacity(0.2);
    mFavIcon->setBackgroundItem(frame->graphicsItem(), -1);
    mFavIcon->setBackground(HbIcon("qtg_fr_multimedia_trans"));
    mFavIcon->setIcon(HbIcon(GLXICON_REMOVE_FAV));
    
    setWidget(mView);

    //Set the Model
    mModel = model;  

    //Set the Layout Correspondingly.
    updateLayout(mWindow->orientation());

    //Shows the Image 
    showImage();
    OstTraceFunctionExit0( GLXDETAILSVIEW_INITIALIZEVIEW_EXIT );
    }

//--------------------------------------------------------------------------------------------------------------------------------------------
//resetView
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsView::resetView()
    {
    GLX_LOG_INFO("GlxDetailsView::resetView");
    OstTrace0( TRACE_NORMAL, GLXDETAILSVIEW_RESETVIEW, "GlxDetailsView::resetView" );
    }

//--------------------------------------------------------------------------------------------------------------------------------------------
//deActivate
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsView::deActivate()
    {
    GLX_LOG_INFO("GlxDetailsView::deActivate");
    OstTraceFunctionEntry0( GLXDETAILSVIEW_DEACTIVATE_ENTRY );

    //Remove the context created to update details
    mModel->setData(QModelIndex(), (int) GlxContextComment,
            GlxRemoveContextRole);
    cleanUp();
    
    OstTraceFunctionExit0( GLXDETAILSVIEW_DEACTIVATE_EXIT );
    }

//--------------------------------------------------------------------------------------------------------------------------------------------
//cleanUp
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsView::cleanUp()
    {
    clearCurrentModel();

    //clear the connections
    clearConnections();   

    delete mFavModel;
    mFavModel = NULL;

    delete mFavIcon;
    mFavIcon = NULL;

    delete mDetailsIcon;
    mDetailsIcon = NULL;
    }

//--------------------------------------------------------------------------------------------------------------------------------------------
//setModel
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsView::setModel(QAbstractItemModel *model)
    {
    OstTrace0( TRACE_NORMAL, GLXDETAILSVIEW_SETMODEL, "GlxDetailsView::setModel" );
    if (mModel == model)
        {
        return;
        }
    clearCurrentModel();
    mModel = model;
    initializeNewModel();
    }

//--------------------------------------------------------------------------------------------------------------------------------------------
//setFavModel
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsView::setFavModel()
{
    GlxModelParm modelParm;
    QString imagePath = (mModel->data(mModel->index(mModel->data(
            mModel->index(0, 0), GlxFocusIndexRole).value<int> (), 0),
            GlxUriRole)).value<QString> ();

    if (mFavModel == NULL)
        {
        modelParm.setCollection(KGlxCollectionPluginFavoritesAlbumId);
        modelParm.setContextMode(GlxContextFavorite);
        modelParm.setPath(imagePath);
        mFavModel = new GlxFavMediaModel(modelParm);
        }
}

//--------------------------------------------------------------------------------------------------------------------------------------------
//initializeNewModel
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsView::initializeNewModel()
    {
    OstTrace0( TRACE_NORMAL, GLXDETAILSVIEW_INITIALIZENEWMODEL, "GlxDetailsView::initializeNewModel" );

    if (mModel)
        {
    connect(mModel, SIGNAL(rowsRemoved(QModelIndex,int,int)), this,
            SLOT(rowsRemoved(QModelIndex,int,int)));
        }
    }

//--------------------------------------------------------------------------------------------------------------------------------------------
//clearCurrentModel
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsView::clearCurrentModel()
    {
    OstTrace0( TRACE_NORMAL, GLXDETAILSVIEW_CLEARCURRENTMODEL, "GlxDetailsView::clearCurrentModel" );

    if (mModel)
        {
        disconnect(mModel, SIGNAL(rowsRemoved(QModelIndex,int,int)), this,
                SLOT(rowsRemoved(QModelIndex,int,int)));
        mModel = NULL;
        }
    }

//--------------------------------------------------------------------------------------------------------------------------------------------
//setConnections
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsView::setConnections()
    {
    connect(mWindow, SIGNAL(orientationChanged(Qt::Orientation)), this,
            SLOT(updateLayout(Qt::Orientation)));
    connect(mFavIcon, SIGNAL(clicked()), this, SLOT(updateFavourites()));
    
    connect(mDescriptions, SIGNAL(labelPressed()), this,
            SLOT(UpdateDescription()));
    
    connect(mModel, SIGNAL( updateDetailsView() ), this, SLOT( FillDetails() ));
    
    connect(mFavModel, SIGNAL( dataChanged(QModelIndex,QModelIndex) ),
               this, SLOT( dataChanged(QModelIndex,QModelIndex) ));    
    }

//--------------------------------------------------------------------------------------------------------------------------------------------
//clearConnections
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsView::clearConnections()
    {
    disconnect(mWindow, SIGNAL(orientationChanged(Qt::Orientation)), this,
            SLOT(updateLayout(Qt::Orientation)));
    
    disconnect(mFavIcon, SIGNAL(clicked()), this, SLOT(updateFavourites()));
    
    disconnect(mDescriptions, SIGNAL(labelPressed()), this,
            SLOT(UpdateDescription()));
    
    disconnect(mModel, SIGNAL( updateDetailsView() ), this, SLOT( FillDetails() ));
    
    disconnect(mFavModel, SIGNAL( dataChanged(QModelIndex,QModelIndex) ),
                this, SLOT( dataChanged(QModelIndex,QModelIndex) ));
    }

//--------------------------------------------------------------------------------------------------------------------------------------------
//getAnimationItem
//--------------------------------------------------------------------------------------------------------------------------------------------
QGraphicsItem * GlxDetailsView::getAnimationItem(GlxEffect transtionEffect)
    {
    OstTrace0( TRACE_NORMAL, GLXDETAILSVIEW_GETANIMATIONITEM, "GlxDetailsView::getAnimationItem" );
    if (transtionEffect == FULLSCREEN_TO_DETAIL || transtionEffect
            == DETAIL_TO_FULLSCREEN)
        {
    return this;
        }
    return NULL;
    }

//--------------------------------------------------------------------------------------------------------------------------------------------
//updateLayout
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsView::updateLayout(Qt::Orientation orient)
    {
    OstTrace1( TRACE_NORMAL, GLXDETAILSVIEW_UPDATELAYOUT, "GlxDetailsView::updateLayout;orient=%d", orient );
    GLX_LOG_INFO("GlxDetailsView::updateLayout ");
    bool loaded = false;
    QString section;

    //Load the widgets accroding to the current Orientation
    if (orient == Qt::Horizontal)
        {
        section = GLX_DETAILSVIEW_LSSECTION;
        }
    else
        {
        section = GLX_DETAILSVIEW_PTSECTION;
        }

    //Load the Sections
    mDocLoader->load(GLX_DETAILSVIEW_DOCMLPATH, section, &loaded);

    //This is just to over come the bug in docloader,once that is fixed we can remove the 
    //below lines of code
     setImageName();
     setDate();

    GLX_LOG_INFO1("GlxDetailsView::updateLayout =%d\n",loaded);
    }

//--------------------------------------------------------------------------------------------------------------------------------------------
//rowsRemoved
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsView::rowsRemoved(const QModelIndex &parent, int start,
        int end)
    {
    Q_UNUSED(parent);
    Q_UNUSED(start);
    Q_UNUSED(end);
    OstTrace0( TRACE_NORMAL, GLXDETAILSVIEW_ROWSREMOVED, "GlxDetailsView::rowsRemoved" );

    if (mModel->rowCount() <= 0)
        {
        return emit actionTriggered(EGlxCmdEmptyData);
        }

    if (start <= mSelIndex && end >= mSelIndex)
        {
        return emit actionTriggered(EGlxCmdBack);
        }
    }

//--------------------------------------------------------------------------------------------------------------------------------------------
//FillData
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsView::FillDetails()
    {
    OstTrace0( TRACE_NORMAL, GLXDETAILSVIEW_SETFORMDATA, "GlxDetailsView::setFormData" );

    qDebug("GlxDetailsView::FillDetails");
    //Call to set the Image Name
    setImageName();

    //Call to set the description
    setDesc();

    //Call to set the date in the from
    setDate();

    //Call to set the time
    setTime();

    //Call to set the size
    setSize();

    }
//--------------------------------------------------------------------------------------------------------------------------------------------
//showImage
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsView::showImage()
    {
    OstTrace0( TRACE_NORMAL, GLXDETAILSVIEW_SHOWIMAGE, "GlxDetailsView::showImage" );

    QVariant variant = mModel->data(mModel->index(0, 0), GlxFocusIndexRole);
    if (variant.isValid() && variant.canConvert<int> ())
        {
        mSelIndex = variant.value<int> ();
        }

    variant = mModel->data(mModel->index(mSelIndex, 0), GlxFsImageRole);
    if (variant.isValid() && variant.canConvert<HbIcon> ())
        {
        QIcon itemIcon = variant.value<HbIcon> ().qicon();
        QPixmap itemPixmap = itemIcon.pixmap(GLX_IMAGE_SIZE, GLX_IMAGE_SIZE);
        QSize sz(GLX_IMAGE_SIZE, GLX_IMAGE_SIZE);
        itemPixmap = itemPixmap.scaled(sz, Qt::IgnoreAspectRatio);

        HbIcon tmp = HbIcon(QIcon(itemPixmap));
        mDetailsIcon->setIcon(tmp);
        }
    }

//--------------------------------------------------------------------------------------------------------------------------------------------
//setImageName
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsView::setImageName()
    {
    OstTraceFunctionEntry0( GLXDETAILSVIEW_SETIMAGENAME_ENTRY );
    QString temp = "<u>";
    QString imagePath = (mModel->data(mModel->index(mModel->data(
            mModel->index(0, 0), GlxFocusIndexRole).value<int> (), 0),
            GlxUriRole)).value<QString> ();
    QString imageName = imagePath.section('\\', -1);
    
    temp.append(imageName);
    temp.append("</u>");
    mImageName->setItemText(temp);    
    OstTraceFunctionExit0( GLXDETAILSVIEW_SETIMAGENAME_EXIT );
    }

//--------------------------------------------------------------------------------------------------------------------------------------------
//setImageName
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsView::setDesc()
    {
    QString description = (mModel->data(mModel->index(mModel->data(
            mModel->index(0, 0), GlxFocusIndexRole).value<int> (), 0),
            GlxDescRole)).value<QString> ();
    mDescriptions->setItemText(description);
    }

//--------------------------------------------------------------------------------------------------------------------------------------------
//setDate
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsView::setDate()
    {
    OstTraceFunctionEntry0( GLXDETAILSVIEW_SETDATE_ENTRY );

    QString datestring;
    QString dateFormat("dd.MM.yyyy");
    QDate date = (mModel->data(mModel->index(mModel->data(
            mModel->index(0, 0), GlxFocusIndexRole).value<int> (), 0),
            GlxDateRole)).value<QDate> ();
    
    datestring = QString("Date: ");
    if (date.isNull() == FALSE)
        {
        OstTrace0( TRACE_NORMAL, GLXDETAILSVIEW_SETDATE, "GlxDetailsView::setDate is not NULL" );
        datestring.append(date.toString(dateFormat));
        }

    mDateLabel->setPlainText(datestring);

    OstTraceFunctionExit0( GLXDETAILSVIEW_SETDATE_EXIT );
    }

//--------------------------------------------------------------------------------------------------------------------------------------------
//setTime
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsView::setTime()
    {
    QString timestring;
    QString timeFormat("h:m ap");
    QTime timevalue = (mModel->data(mModel->index(mModel->data(mModel->index(
            0, 0), GlxFocusIndexRole).value<int> (), 0), GlxTimeRole)).value<
            QTime> ();
    timestring = QString("Time: ");
    if (timevalue.isNull() == FALSE)
        {
        OstTrace0( TRACE_NORMAL, GLXDETAILSVIEW_SETDATE, "GlxDetailsView::setTime is not NULL" );
        timestring.append(timevalue.toString(timeFormat));
        }
    mTimeLabel->setPlainText(timestring);
    }

//--------------------------------------------------------------------------------------------------------------------------------------------
//setSize
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsView::setSize()
    {
    int size = 0;
    size = (mModel->data(mModel->index(mModel->data(mModel->index(0, 0),
            GlxFocusIndexRole).value<int> (), 0), GlxSizeRole)).value<int> ();
    QString sizelabel;
    QString sizestring;
    sizelabel = QString("Size  : ");
    sizestring = sizeinStrings(size);
    sizelabel.append(sizestring);
    mSizeLabel->setPlainText(sizelabel);
    
   }

//--------------------------------------------------------------------------------------------------------------------------------------------
//dataChanged
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsView::dataChanged(QModelIndex startIndex, QModelIndex endIndex)
    {
    Q_UNUSED(endIndex);

    QVariant variant = mFavModel->data(startIndex, GlxFavorites);
    if (variant.isValid() && variant.canConvert<bool> ())
        {
        if (variant.value<bool> ())
            {
            mFavIcon->setIcon(HbIcon(GLXICON_ADD_TO_FAV));
            }
        else
            {
            mFavIcon->setIcon(HbIcon(GLXICON_REMOVE_FAV));
            }
        }
    }

//--------------------------------------------------------------------------------------------------------------------------------------------
//UpdateDescription
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsView::UpdateDescription()
    {
    GLX_LOG_INFO("GlxDetailsView::UpdateDescription ");
    qDebug("GlxDetailsView::UpdateDescription");
    emit actionTriggered(EGlxCmdComment);
    }

//--------------------------------------------------------------------------------------------------------------------------------------------
//updateFavourites
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsView::updateFavourites()
    {
    QVariant variant = mFavModel->data(mFavModel->index(0, 0), GlxFavorites);
    if (variant.isValid() && variant.canConvert<bool> ())
        {
        if (variant.value<bool> ())
            {
            emit actionTriggered(EGlxCmdRemoveFromFav);
            }
        else
            {
            emit actionTriggered(EGlxCmdAddToFav);
            }
        }
    }

//--------------------------------------------------------------------------------------------------------------------------------------------
//sizeinStrings
//--------------------------------------------------------------------------------------------------------------------------------------------
QString GlxDetailsView::sizeinStrings(int size)
    {
    QString sizeString;
    if (size >= KBytesInGB)
        {
        int gbSize = size / KBytesInGB; // Size in GB
        sizeString.setNum(gbSize);
        sizeString.append("GB");
        }
    else if (size >= KBytesInMB)
        {
        int mbSize = size / KBytesInMB; // Size in MB
        sizeString.setNum(mbSize);
        sizeString.append("MB");
        }
    else if (size >= KBytesInKB)
        {
        TInt kBsize = size / KBytesInKB; // bytes to kB
        sizeString.setNum(kBsize);
        sizeString.append("KB");
        }
    else
        {
        sizeString.setNum(size);
        sizeString.append("Bytes");
        }
    return sizeString;
    }
