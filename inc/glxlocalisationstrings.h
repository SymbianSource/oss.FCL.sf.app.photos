/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Localisation strings
*
*/

#ifndef GLXLOCALISATIONSTRINGS_H_
#define GLXLOCALISATIONSTRINGS_H_


//OPTIONS MENU
//Options list item
#define GLX_OPTION_SHARE               hbTrId("txt_photos_opt_share") 

//Options list item. Opens submenu where slide show can be played or settings edited or viewed.
#define GLX_OPTION_SLIDESHOW           hbTrId("txt_photos_opt_slideshow")
//sub menu options for slideshow play

#define GLX_OPTION_SS_PLAY             hbTrId("txt_photos_opt_sub_play")

//sub menu options for slideshow
#define GLX_OPTION_SS_SETTINGS         hbTrId("txt_photos_opt_sub_settings")

//
#define GLX_OPTION_ADD_TO_ALBUM        hbTrId("txt_photos_opt_add_to_album")

//
#define GLX_OPTION_SETTINGS            hbTrId("txt_common_opt_settings")

//Options list item. Note! Only use this text ID if there are no icons.
//Erases something completely from the phone, list, or memory card, 
//for example. Delete should only be used when something in removed permanently.                                            
#define GLX_OPTION_DELETE              hbTrId("txt_common_opt_delete")

//Options list item. Marks all the items in the view for editing, transferring or removing.
#define GLX_OPTION_MARK_ALL            hbTrId("txt_photos_opt_mark_all")

//Options list item. Unmarks all the items in the view.
#define GLX_OPTION_UN_MARK_ALL         hbTrId("txt_photos_opt_unmark_all")

//Options list item. Removes selected item(s) from the folder.
#define GLX_OPTION_REMOVE_FROM_ALBUM   hbTrId("txt_photos_opt_remove_from_album")

//
#define GLX_OPTION_NEW_ALBUM           hbTrId("txt_photos_opt_new_album")

//CONTEXT MENU

//context menu item share , invoke share UI
#define GLX_MENU_SHARE                 hbTrId("txt_photos_menu_share")

//context menu item to play slideshow
#define GLX_MENU_SLIDESHOW             hbTrId("txt_photos_menu_slideshow")

//context menu item for adding current image to album(s)
#define GLX_MENU_ADD_TO_ALBUM          hbTrId("txt_photos_menu_add_to_album")

//Item specific menu item. Note! Only use this text ID if there are no icons. 
//Erases something completely from the phone, list, or memory card, for example. 
//Delete should only be used when something in removed permanently.
#define GLX_MENU_DELETE                hbTrId("txt_common_menu_delete")

//
#define GLX_MENU_RENAME                hbTrId("txt_common_menu_rename_item")


//TITLE RELATED and DEFAULT STRINGS

#define GLX_CAPTION                    hbTrId("txt_short_caption_photos")
#define GLX_TITLE                      hbTrId("txt_photos_title_photos")
#define GLX_CAMERA                     "txt_photos_dblist_my_camera"
#define GLX_MYFAV                      "txt_photos_dblist_my_favorites"
#define GLX_PLACES                     "txt_photos_dblist_places_ln"
#define GLX_ITEMS_CAMERA               "txt_photos_dblist_val_ln_items"
#define GLX_ITEMS_MYFAV                "txt_photos_subtitle_ln_items"
#define GLX_SUBTITLE_CAM_GRIDVIEW      hbTrId("txt_photos_subtitle_my_camera")
#define GLX_SUBTITLE_MYFAV_GRIDVIEW    hbTrId("txt_photos_subtitle_my_favorites")
#define GLX_SUBTITLE_NOITEMS           hbTrId("txt_photos_subtitle_no_image_or_videos_to_display")


//VIEW RELATED STRINGS AND COMMON DIALOGS

//comments lable for photos flip view
#define GLX_DETAILS_DESCRIPTION       hbTrId("txt_photos_formlabel_description")

//Button. Note! Use this text ID only if there are max. two buttons in the dialog. 
//When this button is pressed, the focused item or operation will be selected or accepted.
#define GLX_BUTTON_OK                 hbTrId("txt_common_button_ok") 

//Button. Note! Use this text ID only if there are max. two buttons in the dialog. 
//Allows the user to cancel the action or process.
#define GLX_BUTTON_CANCEL            hbTrId("txt_common_button_cancel")

#define GLX_BUTTON_SELECT            hbTrId("txt_common_button_select")

#define GLX_MENU_OPEN                hbTrId("txt_common_menu_open")

#define GLX_BUTTON_HIDE              hbTrId("txt_common_button_hide")

#endif /* GLXLOCALISATIONSTRINGS_H_ */
