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


#include "glxdetailsnamelabel.h"
#include <QtPlugin>
#include <hbdocumentloaderplugin.h>
#include <QDebug>

class GlxDetailsNameLabelPlugin : public HbDocumentLoaderPlugin
{
public:
    QObject *createObject(const QString& type, const QString &name);
    QList<const QMetaObject *> supportedObjects();  
};
Q_EXPORT_PLUGIN(GlxDetailsNameLabelPlugin)

QObject *GlxDetailsNameLabelPlugin::createObject(const QString& type, const QString &name )
{
    qDebug("GlxDetailsNameLabelPlugin::createObject");
    if( type == GlxDetailsNameLabel::staticMetaObject.className() ) {
    
        qDebug("GlxDetailsNameLabelPlugin::createObject created");
        QObject *object = new GlxDetailsNameLabel();
        object->setObjectName(name);
        return object;    
    }
    return 0;
}

QList<const QMetaObject *> GlxDetailsNameLabelPlugin::supportedObjects()
{
    qDebug("GlxDetailsNameLabelPlugin::supportedObjects");
    QList<const QMetaObject *> result;
    result.append( &GlxDetailsNameLabel::staticMetaObject );
    return result;
}

// end of file 

