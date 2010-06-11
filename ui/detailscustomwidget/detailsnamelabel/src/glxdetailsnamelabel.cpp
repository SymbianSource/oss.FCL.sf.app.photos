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


#include <hblabel.h>
#include "glxdetailsnamelabel.h"
#include <QEvent>
#include <QDebug>

//--------------------------------------------------------------------------------------------------------------------------------------------
//mousePressEvent
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsNameLabel::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event)
}

//--------------------------------------------------------------------------------------------------------------------------------------------
//mouseReleaseEvent
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsNameLabel::mouseReleaseEvent (QGraphicsSceneMouseEvent *event) 
{
    qDebug("GlxDetailsNameLabel::mouseReleaseEvent");
    Q_UNUSED(event)  
    emit labelPressed();
}

//--------------------------------------------------------------------------------------------------------------------------------------------
//GlxDetailsNameLabel
//--------------------------------------------------------------------------------------------------------------------------------------------
GlxDetailsNameLabel::GlxDetailsNameLabel(QGraphicsItem *parent) : HbLabel( parent )
{
    qDebug("GlxDetailsNameLabel::GlxDetailsNameLabel");
     setTextWrapping(Hb::TextWordWrap);
     setAlignment(Qt::AlignHCenter);
}

//--------------------------------------------------------------------------------------------------------------------------------------------
//~GlxDetailsCustomLabel
//--------------------------------------------------------------------------------------------------------------------------------------------
GlxDetailsNameLabel::~GlxDetailsNameLabel()
{

}

//--------------------------------------------------------------------------------------------------------------------------------------------
//setItemText
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsNameLabel::setItemText( const QString &text )
{
    qDebug("GlxDetailsNameLabel::setItemText");
    setHtml(text);
}
