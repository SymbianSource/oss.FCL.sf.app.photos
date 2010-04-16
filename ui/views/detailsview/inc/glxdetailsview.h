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
 
#ifndef GLXDETAILSVIEW_H
#define GLXDETAILSVIEW_H

#include <glxview.h>

class HbDataForm;
class HbDataFormModelItem;
class HbDataFormModel;
class GlxMediaModel;
class QAbstractItemModel;
class HbIconItem;
class GlxDetailsCustomWidgets;

class GlxDetailsView : public GlxView  
{

    Q_OBJECT

public:
    /*
     * class constructor.
     */
    GlxDetailsView(HbMainWindow *window);
    /*
     * class Destructor.
     */
    ~GlxDetailsView();

    /*
     * This is called from the view manager when the view is going to be activated.
     */
    void activate() ;

    /*
     * This is called from the view manager when the view is going to be de-activated.
     */
    void deActivate();

    /*
     * This is called from the view manager before the view is going to Activated.
     */
    void initializeView(QAbstractItemModel *model);

    /*
     * This is called from the view manager before the view is going to de-activated.
     */
    void resetView();

    /*
     * Sets the model corresponding to this view.
     */
    void setModel (QAbstractItemModel *model);
/*
 * return the view item for animation
 */    
    QGraphicsItem * getAnimationItem(GlxEffect transtionEffect);


public slots:
    void updateLayout(Qt::Orientation);
    void rowsRemoved(const QModelIndex &parent, int start, int end);

private:

   //Adds the Widget required to show the details
    void addWidgets();

    //Fills the data in the form
    void setFormData();

    /*
     * Retrieves the image from the model and shows the image
     */
    void showImage();

    /*
     * Sets the Date to the label recieved from MDS
     */
    void setImageName();

    /*
     * Sets the Date to the label received from MDS
     */
    void setDate();
    
    /*
     * Add all the model releted connection and data
     */
    void initializeNewModel();
    /*
     * clear all the model releted connection and data
     */
    void clearCurrentModel();


private:
    HbIconItem *mDetailsIcon;

    //The media Model to acess the attributes not owned
    QAbstractItemModel *mModel; 

    //The reference to the Main Window
    HbMainWindow *mWindow;

    //The data form for the detail 
    HbDataForm * mDataForm;

    //The Model for the Form
    HbDataFormModel *mDetailModel;

    //Custom widgets
    GlxDetailsCustomWidgets *mCustomPrototype;
    
    int mSelIndex;

};
#endif //GLXDETAILSVIEW_H
