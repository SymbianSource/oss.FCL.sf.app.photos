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
* Description:    Media List Command Handler
*
*/




#ifndef __C_GLXMEDIALISTCOMMANDHANDLER_H
#define __C_GLXMEDIALISTCOMMANDHANDLER_H

#include "glxcommandhandler.h"

#include <e32base.h>
#include <mpxcollectionobserver.h>
#include <mpxmediageneraldefs.h>
#include <mglxmedialistobserver.h>

class MGlxMediaList;
class CAknProgressDialog;
class MGlxMediaListProvider;
class CEikProgressInfo;
class UT_CGlxMediaListCommandHandler;
class t_cglxcommandhandlercopytohomenetwork;
    
/**
 * @class CGlxMediaListCommandHandler
 *  
 * Command handler that acts on a media list. This class handles basic
 * and most common command filtering, i.e., checking whether the 
 * command is active based on a selection
 *
 * @ingroup glx_group_command_handlers
 *
 * Filters:  
 *  - Disable if selection contains only system items (see @ref glx_colifspec_medial)
 *       - This allows disabling commands that are not allowed for system items (such as favourites album)
 *   - Disable if selection size is too small or too large
 *      - This allows filtering out an empty view (except for marking commands)
 *      - This allows filtering out static items (except for marking commands)
 *      - This allows filtering when other than 1 item has been selected
 *         - Use case: rename item, and play video
 *  - Disable for static item 
 *      - This can be achieved by setting minimum selection lenght to 1 (or more), since
 *        static items are not part of selection
 *      - Specific static item disabling should only be required when dealing with marking commands, 
 *        since static items do not belong to the selection. There is a selection,
 *        all commands (except marking commands) act on the selected items. If nothing
 *        is selected, focus is treated as selection. If nothing is selected, and 
 *        a static item is focused, selection size will be 0.
 *        Hence, it is faster and simpler to use selection size min and max to filter out 
 *        static items.
 *  - Disabling when view is empty can be achieved via min and max selection length, except 
 *    for marking commands, which need to if view is empty: 
 *      - (Mark all and Mark multiple need to be available also when there is no selection 
 *        zero and focus is on a static item, but not if the view is empty, hence a 
 *        separate check for empty view is needed for marking.)
 *  - Disable if system item 
 *  - Disable by item type: Image/video/static item/album/tag
 *     - Use case: Filter out static items for marking command handler
 *        
 * @lib glxviewbase.lib
 */
class CGlxMediaListCommandHandler : public CGlxCommandHandler
    {
public: 
    /**
     * Command info. Represents one command id. 
     * Command is enabled by default, unless member variables are modified
     */
    struct TCommandInfo 
        {
        /**
         * Constructor that sets all filters not to apply, so
         * command is enabled by default
         */
        IMPORT_C TCommandInfo(TInt aCommandId);

        /// Supported command id
        TInt iCommandId;
           
        /// Stop GIF animation before execution
        TBool iStopAnimationForExecution;
        
        /// Disable command if only system items in selection
        TBool iDisallowSystemItems;

        /// Disable command if only DRM-protected items in selection
        TBool iDisallowDRM;

        /// Disable command if selection smaller than. Default 0.
        TInt iMinSelectionLength;
        /// Disable command if selection larger than. Default KMaxTInt.
        TInt iMaxSelectionLength;
        
        /// Enable only if selected container(s) contains this or more items
        TInt iMinSlideshowPlayableContainedItemCount;
        
        /// Filter out animated GIFs
        TBool iDisallowAnimatedGIFs;
        
        /// Disable if <b>focused</b> item is / is not of the category 
        /// See @ref CGlxMediaListCommandHandler::iCategoryRule
        /// Default is EMPXNoCategory
        TMPXGeneralCategory iCategoryFilter;    
        
        /// If ETrue, 
        /// If EFalse, disables command if all items ARE of category
        /// Default
        /// See @ref CGlxMediaListCommandHandler::iCategoryFilter
        enum TCategoryRule 
            {
            EIgnore,       // Ignore category
            ERequireAll,   // disables command if all items are NOT of category
            EForbidAll     // disables command if all items ARE of category
            };
        TCategoryRule iCategoryRule; 
        
        /// Viewing Statws. Used to filter what commands are on display
        /// relative to Browsing or Viewing
        /// These are bitwise to enable a command to declare more than
        /// one state at a time.
        enum TViewingState 
            {
            EViewingStateUndefined = 0xFFFFFFFF,
            EViewingStateBrowse = 0x01,             // Tile View
            EViewingStateView = 0x02                // Full Screen view
            };
        
        // Records the current viewing state    
        TViewingState iViewingState;
        };

public:    
    /**
     * Destructor
     */
    IMPORT_C virtual ~CGlxMediaListCommandHandler();

    /**
     * Constructor
     * @param aMediaListProvider The owner of the media list to use
     */
    IMPORT_C CGlxMediaListCommandHandler( MGlxMediaListProvider* aMediaListProvider, 
    											TBool aIsToolbarCommand = EFalse );
    
public: // From MGlxCommandHandler
    /// See @ref MGlxCommandHandler::ExecuteL */
    IMPORT_C virtual TBool ExecuteL(TInt aCommand);

    /// See @ref MGlxCommandHandler::DynInitMenuPaneL */
    IMPORT_C virtual void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane,
                                            TBool aIsBrowseMode);
    
    /**
     * See @ref MGlxCommandHandler::Deactivate 
     * Default implementation does nothing
     */
    IMPORT_C virtual void Deactivate();
    
    /**
     * See @ref MGlxCommandHandler::OfferKeyEventL 
     * Default implementation does nothing
     */
    IMPORT_C virtual TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);

    /**
     * See @ref MGlxCommandHandler::PreDynInitMenuPaneL 
     * Default implementation does nothing
     * @param aResourceId Menu resource id.
     */
	IMPORT_C virtual void PreDynInitMenuPaneL( TInt aResourceId );
   	
    /**
	 * See @ref MGlxCommandHandler::GetRequiredAttributesL
     */
    IMPORT_C void GetRequiredAttributesL(RArray< TMPXAttribute >& aAttributes, 
    		                              TBool aFilterUsingSelection,
    		                              TBool aFilterUsingCommandId,
    		                              TInt aCommandId = 0) const;
    
protected: 
    /**
     * Execute the command, if applicable. Called after basic filtering
     * @param aCommand The command to handle
     * @return ETrue iff command was handled
     * @see BypassFiltersForExecute
     */
    virtual TBool DoExecuteL(TInt aCommandId, MGlxMediaList& aList) = 0;
    
    /**
     * See @ref CGlxCommandHandler::DoActivateL 
     * Default implementation does nothing
     */
    IMPORT_C virtual void DoActivateL( TInt aViewId );
    
    /**
     * Check if command is disabled. Allows deriving class to provide
     * more elaborate filtering that supported by this class.
     * Default implementation returns EFalse.
     * @return ETrue if command is disabled
     */
    IMPORT_C virtual TBool DoIsDisabled(TInt aCommandId, MGlxMediaList& aList) const;

    /**
     * Allow deriving classes to make any further alterations to the menu pane.
     * Only called on classes with at least on enabled command present in menu
	   * @param aResourceId The resource ID of the menu
	   * @param aMenuPane The in-memory representation of the menu pane
	   * @see BypassFiltersForMenu
	   */
    IMPORT_C virtual void DoDynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);

    /**
     * If deriving class returns true for this, ALL commands will be sent to DoExecuteL
     * @return Whether to bypass filters for command execution
     */
    IMPORT_C virtual TBool BypassFiltersForExecute() const;

    /**
     * If deriving class returns true for this, ALL menus will be passed to DoDynInitMenuPaneL
     * @return Whether to bypass filters for menu initialization
     */
    IMPORT_C virtual TBool BypassFiltersForMenu() const;
    
    /**
     * Optionally implemented by deriving classes to specify attributes that should be retrieved
     * @param aAttributes array to append attributes to.
     * @param aFilterUsingSelection If ETrue, the deriving class should only append
     *        attributes relevant to the current selection, If EFalse all attributes
     *        should be appended.
     */
    IMPORT_C virtual void DoGetRequiredAttributesL( RArray<TMPXAttribute>& aAttributes, TBool aFilterUsingSelection) const;

protected:
    /**
     * Returns a reference to the current media list
     * @return reference to the current media list
     */
    IMPORT_C MGlxMediaList& MediaList();

    /**
     * Returns a const reference to the current media list
     * @return const reference to the current media list
     */
    IMPORT_C const MGlxMediaList& MediaList() const;
    /**
     * AddCommandL
     */
    IMPORT_C void AddCommandL(const TCommandInfo& aCommand);

    /**
      * Check if the command handler is currently disabled
      */
    IMPORT_C TBool IsDisabledL(TInt aCommandId, MGlxMediaList& aList) const;

    /** 
     * @return ETrue if command is handled by this command handler
     */ 
    IMPORT_C TBool IsSupported(TInt aCommandId) const;

    /**
     * @return the length of selection
     *         If static item is focused, length is 0
     *         If list is empty, lenght is 0
     *         If no items are selected, but a non-static item is focused length is 1
     */
    IMPORT_C TInt SelectionLength() const;
    
    /**
     * @return command info by index
     */
    IMPORT_C TCommandInfo& CommandInfo(TInt aIndex);
    
    /**
     * @return the current viewing state.
     */

    IMPORT_C TCommandInfo::TViewingState ViewingState() const;

    /**
     * @return command info by index
     */
    const TCommandInfo& CommandInfo(TInt aIndex) const;
   	
private:
    /**
     * @return index of command in command info array or KErrNotFound
     */
    TInt CommandInfoIndex(TInt aCommandId) const;

    /**
     * Check if a given command (menu option) is viewable in the current view
     * @param aInfo The command in question.
     * @return ETrue if viewable, EFalse if not.
     */
        
    TBool IsViewable(const TCommandInfo& aInfo) const;
    
    /**
     * Checks to see if the command handler is disabled
     * based on the selection and IsViewable(). Other
     * conditions may also disable the command handler.
     * @param aInfo The command in question.
     * @return ETrue, if the command handler is disabled
     * EFalse if the command handler may not be disabled.
     */
    TBool IsDisabled(const TCommandInfo& aInfo) const;
    
    /**
	 * Append required attributes to aAttributes
	 * @param aAttributes array to append attributes to.
	 * @param aFilterUsingSelection If ETrue, the deriving class should only append
	 *        attributes relevant to the current selection, If EFalse all attributes
	 *        should be appended.
	 */
    void GetRequiredAttributesL(RArray< TMPXAttribute >& aAttributes, 
                                     TBool aFilterUsingSelection) const;
    
	/**
     * Check if the command handler is currently disabled in browse mode
  	 * @param aCommandId command ID of the command.
	 * @param aMediaList Medialist instance to check the counts
 	 * @param aIsContextItem ETrue if the current item is an context menu item.
     */
    TBool CGlxMediaListCommandHandler::CheckDisabledForBrowseModeL(
													TInt aCommandId, 
										            MGlxMediaList& aMediaList,
													TBool aIsContextItem);
    
private:

    /** Provider of media list */
    MGlxMediaListProvider* iMediaListProvider;
    
    /** Command info array */
    RArray<TCommandInfo> iCommandInfoArray;
    
    /** Current viewing state */
    TCommandInfo::TViewingState iCurrentViewingState;
    
    friend class UT_CGlxMediaListCommandHandler;
    friend class t_cglxcommandhandlercopytohomenetwork;
    };
    
#endif // __C_GLXMEDIALISTCOMMANDHANDLER_H
