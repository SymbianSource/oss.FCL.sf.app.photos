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
* Description: Handles command related to mediaeditors in fullscreen 
* for image Rotate, Crop & Set as Wallpaper
*
*/



#include <glxcommandfactory.h>
#include <photoeditor_highway.hrh>
#include "glxcommandhandlereditimage.h"
#include <glxcommandhandlers.hrh>
#include <glxmodelparm.h>
#include <glxmediamodel.h>
#include <XQServiceRequest.h>
#include <XQAiwRequest.h>

GlxCommandHandlerEditImage::GlxCommandHandlerEditImage() : mReq(NULL)
    {
    //Nothing to do here
    }

GlxCommandHandlerEditImage::~GlxCommandHandlerEditImage()
    {
    delete mReq;
    mReq = NULL;
    }

void GlxCommandHandlerEditImage::executeCommand(int commandId,int collectionId, QList<QModelIndex> /*indexList*/)
    {
    const QString service = QLatin1String("PhotoEditor");
    const QString interface = QLatin1String("com.nokia.symbian.imageeditor");
    const QString operation = QLatin1String("view(QString,int)");
    
    //Connect to service provider
    if(mReq == NULL)
        {
        mReq = mAppmgr.create(service, interface, operation, true);
        mReq->setEmbedded(true);
        mReq->setSynchronous(true);
        }
    
    if(mReq == NULL)
        {
        return;
        }

    GlxModelParm modelParm (collectionId, 0);
    GlxMediaModel* mediaModel = new GlxMediaModel (modelParm);
    
    //Get the file path for the item selected
    QString imagePath = (mediaModel->data(mediaModel->index(mediaModel->data(mediaModel->index(0,0),GlxFocusIndexRole).value<int>(),0),GlxUriRole)).value<QString>();
    delete mediaModel;
    
    QList<QVariant> args;
    args << imagePath;
	if(EGlxCmdSetWallpaper == commandId)
		{
		args << EEditorHighwayWallpaperCrop;
		}
	else if(EGlxCmdRotateImgCrop == commandId)
        {
        args << EEditorHighwayFreeCrop;
        }
    else if(EGlxCmdRotateImgCW == commandId)
        {
        args << EEditorHighwayRotateCW;
        }
    else // if(EGlxCmdRotateImgCCW == aCommandId)
        {
        args << EEditorHighwayRotateCCW;
        }
    mReq->setArguments(args);
    
    // Send the request
    bool res = mReq->send();
    if  (!res) 
        {
       // Request failed. 
        qDebug("QtSamplePhotos::launchPhotoEditor request cannot be send");
        }
    }

void GlxCommandHandlerEditImage::doHandleUserAction(GlxMediaModel* /*model*/,QList<QModelIndex> /*indexList*/) const 
    {
    //Dummy, to keepup with compiler errore
    }
