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



#ifndef GLXVIEWIDS_H
#define GLXVIEWIDS_H

#define GLX_GRIDVIEW_ID          0x12121212
#define GLX_LISTVIEW_ID          0x12121213
#define GLX_FULLSCREENVIEW_ID    0x12121214
#define GLX_SLIDESHOWVIEW_ID     0x12121215
#define GLX_DETAILSVIEW_ID       0x12121216
#define GLX_SLIDESHOWSETTINGSVIEW_ID 0x12121217


typedef enum 
{
    NO_GRID_S,
    ALL_ITEM_S,
    ALBUM_ITEM_S,
} GridState;

typedef enum 
{
    NO_FULLSCREEN_S,
    EXTERNAL_S,
    IMAGEVIEWER_S,
	FETCHER_S
} FullScreenState;

typedef enum 
{
    NO_SLIDESHOW_S,
    SLIDESHOW_GRID_ITEM_S,
    SLIDESHOW_ALBUM_ITEM_S,
} SlideShowState;

#endif /* GLXVIEWIDS_H */
