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
* Description:    This class browses file system
*
*/




// INCLUDE FILES

#include "glxcollectionpluginmonths.h"

#include <e32cmn.h>
#include <mpxcollectionpath.h>
#include <mpxcollectionpluginobserver.h>
#include <mpxcmn.h>
#include <mpxmediageneraldefs.h>
#include <mpxmediacontainerdefs.h>
#include <mpxmedia.h>
#include <mpxmediaarray.h>
#include <glxmediacollectioninternaldefs.h>
#include <glxmediageneraldefs.h>
#include <glxcollectiongeneraldefs.h>
#include <glxpluginmonths.rsg>
#include <StringLoader.h>
#include <glxpanic.h>
#include <mglxdatasource.h>
#include <glxrequest.h>
#include <glxidlistrequest.h>
#include <glxlog.h>
#include <glxtracer.h>
#include <AknUtils.h>

#include "glxcollectionpluginmonths.hrh"
/**
 * @internal reviewed 14/06/2007 by Alex Birkett
 */
// CONSTANTS

// Constants introduced to solve Codescanner warnings
// about magic number usage
const TInt KYearBufferSize = 8;
const TInt KDateBufferPaddingMin = 10;
const TInt KDateBufferPaddingMax = 20;
const TInt KDateFormat1 = 1;

#ifdef SHOW_MONTHS_START_END_SUB_TITLE 
const TInt KDateFormat2 = 2;
const TInt KDateFormat3 = 3;
#endif

_LIT(KResourceFile, "z:glxpluginmonths.rsc");

// ============================ LOCAL FUNCTIONS ==============================
    
// ============================ MEMBER FUNCTIONS ==============================
// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CGlxCollectionPluginMonths* CGlxCollectionPluginMonths::NewL(TAny* aObs)
    {
    CGlxCollectionPluginMonths* self = new (ELeave) CGlxCollectionPluginMonths(
                            static_cast<MMPXCollectionPluginObserver*>(aObs));
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
    }

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//
CGlxCollectionPluginMonths::~CGlxCollectionPluginMonths()
    {
    }

// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
//
CGlxCollectionPluginMonths::CGlxCollectionPluginMonths(
    MMPXCollectionPluginObserver* aObs)
    {
    iObs = aObs;
    }

// ----------------------------------------------------------------------------
// ConstructL
// ----------------------------------------------------------------------------
//
void CGlxCollectionPluginMonths::ConstructL()
    {
    iDataSource = MGlxDataSource::OpenDataSourceL(KGlxDefaultDataSourceUid, *this);
	}

 
// ----------------------------------------------------------------------------
// CpiAttributeAdditionalAttributes
// ----------------------------------------------------------------------------
//
void CGlxCollectionPluginMonths::CpiAttributeAdditionalAttributes(
	const TMPXAttribute& aCpiAttribute, RArray<TMPXAttribute>& aAttributeArray)
	{
    TRACER("CGlxCollectionPluginMonths::CpiAttributeAdditionalAttributes");
    // Only need to process KGlxMediaCollectionPluginSpecificSubTitle here as all the others are reading straight from resource files
    // KGlxMediaCollectionPluginSpecificSubTitle requires a usage count
	if (aCpiAttribute == KGlxMediaCollectionPluginSpecificSubTitle)
		{
		// need to add the usage count. but check first if it is already present
		TInt attrCount = aAttributeArray.Count();
		TBool found = EFalse;
#if defined(GLX_SUB_TITLE_REL8) || defined(SHOW_MONTHS_START_END_SUB_TITLE)		
		TBool startfound = EFalse;
		TBool endfound = EFalse;
#endif
		
#ifdef GLX_SUB_TITLE_REL8		

		TBool videofound = EFalse;
		TBool imagefound = EFalse;

		for ( TInt index = 0 ; index < attrCount ; index++)
			{
			if (aAttributeArray[index] == KMPXMediaGeneralCount)
				{
				found = ETrue;
				}
			else if ( aAttributeArray[index] == KGlxMediaCollectionInternalStartDate )
				{
				startfound = ETrue;
				}

			else if (aAttributeArray[index] == KGlxMediaItemTypeVideo)
				{
				videofound = ETrue;	            
				}
			else if(aAttributeArray[index] == KGlxMediaItemTypeImage)	            
				{
				imagefound = ETrue;
				}			    
			}

		if (!found)
			{
			aAttributeArray.Append(KGlxMediaCollectionInternalUsageCount);
			}
			
		if (!startfound)
			{
			aAttributeArray.Append(KGlxMediaCollectionInternalStartDate);
			}
		// end date will be returned when start date is requested.	        

		if (!videofound)
			{
			aAttributeArray.Append(KGlxMediaItemTypeVideo);
			}

		if (!imagefound)	        
			{
			aAttributeArray.Append(KGlxMediaItemTypeImage);
			}	
#else

		for ( TInt index = 0 ; index < attrCount ; index++)
	        {
	        if (aAttributeArray[index] == KMPXMediaGeneralCount)
	            {
	            found = ETrue;
	            }
#ifdef SHOW_MONTHS_START_END_SUB_TITLE			
	        else if ( aAttributeArray[index] == KGlxMediaCollectionInternalStartDate )
	            {
	            startfound = ETrue;
	            }
	        else if(aAttributeArray[index] == KGlxMediaCollectionInternalEndDate )
	        	{
	        	endfound = ETrue;
	        	}
#endif // #ifdef SHOW_MONTHS_START_END_SUB_TITLE				
	        }
	        
	    if (!found)
	        {
	        aAttributeArray.Append(KGlxMediaCollectionInternalUsageCount);
	        }
#ifdef SHOW_MONTHS_START_END_SUB_TITLE				
	    if (!startfound)
	        {
	        aAttributeArray.Append(KGlxMediaCollectionInternalStartDate);
	        }
	    if(!endfound)
	    	{
	    	aAttributeArray.Append(KGlxMediaCollectionInternalEndDate);
	        }
#endif // #ifdef SHOW_MONTHS_START_END_SUB_TITLE
#endif // #ifdef GLX_SUB_TITLE_REL8
	    // end date will be returned when start date is requested.			
		}
	}

// ----------------------------------------------------------------------------
// HandleCpiAttributeResponseL
// ----------------------------------------------------------------------------
// 
void CGlxCollectionPluginMonths::HandleCpiAttributeResponseL(CMPXMedia* aResponse,
		TArray<TMPXAttribute> aCpiAttributes, TArray<TGlxMediaId> aMediaIds)
    {
    TRACER("CGlxCollectionPluginMonths::HandleCpiAttributeResponseL");
    
    const TInt mediaIdCount = aMediaIds.Count();
    
    switch (mediaIdCount)
        {
    case 0:
        User::Leave(KErrNotSupported);
        break;
        
    case 1:
        HandleCpiAttributeResponseL(aResponse, aCpiAttributes, aMediaIds[0]);
        break;
        
        
    default:
            {
            // We have an array of CMPXMedia items
            
            if (TGlxMediaId(KGlxCollectionRootId) == aMediaIds[0])
                {
                User::Leave(KErrNotSupported);
                }
                
            CMPXMediaArray* mediaArray =
            	aResponse->ValueCObjectL<CMPXMediaArray>(KMPXMediaArrayContents);
            CleanupStack::PushL(mediaArray);

            const TInt arrayCount = mediaArray->Count();
            GLX_DEBUG2("CGlxCollectionPluginMonths::HandleCpiAttributeResponseL arrayCount=%d", arrayCount);    
            
            // Sanity check
            if (arrayCount != mediaIdCount)
                {
                User::Leave(KErrArgument);
                }
            
            for (TInt index = 0; index < arrayCount; index++)
                {
                HandleCpiAttributeResponseL((*mediaArray)[index],
                		aCpiAttributes, aMediaIds[index]);
                }

            aResponse->SetCObjectValueL(KMPXMediaArrayContents, mediaArray);
            CleanupStack::PopAndDestroy(mediaArray);
            }
        break;
        }
    }


// ----------------------------------------------------------------------------
// HandleCpiAttributeResponseL
// ----------------------------------------------------------------------------
// 
void CGlxCollectionPluginMonths::HandleCpiAttributeResponseL(CMPXMedia* aResponse,
		TArray<TMPXAttribute> aCpiAttributes, TGlxMediaId aMediaId)
    {
    TRACER("CGlxCollectionPluginMonths::HandleCpiAttributeResponseL");
    
	TInt count = aCpiAttributes.Count();
    GLX_DEBUG2("CGlxCollectionPluginMonths::HandleCpiAttributeResponseL count=%d", count);    
	
	TLanguage lang;
    lang = User::Language();
	
	for (TInt index = 0; index < count ; index++)
	    {
	    const TMPXAttribute attr = aCpiAttributes[index];
	    
	    if (attr == KGlxMediaCollectionPluginSpecificSubTitle)
			{
			GLX_LOG_INFO("Attribute : SubTitle");
#if defined(GLX_SUB_TITLE_REL8) || defined(SHOW_MONTHS_START_END_SUB_TITLE)				
			if (TGlxMediaId(KGlxCollectionRootId) == aMediaId)
				{
				GLX_LOG_INFO("Attribute : SubTitle-RootId");
#ifdef GLX_SUB_TITLE_REL8
					            
				//The usage Count is required for the subtitle in the main list view	                
				if (!aResponse->IsSupported(KGlxMediaCollectionInternalUsageCount))
					{
					User::Leave(KErrNotSupported);
					}
				else
					{
					TInt usageCount = aResponse->ValueTObjectL<TInt>(KGlxMediaCollectionInternalUsageCount);

					// Get the format string
					HBufC* tempTitle = NULL;

					if(0 == usageCount)	
						{	
						GLX_LOG_INFO("SubTitle:0 Items");
											
						tempTitle = LoadLocalizedStringLC(KResourceFile, R_MONTHS_SUB_TITLE_NO_IMAGE_NO_VIDEO);
						aResponse->SetTextValueL(attr, *tempTitle);
						}

					// Get the format string
					else if (1 == usageCount)
						{
						GLX_LOG_INFO("SubTitle:1 Item");
						
						tempTitle = LoadLocalizedStringLC(KResourceFile, R_MONTHS_SUB_TITLE_SINGLE_BY_MONTH);
						aResponse->SetTextValueL(attr, *tempTitle);
						}
					else 
						{
						GLX_LOG_INFO1("SubTitle: %d Items",usageCount);
						
						tempTitle = LoadLocalizedStringLC(KResourceFile, R_MONTHS_SUB_TITLE_MULTI_BY_MONTH);						
						TPtr formatString = tempTitle->Des();

						// Now create a buffer that will contain the result. needs to be length of format string plus a few extra for the number
						HBufC* title = HBufC::NewLC(formatString.Length() + KDateBufferPaddingMin);
						TPtr ptr = title->Des();
						StringLoader::Format(ptr, formatString, -1, usageCount);

						// Set the title in the response.
						aResponse->SetTextValueL(attr, *title);    
						CleanupStack::PopAndDestroy(title);						
						}
					CleanupStack::PopAndDestroy(tempTitle);
					}   
#else
			 	if (!aResponse->IsSupported(KGlxMediaCollectionInternalStartDate))
                    {
                    User::Leave(KErrNotSupported);
                    }
                if (!aResponse->IsSupported(KGlxMediaCollectionInternalEndDate))
                    {
                    User::Leave(KErrNotSupported);
                    }
	            TTime start = aResponse->ValueTObjectL<TInt64>(KGlxMediaCollectionInternalStartDate);
	            TTime end = aResponse->ValueTObjectL<TInt64>(KGlxMediaCollectionInternalEndDate);
	            HBufC* tempTitle = NULL;
                if( TTime(0) == start )
                    {
                    tempTitle = LoadLocalizedStringLC(KResourceFile, R_MONTHS_SUB_TITLE_NO_IMAGE_NO_VIDEO);
                    aResponse->SetTextValueL(attr, *tempTitle);
                    CleanupStack::PopAndDestroy(tempTitle);
                    continue;
                    }
                 _LIT(KGlxTempMonthYearTitleFormat, "%F%Y");
	            
                tempTitle = LoadLocalizedStringLC(KResourceFile, R_MONTHS_SUB_TITLE);
                TPtr formatString = tempTitle->Des();
                TBuf<KYearBufferSize> yearTitle;
                start.FormatL(yearTitle, KGlxTempMonthYearTitleFormat);

				// Ensure that the number conversion takes place only for Arabic
                if (lang == ELangArabic)
                	{
                	AknTextUtils::DisplayTextLanguageSpecificNumberConversion(yearTitle);
					}

                HBufC* monthTitle = GetMonthNameAsStringLC(
                        start.DateTime().Month(), EMonthNameAsSubtitle );
                TBuf<KYearBufferSize> yearTitle2;
                end.FormatL(yearTitle2, KGlxTempMonthYearTitleFormat);

				// Ensure that the number conversion takes place only for Arabic
				if (lang == ELangArabic)
					{
                	AknTextUtils::DisplayTextLanguageSpecificNumberConversion(yearTitle2);
					}

                HBufC* monthTitle2 = GetMonthNameAsStringLC(
                        end.DateTime().Month(), EMonthNameAsSubtitle );

                HBufC* title1 = HBufC::NewLC(formatString.Length() + KDateBufferPaddingMax);
                TPtr ptr1 = title1->Des();
                HBufC* title2 = HBufC::NewLC(formatString.Length() + KDateBufferPaddingMax);
                TPtr ptr2 = title2->Des();
                HBufC* title3 = HBufC::NewLC(formatString.Length() + KDateBufferPaddingMax);
                TPtr ptr3 = title3->Des();
                HBufC* title4 = HBufC::NewLC(formatString.Length() + KDateBufferPaddingMax);
                TPtr ptr4 = title4->Des();
                TPtr monthPtr = monthTitle->Des();
                TPtr monthPtr2 = monthTitle2->Des();
                StringLoader::Format(ptr1, formatString, 0, monthPtr);
                StringLoader::Format(ptr2, ptr1, KDateFormat1, yearTitle);
                StringLoader::Format(ptr3, ptr2, KDateFormat2, monthPtr2);
                StringLoader::Format(ptr4, ptr3, KDateFormat3, yearTitle2);
                

                aResponse->SetTextValueL(attr, *title4);  
                CleanupStack::PopAndDestroy(title4); 
                CleanupStack::PopAndDestroy(title3); 
                CleanupStack::PopAndDestroy(title2); 
                CleanupStack::PopAndDestroy(title1); 
                CleanupStack::PopAndDestroy(monthTitle2); 
                CleanupStack::PopAndDestroy(monthTitle); 
                CleanupStack::PopAndDestroy(tempTitle); 
#endif				                 
				}
			else
#endif // #if defined(GLX_SUB_TITLE_REL8) || defined(SHOW_MONTHS_START_END_SUB_TITLE)			    
				{
#ifdef GLX_SUB_TITLE_REL8				

				HBufC* tempTitle = NULL;

				TInt videoCount = aResponse->ValueTObjectL<TInt>(KGlxMediaItemTypeVideo);

				TInt imageCount = aResponse->ValueTObjectL<TInt>(KGlxMediaItemTypeImage);

				//No Images or Videos
				if ((0 == videoCount) && (0 == imageCount ))
					{  
					GLX_LOG_INFO("VideoCount:0,ImageCount:0");
					              	
					tempTitle = LoadLocalizedStringLC(KResourceFile,
										R_MONTHS_SUB_TITLE_NO_IMAGE_NO_VIDEO);
					aResponse->SetTextValueL(attr, *tempTitle);	
					}
				else
					{
					// 1 Image and multi/0 Videos                	
					if (1 == imageCount) 
						{
						GLX_LOG_INFO1("ImageCount:1,VideoCount:%d",videoCount);
						
						tempTitle = LoadLocalizedStringLC(KResourceFile,
										R_MONTHS_SUB_TITLE_ONE_IMAGE_MULTI_VIDEO);

						TPtr formatString = tempTitle->Des();

						// Now create a buffer that will contain the result. needs to be length of format string plus a few extra for the number
						HBufC* title = HBufC::NewLC(formatString.Length() + KDateBufferPaddingMin);
						TPtr ptr = title->Des();

						StringLoader::Format(ptr, formatString, -1, videoCount);

						// Set the title in the response.	
						aResponse->SetTextValueL(attr, *title);

						CleanupStack::PopAndDestroy(title);
						}

					// Multi/0 Image and 1 Video					
					else if (1 == videoCount)
						{
						GLX_LOG_INFO1("ImageCount: %d,VideoCount:1",imageCount);
						
						tempTitle = LoadLocalizedStringLC(KResourceFile,
									R_MONTHS_SUB_TITLE_MULTI_IMAGE_ONE_VIDEO);

						TPtr formatString = tempTitle->Des();

						// Now create a buffer that will contain the result. needs to be length of format string plus a few extra for the number
						HBufC* title = HBufC::NewLC(formatString.Length() + KDateBufferPaddingMin);
						TPtr ptr = title->Des();

						StringLoader::Format(ptr, formatString, -1, imageCount);

						// Set the title in the response.	
						aResponse->SetTextValueL(attr, *title);

						CleanupStack::PopAndDestroy(title);
						}

					// Multi Image and Multi Video
					else 
						{						
						GLX_LOG_INFO2("ImageCount %d,VideoCount %d",imageCount,videoCount);
						
						tempTitle = LoadLocalizedStringLC(KResourceFile,
									R_MONTHS_SUB_TITLE_MULTI_IMAGE_MULTI_VIDEO);

						TPtr formatString = tempTitle->Des();	

						HBufC* title = HBufC::NewLC(formatString.Length() + KDateBufferPaddingMin);
						TPtr ptr = title->Des();

						HBufC* title1 = HBufC::NewLC(formatString.Length() + KDateBufferPaddingMin);
						TPtr ptr1 = title1->Des();

						StringLoader::Format(ptr, formatString, 0, imageCount);
						StringLoader::Format(ptr1, ptr, KDateFormat1, videoCount);

						// Set the title in the response.	
						aResponse->SetTextValueL(attr, *title1);

						CleanupStack::PopAndDestroy(title1);
						CleanupStack::PopAndDestroy(title);
						}
					}
				CleanupStack::PopAndDestroy(tempTitle);
#else
	   			
	   			TInt usageCount = 0;
	   			
                if ( aResponse->IsSupported(KMPXMediaGeneralCount) )
                    {
                    usageCount = aResponse->ValueTObjectL<TInt>(KMPXMediaGeneralCount);
                    }
                else if ( aResponse->IsSupported(KGlxMediaCollectionInternalUsageCount) )
                    {
                    usageCount =
                    aResponse->ValueTObjectL<TInt>(KGlxMediaCollectionInternalUsageCount);
                    }
                else
                    {
                    User::Leave(KErrNotSupported);
                    }
                HBufC* tempTitle = NULL;

            	if(0 == usageCount)
            		{
                	tempTitle = LoadLocalizedStringLC(KResourceFile,
                				R_MONTHS_SUB_TITLE_NO_IMAGE_NO_VIDEO);

                	// Set the title in the response.
            		aResponse->SetTextValueL(attr, *tempTitle);
            		CleanupStack::PopAndDestroy(tempTitle);
            		continue;
            		}

            	else if (1 == usageCount)
                    {
                    tempTitle = LoadLocalizedStringLC(KResourceFile,
                    			R_MONTHS_SUB_TITLE_SINGLE);

                    aResponse->SetTextValueL(attr, *tempTitle);
                    CleanupStack::PopAndDestroy(tempTitle);
                    continue;
                    }
                else
                    {
                    tempTitle = LoadLocalizedStringLC(KResourceFile,
                    			R_MONTHS_SUB_TITLE_MULTI);
                    }
                
                TPtr formatString = tempTitle->Des();
                
                // Now create a buffer that will contain the result. needs to be length of format string plus a few extra for the number
                HBufC* title = HBufC::NewLC(formatString.Length() + KDateBufferPaddingMin);
                TPtr ptr = title->Des();
                StringLoader::Format(ptr, formatString, -1, usageCount);
                
                // Set the title in the response.
                aResponse->SetTextValueL(attr, *title);    

                CleanupStack::PopAndDestroy(title);
                CleanupStack::PopAndDestroy(tempTitle);	
#endif                			
				}	
			}
	    else if (attr == KGlxMediaCollectionPluginSpecificSelectMediaPopupTitle)
	        {
	        GLX_LOG_INFO("Attribute : MediaPopupTitle");
	        
	        User::Leave(KErrNotSupported);
	        }
	    else if (attr == KGlxMediaCollectionPluginSpecificNewMediaItemTitle)
	        {
	        GLX_LOG_INFO("Attribute : NewMediaItemTitle");
	        
	        User::Leave(KErrNotSupported);
	        }
	    else if (attr == KGlxMediaCollectionPluginSpecificDefaultMediaTitle)
	        {
	        GLX_LOG_INFO("Attribute : DefaultMediaTitle");
	        
	        User::Leave(KErrNotSupported);
	        }
        else if (attr == KMPXMediaGeneralTitle)
            {
            GLX_LOG_INFO("Attribute : GeneralTitle");
            
            if( TGlxMediaId(KGlxCollectionRootId) == aMediaId )
                {
                GLX_LOG_INFO("Attribute : GeneralTitle:RootId");
                
                HBufC* title = LoadLocalizedStringLC(KResourceFile,
                			R_MONTHS_GENERAL_TITLE);
                aResponse->SetTextValueL(attr, *title);  
                CleanupStack::PopAndDestroy(title); 
                }
            else
                {
                if( aResponse->IsSupported(KGlxMediaCollectionInternalStartDate) )
                    {
                    HBufC* tempTitle = LoadLocalizedStringLC(KResourceFile,
                    			R_MONTHS_ITEM_TITLE);
                    TPtr formatString = tempTitle->Des();
                    TTime month =
                    aResponse->ValueTObjectL<TInt64>(KGlxMediaCollectionInternalStartDate);
                    _LIT(KGlxTempMonthYearTitleFormat, "%F%Y");
                    TBuf<KYearBufferSize> yearTitle;
                    month.FormatL(yearTitle, KGlxTempMonthYearTitleFormat);

					// Ensure that the number conversion takes place only for Arabic
 					if (lang == ELangArabic)
	                	{
	                	AknTextUtils::DisplayTextLanguageSpecificNumberConversion(yearTitle);
						}

                    HBufC* monthTitle = GetMonthNameAsStringLC( 
                            month.DateTime().Month(), EMonthNameAsTitle );

                    HBufC* title1 = HBufC::NewLC(formatString.Length() + KDateBufferPaddingMax);
                    TPtr ptr = title1->Des();
                    HBufC* title = HBufC::NewLC(formatString.Length() + KDateBufferPaddingMax);
                    TPtr ptr2 = title->Des();
                    TPtr monthPtr = monthTitle->Des();
                    StringLoader::Format(ptr, formatString, 0, monthPtr);
                    StringLoader::Format(ptr2, ptr, KDateFormat1, yearTitle);
                    

                    aResponse->SetTextValueL(attr, *title);  
                    CleanupStack::PopAndDestroy(title); 
                    CleanupStack::PopAndDestroy(title1); 
                    CleanupStack::PopAndDestroy(monthTitle); 
                    CleanupStack::PopAndDestroy(tempTitle); 
                    }
                }
            }
	    }
    aResponse->Delete(KGlxMediaCollectionInternalUsageCount);
    aResponse->Delete(KGlxMediaCollectionInternalStartDate);
    aResponse->Delete(KGlxMediaCollectionInternalEndDate);
    aResponse->Delete(KGlxMediaItemTypeImage);    
    aResponse->Delete(KGlxMediaItemTypeVideo); 
	}

// ----------------------------------------------------------------------------
// IsUpdateMessageIgnored
// ----------------------------------------------------------------------------
// 

TBool CGlxCollectionPluginMonths::IsUpdateMessageIgnored(CMPXMessage& /*aMessage*/)
	{
	TBool ignore = EFalse;
	return ignore;
	}

// ----------------------------------------------------------------------------
// DefaultFilter
// ----------------------------------------------------------------------------
// 	
TGlxFilterProperties CGlxCollectionPluginMonths::DefaultFilter(TInt aLevel)
    {
    __ASSERT_DEBUG(( (aLevel == KGlxCollectionRootLevel) || 
    (aLevel == KGlxCollectionMonthsLevel) || (aLevel == KGlxCollectionMonthContentsLevel) 
     || (aLevel == KGlxCollectionMonthFSLevel)), Panic(EGlxPanicInvalidPathLevel));
    TGlxFilterProperties filterProperties;
    filterProperties.iSortOrder = EGlxFilterSortOrderCaptureDate;
    filterProperties.iSortDirection = EGlxFilterSortDirectionDescending;
    filterProperties.iOrigin = EGlxFilterOriginAll;
    switch(aLevel)
        {
        case KGlxCollectionRootLevel:
        case KGlxCollectionMonthsLevel: 
        case KGlxCollectionMonthFSLevel:
            {
            filterProperties.iSortDirection = EGlxFilterSortDirectionDescending;
            filterProperties.iItemType = EGlxFilterMonth;
            break;
            }
        }
    return filterProperties;
    }
    
// ----------------------------------------------------------------------------
// GetMonthNameAsStringLC
// ----------------------------------------------------------------------------
//  
HBufC* CGlxCollectionPluginMonths::GetMonthNameAsStringLC(
        const TMonth& aMonth, const TMonthStringType& aStrType)
    {
    TInt monthResourceId = 0;

    switch (aMonth)
        {
        case EJanuary:
            {
            monthResourceId
                    = (EMonthNameAsSubtitle == aStrType)
                                                         ? R_MONTHS_SUB_TITLE_JAN
                                                            : R_MONTHS_TITLE_JAN;
            break;
            }
        case EFebruary:
            {
            monthResourceId
                    = (EMonthNameAsSubtitle == aStrType)
                                                         ? R_MONTHS_SUB_TITLE_FEB
                                                            : R_MONTHS_TITLE_FEB;
            break;
            }
        case EMarch:
            {
            monthResourceId
                    = (EMonthNameAsSubtitle == aStrType)
                                                         ? R_MONTHS_SUB_TITLE_MAR
                                                            : R_MONTHS_TITLE_MAR;
            break;
            }
        case EApril:
            {
            monthResourceId
                    = (EMonthNameAsSubtitle == aStrType)
                                                         ? R_MONTHS_SUB_TITLE_APR
                                                            : R_MONTHS_TITLE_APR;
            break;
            }
        case EMay:
            {
            monthResourceId
                    = (EMonthNameAsSubtitle == aStrType)
                                                         ? R_MONTHS_SUB_TITLE_MAY
                                                            : R_MONTHS_TITLE_MAY;
            break;
            }
        case EJune:
            {
            monthResourceId
                    = (EMonthNameAsSubtitle == aStrType)
                                                         ? R_MONTHS_SUB_TITLE_JUN
                                                            : R_MONTHS_TITLE_JUN;
            break;
            }
        case EJuly:
            {
            monthResourceId
                    = (EMonthNameAsSubtitle == aStrType)
                                                         ? R_MONTHS_SUB_TITLE_JUL
                                                            : R_MONTHS_TITLE_JUL;
            break;
            }
        case EAugust:
            {
            monthResourceId
                    = (EMonthNameAsSubtitle == aStrType)
                                                         ? R_MONTHS_SUB_TITLE_AUG
                                                            : R_MONTHS_TITLE_AUG;
            break;
            }
        case ESeptember:
            {
            monthResourceId
                    = (EMonthNameAsSubtitle == aStrType)
                                                         ? R_MONTHS_SUB_TITLE_SEP
                                                            : R_MONTHS_TITLE_SEP;
            break;
            }
        case EOctober:
            {
            monthResourceId
                    = (EMonthNameAsSubtitle == aStrType)
                                                         ? R_MONTHS_SUB_TITLE_OCT
                                                            : R_MONTHS_TITLE_OCT;
            break;
            }
        case ENovember:
            {
            monthResourceId
                    = (EMonthNameAsSubtitle == aStrType)
                                                         ? R_MONTHS_SUB_TITLE_NOV
                                                            : R_MONTHS_TITLE_NOV;
            break;
            }
        case EDecember:
            {
            monthResourceId
                    = (EMonthNameAsSubtitle == aStrType)
                                                         ? R_MONTHS_SUB_TITLE_DEC
                                                            : R_MONTHS_TITLE_DEC;
            break;
            }
        }

    return LoadLocalizedStringLC(KResourceFile, monthResourceId);
    }
 // End of file
