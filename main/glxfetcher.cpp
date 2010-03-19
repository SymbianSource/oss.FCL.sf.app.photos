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
#include <glxfetcher.h>
#include <glxcollectionpluginall.hrh>

#include <xqserviceutil.h>

// ----------------------------------------------------------------------------
// GlxFetcher()
// ----------------------------------------------------------------------------
//
GlxFetcher::GlxFetcher():HbMainWindow() ,mModel(NULL),mView(NULL),mService(NULL)
{
    mService = new GlxGetImageService( this);

#ifdef _DEBUG
	QString t;
	QStringList args = QApplication::arguments();
    foreach (QString arg, args)
    {
	t += "GlxFetcher::cmdline arg=" + arg + "\n";
    }
	qDebug()<< t;
#endif
}

// ----------------------------------------------------------------------------
// ~GlxFetcher()
// ----------------------------------------------------------------------------
//
GlxFetcher::~GlxFetcher()
{
    if(mView){
        removeView(mView);
    }
    delete mView;
    delete mModel;
    delete mService;
}

// ----------------------------------------------------------------------------
// itemSelected()
// ----------------------------------------------------------------------------
//
void GlxFetcher::itemSelected(const QModelIndex &  index)
{
    qDebug()<< "GlxFetcher::itemSelected" ;
    if ( mService->isActive() ){
        qDebug()<< "GlxFetcher::itemSelected :: SERVICE ACTIVE" ;
        QVariant variant = mModel->data( index, GlxUriRole );
        if ( variant.isValid()  ) {
            QString itemPath = variant.value<QString>();
            qDebug()<< "GlxFetcher::itemSelected :: VALID URI -->" << itemPath;
            QStringList list = (QStringList() << itemPath );
            mService->complete( list );
        }
    }
}	

void GlxFetcher::launchFetcher()
{
    GlxModelParm modelParm (KGlxCollectionPluginAllImplementationUid, 0);
    mModel = new GlxMediaModel (modelParm);

    mView = GlxViewsFactory::createView(GLX_GRIDVIEW_ID, this);
    mView->activate();
    mView->setModel(mModel);
    addView(mView);
    connect(mView, SIGNAL(gridItemSelected(const QModelIndex &)), this, SLOT( itemSelected(const QModelIndex &)));

}
// ----------------------------------------------------------------------------
// GlxGetImageService()
// ----------------------------------------------------------------------------
//
GlxGetImageService::GlxGetImageService(GlxFetcher* parent)
: XQServiceProvider(QLatin1String("com.nokia.services.media.Image"),parent),mServiceApp(parent)
{
    mImageRequestIndex=-1;
    publishAll();
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
void GlxGetImageService::fetchFailed( int errorCode )
{
    QStringList filesList;
    filesList.insert(0, QString::number( errorCode ));//result
    doComplete(filesList);
}

// ----------------------------------------------------------------------------
// complete()
// ----------------------------------------------------------------------------
//
void GlxGetImageService::complete( QStringList filesList )
{
    doComplete(filesList);
}
    
// ----------------------------------------------------------------------------
// doComplete()
// ----------------------------------------------------------------------------
//
void GlxGetImageService::doComplete( QStringList filesList)
{
    if ( isActive() ){
        completeRequest(mImageRequestIndex, filesList);
        mImageRequestIndex=-1;
        connect(this, SIGNAL(returnValueDelivered()), qApp, SLOT(quit()));
        }
}

// ----------------------------------------------------------------------------
// isActive()
// ----------------------------------------------------------------------------
//
bool GlxGetImageService::isActive()
{
    return mImageRequestIndex> -1;
}

// ----------------------------------------------------------------------------
// fetch()
// ----------------------------------------------------------------------------
//
void GlxGetImageService::fetch( QVariantMap filter, QVariant flag)
{
    Q_UNUSED(filter)
    Q_UNUSED(flag)
	mImageRequestIndex  = setCurrentRequestAsync();
	mServiceApp->launchFetcher();
    }
