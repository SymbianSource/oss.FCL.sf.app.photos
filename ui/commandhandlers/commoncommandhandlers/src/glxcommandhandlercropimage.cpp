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
* Description: 
*
*/

#include "glxcommandhandlercropimage.h"

#include <glxcommandfactory.h>
#include <photoeditor_highway.hrh>
#include <glxcommandhandlers.hrh>
#include <XQServiceRequest.h>
#include <XQAiwRequest.h>
#include <glxmodelparm.h>
#include <glxmediamodel.h>


GlxCommandHandlerCropImage::GlxCommandHandlerCropImage() : mReq(NULL)
    {
    //Nothing to do here for now
    }

GlxCommandHandlerCropImage::~GlxCommandHandlerCropImage()
    {
    delete mReq;
    mReq = NULL;
    }

void GlxCommandHandlerCropImage::doHandleUserAction(GlxMediaModel* model,
        QList<QModelIndex> /*indexList*/) const
    {
    const QString interface = QLatin1String("com.nokia.symbian.imageeditor");
    const QString operation = QLatin1String("view(QString,int)");
    const QString service = QLatin1String("PhotoEditor");
    
    if(mReq == NULL)
        {
        //Connect to service provider    
        mReq = mAppmgr.create(service, interface, operation, true);
        mReq->setEmbedded(true);
        mReq->setSynchronous(true);
        }
    
    if(mReq == NULL)
        {
        qDebug("QtSamplePhotos::launchPhotoEditor request not Created");
        return;
        }
    
    //Get the file path for the item selected
    QString imagePath = (model->data(model->index(model->data(model->index(0,0),GlxFocusIndexRole).value<int>(),0),GlxUriRole)).value<QString>();
       
    QList<QVariant> args;
    args << imagePath;
    args << EEditorHighwayFreeCrop;
    mReq->setArguments(args);
    
    // Send the request
    bool res = mReq->send();
    if  (!res) 
        {
       // Request failed. 
        qDebug("QtSamplePhotos::launchPhotoEditor request cannot be send");
        }
    
    }
