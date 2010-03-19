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
* Description:   
*
*/



/**
 * @internal reviewed 11/07/2007 by M Byrne
 * @internal reviewed 24/07/2007 by K Hoikka, SCO 403-8472 removal of all view
 */

#include "glxdatasourcetaskmds.h"

#include <glxcollectionplugincamera.hrh>
#include <glxcollectionplugindownloads.hrh>
#include <glxcollectionpluginmonths.hrh>
#include <glxcollectionpluginalbums.hrh>
#include <glxcollectionpluginall.hrh>
#include <glxcollectionplugintags.hrh>
#include <glxerrors.h>
#include <glxfilterfactory.h>
#include <glxgetrequest.h>
#include <glxidlistrequest.h>
#include <glxlog.h>
#include <glxmediacollectioninternaldefs.h>
#include <glxmediageneraldefs.h>
#include <glxrequest.h>
#include <glxthumbnailattributeinfo.h>
#include <glxthumbnailrequest.h>
#include <glxtndatabase.h>
#include <glxtnfileinfo.h>
#include <glxtnthumbnailcreator.h>
#include <glxtracer.h>
#include <lbsposition.h>
#include <mdeconstants.h>
#include <mdelogiccondition.h>
#include <mdeobject.h>
#include <mdeobjectdef.h>
#include <mdeobjectquery.h>
#include <mdepropertydef.h>
#include <mderelationdef.h>
#include <mdesession.h>
#include <mdetextproperty.h>
#include <mpxmedia.h>
#include <mpxmediaarray.h> 
#include <mpxmediacollectiondetaildefs.h>
#include <mpxmediacontainerdefs.h>
#include <mpxmediadrmdefs.h>
#include <mpxmediageneraldefs.h>

#include "glxdatasourcemds.h"
#include "glxdatasourcemds.hrh"
#include "glxdatasourcemdsutility.h"

// CONSTANTS
_LIT(KPropertyDefNameCreationDate, "CreationDate");
_LIT(KPropertyDefNameLastModifiedDate, "LastModifiedDate");
_LIT(KPropertyDefNameTitle, "Title");
//Code commented cout below, so commenting the below line to remove BAD warning
//_LIT(KPropertyDefNameDRM, "DRM");
_LIT(KPropertyDefNameFrameCount, "FrameCount");
_LIT(KPropertyDefNameOrigin, "Origin");

// ----------------------------------------------------------------------------
//  Constructor
// ----------------------------------------------------------------------------
//  
CGlxDataSourceTaskMde:: CGlxDataSourceTaskMde(CGlxRequest* aRequest, 
        MGlxDataSourceRequestObserver& aObserver, CGlxDataSource* aDataSource)
    : CGlxDataSourceTask(aRequest, aObserver, aDataSource)
    {
    TRACER("CGlxDataSourceTaskMde::CGlxDataSourceTaskMde()")
    // No implementation required
    }

// ----------------------------------------------------------------------------
//  Destructor
// ----------------------------------------------------------------------------
//  
CGlxDataSourceTaskMde::~CGlxDataSourceTaskMde()
    {
    TRACER("CGlxDataSourceTaskMde::~CGlxDataSourceTaskMde()")
    DestroyQueries();
    }

// ----------------------------------------------------------------------------
//  Second stage constructor
// ----------------------------------------------------------------------------
//  	
void CGlxDataSourceTaskMde::ConstructL()
    {
    TRACER("CGlxDataSourceTaskMde::ConstructL()")
    CreateResponseL();
#ifdef USE_S60_TNM
    DataSource()->CancelFetchThumbnail();
#else    
    DataSource()->ThumbnailCreator().CancelRequest( TGlxMediaId(0) );
#endif
    }

// ----------------------------------------------------------------------------
//  CGlxDataSourceTaskMde::CancelRequest
// ----------------------------------------------------------------------------
//  
void CGlxDataSourceTaskMde::CancelRequest()
    {
    TRACER("CGlxDataSourceTaskMde::CancelRequest()")
    DestroyQueries();
    iCancelled = ETrue;
    }

// ----------------------------------------------------------------------------
//  CGlxDataSourceTaskMde::HandleQueryNewResults
// ----------------------------------------------------------------------------
//      
void CGlxDataSourceTaskMde::HandleQueryNewResults( CMdEQuery& /*aQuery*/,
                                                     TInt /*aFirstNewItemIndex*/,
                                                     TInt /*aNewItemCount*/ )
    {
    TRACER("CGlxDataSourceTaskMde::HandleQueryNewResults()")
    // Not used.
    }


// ----------------------------------------------------------------------------
//  CGlxDataSourceTaskMde::HandleQueryNewResults
// ----------------------------------------------------------------------------
//    
void CGlxDataSourceTaskMde::HandleQueryNewResults( CMdEQuery& /*aQuery*/,
                                TInt /*aNewObjectItemCount*/,
                                TInt /*aNewRelationItemCount*/,
                                TInt /*aNewEventItemCount*/)
    {
    TRACER("CGlxDataSourceTaskMde::HandleQueryNewResults()")
    // Not used.
    }


// ----------------------------------------------------------------------------
// CGlxDataSourceTaskMde::HandleQueryCompleted
// ----------------------------------------------------------------------------
//
void CGlxDataSourceTaskMde::HandleQueryCompleted(CMdEQuery& aQuery, TInt aError)
    {
    TRACER("CGlxDataSourceTaskMde::HandleQueryCompleted()")
    __ASSERT_ALWAYS(&aQuery == iQueries[0], Panic(EGlxPanicQueryLogicError));

    TInt err = aError;
    if (err == KErrNone)
        {
        TRAP(err, HandleQueryCompletedL(aQuery));
        }

    if (err != KErrNone)
        {
        HandleRequestComplete(err);
        }
    }

// ----------------------------------------------------------------------------
//  CGlxDataSourceTaskMde::DataSource
// ----------------------------------------------------------------------------
//   
CGlxDataSourceMde* CGlxDataSourceTaskMde::DataSource()
    {
    TRACER("CGlxDataSourceTaskMde::DataSource()")
    return static_cast<CGlxDataSourceMde*>(iDataSource);
    }

// ----------------------------------------------------------------------------
//  CGlxDataSourceTaskMde::AddMonthFilterL
// ----------------------------------------------------------------------------
//     
void CGlxDataSourceTaskMde::AddMonthFilterL(const TGlxMediaId& aContainerId, TGlxFilterProperties& aFilterProperties)
    {
    TRACER("CGlxDataSourceTaskMde::AddMonthFilterL(const TGlxMediaId& aContainerId, TGlxFilterProperties& aFilterProperties)")
    CMdEObject* month = DataSource()->Session().GetObjectL(aContainerId.Value());
    if( !month )
        {
        User::Leave(KErrNotFound);
        }
    CleanupStack::PushL(month);
    
    AddMonthFilterL(month, aFilterProperties);
        
    CleanupStack::PopAndDestroy(month);
    }


// ----------------------------------------------------------------------------
//  CGlxDataSourceTaskMde::AddMonthFilterL
// ----------------------------------------------------------------------------
//   
void CGlxDataSourceTaskMde::AddMonthFilterL(CMdEObject* aMonth, TGlxFilterProperties& aFilterProperties)
    {
    TRACER("CGlxDataSourceTaskMde::AddMonthFilterL(CMdEObject* aMonth, TGlxFilterProperties& aFilterProperties)")
    CMdEPropertyDef& creationDateDef = DataSource()->ObjectDef().GetPropertyDefL(KPropertyDefNameCreationDate);
    if (creationDateDef.PropertyType() != EPropertyTime)
        {
        User::Leave(KErrCorrupt);
        }

    CMdEPropertyDef& lmDateDef = DataSource()->ObjectDef().GetPropertyDefL(KPropertyDefNameLastModifiedDate);
    if (lmDateDef.PropertyType() != EPropertyTime)
        {
        User::Leave(KErrCorrupt);
        }
        
    CMdEProperty* startDate;
    TInt startDateIndex = aMonth->Property(creationDateDef, startDate);
    if( KErrNotFound == startDateIndex) 
        {
        User::Leave(KErrCorrupt);
        }
    aFilterProperties.iStartDate = static_cast<CMdETimeProperty*>(startDate)->Value();
     
    CMdEProperty* endDate;
    TInt endDateIndex = aMonth->Property(lmDateDef, endDate);
    if( KErrNotFound == endDateIndex) 
        {
        User::Leave(KErrCorrupt);
        }
    aFilterProperties.iEndDate = static_cast<CMdETimeProperty*>(endDate)->Value();
    }

// ----------------------------------------------------------------------------
//  CGlxDataSourceTaskMde::SetQueryConditionsL
// ----------------------------------------------------------------------------
//    
void CGlxDataSourceTaskMde::SetQueryConditionsL(CMdEQuery& aQuery, const TGlxFilterProperties& aFilterProperties, const TGlxMediaId aContainerId, CMdEObjectDef& aObjectDef)
    {
    TRACER("CGlxDataSourceTaskMde::SetQueryConditionsL()")
    
    CMdELogicCondition& rootCondition = aQuery.Conditions();
    CMdEObjectDef* objectDef = &aObjectDef;

    SetQueryFilterConditionsL(rootCondition, *objectDef, aFilterProperties);
    SetSortOrderL(aQuery, aObjectDef, aFilterProperties);
    
    if( KGlxCollectionRootId != aContainerId.Value() )
        {
        CMdELogicCondition* containerCondition = NULL;
        CMdERelationCondition* relationCondition = NULL;
        objectDef = &DataSource()->ObjectDef();
        if( DataSource()->ContainerIsLeft(aObjectDef) )
            {
            relationCondition = &rootCondition.AddRelationConditionL(DataSource()->ContainsDef(), ERelationConditionSideRight);
            containerCondition = &relationCondition->LeftL();
            }
        else
            {
            relationCondition = &rootCondition.AddRelationConditionL(DataSource()->ContainsDef(), ERelationConditionSideLeft);
            containerCondition = &relationCondition->RightL();
            }
        containerCondition->AddObjectConditionL(aContainerId.Value());
        }

    }

// ----------------------------------------------------------------------------
//  CGlxDataSourceTaskMde::SetQueryFilterConditionsL
// ----------------------------------------------------------------------------
//  
void CGlxDataSourceTaskMde::SetQueryFilterConditionsL(CMdELogicCondition& 
                                   aLogicCondition, CMdEObjectDef& aObjectDef,
                                 const TGlxFilterProperties& aFilterProperties)
    {
    TRACER("CGlxDataSourceTaskMde::SetQueryFilterConditionsL()")
    
    if( aFilterProperties.iUri )
        {
        aLogicCondition.AddObjectConditionL(EObjectConditionCompareUri, 
                                                     *aFilterProperties.iUri);
        }

    if( 0 == aObjectDef.Compare(DataSource()->ObjectDef()) ) 
        {
        // Filter on content type
        if ( EGlxFilterVideoAndImages == aFilterProperties.iItemType )
            {
            CMdELogicCondition& logicCondition = 
                aLogicCondition.AddLogicConditionL(ELogicConditionOperatorOr);
            logicCondition.AddObjectConditionL( DataSource()->ImageDef() ); 
            logicCondition.AddObjectConditionL( DataSource()->VideoDef() );
            }
        }
    
    if( ( aFilterProperties.iMinCount > 0 ) && ( CGlxDataSource::EContainerTypeNotAContainer != DataSource()->ContainerType(&aObjectDef) ) )
        {        
		TMdEUintRange range(aFilterProperties.iMinCount,aFilterProperties.iMinCount,EMdERangeTypeGreaterOrEqual);
		aLogicCondition.AddObjectConditionL(range);
        }

    if( 0 != aFilterProperties.iContainsItem.Value() )
        {
        
        if( DataSource()->ContainerIsLeft(aObjectDef) )
            {
            CMdERelationCondition& relationCondition = 
                    aLogicCondition.AddRelationConditionL(DataSource()->ContainsDef(), ERelationConditionSideLeft);
            relationCondition.RightL().AddObjectConditionL(aFilterProperties.iContainsItem.Value());
            }
        else
            {
            CMdERelationCondition& relationCondition = aLogicCondition.AddRelationConditionL(DataSource()->ContainsDef(), ERelationConditionSideRight);
            relationCondition.LeftL().AddObjectConditionL(aFilterProperties.iContainsItem.Value());
            }
        }
        
    if( EGlxFilterOriginNotUsed != aFilterProperties.iOrigin )
        {
        CMdEPropertyDef& originProperty = aObjectDef.GetPropertyDefL(KPropertyDefNameOrigin);
        if( EGlxFilterOriginDownload == aFilterProperties.iOrigin )
            {
            // The download collection shows all but captured items
            aLogicCondition.AddPropertyConditionL(originProperty, TMdEUintNotEqual(MdeConstants::Object::ECamera));
            }
		else if(EGlxFilterOriginCamera == aFilterProperties.iOrigin )            
			{
			// The camera collection Shows the captured Items
			aLogicCondition.AddPropertyConditionL(originProperty, TMdEUintEqual(MdeConstants::Object::ECamera));            
			}
        }
        
    if( aFilterProperties.iExcludeAnimation )
        {
        //__ASSERT_DEBUG((EGlxFilterImage == aFilterProperties.iItemType), Panic(EGlxPanicIllegalArgument));
        // Exclude any image with a frame count > 1
        const TInt excludeAllImagesAboveOrEqualToThisFrameCount = 2;
        CMdEPropertyDef& frameCountProperty = DataSource()->ImageDef().GetPropertyDefL(KPropertyDefNameFrameCount);
        aLogicCondition.AddPropertyConditionL(frameCountProperty, TMdEIntLess(excludeAllImagesAboveOrEqualToThisFrameCount));
        }
        
    if( aFilterProperties.iNoDRM )
        {
        __ASSERT_DEBUG((EGlxFilterImage == aFilterProperties.iItemType), Panic(EGlxPanicIllegalArgument));
        // Exclude any image which is DRM protected
        //CMdEPropertyDef& drmProperty = DataSource()->ImageDef().GetPropertyDefL(KPropertyDefNameDRM);
        //aLogicCondition.AddPropertyConditionL(drmProperty, EFalse);
        }
        
    if( aFilterProperties.iPath )          // If this is set. Then we need to filter on the Ids it supplies
        {
        TArray<TInt> selection = aFilterProperties.iPath->Selection();
        TInt selectionCount = selection.Count();
        RArray<TItemId> itemList;
        CleanupClosePushL(itemList);
        GLX_DEBUG2("Reading Ids from Path, count = %d", selectionCount);
        if( selectionCount )
            {
            for( TInt i = 0; i < selectionCount; i++ )
                {
                const TMPXItemId& itemId = aFilterProperties.iPath->IdOfIndex(selection[i]);
                itemList.AppendL(itemId);
                }
            aLogicCondition.AddObjectConditionL(itemList);
            }
        CleanupStack::Pop(&itemList);
        }
        
    if( !aFilterProperties.iIncludeCameraAlbum )
        {
        CMdEObjectCondition& objectCondition = aLogicCondition.AddObjectConditionL(DataSource()->CameraAlbumId().Value());
        objectCondition.SetNegate(ETrue);
        }
    
    if( TTime(0) != aFilterProperties.iStartDate )
        {
        CMdEPropertyDef& creationDateDef = aObjectDef.GetPropertyDefL(KPropertyDefNameCreationDate);
        if (creationDateDef.PropertyType() != EPropertyTime)
            {
            User::Leave(KErrCorrupt);
            }

        aLogicCondition.AddPropertyConditionL(creationDateDef, TMdETimeBetween(aFilterProperties.iStartDate, aFilterProperties.iEndDate));
        }
    }

// ----------------------------------------------------------------------------
//  CGlxDataSourceTaskMde::SetSortOrderL
// ----------------------------------------------------------------------------
// 
void CGlxDataSourceTaskMde::SetSortOrderL(CMdEQuery& aQuery, CMdEObjectDef& aObjectDef, 
                                         const TGlxFilterProperties& aFilterProperties)
    {
    TRACER("CGlxDataSourceTaskMde::SetSortOrderL()")
    switch(aFilterProperties.iSortOrder)
        {
        case EGlxFilterSortOrderAlphabetical:
            {
            CMdEPropertyDef& titleProperty = aObjectDef.GetPropertyDefL(KPropertyDefNameTitle);
            TMdEOrderRule orderRule(titleProperty, aFilterProperties.iSortDirection == EGlxFilterSortDirectionAscending);
            orderRule.SetType(EOrderRuleTypeProperty);
            orderRule.SetCaseSensitive(EFalse);
            aQuery.AppendOrderRuleL(orderRule);
            break;
            }
         case EGlxFilterSortOrderItemCount:
            {            
            TMdEOrderRule orderRule(EOrderRuleTypeUsageCount, aFilterProperties.iSortDirection == EGlxFilterSortDirectionAscending);
            aQuery.AppendOrderRuleL(orderRule);            
            break;
            }
        case EGlxFilterSortOrderCaptureDate:
            {
            CMdEPropertyDef& creationDateProperty = aObjectDef.GetPropertyDefL(KPropertyDefNameCreationDate);
            TMdEOrderRule orderRule(creationDateProperty, aFilterProperties.iSortDirection == EGlxFilterSortDirectionAscending);
            orderRule.SetType(EOrderRuleTypeProperty);
            aQuery.AppendOrderRuleL(orderRule);
            TMdEOrderRule orderRule2(EOrderRuleTypeItemID, aFilterProperties.iSortDirection == EGlxFilterSortDirectionAscending);
            aQuery.AppendOrderRuleL(orderRule2);
            break;
            }
         case EGlxFilterSortOrderModifiedDate:
            {
            CMdEPropertyDef& modifiedDateProperty = aObjectDef.GetPropertyDefL(KPropertyDefNameLastModifiedDate);
            TMdEOrderRule orderRule(modifiedDateProperty, aFilterProperties.iSortDirection == EGlxFilterSortDirectionAscending);
            orderRule.SetType(EOrderRuleTypeProperty);
            aQuery.AppendOrderRuleL(orderRule);
            TMdEOrderRule orderRule2(EOrderRuleTypeItemID, aFilterProperties.iSortDirection == EGlxFilterSortDirectionAscending);
            aQuery.AppendOrderRuleL(orderRule2);
            break;
            }
        case EGlxFilterSortOrderNotUsed:
            {
            break;
            }
        }
    }

// ----------------------------------------------------------------------------
//  CGlxDataSourceTaskMde::MaxQueryResultsCount
// ----------------------------------------------------------------------------
// 
TInt CGlxDataSourceTaskMde::MaxQueryResultsCount(const TGlxFilterProperties& aFilterProperties) const
    {
    TRACER("CGlxDataSourceTaskMde::MaxQueryResultsCount()")
    TInt ret = KMdEQueryDefaultMaxCount;
    if (aFilterProperties.iLastCaptureDate)
        {
        ret = 1;
        }
    return ret;
    }

// ----------------------------------------------------------------------------
//  CGlxDataSourceTaskMde::RemoveQuery
// ----------------------------------------------------------------------------
// 
void CGlxDataSourceTaskMde::RemoveQuery()
    {
    TRACER("CGlxDataSourceTaskMde::RemoveQuery()")
    CMdEQuery* query = iQueries[0];
    iQueryTypes.Remove(0);
    iQueries.Remove(0);
    query->RemoveObserver(*this);
    delete query;
    }

// ----------------------------------------------------------------------------
//  CGlxDataSourceTaskMde::DoQueryL
// ----------------------------------------------------------------------------
// 	
void CGlxDataSourceTaskMde::DoQueryL(CMdEObjectDef& aObjectDef, 
        TBool aIsContent, TGlxQueryType aQueryType, TQueryResultMode aResultMode, 
                                                const TGlxMediaId& aContainerId)
    {
    TRACER("CGlxDataSourceTaskMde::DoQueryL()")
    
    CMdEObjectDef* queryBaseObject = &aObjectDef;
    if( aIsContent )
        {
        switch(iFilterProperties.iItemType)
            {
            case EGlxFilterImage: 
                {
                queryBaseObject = &DataSource()->ImageDef();
                break;
                }
            case EGlxFilterVideo:
                {
                queryBaseObject = &DataSource()->VideoDef();
                break;
                }
            default:
                {
                queryBaseObject = &DataSource()->ObjectDef();
                break;
                }
            }
        }

    CMdEQuery* query = DataSource()->Session().NewObjectQueryL(*DataSource()->NamespaceDef(), *queryBaseObject, this);
    CleanupStack::PushL(query);
    
    SetQueryConditionsL(*query, iFilterProperties, aContainerId, aObjectDef);   
    query->SetResultMode(aResultMode);
    
    CleanupStack::Pop(query);
    
    AppendQueryL(query, aQueryType);

    query->FindL(MaxQueryResultsCount(iFilterProperties));    
    }

// ----------------------------------------------------------------------------
// CGlxDataSourceTask::QueueImageVideoObjectQueriesL
// ----------------------------------------------------------------------------
//
void CGlxDataSourceTaskMde::QueueImageVideoObjectQueriesL(const RArray<TGlxMediaId>& aObjectIds,
                                          const TGlxFilterProperties& aFilterProperties)
    {
    TRACER("CGlxDataSourceTaskMde::QueueImageVideoObjectQueriesL()")
    if (aFilterProperties.iItemType == EGlxFilterImage)
        {
        // Only perform the image query
        QueueObjectQueryL(DataSource()->ImageDef(), aObjectIds, EImageVideoQuery);
        }
    else if (aFilterProperties.iItemType == EGlxFilterVideo)
        {
        // Only perform the video query
        QueueObjectQueryL(DataSource()->VideoDef(), aObjectIds, EImageVideoQuery);
        }
    else
        {
        QueueObjectQueryL(DataSource()->ImageDef(), aObjectIds, EImageVideoQuery);
       QueueObjectQueryL(DataSource()->VideoDef(), aObjectIds, EImageVideoQuery);
        // Perform both the image and video queries
        }
    }

// ----------------------------------------------------------------------------
// CGlxDataSourceTask::QueueTagObjectQueryL
// ----------------------------------------------------------------------------
//
void CGlxDataSourceTaskMde::QueueTagObjectQueryL(const RArray<TGlxMediaId>& aObjectIds)
    {
    TRACER("CGlxDataSourceTaskMde::QueueTagObjectQueryL()")
    QueueObjectQueryL(DataSource()->TagDef(), aObjectIds, EImageVideoQuery);
    }

// ----------------------------------------------------------------------------
// CGlxDataSourceTask::QueueAlbumObjectQueryL
// ----------------------------------------------------------------------------
//
void CGlxDataSourceTaskMde::QueueAlbumObjectQueryL(const RArray<TGlxMediaId>& aObjectIds)
    {
    TRACER("CGlxDataSourceTaskMde::QueueAlbumObjectQueryL()")
    QueueObjectQueryL(DataSource()->AlbumDef(), aObjectIds, EImageVideoQuery);
    }


void CGlxDataSourceTaskMde::QueueMonthObjectQueryL(const RArray<TGlxMediaId>& aObjectIds)
    {
    TRACER("CGlxDataSourceTaskMde::QueueMonthObjectQueryL()")
    QueueObjectQueryL(DataSource()->MonthDef(), aObjectIds, EImageVideoQuery);
    }

// ----------------------------------------------------------------------------
// CGlxDataSourceTask::QueueObjectQueryL
// ----------------------------------------------------------------------------
//
void CGlxDataSourceTaskMde::QueueObjectQueryL(CMdEObjectDef& aObjectDef, 
        const RArray<TGlxMediaId>& aObjectIds, const TGlxQueryType& aQueryType)
    {
    TRACER("CGlxDataSourceTaskMde::QueueObjectQueryL()")
    CMdEQuery* query = DataSource()->Session().NewObjectQueryL(*DataSource()->NamespaceDef(), aObjectDef,  this);
    CleanupStack::PushL(query);
 
    CMdELogicCondition& lc = query->Conditions();
    NGlxDataSourceMdsUtility::AddObjectConditionL(lc,aObjectIds);
    
    query->SetResultMode(EQueryResultModeObjectWithFreetexts);
    
    CleanupStack::Pop(query);
    AppendQueryL(query, aQueryType);
    
    }

// ----------------------------------------------------------------------------
//  CGlxDataSourceTaskMde::AppendQueryL
// ----------------------------------------------------------------------------
// 
void CGlxDataSourceTaskMde::AppendQueryL(CMdEQuery* aQuery, const TGlxQueryType& aQueryType)
    {
    TRACER("CGlxDataSourceTaskMde::AppendQueryL()")
    CleanupStack::PushL(aQuery);
    
    TInt err = iQueryTypes.Append(aQueryType);
    
    if (KErrNone == err)
        {
        iQueries.AppendL(aQuery);
        CleanupStack::Pop(aQuery);
        }
    else
        {
        User::Leave(err);
        }
    }

// ----------------------------------------------------------------------------
// CGlxDataSourceTaskMde::ExecuteQueryL
// ----------------------------------------------------------------------------
// 
void CGlxDataSourceTaskMde::ExecuteQueryL()
    {
    TRACER("CGlxDataSourceTaskMde::ExecuteQueryL()")
    __ASSERT_DEBUG(iQueries.Count(), Panic(EGlxPanicQueryLogicError));
    iQueries[0]->FindL();
    }

// ----------------------------------------------------------------------------
//  CGlxDataSourceTaskMde::HandleQueryCompletedL
// ----------------------------------------------------------------------------
// 
void CGlxDataSourceTaskMde::HandleQueryCompletedL(CMdEQuery& aQuery)
    {
    TRACER("CGlxDataSourceTaskMde::HandleQueryCompletedL()")
    DoHandleQueryCompletedL(aQuery);
    RemoveQuery();
    DoNextQueryL();
    }

// ----------------------------------------------------------------------------
//  CGlxDataSourceTaskMde::DestroyQueries
// ----------------------------------------------------------------------------
// 
void CGlxDataSourceTaskMde::DestroyQueries()
    {
    TRACER("CGlxDataSourceTaskMde::DestroyQueries()")
    for (TInt i = 0; i < iQueries.Count(); i++)
        {
        // Ensure that there are not any running queries
        iQueries[i]->RemoveObserver(*this);
        iQueries[i]->Cancel();
        }
    iQueries.ResetAndDestroy();
    iQueryTypes.Close();
    }



