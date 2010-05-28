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


#ifndef GLXFLIPEFFECTPLUGIN_H
#define GLXFLIPEFFECTPLUGIN_H

#include "glxeffectpluginbase.h"

class GlxFlipEffectPlugin : public GlxEffectPluginBase
{
public :
    /*
     *  Constructor
     */
    GlxFlipEffectPlugin();
    
    /*
     *  Destructor
     */
    ~GlxFlipEffectPlugin();
    
    /*
     *  Get the flip hide and Flip Show file list
     */
    QList <QString > effectFileList() { return mEffectFileList; }
    
    /*
     * setup the item postion and set the mItem value
     */
    void setUpItems( QList< QGraphicsItem * > &  items );
    
    /*
     * second animation will be run later
     */    
    bool isAnimationLater(int index) ;
    
    /*
     *  return the second animation object to play the flip show animation
     */
    QGraphicsItem * animationItem() ;
    
    /*
     * get the name of the effect, use to shown in the slide show setting view
     */
    static QString effectName() { return QString ("Flip") ; }
	
private :
    QList <QString > mEffectFileList;
    QGraphicsItem *mItem;
};

#endif /*GLXFLIPEFFECTPLUGIN_H*/

