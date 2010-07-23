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
#include <QCoreApplication>
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

#include <glxtvoutwrapper.h>

#include <glxlog.h>
#include <glxtracer.h>

GlxSlideShowView::GlxSlideShowView(HbMainWindow *window,HbDocumentLoader *DocLoader) : GlxView(GLX_SLIDESHOWVIEW_ID), mModel(NULL),
                  mWindow(window), mSlideShowWidget(NULL),mTvOutWrapper(NULL)
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
    TRACER("GlxSlideShowView::activate()");
    mWindow->setOrientation(Qt::Horizontal, false);

    //finds the widgets from the docml
    loadObjects();
    setTitleBarVisible(FALSE);
    setStatusBarVisible(FALSE);
    connect( mSlideShowWidget, SIGNAL( slideShowEvent( GlxSlideShowEvent ) ), this, SLOT( slideShowEventHandler( GlxSlideShowEvent ) ) ); 
    connect( mSlideShowWidget, SIGNAL( indexchanged() ), this, SLOT( indexchanged() ) );
    
    QCoreApplication::instance()->installEventFilter(this);
    
    if (!mTvOutWrapper) {
        mTvOutWrapper = new GlxTvOutWrapper();
    }
}

void GlxSlideShowView::deActivate()
{
    TRACER("GlxSlideShowView::deActivate()");
    mWindow->unsetOrientation(false);
    
    setStatusBarVisible(TRUE);
    setTitleBarVisible(TRUE);
    
                
    disconnect( mSlideShowWidget, SIGNAL( slideShowEvent( GlxSlideShowEvent ) ), this, SLOT( slideShowEventHandler( GlxSlideShowEvent ) ) );
    disconnect( mSlideShowWidget, SIGNAL( indexchanged() ), this, SLOT( indexchanged() ) );
        
    //Delete the Items in the slide show widget
    mSlideShowWidget->cleanUp();
    
    QCoreApplication::instance()->removeEventFilter(this);

    if (mTvOutWrapper){
        delete mTvOutWrapper;
        mTvOutWrapper = NULL;
    }
}

void GlxSlideShowView::setModel(QAbstractItemModel *model)
{
    TRACER("GlxSlideShowView::setModel()");
    GLX_LOG_INFO2("GlxSlideShowView::setModel() model %u mModel %u", model, mModel);
    if ( mModel != model ) {
    
    modelDestroyed();
    mModel = model;
    connect( mModel, SIGNAL( destroyed() ), this, SLOT( modelDestroyed() ) );
    } 
    setModelContext();    
    mSlideShowWidget->setModel(mModel);
    
    if (mTvOutWrapper){
        mTvOutWrapper->setModel(mModel, screenSize(), true);
        mTvOutWrapper->setImagetoHDMI();
    }
}

void GlxSlideShowView::setModelContext()
{
    TRACER("GlxSlideShowView::setModelContext()");
    if ( mModel && mWindow ) {
        GLX_LOG_INFO1("GlxSlideShowView::setModelContext %d", mWindow->orientation() );
    
            mModel->setData(QModelIndex(), (int)GlxContextLsFs, GlxContextRole );
      /* if ( mWindow->orientation() == Qt::Horizontal ) {
            mModel->setData(QModelIndex(), (int)GlxContextLsFs, GlxContextRole );
        }
        else {
            mModel->setData(QModelIndex(), (int)GlxContextPtFs, GlxContextRole );
        } */
    }
}

void GlxSlideShowView::orientationChanged(Qt::Orientation)
{
    TRACER("GlxSlideShowView::orientationChanged()");
    setModelContext();
    mSlideShowWidget->orientationChanged( screenGeometry() );
}

void GlxSlideShowView::slideShowEventHandler( GlxSlideShowEvent e) 
{
    TRACER("GlxSlideShowView::slideShowEventHandler()");
    GLX_LOG_INFO1("GlxSlideShowView::slideShowEventHandler() event %d", e);
    switch ( e ) {
        case UI_ON_EVENT :
            setTitleBarVisible(TRUE);
            setStatusBarVisible(TRUE);
            break;

        case UI_OFF_EVENT :
            setTitleBarVisible(FALSE);
            setStatusBarVisible(FALSE);
            break;

        case EMPTY_DATA_EVENT :
            emit actionTriggered( EGlxCmdEmptyData );
            break;
            
        case EFFECT_STARTED:
            if (mTvOutWrapper){
            mTvOutWrapper->fadeSurface(false);
            }
            break;

        default :
            break;        
    }
}

void GlxSlideShowView::indexchanged()
{
    TRACER("GlxSlideShowView::indexchanged()");
    if (mTvOutWrapper){
        mTvOutWrapper->setImagetoHDMI();
    }
}

void GlxSlideShowView::modelDestroyed()
{
    if ( mModel ) {
        disconnect( mModel, SIGNAL( destroyed() ), this, SLOT( modelDestroyed() ) );
        mModel = NULL; 
    }
}

bool GlxSlideShowView::eventFilter(QObject *obj, QEvent *event)
{
    TRACER("GlxSlideShowView::event()");
    GLX_LOG_INFO1("GlxSlideShowView::event() %d event type", event->type());
    if ( event->type() ==  QEvent::ApplicationActivate && mSlideShowWidget) {
        if (mTvOutWrapper){
        GLX_LOG_INFO("GlxSlideShowView::event() shift to native - CGlxHdmi");
        mTvOutWrapper->setToNativeMode();    
        }
        mSlideShowWidget->startSlideShow();
    }
    if ( event->type() ==  QEvent::ApplicationDeactivate && mSlideShowWidget) {
        if (mTvOutWrapper){
        GLX_LOG_INFO("GlxSlideShowView::event() shift to Clone - CGlxHdmi");
        mTvOutWrapper->setToCloningMode();    
        }
       mSlideShowWidget->stopSlideShow();
    }
    return HbView::eventFilter(obj,event);
}

void GlxSlideShowView::loadObjects()
{
    TRACER("GlxSlideShowView::loadObjects");    
    //Load/Retrieve the widgets
    HbView *view     = static_cast<GlxSlideShowView*>(mDocLoader->findWidget(GLXSLIDESHOW_VIEW));
    mSlideShowWidget = static_cast<GlxSlideShowWidget*>(mDocLoader->findWidget(GLXSLIDESHOW_WIDGET));
    
    //Initialise the Slideshow widget
    mSlideShowWidget->setSlideShowWidget(mDocLoader);
    mSlideShowWidget->setItemGeometry( screenGeometry() );  
}

