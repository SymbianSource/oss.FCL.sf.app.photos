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

#ifndef GLXBACKWARDTRANSITIONPLUGIN_H
#define GLXBACKWARDTRANSITIONPLUGIN_H

#include "glxeffectpluginbase.h"

class GlxBackwardTransitionPlugin : public GlxEffectPluginBase
{
public :
    GlxBackwardTransitionPlugin();
    ~GlxBackwardTransitionPlugin();
    QList <QString > getEffectFileList() { return mEffectFileList ; }
	
private :
    QList <QString > mEffectFileList;
};

#endif /*GLXBACKWARDTRANSITIONPLUGIN_H*/

