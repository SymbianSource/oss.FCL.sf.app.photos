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



#ifndef GLXMODELROLES_H
#define GLXMODELROLES_H
enum
{
    GlxHgVisibilityRole = Qt::UserRole + 1, // VisibilityRole For MediaWall ,  Dont Change This Value - Depedency To HgWidget
    GlxFsImageRole,                         //to get the fullscreen image
    GlxContextRole,                     //to get and set the context   
    GlxFocusIndexRole,                  //to get and set the selected item index
    GlxVisualWindowIndex,               //to set the visual window index in media list
    GlxUriRole,                         //to get the URI of the image
    GlxDimensionsRole,                  
    GlxSelectedIndexRole,               //to get and set the seleceted item list
    GlxDateRole,                        //to get the image created or modified date 
    GlxFrameCount,                      //to get the number of frame in an image
    GlxSubStateRole,                    //to publish the sub state of a SM states
    GlxHdmiBitmap,                      //to get the pre decoded thumbnail for HDMI
	GlxListItemCount,                   //to get the number of item in the album list
	GlxSystemItemRole,                   //to get the item is system item or not
    GlxFavorites,                       //to get the image is in favorites folder or not
    GlxQImageSmall,                     //To Get Grid Thumbnail as QImage
    GlxQImageLarge,                     //To Get FullScreen Thumbnail as QImage
    GlxDefaultImage,					//To Get Default Thumbnail 
    GlxViewTitle,
    GlxPopulated,
	GlxTimeRole,                        //to get the time image was modified
	GlxSizeRole,                         //to get the size of the image
	GlxDescRole,                         //to get the description of the images
	GlxRemoveContextRole,                 //to remove the context
	GlxTempVisualWindowIndex,              //to store the visual index obtained from AM temporarily
	GlxImageCorruptRole,                 //To get the corrupt image status
	GlxPaintPageFlag,
	GlxImageTitle                        //to fetch the title of the image
};


#endif 
