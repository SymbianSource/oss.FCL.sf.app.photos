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

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "glxdetailsviewTraces.h"
#endif


//--------------------------------------------------------------------------------------------------------------------------------------------
//GlxDetailsView
//--------------------------------------------------------------------------------------------------------------------------------------------
GlxDetailsView::GlxDetailsView(HbMainWindow *window) :  GlxView ( GLX_DETAILSVIEW_ID),
    mDetailsIcon(NULL), 
    mModel(NULL), 
    mWindow(window), 
    mDataForm(NULL), 
    mDetailModel(NULL),
    mCustomPrototype(NULL),
    mSelIndex (0)
    {
    OstTraceFunctionEntry0( GLXDETAILSVIEW_GLXDETAILSVIEW_ENTRY );
    
    setContentFullScreen( true );//for smooth transtion between grid to full screen and vice versa
    OstTraceFunctionExit0( GLXDETAILSVIEW_GLXDETAILSVIEW_EXIT );
    }

//--------------------------------------------------------------------------------------------------------------------------------------------
//~GlxDetailsView
//--------------------------------------------------------------------------------------------------------------------------------------------
GlxDetailsView::~GlxDetailsView()
    {
    OstTrace0( TRACE_IMPORTANT, GLXDETAILSVIEW_GLXDETAILSVIEW, "GlxDetailsView::~GlxDetailsView" );

    if(mDetailsIcon) {
    delete mDetailsIcon;
    mDetailsIcon = NULL;
    }

    if(mDataForm && mDataForm->model()) {
    delete mDataForm->model();
    mDataForm->setModel(0);
    }

    if(mDataForm) {
    delete mDataForm;
    mDataForm = NULL;
    }



    clearCurrentModel();
    disconnect(mWindow, SIGNAL(orientationChanged(Qt::Orientation)), this, SLOT(updateLayout(Qt::Orientation)));
    }

//--------------------------------------------------------------------------------------------------------------------------------------------
//activate
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsView::activate()
    {
    OstTraceFunctionEntry0( GLXDETAILSVIEW_ACTIVATE_ENTRY );
    
    setFormData();
    connect(mWindow, SIGNAL(orientationChanged(Qt::Orientation)), this, SLOT(updateLayout(Qt::Orientation)));
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
        mDetailsIcon = new HbIconItem(this);

        //Create the form and the model for the data form
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
    
    //Do Nothing here
    }

//--------------------------------------------------------------------------------------------------------------------------------------------
//deActivate
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsView::deActivate()
{ 
    OstTraceFunctionEntry0( GLXDETAILSVIEW_DEACTIVATE_ENTRY );
    clearCurrentModel();
    disconnect(mWindow, SIGNAL(orientationChanged(Qt::Orientation)), this, SLOT(updateLayout(Qt::Orientation)));
    OstTraceFunctionExit0( GLXDETAILSVIEW_DEACTIVATE_EXIT );
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

    HbDataFormModelItem *imageLabelitem =
    mDetailModel->appendDataFormItem(HbDataFormModelItem::TextItem, QString("Name"), mDetailModel->invisibleRootItem());
    imageLabelitem->setData(HbDataFormModelItem::KeyRole, QString(""));
    imageLabelitem->setContentWidgetData(QString("text"),QString(""));
    
    //---------------------------DATE LABEL --------------------------------------------//
    OstTrace0( TRACE_NORMAL, DUP2_GLXDETAILSVIEW_ADDWIDGETS, "GlxDetailsView::addWidgets date label" );
    HbDataFormModelItem *dateLabelItem = mDetailModel->appendDataFormItem((HbDataFormModelItem::DataItemType)(DateLabelItem), QString("Date"), mDetailModel->invisibleRootItem());
    dateLabelItem->setData(HbDataFormModelItem::KeyRole,QString(""));

    //----------------------------COMMENTS TEXT ITEM---------------------------------------------//
    OstTrace0( TRACE_NORMAL, DUP5_GLXDETAILSVIEW_ADDWIDGETS, "GlxDetailsView::addWidgets comment text" );
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
    OstTrace0( TRACE_NORMAL, GLXDETAILSVIEW_SETMODEL, "GlxDetailsView::setModel" );
    if ( mModel == model ) {
    return ;
    }
    clearCurrentModel();
    mModel = model;
    initializeNewModel();
    }

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
    
    QRect screen_rect = mWindow->geometry(); 
    setGeometry(screen_rect);

    if(orient == Qt::Horizontal)
        {
        OstTrace0( TRACE_NORMAL, DUP1_GLXDETAILSVIEW_UPDATELAYOUT, "GlxDetailsView::updateLayout HORIZONTAL" );
        mDetailsIcon->setPos(15,75);
        mDataForm->setGeometry(301,60,335,300);

        }
    else
        {
        OstTrace0( TRACE_NORMAL, DUP2_GLXDETAILSVIEW_UPDATELAYOUT, "GlxDetailsView::updateLayout VERTICAL" );
        mDetailsIcon->setPos(25,75);
        mDataForm->setGeometry(5,351,335,300);
        mDataForm->setMaximumWidth(340);
        }
    }

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
        if(mWindow->orientation() == Qt::Horizontal)
            {
            OstTrace0( TRACE_NORMAL, DUP1_GLXDETAILSVIEW_SHOWIMAGE, "GlxDetailsView::showImage HORIZONTAL" );
            
            mDetailsIcon->resize(QSize(270, 270));
            mDetailsIcon->setIcon(variant.value<HbIcon>());
            }
        else
            {
            OstTrace0( TRACE_NORMAL, DUP2_GLXDETAILSVIEW_SHOWIMAGE, "GlxDetailsView::showImage VERTICAL" );
            mDetailsIcon->resize(QSize(310, 260));
            mDetailsIcon->setIcon(variant.value<HbIcon>());
            }
        }
    }

//--------------------------------------------------------------------------------------------------------------------------------------------
//FillData
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsView::setFormData()
    {
    OstTrace0( TRACE_NORMAL, GLXDETAILSVIEW_SETFORMDATA, "GlxDetailsView::setFormData" );
    
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

    //Fetch the text edit for displaying the Name from the Form Model
    GlxDetailsCustomWidgets * imageLabel = (GlxDetailsCustomWidgets *)mDataForm->itemByIndex(mDetailModel->index(0,0));

    if(imageLabel)
        {
        OstTrace0( TRACE_NORMAL, GLXDETAILSVIEW_SETIMAGENAME, "GlxDetailsView::setImageName ImageLabel !=NULL" );
        HbLineEdit* label = static_cast<HbLineEdit*>(imageLabel->dataItemContentWidget());

        if(label) 
            {
            OstTrace0( TRACE_NORMAL, DUP1_GLXDETAILSVIEW_SETIMAGENAME, "GlxDetailsView::setImageName SetText" );
            label->setText(imageName);
            }
        }
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

    //Fetch the Label from the Form Model
    GlxDetailsCustomWidgets * dateLabel = (GlxDetailsCustomWidgets *)mDataForm->itemByIndex(mDetailModel->index(1,0));

    if(dateLabel) 
        {
        OstTrace0( TRACE_NORMAL, DUP1_GLXDETAILSVIEW_SETDATE, "GlxDetailsView::setDate Datelabel is not NULL" );
        
        HbLabel* label = static_cast<HbLabel*>(dateLabel->dataItemContentWidget());

        if(label) 
            {
            OstTrace0( TRACE_NORMAL, DUP2_GLXDETAILSVIEW_SETDATE, "GlxDetailsView::setDate SetText" );
            label->setPlainText(datestring);
            }
        }    
    OstTraceFunctionExit0( GLXDETAILSVIEW_SETDATE_EXIT );
    }

void GlxDetailsView::initializeNewModel()
    {
    OstTrace0( TRACE_NORMAL, GLXDETAILSVIEW_INITIALIZENEWMODEL, "GlxDetailsView::initializeNewModel" );
    
    if ( mModel ) {
    connect(mModel, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(rowsRemoved(QModelIndex,int,int)));
    }
    }

void GlxDetailsView::clearCurrentModel()
    {
    OstTrace0( TRACE_NORMAL, GLXDETAILSVIEW_CLEARCURRENTMODEL, "GlxDetailsView::clearCurrentModel" );
    
    if ( mModel ) {
    disconnect(mModel, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(rowsRemoved(QModelIndex,int,int)));
    mModel = NULL ;
    }    
    }
