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

#include <glxmediamodel.h>
#include <glxmodelcommandhandler.h>
#include <glxmodelparm.h>

GlxModelCommandHandler::GlxModelCommandHandler()
    {
    }

GlxModelCommandHandler::~GlxModelCommandHandler()
    {
    }

void GlxModelCommandHandler::executeCommand(int commandId, int collectionId,QList<QModelIndex> indexList)
    {
    GlxModelParm modelParm (collectionId, 0);
    GlxMediaModel* mediaModel = new GlxMediaModel (modelParm);
    doHandleUserAction(mediaModel,indexList);
    delete mediaModel;
    }
