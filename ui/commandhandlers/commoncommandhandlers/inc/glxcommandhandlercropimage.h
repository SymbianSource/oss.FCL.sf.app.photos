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

#ifndef GLXCOMMANDHANDLERCROPIMAGE_H_
#define GLXCOMMANDHANDLERCROPIMAGE_H_


#include "glxmodelcommandhandler.h"
#include <xqappmgr.h>               // for XQApplicationManager

#ifdef BUILD_COMMONCOMMANDHANDLERS
#define GLX_COMMONCOMMANDHANDLERS_EXPORT Q_DECL_EXPORT
#else
#define GLX_COMMONCOMMANDHANDLERS_EXPORT Q_DECL_IMPORT
#endif

//Forward Declaration
class XQAiwRequest;

class GLX_COMMONCOMMANDHANDLERS_EXPORT GlxCommandHandlerCropImage : public GlxModelCommandHandler
{
  
public:
    GlxCommandHandlerCropImage();
    ~GlxCommandHandlerCropImage();
    void doHandleUserAction(GlxMediaModel* model,QList<QModelIndex> indexList) const ;

private:
    mutable XQAiwRequest* mReq;
    mutable XQApplicationManager mAppmgr;
};


#endif /* GLXCOMMANDHANDLERCROPIMAGE_H_ */
