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

#include <mpxcollectionpath.h>
#include <mglxmedialist.h>
#include <glxcommandfactory.h>
#include <glxcommandhandlerdelete.h>
#include <QDebug>

GlxCommandHandlerDelete::GlxCommandHandlerDelete()
{
    qDebug("GlxCommandHandlerDelete::GlxCommandHandlerDelete() ");
}

GlxCommandHandlerDelete::~GlxCommandHandlerDelete()
{
    qDebug("GlxCommandHandlerDelete::~GlxCommandHandlerDelete() ");
}

CMPXCommand* GlxCommandHandlerDelete::CreateCommandL(TInt aCommandId, MGlxMediaList& aMediaList, TBool& aConsume) const 
{
	Q_UNUSED(aCommandId);
	Q_UNUSED(aConsume);
    qDebug("GlxCommandHandlerDelete::CreateCommandL");
    CMPXCollectionPath* path = aMediaList.PathLC( NGlxListDefs::EPathFocusOrSelection );
    CMPXCommand* command = TGlxCommandFactory::DeleteCommandLC(*path);
    CleanupStack::Pop(command);
    CleanupStack::PopAndDestroy(path);
    return command;
}
