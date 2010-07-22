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
*  Description:  ??
*
*/
 

#include "glxdetailsdescriptionedit.h"
#include <QtPlugin>
#include <hbdocumentloaderplugin.h>

class GlxDetailsDescriptionEditPlugin : public HbDocumentLoaderPlugin
{
public:
    QObject *createObject(const QString& type, const QString &name);
    QList<const QMetaObject *> supportedObjects();  
};
Q_EXPORT_PLUGIN(GlxDetailsDescriptionEditPlugin)

QObject *GlxDetailsDescriptionEditPlugin::createObject(const QString& type, const QString &name )
{
    if( type == GlxDetailsDescriptionEdit::staticMetaObject.className() ) {
        QObject *object = new GlxDetailsDescriptionEdit();
        object->setObjectName(name);
        return object;    
    }
    return 0;
}

QList<const QMetaObject *> GlxDetailsDescriptionEditPlugin::supportedObjects()
{
    QList<const QMetaObject *> result;
    result.append( &GlxDetailsDescriptionEdit::staticMetaObject );
    return result;
}

// end of file 

