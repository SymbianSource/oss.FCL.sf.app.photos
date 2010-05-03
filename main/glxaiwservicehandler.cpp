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

#include <hbapplication.h>
#include <hbview.h>
#include <hbmenu.h>
#include <hbaction.h>
#include <hbtoolbar.h>
#include <hbstyleloader.h>

#include <QtDebug>
#include <Qt>
#include <qstringlist.h>
#include <qmessagebox.h>

#include <glxmediamodel.h>
#include <glxviewsfactory.h>
#include <glxviewids.h>
#include <glxview.h>
#include <glxgridview.h>
#include <glxmodelparm.h>
#include <glxaiwservicehandler.h>
#include <glxcollectionpluginall.hrh>
#include <glxcollectionpluginimageviewer.hrh>
#include <xqserviceutil.h>
#include <glxstatemanager.h>
#include <glximageviewermanager.h>
#include <glxexternalutility.h>
#include "glxlocalisationstrings.h"

// ----------------------------------------------------------------------------
// GlxAiwServiceHandler()
// ----------------------------------------------------------------------------
//
GlxAiwServiceHandler::GlxAiwServiceHandler() :
    HbMainWindow(), mModel(NULL), mView(NULL), mService(NULL),
            mStateMgr(NULL),mFSView(NULL),mFetcherContextMenu(NULL)
    {
    mService = new GlxGetImageService(this);
    mImageViewerService = new GlxImageViewerService(this);

#ifdef _DEBUG
	QString t;
	QStringList args = QApplication::arguments();
    foreach (QString arg, args)
        {
        t += "GlxAiwServiceHandler::cmdline arg=" + arg + "\n";
        }
	qDebug()<< t;
#endif
    }

// ----------------------------------------------------------------------------
// ~GlxAiwServiceHandler()
// ----------------------------------------------------------------------------
//
GlxAiwServiceHandler::~GlxAiwServiceHandler()
    {
    HbStyleLoader::unregisterFilePath(":/data/photos.css");
    if (mView)
        {
        removeView(mView);
        }
    delete mView;
	delete mFSView;
    delete mModel;
    delete mService;
    }

void GlxAiwServiceHandler::handleClientDisconnect()
    {
    // Just quit application
    qApp->quit();
    }

// ----------------------------------------------------------------------------
// itemSelected()
// ----------------------------------------------------------------------------
//
void GlxAiwServiceHandler::itemSelected(const QModelIndex & index)
    {
    qDebug() << "GlxFetcher::itemSelected";
    if (mService->isActive())
        {
        qDebug() << "GlxFetcher::itemSelected :: SERVICE ACTIVE";
        QVariant variant = mModel->data(index, GlxUriRole);
        if (variant.isValid())
            {
            QString itemPath = variant.value<QString> ();
            qDebug() << "GlxFetcher::itemSelected :: VALID URI -->" << itemPath;
            QStringList list = (QStringList() << itemPath);
            mService->complete(list);
            }
        }
    }	



void GlxAiwServiceHandler::launchFetcher()
    {
    HbStyleLoader::registerFilePath(":/data/photos.css");
    GlxModelParm modelParm(KGlxCollectionPluginAllImplementationUid, 0);
    mModel = new GlxMediaModel(modelParm);

    if ( this->orientation() == Qt::Horizontal ) {
        mModel->setData(QModelIndex(), (int)GlxContextLsFs, GlxContextRole );
    }
    else {
        mModel->setData(QModelIndex(), (int)GlxContextPtFs, GlxContextRole );
    }    

    mView = GlxViewsFactory::createView(GLX_GRIDVIEW_ID, this);
    mView->activate();
    mView->setModel(mModel);
    addView(mView);
    connect(mView, SIGNAL(gridItemSelected(const QModelIndex &)), this,
            SLOT( itemSelected(const QModelIndex &)));
			
	connect ( mView, SIGNAL(itemSpecificMenuTriggered(qint32,QPointF ) ),
			this, SLOT( itemSpecificMenuTriggered(qint32,QPointF ) ),
			Qt::QueuedConnection );
    }

void GlxAiwServiceHandler::itemSpecificMenuTriggered(qint32 viewId,QPointF pos)
	{
    mFetcherContextMenu = new HbMenu();
	HbAction *action = mFetcherContextMenu->addAction(GLX_MENU_OPEN);
	connect(action, SIGNAL(triggered()), this, SLOT(openFSView()));
	connect(this, SIGNAL(aboutToChangeOrientation ()), mFetcherContextMenu, SLOT(close()));
	connect( mFetcherContextMenu, SIGNAL( aboutToClose () ), this, SLOT( closeContextMenu() ) );
	mFetcherContextMenu->setPreferredPos( pos );
	mFetcherContextMenu->show();
 	}

void GlxAiwServiceHandler::closeContextMenu()
    {
    disconnect( this, SIGNAL( aboutToChangeOrientation () ), mFetcherContextMenu, SLOT( close() ) );
    disconnect( mFetcherContextMenu, SIGNAL( aboutToClose () ), this, SLOT( closeContextMenu() ) ); 
    mFetcherContextMenu->deleteLater();
    mFetcherContextMenu = NULL;
    }
	
void GlxAiwServiceHandler::openFSView()
	{
    HbAction* selectAction = new HbAction(GLX_BUTTON_SELECT);
	connect(selectAction, SIGNAL(triggered()), this, SLOT(handleFSSelect()));
    HbToolBar* toolBar = new HbToolBar();
    toolBar->setOrientation( Qt::Horizontal );
    toolBar->setVisible(true);
	toolBar->addAction(selectAction);
	
	mModel->setData( QModelIndex(), FETCHER_S, GlxSubStateRole );
    mFSView = GlxViewsFactory::createView(GLX_FULLSCREENVIEW_ID, this);
    //ownership transfered to view
	mFSView->setToolBar(toolBar);
    mFSView->activate();
    mFSView->setModel(mModel);
    addView(mFSView);
    setCurrentView(mFSView,false);
	}


void GlxAiwServiceHandler::handleFSSelect()
	{
	QModelIndex selectedIndex = mModel->index(mModel->data(mModel->index(0,0),GlxFocusIndexRole).value<int>(),0);
	itemSelected(selectedIndex);
    }

void GlxAiwServiceHandler::launchImageViewer()
    {
    qApp->setApplicationName("Image Viewer");
    GlxExternalUtility* util = GlxExternalUtility::instance();
    util->setMainWindow(this);
    delete mStateMgr;
    mStateMgr = NULL;
    mStateMgr = new GlxStateManager();
    mStateMgr->launchFromExternal();
    if (mImageViewerService && mImageViewerService->asyncRequest())
        {
        connect(mImageViewerService, SIGNAL(returnValueDelivered()), this,
                SLOT(handleAnswerDelivered()));
        mImageViewerService->complete(true);
        }
    return;
    }
// ----------------------------------------------------------------------------
// GlxGetImageService()
// ----------------------------------------------------------------------------
//
GlxGetImageService::GlxGetImageService(GlxAiwServiceHandler* parent) :
            XQServiceProvider(
                    QLatin1String("com.nokia.services.media.Image"), parent),
            mServiceApp(parent)
    {
    mImageRequestIndex = -1;
    publishAll();
    connect(this, SIGNAL(clientDisconnected()), mServiceApp,
            SLOT(handleClientDisconnect()));
    }

// ----------------------------------------------------------------------------
// ~GlxGetImageService()
// ----------------------------------------------------------------------------
//
GlxGetImageService::~GlxGetImageService()
    {
    }

// ----------------------------------------------------------------------------
// fetchFailed()
// ----------------------------------------------------------------------------
//
void GlxGetImageService::fetchFailed(int errorCode)
    {
    QStringList filesList;
    filesList.insert(0, QString::number(errorCode));//result
    doComplete(filesList);
    }

// ----------------------------------------------------------------------------
// complete()
// ----------------------------------------------------------------------------
//
void GlxGetImageService::complete(QStringList filesList)
    {
    doComplete(filesList);
    }

// ----------------------------------------------------------------------------
// doComplete()
// ----------------------------------------------------------------------------
//
void GlxGetImageService::doComplete(QStringList filesList)
    {
    if (isActive())
        {
        completeRequest(mImageRequestIndex, filesList);
        mImageRequestIndex = -1;
        connect(this, SIGNAL(returnValueDelivered()), qApp, SLOT(quit()));
        }
    }

// ----------------------------------------------------------------------------
// isActive()
// ----------------------------------------------------------------------------
//
bool GlxGetImageService::isActive()
    {
    return mImageRequestIndex > -1;
    }

// ----------------------------------------------------------------------------
// fetch()
// ----------------------------------------------------------------------------
//
void GlxGetImageService::fetch(QVariantMap filter, QVariant flag)
    {
    Q_UNUSED(filter)
    Q_UNUSED(flag)
    mImageRequestIndex = setCurrentRequestAsync();
    mServiceApp->launchFetcher();
    }

// ----------GlxImageViewerService---------------

GlxImageViewerService::GlxImageViewerService(GlxAiwServiceHandler* parent) :
    XQServiceProvider(QLatin1String(
            "com.nokia.services.media.com.nokia.symbian.IFileView"), parent),
            mServiceApp(parent), mAsyncReqId(-1), mAsyncRequest(false),
            mImageViewerInstance(NULL)

    {
    publishAll();
    connect(this, SIGNAL(clientDisconnected()), mServiceApp,
            SLOT(handleClientDisconnect()));
    }

GlxImageViewerService::~GlxImageViewerService()
    {
    if (mImageViewerInstance)
        {
        mImageViewerInstance->Close();
        }
    }

void GlxImageViewerService::complete(bool ok)
    {
    if (mAsyncReqId == -1)
        return;
    completeRequest(mAsyncReqId, QVariant(ok));
    }

bool GlxImageViewerService::view(QString file)
    {
    XQRequestInfo info = requestInfo();
    mAsyncRequest = !info.isSynchronous();
    if (!mImageViewerInstance)
        {
        mImageViewerInstance = CGlxImageViewerManager::InstanceL();
        }
    file.replace(QString("/"), QString("\\"));
    TPtrC16 str(reinterpret_cast<const TUint16*> (file.utf16()));
    HBufC* uri = str.Alloc();

    mImageViewerInstance->SetImageUriL(*uri);
    if (mAsyncRequest)
        {
        mAsyncReqId = setCurrentRequestAsync();
        }
    mServiceApp->launchImageViewer();
    return true;
    }

bool GlxImageViewerService::view(XQSharableFile sf)
    {
    if (!mImageViewerInstance)
        {
        mImageViewerInstance = CGlxImageViewerManager::InstanceL();
        }
    RFile file;
    bool ok = sf.getHandle(file);
    if (ok)
        {
        mImageViewerInstance->SetImageFileHandleL(file);
        sf.close();
        }
    mServiceApp->launchImageViewer();
    mAsyncRequest = !XQServiceUtil::isEmbedded();

    if (mAsyncRequest)
        {
        mAsyncReqId = setCurrentRequestAsync();
        connect(this, SIGNAL(clientDisconnected()), this,
                SLOT(handleClientDisconnect()));
        }
    return true;
    }


	
