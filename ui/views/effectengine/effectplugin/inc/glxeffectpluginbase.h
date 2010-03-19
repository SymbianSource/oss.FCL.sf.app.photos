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

#ifndef GLXEFFECTPLUGINBASE_H
#define GLXEFFECTPLUGINBASE_H

#include <QList>
#include <QString>
#include <QGraphicsItem>

class GlxEffectPluginBase
{

public :
    GlxEffectPluginBase() {  }
    virtual ~GlxEffectPluginBase() {}
    virtual QList <QString > getEffectFileList() = 0;
    virtual void setUpItems( QList< QGraphicsItem * > &  items ) { Q_UNUSED( items ) }
/*
 * second animation will run same time(false) or later (true)
 */    
    virtual bool isAnimationLater(int index) 
    { 
        Q_UNUSED( index )
        return false ; 
    }
    virtual QString ItemType() { return QString("HbIconItem") ; }
    virtual QGraphicsItem * animationItem() { return NULL; }
};

#endif /*GLXEFFECTPLUGINBASE_H*/