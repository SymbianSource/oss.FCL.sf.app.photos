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

//--------------------------------------------------------------------------------------------------------------------------------------------
#include <hblabel.h>
#include <hbdataform.h>
#include <hbinstance.h>
#include <hbiconitem.h>
#include <hblineedit.h>
#include <hbdataformmodel.h>
#include <hbdataformmodelitem.h>
#include <hbdataformviewitem.h>

//--------------------------------------------------------------------------------------------------------------------------------------------
#include "glxviewids.h"
#include "glxicondefs.h" //Contains the icon names/Ids
#include "glxmodelparm.h"
#include "glxdetailsview.h"
#include "glxfavmediamodel.h"
#include <glxcommandhandlers.hrh>
#include "glxdetailscustomicon.h"
#include "glxdetailscustomwidgets.h"
#include <glxcollectionpluginalbums.hrh>


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "glxdetailsviewTraces.h"
#endif


//SIZE OF THE IMAGE , LAYOUTS TEAM NEED TO GIVER THE SIZE IN UNITS
#define GLX_IMAGE_SIZE 215 

//--------------------------------------------------------------------------------------------------------------------------------------------
//GlxDetailsView
//--------------------------------------------------------------------------------------------------------------------------------------------
GlxDetailsView::GlxDetailsView(HbMainWindow *window) :  
    GlxView ( GLX_DETAILSVIEW_ID),
    mDetailsIcon(NULL),
    mFavIcon(NULL),
    mModel(NULL),
    mFavModel(NULL),
    mDetailModel(NULL),
    mWindow(window), 
    mDataForm(NULL),     
    mCustomPrototype(NULL),
    mSelIndex(0),
    mDateLabelItem(NULL),
    mImageLabelitem(NULL),
    mCommentsLabelitem(NULL)
        {
    OstTraceFunctionEntry0( GLXDETAILSVIEW_GLXDETAILSVIEW_ENTRY );    
    setContentFullScreen( true );//for smooth transtion between grid to full screen and vice versa
    OstTraceFunctionExit0( GLXDETAILSVIEW_GLXDETAILSVIEW_EXIT );
        }

//--------------------------------------------------------------------------------------------------------------------------------------------
//activate
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsView::activate()
    {
    OstTraceFunctionEntry0( GLXDETAILSVIEW_ACTIVATE_ENTRY );
    setFormData();    
    connect(mWindow, SIGNAL(orientationChanged(Qt::Orientation)), this, SLOT(updateLayout(Qt::Orientation)));
    connect(mFavIcon,SIGNAL(updateFavourites()),this ,SLOT(updateFavourites()));
    OstTraceFunctionExit0( GLXDETAILSVIEW_ACTIVATE_EXIT );
    }

//--------------------------------------------------------------------------------------------------------------------------------------------
//initializeView
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsView::initializeView(QAbstractItemModel *model)
    {   
    OstTraceFunctionEntry0( GLXDETAILSVIEW_INITIALIZEVIEW_ENTRY );

    //To show the thumbnail 
    if ( mDataForm == NULL) {
    mDataForm = new HbDataForm(this);
    mDetailModel = new HbDataFormModel();

    //custom prototype
    mCustomPrototype = new GlxDetailsCustomWidgets(mDataForm);           
    QList <HbAbstractViewItem*> protos = mDataForm->itemPrototypes();
    protos.append(mCustomPrototype);
    mDataForm->setItemPrototypes(protos);

    //Add the Widgets according to the mime type
    addWidgets();
    }   

    if( mDetailsIcon == NULL) {
    mDetailsIcon = new HbIconItem(this);
    mFavIcon = new GlxDetailsCustomIcon(this);
    }

    //Set the Model
    mModel = model;
    initializeNewModel();

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
    OstTrace0( TRACE_NORMAL, GLXDETAILSVIEW_RESETVIEW, "GlxDetailsView::resetView" );   
    }

//--------------------------------------------------------------------------------------------------------------------------------------------
//deActivate
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsView::deActivate()
    { 
    OstTraceFunctionEntry0( GLXDETAILSVIEW_DEACTIVATE_ENTRY );
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
    if(mWindow) {
    disconnect(mWindow, SIGNAL(orientationChanged(Qt::Orientation)), this, SLOT(updateLayout(Qt::Orientation)));
    }

    if(mFavModel) {
    disconnect( mFavModel, SIGNAL( dataChanged(QModelIndex,QModelIndex) ), this, SLOT( dataChanged(QModelIndex,QModelIndex) ) );
    }

    if(mFavIcon) {
    disconnect(mFavIcon,SIGNAL(updateFavourites()),this ,SLOT(updateFavourites()));
    }

    delete mFavModel;
    mFavModel = NULL;

    delete mFavIcon;
    mFavIcon = NULL;

    delete mDetailsIcon;
    mDetailsIcon = NULL;    
    }  

//--------------------------------------------------------------------------------------------------------------------------------------------
//~GlxDetailsView
//--------------------------------------------------------------------------------------------------------------------------------------------
GlxDetailsView::~GlxDetailsView()
    {
    OstTrace0( TRACE_IMPORTANT, GLXDETAILSVIEW_GLXDETAILSVIEW, "GlxDetailsView::~GlxDetailsView" );

    cleanUp();     

    if(mDetailModel) {
      delete mDetailModel;
      mDataForm->setModel(0);
    }

    if(mDataForm) {
    delete mDataForm;
    mDataForm = NULL;
    }       
    }

//--------------------------------------------------------------------------------------------------------------------------------------------
//addWidgets
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsView::addWidgets()
    {
    OstTrace0( TRACE_NORMAL, GLXDETAILSVIEW_ADDWIDGETS, "GlxDetailsView::addWidgets create Form" );

    //----------------------------START OF CREATION OF WIDGETS---------------------------------//
    // To add new widgets in the details view, add it here.

    //---------------------------IMAGE NAME LABEL --------------------------------------------//
    OstTrace0( TRACE_NORMAL, DUP1_GLXDETAILSVIEW_ADDWIDGETS, "GlxDetailsView::addWidgets create Image Label" );
    mImageLabelitem = mDetailModel->appendDataFormItem(HbDataFormModelItem::TextItem, QString("Name"), NULL);

    //---------------------------DATE LABEL --------------------------------------------//
    OstTrace0( TRACE_NORMAL, DUP2_GLXDETAILSVIEW_ADDWIDGETS, "GlxDetailsView::addWidgets date label" );
    mDateLabelItem = mDetailModel->appendDataFormItem((HbDataFormModelItem::DataItemType)(DateLabelItem), QString("Date"), NULL);


    //----------------------------COMMENTS TEXT ITEM---------------------------------------------//
    OstTrace0( TRACE_NORMAL, DUP5_GLXDETAILSVIEW_ADDWIDGETS, "GlxDetailsView::addWidgets comment text" );
    mCommentsLabelitem = mDetailModel->appendDataFormItem(HbDataFormModelItem::TextItem, QString("Description"), NULL);

    //----------------------------END OF CREATION OF WIDGETS-------------------------------------//

    //Set the model to the Data Form
    mDataForm->setModel(mDetailModel);
    }

//--------------------------------------------------------------------------------------------------------------------------------------------
//setFavModel
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsView::setFavModel()
    {
    GlxModelParm modelParm;

    QString imagePath = (mModel->data(mModel->index(mModel->data(mModel->index(0,0),GlxFocusIndexRole).value<int>(),0),GlxUriRole)).value<QString>();

    if(mFavModel == NULL) {
    modelParm.setCollection( KGlxCollectionPluginFavoritesAlbumId );
    modelParm.setContextMode(GlxContextFavorite);
    modelParm.setPath(imagePath);
    mFavModel = new GlxFavMediaModel( modelParm );
    }
    QString imageName = imagePath.section('\\',-1);
    connect( mFavModel, SIGNAL( dataChanged(QModelIndex,QModelIndex) ), this, SLOT( dataChanged(QModelIndex,QModelIndex) ) );   
    }

//--------------------------------------------------------------------------------------------------------------------------------------------
//setModel
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsView::setModel(QAbstractItemModel *model)
    {
    OstTrace0( TRACE_NORMAL, GLXDETAILSVIEW_SETMODEL, "GlxDetailsView::setModel" );
    if ( mModel == model ) {
    return ;
    }
    clearCurrentModel();
    mModel = model;
    initializeNewModel();
    }

//--------------------------------------------------------------------------------------------------------------------------------------------
//getAnimationItem
//--------------------------------------------------------------------------------------------------------------------------------------------
QGraphicsItem * GlxDetailsView::getAnimationItem(GlxEffect transtionEffect)
    {
    OstTrace0( TRACE_NORMAL, GLXDETAILSVIEW_GETANIMATIONITEM, "GlxDetailsView::getAnimationItem" );
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
    OstTrace1( TRACE_NORMAL, GLXDETAILSVIEW_UPDATELAYOUT, "GlxDetailsView::updateLayout;orient=%d", orient );

    qreal IconPosX = 0.0;
    qreal IconPosY = 0.0;
    qreal FavIconPosX = 0.0;
    qreal FavIconPosY = 0.0;
    qreal DataFormX = 0.0;
    qreal DataFormY = 0.0;
    qreal DataFormWidth = 0.0;
    qreal DataFormHeight = 0.0;


    qreal screenWidth = 0.0;
    qreal screenHeight = 0.0;
    qreal leftMargin = 0.0;
    qreal bottomMargin = 0.0;
    qreal screenMargin = 0.0;
    qreal favIconSize = 0.0;
    qreal chromeHeight = 0.0;
    qreal middleVerticalMargin = 0.0;

    style()->parameter("hb-param-screen-width", screenWidth);
    style()->parameter("hb-param-screen-height", screenHeight);

    style()->parameter("hb-param-margin-gene-middle-vertical", middleVerticalMargin);
    style()->parameter("hb-param-margin-gene-left", leftMargin);
    style()->parameter("hb-param-margin-gene-bottom", bottomMargin);
    style()->parameter("hb-param-margin-gene-screen", screenMargin);

    style()->parameter("hb-param-graphic-size-primary-small", favIconSize);
    style()->parameter("hb-param-widget-chrome-height", chromeHeight);


    if(orient == Qt::Horizontal)
        {
    OstTrace0( TRACE_NORMAL, DUP1_GLXDETAILSVIEW_UPDATELAYOUT, "GlxDetailsView::updateLayout HORIZONTAL" );
    setGeometry(0,0,640,360);
    IconPosX    = screenMargin;
    IconPosY    = (screenMargin + chromeHeight);
    FavIconPosX = (IconPosX + GLX_IMAGE_SIZE - (favIconSize +10));
    FavIconPosY = (IconPosY + GLX_IMAGE_SIZE - (favIconSize + 10));
    DataFormX =  screenMargin + GLX_IMAGE_SIZE + leftMargin ;
    DataFormY =  screenMargin + chromeHeight ;

    //This Commented code should be un commented , once the screen width and height size issue is solved and also
    //the dataform issue of wrapping issue is solved
    // DataFormWidth =  screenWidth(640) - DataFormX  - screenMargin ;
    // DataFormHeight = screenHeight(360) - DataFormY - screenMargin ;
    DataFormWidth = screenWidth - (screenMargin  + screenMargin) ;
    DataFormHeight = screenHeight - DataFormY;

        }
    else
        {
    OstTrace0( TRACE_NORMAL, DUP2_GLXDETAILSVIEW_UPDATELAYOUT, "GlxDetailsView::updateLayout VERTICAL" );
    setGeometry(0,0,screenWidth,screenHeight);
    IconPosX    = (screenWidth/2) - (GLX_IMAGE_SIZE/2);
    IconPosY    = (screenMargin + chromeHeight);
    FavIconPosX = (IconPosX + GLX_IMAGE_SIZE - (favIconSize +10));
    FavIconPosY = (IconPosY + GLX_IMAGE_SIZE - (favIconSize +10));
    DataFormX =  screenMargin;
    DataFormY =  IconPosY + GLX_IMAGE_SIZE + middleVerticalMargin + bottomMargin ;
    DataFormWidth = screenWidth - (screenMargin  + screenMargin) ;
    DataFormHeight = screenHeight - DataFormY;
        }

    mDetailsIcon->setPos(IconPosX,IconPosY);
    //The Numerical should be updated once we get the parameters size from the layouts.
    mFavIcon->setItemGeometry(QRect(FavIconPosX-2 ,FavIconPosY-2 ,favIconSize+12,favIconSize + 12)) ;
    //Place the FavIcon with respect to the Widget.
    mFavIcon->setItemPos(2 ,2 );
    mDataForm->setGeometry(DataFormX,DataFormY,DataFormWidth,DataFormHeight);    
    }

//--------------------------------------------------------------------------------------------------------------------------------------------
//rowsRemoved
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsView::rowsRemoved(const QModelIndex &parent, int start, int end)
    {
    Q_UNUSED(parent);
    Q_UNUSED(start);
    Q_UNUSED(end);
    OstTrace0( TRACE_NORMAL, GLXDETAILSVIEW_ROWSREMOVED, "GlxDetailsView::rowsRemoved" );

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
    OstTrace0( TRACE_NORMAL, GLXDETAILSVIEW_SHOWIMAGE, "GlxDetailsView::showImage" );

    QVariant variant = mModel->data( mModel->index(0,0), GlxFocusIndexRole );    
    if ( variant.isValid() &&  variant.canConvert<int> () ) {
    mSelIndex = variant.value<int>();  
    }

    variant = mModel->data( mModel->index( mSelIndex ,0), GlxFsImageRole);
    if ( variant.isValid() &&  variant.canConvert<HbIcon> () )
        {
        QIcon itemIcon = variant.value<HbIcon>().qicon();
        QPixmap itemPixmap = itemIcon.pixmap(GLX_IMAGE_SIZE,GLX_IMAGE_SIZE);
        QSize sz(GLX_IMAGE_SIZE,GLX_IMAGE_SIZE);
        itemPixmap = itemPixmap.scaled(sz, Qt::IgnoreAspectRatio );
    
    
        HbIcon tmp = HbIcon( QIcon(itemPixmap)) ;
        mDetailsIcon->setSize(QSize(GLX_IMAGE_SIZE, GLX_IMAGE_SIZE));
        mDetailsIcon->setIcon(tmp);
        mFavIcon->setItemIcon(HbIcon("qtg_graf_ratingslider_unrated"));
    
        qreal favIconSize = 0.0;
        style()->parameter("hb-param-graphic-size-primary-small", favIconSize);
        mFavIcon->setItemSize(QSize(favIconSize+10, favIconSize+10));
        }        
    }

//--------------------------------------------------------------------------------------------------------------------------------------------
//FillData
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsView::setFormData()
    {
    OstTrace0( TRACE_NORMAL, GLXDETAILSVIEW_SETFORMDATA, "GlxDetailsView::setFormData" );

    //create and set the Favourite Model
    setFavModel();

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
    OstTraceFunctionEntry0( GLXDETAILSVIEW_SETIMAGENAME_ENTRY );

    QString imagePath = (mModel->data(mModel->index(mModel->data(mModel->index(0,0),GlxFocusIndexRole).value<int>(),0),GlxUriRole)).value<QString>();
    QString imageName = imagePath.section('\\',-1);

    mImageLabelitem->setContentWidgetData("text",imageName);
    OstTraceFunctionExit0( GLXDETAILSVIEW_SETIMAGENAME_EXIT );
    }

//--------------------------------------------------------------------------------------------------------------------------------------------
//setDate
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsView::setDate()
    {
    OstTraceFunctionEntry0( GLXDETAILSVIEW_SETDATE_ENTRY );

    QString datestring;
    QString str("dd.MM.yyyy");
    QDate date = (mModel->data(mModel->index(mModel->data(mModel->index(0,0),GlxFocusIndexRole).value<int>(),0),GlxDateRole)).value<QDate>();

    if(date.isNull() == FALSE )
        {
    OstTrace0( TRACE_NORMAL, GLXDETAILSVIEW_SETDATE, "GlxDetailsView::setDate is not NULL" );
    datestring = date.toString(str);
        }

    mDateLabelItem->setContentWidgetData("plainText",datestring);

    OstTraceFunctionExit0( GLXDETAILSVIEW_SETDATE_EXIT );
    }

//--------------------------------------------------------------------------------------------------------------------------------------------
//initializeNewModel
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsView::initializeNewModel()
    {
    OstTrace0( TRACE_NORMAL, GLXDETAILSVIEW_INITIALIZENEWMODEL, "GlxDetailsView::initializeNewModel" );

    if ( mModel ) {
    connect(mModel, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(rowsRemoved(QModelIndex,int,int)));
    }
    }

//--------------------------------------------------------------------------------------------------------------------------------------------
//clearCurrentModel
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsView::clearCurrentModel()
    {
    OstTrace0( TRACE_NORMAL, GLXDETAILSVIEW_CLEARCURRENTMODEL, "GlxDetailsView::clearCurrentModel" );

    if ( mModel ) {
    disconnect(mModel, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(rowsRemoved(QModelIndex,int,int)));
    mModel = NULL ;
    }    
    }

//--------------------------------------------------------------------------------------------------------------------------------------------
//dataChanged
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsView::dataChanged(QModelIndex startIndex, QModelIndex endIndex)
    {
    Q_UNUSED(endIndex);      
 
    QVariant variant = mFavModel->data( startIndex, GlxFavorites );
    if ( variant.isValid() &&  variant.canConvert<bool> () ) 
        {
        if(variant.value<bool>() )
            {  
             mFavIcon->setItemIcon(HbIcon(GLXICON_ADD_TO_FAV));
            }
        else 
            {
             mFavIcon->setItemIcon(HbIcon(GLXICON_REMOVE_FAV));
            }
        }
    }

//--------------------------------------------------------------------------------------------------------------------------------------------
//updateFavourites
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsView::updateFavourites()
    {
    QVariant variant = mFavModel->data(mFavModel->index(0,0), GlxFavorites );
    if ( variant.isValid() &&  variant.canConvert<bool> ())
        {
        if(variant.value<bool>() )
            {
             emit actionTriggered( EGlxCmdRemoveFromFav); 
            }
        else
            {
            emit actionTriggered(EGlxCmdAddToFav);
            }
        }
    }
