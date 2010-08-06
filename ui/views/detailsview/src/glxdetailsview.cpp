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
#include <hbparameterlengthlimiter.h>
//--------------------------------------------------------------------------------------------------------------------------------------------
#include "glxviewids.h"
#include "glxicondefs.h" //Contains the icon names/Ids
#include "glxmodelparm.h"
#include "glxmediamodel.h"
#include "glxdetailsview.h"
#include "glxfavmediamodel.h"
#include "glxdocloaderdefs.h"
#include <glxcommandhandlers.hrh>
#include  "glxlocalisationstrings.h"
#include "glxdetailstextedit.h"
#include "glxdetailsicon.h"


#include "glxviewdocloader.h"
#include <glxcollectionpluginalbums.hrh>

#include "glxlog.h"
#include "glxloggerenabler.h"


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "glxdetailsviewTraces.h"
#endif

//SIZE OF THE IMAGE , LAYOUTS TEAM NEED TO GIVER THE SIZE IN UNITS
#define GLX_IMAGE_WIDTH 333
#define GLX_IMAGE_HEIGHT 215 
const int KBytesInKB = 1024;
const int KBytesInMB = 1024 * 1024;
const int KBytesInGB = 1024 * 1024 * 1024;

//--------------------------------------------------------------------------------------------------------------------------------------------
//GlxDetailsView
//--------------------------------------------------------------------------------------------------------------------------------------------
GlxDetailsView::GlxDetailsView(HbMainWindow *window) 
    : GlxView( GLX_DETAILSVIEW_ID ),
      mDetailsIcon( NULL ),
      mFavIcon( NULL ), 
      mModel( NULL ), 
      mFavModel( NULL ),
      mWindow( window ),
      mSelIndex( 0 ),
      mDocLoader( NULL ),
      mImageName( NULL ),
      mDescriptions( NULL ),
      mDateLabel( NULL ),
      mSizeLabel( NULL ),
      mTimeLabel( NULL )
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
    if(getSubState() != IMAGEVIEWER_DETAIL_S) {
       setFavModel();
    }
    
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
void GlxDetailsView::initializeView( QAbstractItemModel *model, GlxView *preView)
{
    Q_UNUSED( preView )
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
    
    mDetailsIcon = static_cast<HbLabel*> (mDocLoader->findWidget(
            GLX_DETAILSVIEW_IMAGE));
    mFavIcon = static_cast<GlxDetailsIcon*> (mDocLoader->findWidget(
            GLX_DETAILSVIEW_FAVICON));


    mDescriptions = static_cast<GlxDetailsTextEdit*> (mDocLoader->findWidget(
            GLX_DETAILSVIEW_DESCRPTIONTEXT));
    
    mImageName = static_cast<GlxDetailsTextEdit*> (mDocLoader->findWidget(
            GLX_DETAILSVIEW_IMGNAME));
    
    mDateLabel = static_cast<HbLabel*> (mDocLoader->findWidget(
            GLX_DETAILSVIEW_DATETEXT));
    
    mTimeLabel = static_cast<HbLabel*> (mDocLoader->findWidget(
            GLX_DETAILSVIEW_TIMETEXT));
    
    mSizeLabel = static_cast<HbLabel*> (mDocLoader->findWidget(
            GLX_DETAILSVIEW_SIZETEXT));

    mFavIconEnabled  =  HbIcon(GLXICON_ADD_TO_FAV);
    mFavIconDisabled =  HbIcon(GLXICON_REMOVE_FAV);   
        
    //Set the Model
    mModel = model;  
   if(getSubState() == IMAGEVIEWER_DETAIL_S) {
       mFavIcon->hide();
    }
    else
    {
       mFavIcon->setItemIcon(HbIcon(GLXICON_REMOVE_FAV));
     }	
	 
	setWidget(mView);
	
    //Set the Layout Correspondingly.
    updateLayout(mWindow->orientation());

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
    qDebug("GlxDetailsView::cleanUp Enter");
	//clear the connections
    clearConnections();   
    
    clearCurrentModel();

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

    if (mFavModel == NULL) {
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
    
    if (mModel) {
        connect( mModel, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(rowsRemoved(QModelIndex,int,int)));
        connect( mModel, SIGNAL( updateDetailsView() ), this, SLOT( FillDetails() ));
        connect( mModel, SIGNAL( destroyed() ), this, SLOT( modelDestroyed() ) );
    }
}

//--------------------------------------------------------------------------------------------------------------------------------------------
//clearCurrentModel
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsView::clearCurrentModel()
{
    OstTrace0( TRACE_NORMAL, GLXDETAILSVIEW_CLEARCURRENTMODEL, "GlxDetailsView::clearCurrentModel" );

    if (mModel) {
        disconnect(mModel, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(rowsRemoved(QModelIndex,int,int)));
        disconnect(mModel, SIGNAL( updateDetailsView() ), this, SLOT( FillDetails() ));
        disconnect( mModel, SIGNAL( destroyed() ), this, SLOT( modelDestroyed() ) );
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
    
    if(getSubState() != IMAGEVIEWER_DETAIL_S) {    
    connect(mFavIcon, SIGNAL(updateFavourites()), this, SLOT(updateFavourites()));
    
    connect(mDescriptions, SIGNAL(labelPressed()), this,
            SLOT(UpdateDescription()));
    
    connect(mFavModel, SIGNAL( dataChanged(QModelIndex,QModelIndex) ),
                   this, SLOT( dataChanged(QModelIndex,QModelIndex) ));
    }
}

//--------------------------------------------------------------------------------------------------------------------------------------------
//clearConnections
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsView::clearConnections()
{
    
    qDebug("GlxDetailsView:: clearConnections");
    disconnect(mWindow, SIGNAL(orientationChanged(Qt::Orientation)), this,
            SLOT(updateLayout(Qt::Orientation)));
           
    if(mModel && getSubState() != IMAGEVIEWER_DETAIL_S) {
    disconnect(mFavIcon, SIGNAL(updateFavourites()), this, SLOT(updateFavourites()));
    disconnect(mDescriptions, SIGNAL(labelPressed()), this,
            SLOT(UpdateDescription()));
    disconnect(mFavModel, SIGNAL( dataChanged(QModelIndex,QModelIndex) ),
                    this, SLOT( dataChanged(QModelIndex,QModelIndex) ));
    }
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

    showImage();
	//This is just to over come the bug in docloader,once that is fixed we can remove the 
    //below lines of code
    FillDetails();
    
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

void GlxDetailsView::modelDestroyed()
{
    mModel = NULL;
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
         mDetailsIcon->setIcon(variant.value<HbIcon> ());
        }
   }

//--------------------------------------------------------------------------------------------------------------------------------------------
//setImageName
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsView::setImageName()
    {
    OstTraceFunctionEntry0( GLXDETAILSVIEW_SETIMAGENAME_ENTRY );
    QString imagePath = (mModel->data(mModel->index(mModel->data(
            mModel->index(0, 0), GlxFocusIndexRole).value<int> (), 0),
            GlxUriRole)).value<QString> ();
    QString imageName = imagePath.section('\\', -1);
    
    mImageName->setItemText(imageName);    
    OstTraceFunctionExit0( GLXDETAILSVIEW_SETIMAGENAME_EXIT );
    }

//--------------------------------------------------------------------------------------------------------------------------------------------
//setImageName
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsView::setDesc()
    {
    
    //This is a hack , Initialize to some characters and then set the text
    //as of know if the description text is Null , we cant see the Description edit box
    //The issue has been informed to Application designer team.
    mDescriptions->setItemText("a");
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
        
    if (date.isNull() == FALSE)
        {
        OstTrace0( TRACE_NORMAL, GLXDETAILSVIEW_SETDATE, "GlxDetailsView::setDate is not NULL" );
        QString dateStringValue = date.toString(dateFormat);
        datestring = hbTrId(GLX_DETAILS_DATE).arg(dateStringValue);
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
        
    if (timevalue.isNull() == FALSE)
        {
        OstTrace0( TRACE_NORMAL, GLXDETAILSVIEW_SETDATE, "GlxDetailsView::setTime is not NULL" );
        QString timeStringValue = timevalue.toString(timeFormat);
        timestring = hbTrId(GLX_DETAILS_TIME).arg(timeStringValue);
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
    
    QString sizeString;
    sizeString = sizeinStrings(size);
    mSizeLabel->setPlainText(sizeString);
    
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
            mFavIcon->setItemIcon(mFavIconEnabled);
            }
        else
            {
            mFavIcon->setItemIcon(mFavIconDisabled);
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
        sizeString = HbParameterLengthLimiter(GLX_DETAILS_SIZE_GB, gbSize);         
        }
    else if (size >= KBytesInMB)
        {
        int mbSize = size / KBytesInMB; // Size in MB
        sizeString = HbParameterLengthLimiter(GLX_DETAILS_SIZE_MB, mbSize);         
        }
    else if (size >= KBytesInKB)
        {
        int kbSize = size / KBytesInKB; // Size in KB
        sizeString = HbParameterLengthLimiter(GLX_DETAILS_SIZE_KB, kbSize);
        }
    else
        {
         sizeString = HbParameterLengthLimiter(GLX_DETAILS_SIZE_BYTES, size);
        }
    return sizeString;
    }

//--------------------------------------------------------------------------------------------------------------------------------------------
//getSubState
//--------------------------------------------------------------------------------------------------------------------------------------------
int GlxDetailsView::getSubState()
  {
    int substate = NO_DETAIL_S;

    if (mModel) {
      QVariant variant = mModel->data(mModel->index(0, 0), GlxSubStateRole);

      if (variant.isValid() && variant.canConvert<int> ()) {
           substate = variant.value<int> ();
      }
    } 
    return substate;
 }
