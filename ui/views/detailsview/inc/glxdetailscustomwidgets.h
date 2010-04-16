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

#ifndef GLXDETAILSCUSTOMWIDGETS_H
#define GLXDETAILSCUSTOMWIDGETS_H

#include <hbdataformmodelitem.h>
#include <hbdataformviewitem.h>


class HbIconItem;
class HbLabel;
class HbDataFormModelItem;
enum 
    {
     ImageNameItem = HbDataFormModelItem::CustomItemEnd +1,
     DateLabelItem,
     LocationTagItem,
     DurationItem,
     ShareWithItem,
     FaceTagItem,
     CommentsItem
     
    };

class GlxDetailsCustomWidgets: public HbDataFormViewItem
{
    Q_OBJECT

public:
    GlxDetailsCustomWidgets(QGraphicsItem *parent);
    ~GlxDetailsCustomWidgets();
     virtual HbAbstractViewItem* createItem();     
     bool canSetModelIndex(const QModelIndex &index) const;

protected:
    virtual HbWidget* createCustomWidget();

private:
    HbLabel *mLabel;
};

#endif // GLXDETAILSCUSTOMWIDGETS_H
