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

#ifndef GLXDETAILSCUSTOMICON_H
#define GLXDETAILSCUSTOMICON_H

#include <hbwidget.h>
class HbIconItem;
class HbIcon;

class GlxDetailsCustomIcon : public HbWidget 
{	
    Q_OBJECT
    
public:
    GlxDetailsCustomIcon(QGraphicsItem *parent);
    ~GlxDetailsCustomIcon();
    /*
     * Sets the Geometry of the Custom Widget
     */
    void setItemGeometry(QRect screenRect);
    
    /*
     * Sets the Size of the Favourite Icon
     */
	void setItemSize(const QSizeF &size);
	
	/*
	 * Sets the Position of the Favourite Icon
	 */
	void setItemPos(qreal ax, qreal ay);
	
	/*
	 * Sets the  Icon of Favourite IconItem.
	 */
	void setItemIcon(const HbIcon &icon);
	
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);	
    void mouseReleaseEvent (QGraphicsSceneMouseEvent *event);
    
signals :
     void updateFavourites();

private:
   HbIconItem *mFavIcon;   
};

#endif // GLXDETAILSCUSTOMICON_H
