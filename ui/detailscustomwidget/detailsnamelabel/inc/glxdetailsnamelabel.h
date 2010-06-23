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

#ifndef GlXDETAILSNAMELABEL_H
#define GlXDETAILSNAMELABEL_H

#include <hblabel.h>

#ifdef BUILD_NAMELABEL
#define MY_EXPORT Q_DECL_EXPORT
#else
#define MY_EXPORT Q_DECL_IMPORT
#endif


class MY_EXPORT GlxDetailsNameLabel : public HbLabel 
{	
    Q_OBJECT
    
public:
    GlxDetailsNameLabel(QGraphicsItem *parent = NULL);
    ~GlxDetailsNameLabel();    	
    void setItemText( const QString &text );
		
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);	
    void mouseReleaseEvent (QGraphicsSceneMouseEvent *event);
    
signals :
     void labelPressed();
     
};

#endif // GlXDETAILSNAMELABEL_H
