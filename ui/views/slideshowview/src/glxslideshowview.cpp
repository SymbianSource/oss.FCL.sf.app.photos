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
#include <QEvent>
#include <QDebug>
#include <hbmainwindow.h>
#include <hbdocumentloader.h>


//User Includes
#include "glxuistd.h"
#include "glxviewids.h"
#include "glxmodelparm.h"
#include "glxdocloaderdefs.h"
#include "glxslideshowview.h"
#include "glxslideshowwidget.h"
#include "glxcommandhandlers.hrh" //To:Do temporary remove once orbit back problem is resolved

#include <glxhdmicontroller.h>
#include <glxlog.h>
#include <glxtracer.h>

GlxSlideShowView::GlxSlideShowView(HbMainWindow *window,HbDocumentLoader *DocLoader) : GlxView(GLX_SLIDESHOWVIEW_ID), mModel(NULL),
                  mWindow(window), mSlideShowWidget(NULL),iHdmiController(NULL)
    {
    TRACER("GlxSlideShowView::GlxSlideShowView()");
    mDocLoader = DocLoader;
    }

GlxSlideShowView::~GlxSlideShowView()
    {
    TRACER("GlxSlideShowView::~GlxSlideShowView()");
    if(mSlideShowWidget){
        disconnect( mSlideShowWidget, SIGNAL( slideShowEvent( GlxSlideShowEvent ) ), this, SLOT( slideShowEventHandler( GlxSlideShowEvent ) ) );
        delete mSlideShowWidget;
        mSlideShowWidget = NULL;
        }

    if(mDocLoader) {
        mDocLoader->reset();
        delete mDocLoader;
        mDocLoader = NULL;
        }
    }

void GlxSlideShowView::activate()
    {
    //To:Do error handling
    TRACER("GlxSlideShowView::activate()");

    //finds the widgets from the docml
    loadObjects();
    mWindow->setItemVisible(Hb::AllItems, false) ;
    connect(mWindow, SIGNAL(orientationChanged(Qt::Orientation)), this, SLOT(orientationChanged(Qt::Orientation)));
    connect( mSlideShowWidget, SIGNAL( slideShowEvent( GlxSlideShowEvent ) ), this, SLOT( slideShowEventHandler( GlxSlideShowEvent ) ) ); 
    connect( mSlideShowWidget, SIGNAL( indexchanged() ), this, SLOT( indexchanged() ) );
    if (!iHdmiController) {
        GLX_LOG_INFO("GlxSlideShowView::activate() - CGlxHdmi" );
        iHdmiController = CGlxHdmiController::NewL();
        }   
    }

void GlxSlideShowView::deActivate()
    {
    TRACER("GlxSlideShowView::deActivate()");
    mWindow->setItemVisible(Hb::AllItems , TRUE);
    disconnect(mWindow, SIGNAL(orientationChanged(Qt::Orientation)), this, SLOT(orientationChanged(Qt::Orientation)));
    disconnect( mSlideShowWidget, SIGNAL( slideShowEvent( GlxSlideShowEvent ) ), this, SLOT( slideShowEventHandler( GlxSlideShowEvent ) ) );
    disconnect( mSlideShowWidget, SIGNAL( indexchanged() ), this, SLOT( indexchanged() ) );
    //Delete the Items in the slide show widget
    mSlideShowWidget->cleanUp();
    mModel = NULL;
    if (iHdmiController) {
        GLX_LOG_INFO("GlxSlideShowView() deActivate delete iHdmiController " );
        delete iHdmiController;
        iHdmiController = NULL;
        }
    }

void GlxSlideShowView::setModel(QAbstractItemModel *model)
    {
    TRACER("GlxSlideShowView::setModel()");
    GLX_LOG_INFO2("GlxSlideShowView::setModel() model %u mModel %u", model, mModel);
    if ( mModel == model ) {
    return ;
    }
    mModel = model;
    mSlideShowWidget->setModel(mModel);
    SetImageToHdmiL();
    }

void GlxSlideShowView::setModelContext()
    {
    TRACER("GlxSlideShowView::setModelContext()");
    if ( mModel && mWindow ) {
    GLX_LOG_INFO1("GlxSlideShowView::setModelContext %d", mWindow->orientation() );

    if ( mWindow->orientation() == Qt::Horizontal ) {
    mModel->setData(QModelIndex(), (int)GlxContextLsFs, GlxContextRole );
    }
    else {
    mModel->setData(QModelIndex(), (int)GlxContextPtFs, GlxContextRole );
    }
    }
    }

void GlxSlideShowView::orientationChanged(Qt::Orientation)
    {
    TRACER("GlxSlideShowView::orientationChanged()");
    QRect screenRect = mWindow->geometry(); 
    setModelContext();
    mSlideShowWidget->orientationChanged(screenRect);
    }

void GlxSlideShowView::slideShowEventHandler( GlxSlideShowEvent e) 
    {
    TRACER("GlxSlideShowView::slideShowEventHandler()");
    GLX_LOG_INFO1("GlxSlideShowView::slideShowEventHandler() event %d", e);
    switch ( e ) {
        case UI_ON_EVENT :
            mWindow->setItemVisible(Hb::AllItems, TRUE) ;
            break;

        case UI_OFF_EVENT :
            mWindow->setItemVisible(Hb::AllItems, false) ;
            break;

        case EMPTY_DATA_EVENT :
            emit actionTriggered( EGlxCmdEmptyData );
            break;

        default :
            break;        
    }
    }

void GlxSlideShowView::indexchanged()
    {
    TRACER("GlxSlideShowView::indexchanged()");
    SetImageToHdmiL();
    }

bool GlxSlideShowView::event(QEvent *event)
    {
    TRACER("GlxSlideShowView::event()");
    GLX_LOG_INFO1("GlxSlideShowView::event() %d event type", event->type());
    if ( event->type() ==  QEvent::WindowActivate && mSlideShowWidget) {
    mSlideShowWidget->startSlideShow();
    }

    if ( event->type() ==  QEvent::WindowDeactivate && mSlideShowWidget) {
    mSlideShowWidget->stopSlideShow();
    }
    return HbView::event(event);
    }

void GlxSlideShowView::loadObjects()
    {
    TRACER("GlxSlideShowView::loadObjects");    
    //Load/Retrieve the widgets
    HbView *view     = static_cast<GlxSlideShowView*>(mDocLoader->findWidget(GLXSLIDESHOW_VIEW));
    mSlideShowWidget = static_cast<GlxSlideShowWidget*>(mDocLoader->findWidget(GLXSLIDESHOW_WIDGET));
    
    //Initialise the Slideshow widget
    mSlideShowWidget->setSlideShowWidget(mDocLoader);
    mSlideShowWidget->setItemGeometry(mWindow->geometry());  
    }

void GlxSlideShowView::SetImageToHdmiL()
    {
    TRACER("GlxSlideShowView::SetImageToHdmiL() - CGlxHdmi 1" );
    if (iHdmiController)
        {
        GLX_LOG_INFO("GlxSlideShowView::SetImageToHdmiL() - CGlxHdmi 2" );
        // Get the image uri
        QString imagePath = (mModel->data(mModel->index(mModel->data(mModel->index(0,0),GlxFocusIndexRole).value<int>(),0),GlxUriRole)).value<QString>();
        if(imagePath.isNull())
            {
            GLX_LOG_INFO("GlxSlideShowView::SetImageToHdmiL() path is null" );
            }
        qDebug() << "GlxSlideShowView::SetImageToHdmiL() imagePath= " << imagePath;
        TPtrC aPtr = reinterpret_cast<const TUint16*>(imagePath.utf16());

        // Get the image Dimensions
        QSize imageDimension = (mModel->data(mModel->index(mModel->data(mModel->index(0,0),GlxFocusIndexRole).value<int>(),0),GlxDimensionsRole)).value<QSize>();
        TSize imageSize(imageDimension.width(),imageDimension.height());
        
        iHdmiController->SetImageL(aPtr,imageSize );
        }
    }
