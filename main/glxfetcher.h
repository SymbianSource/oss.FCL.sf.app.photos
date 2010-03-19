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


//FORWARD CLASS DECLARATION
class GlxView;
class HbPushButton;
class QGraphicsGridLayout; 
class GlxGetImageService;
class GlxMediaModel;
class QModelIndex;
/**
 *  GlxFetcher
 * 
 */
class GlxFetcher: public HbMainWindow
    {
    Q_OBJECT
public:
    /**
     * Constructor
     */
    GlxFetcher();

    /**
     * Destructor.
     */
    ~GlxFetcher();
	void launchFetcher();
public slots:  
    void itemSelected(const QModelIndex &  index);    
    
private:
    GlxMediaModel *mModel;
    GlxView* mView;
    GlxGetImageService* mService;
    };

/**
 *  GlxGetImageService
 * 
 */	
class GlxGetImageService : public XQServiceProvider
{
    Q_OBJECT
public:
    GlxGetImageService( GlxFetcher *parent = 0 );
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
    GlxFetcher* mServiceApp;
};
    
#endif //GLXFETCHER_H
