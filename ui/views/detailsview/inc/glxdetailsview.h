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
class GlxDetailsCustomIcon;
class GlxFavMediaModel;


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
    void dataChanged(QModelIndex startIndex, QModelIndex endIndex);
    void updateFavourites();
    
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
    
    /*
     * cleans up the resources used.
     */
    void cleanUp();
    
    /*
     * create the favourite model
     */
    void setFavModel();


private:
    
    //Contains the thumbnail shown in teh details view.
    HbIconItem *mDetailsIcon;
    
    //Contains the favourite icon which adds or removes the image to favourite folder
    GlxDetailsCustomIcon *mFavIcon;
    
    //The media Model to acess the attributes; not owned,dont delete.
    QAbstractItemModel *mModel; 
    
    //The media Model to acess the attributes whether image is in favourite folder or not;owned by this view
    GlxFavMediaModel *mFavModel;
    
    //The Model for the Form
    HbDataFormModel *mDetailModel;

    //The reference to the Main Window
    HbMainWindow *mWindow;

    //The data form for the detail 
    HbDataForm * mDataForm;

    //Custom widgets which will contain the widgets not created/supported by data form 
    GlxDetailsCustomWidgets *mCustomPrototype;
    
    int mSelIndex;
    
    //Contains the Datelabel item
    HbDataFormModelItem *mDateLabelItem;
    
    //Contains the ImageLabel item
    HbDataFormModelItem *mImageLabelitem;
    
    //Contains the Comments item
    HbDataFormModelItem *mCommentsLabelitem;
};
#endif //GLXDETAILSVIEW_H
