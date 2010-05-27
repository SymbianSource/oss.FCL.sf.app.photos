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


#ifndef GLXFORWARDTRANSITIONPLUGIN_H
#define GLXFORWARDTRANSITIONPLUGIN_H

#include "glxeffectpluginbase.h"

class GlxForwardTransitionPlugin : public GlxEffectPluginBase
{
public :
    GlxForwardTransitionPlugin();
    ~GlxForwardTransitionPlugin();
    QList <QString > getEffectFileList() { return mEffectFileList; }
    
    /*
     * setup the item postion and set the mItem value
     */
        void setUpItems( QList< QGraphicsItem * > &  items );
    /*
     * second animation will be run later
     */    
    bool isAnimationLater(int index) ;
    
    QGraphicsItem * animationItem() 
    { 
        mItem->show();
        return mItem ; 
    }
	
private :
    QList <QString > mEffectFileList;
    QGraphicsItem *mItem;
};

#endif /*GLXFORWARDTRANSITIONPLUGIN_H*/

