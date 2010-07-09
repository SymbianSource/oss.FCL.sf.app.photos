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

#ifndef GlxCommandHandlerEditImage_H_
#define GlxCommandHandlerEditImage_H_


#include <glxmodelcommandhandler.h>
#include <xqappmgr.h>               // for XQApplicationManager

#ifdef BUILD_COMMONCOMMANDHANDLERS
#define GLX_COMMONCOMMANDHANDLERS_EXPORT Q_DECL_EXPORT
#else
#define GLX_COMMONCOMMANDHANDLERS_EXPORT Q_DECL_IMPORT
#endif

//Forward Declaration
class XQAiwRequest;

class GLX_COMMONCOMMANDHANDLERS_EXPORT GlxCommandHandlerEditImage : public GlxModelCommandHandler
{
  
public:
    GlxCommandHandlerEditImage();
    ~GlxCommandHandlerEditImage();
    void executeCommand(int commandId,int collectionId, QList<QModelIndex> indexList = QList<QModelIndex>() );
    void doHandleUserAction(GlxMediaModel* model,QList<QModelIndex> indexList) const ;

private:
    XQAiwRequest* mReq;
    XQApplicationManager mAppmgr;
};


#endif /* GlxCommandHandlerEditImage_H_ */
