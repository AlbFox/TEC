// Easy Utility AI - (C) 2022 by Michael Hegemann

#pragma once

#include "CoreMinimal.h"
#include "EasyAIQueryObject.h"
#include "EasyAILocationQuery.generated.h"


UCLASS()
class EASYUTILITYAI_API UEasyAILocationQuery : public UEasyAIQueryObject
{
	GENERATED_BODY()
	
public:

	/**
	* This returns the results of the Query.
	*
	* @param Actors - The scored and sorted Actors, first entry is best.
	* @param bIsValid - If the Query returns anything at all.
	*/
	UFUNCTION(BlueprintCallable, Category = "Query")
	void GetQueryResults(TArray<FVector>& Locations, bool& bIsValid);

	/**
	* Apply utility scorers to the query locations.
	*
	* @param Location - Input location of query.
	* @param Scorers - The output scorers to use for sorting the query locations.
	*/
	UFUNCTION(BlueprintNativeEvent, Category = "Query")
	void GetScorers(const FVector& Location, TArray<FUtilityScore>& Scorers);

	/**
	* Gets the Array of Locations that should be scored..
	*
	* @param LocationsToScore - Output Location Array that should be scored.
	*/
	UFUNCTION(BlueprintNativeEvent, Category = "Query")
	void GetLocations(TArray<FVector>& LocationsToScore);
};
