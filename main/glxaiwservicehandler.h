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

#ifndef GLXFETCHER_H
#define GLXFETCHER_H

#include <hbmainwindow.h>
#include <xqserviceprovider.h>
#include <QStringList>
#include <xqsharablefile.h>

//FORWARD CLASS DECLARATION
class GlxView;
class HbPushButton;
class HbMenu;
class QGraphicsGridLayout; 
class GlxGetImageService;
class GlxMediaModel;
class QModelIndex;
class GlxImageViewerService;
class GlxStateManager;
class CGlxImageViewerManager;
/**
 *  GlxAiwServiceHandler
 * 
 */
class GlxAiwServiceHandler: public HbMainWindow
    {
    Q_OBJECT
public:
    /**
     * Constructor
     */
    GlxAiwServiceHandler();

    /**
     * Destructor.
     */
    ~GlxAiwServiceHandler();
	void launchFetcher();
	void launchImageViewer();
public slots:  
    void itemSelected(const QModelIndex &  index);    
    void handleClientDisconnect();
	void itemSpecificMenuTriggered(qint32,QPointF );
    void openFSView();
	void handleFSSelect();
    void closeContextMenu();
    
private:
    GlxMediaModel *mModel;
    GlxView* mView;
    GlxGetImageService* mService;
	GlxImageViewerService* mImageViewerService;
	GlxStateManager *mStateMgr;
	GlxView* mFSView;
    HbMenu *mFetcherContextMenu;
    };

/**
 *  GlxGetImageService
 * 
 */	
class GlxGetImageService : public XQServiceProvider
{
    Q_OBJECT
public:
    GlxGetImageService( GlxAiwServiceHandler *parent = 0 );
    ~GlxGetImageService();
    bool isActive();
    void complete( QStringList filesList);
    
public slots://for QTHighway to notify provider about request
    void fetch( QVariantMap filter, QVariant flag);
    
public slots://for provider to notify client
    void fetchFailed( int errorCode );
    
private:
    void doComplete( QStringList filesList);
    
private:
    int mImageRequestIndex;
    GlxAiwServiceHandler* mServiceApp;
};


class GlxImageViewerService : public XQServiceProvider
{
    Q_OBJECT
    public:
        GlxImageViewerService( GlxAiwServiceHandler *parent = 0 );
        ~GlxImageViewerService();
        void complete(bool ok);

    public slots:
        bool view(QString file);
        bool view(XQSharableFile file);
        bool asyncRequest() {return mAsyncRequest;}

    private:
        GlxAiwServiceHandler* mServiceApp;
        int mAsyncReqId;
        bool mRetValue;
		bool mAsyncRequest;
		CGlxImageViewerManager* mImageViewerInstance;
};

    
#endif //GLXFETCHER_H
