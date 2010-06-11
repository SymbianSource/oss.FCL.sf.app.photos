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
*  Description:   This is custom widget for details view in photos for displaying the description of image .
*
*/
 


#include "glxdetailsdescriptionedit.h"
#include <hbeditorinterface>
#include <QEvent>



void  GlxDetailsDescriptionEdit::focusInEvent (QFocusEvent *event)
    {
    if(event->reason() == Qt::MouseFocusReason )
        {
          event->setAccepted(TRUE);
          emit labelPressed();
        }
    
    }
void  GlxDetailsDescriptionEdit::focusOutEvent (QFocusEvent *event) 
    {
     event->setAccepted(TRUE);    
    }


//--------------------------------------------------------------------------------------------------------------------------------------------
//GlxDetailsDescLabel
//--------------------------------------------------------------------------------------------------------------------------------------------
GlxDetailsDescriptionEdit::GlxDetailsDescriptionEdit(QGraphicsItem *parent) : HbTextEdit( parent )
{
   setAlignment(Qt::AlignLeft);
   mDesc = new HbEditorInterface(this);
   mDesc->setInputConstraints (HbEditorConstraintIgnoreFocus);
}

//--------------------------------------------------------------------------------------------------------------------------------------------
//~GlxDetailsDescLabel
//--------------------------------------------------------------------------------------------------------------------------------------------
GlxDetailsDescriptionEdit::~GlxDetailsDescriptionEdit()
{
  delete mDesc;
   mDesc = NULL;
}

//--------------------------------------------------------------------------------------------------------------------------------------------
//setItemText
//--------------------------------------------------------------------------------------------------------------------------------------------
void GlxDetailsDescriptionEdit::setItemText( const QString &text )
{
    setPlainText( text );
}
