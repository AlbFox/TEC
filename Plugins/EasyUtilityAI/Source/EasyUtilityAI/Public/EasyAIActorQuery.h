// Easy Utility AI - (C) 2022 by Michael Hegemann

#pragma once

#include "CoreMinimal.h"
#include "EasyAIQueryObject.h"
#include "EasyAIActorQuery.generated.h"


UCLASS()
class EASYUTILITYAI_API UEasyAIActorQuery : public UEasyAIQueryObject
{
	GENERATED_BODY()

public:

	/**
	* This returns the results of the Query. Requires input array of Actors.
	*
	* @param Actors - The scored and sorted Actors, first entry is best.
	* @param bIsValid - If the Query returns anything at all.
	*/
	UFUNCTION(BlueprintCallable, Category = "Query")
	void GetQueryResults(TArray<AActor*>& Actors, bool& bIsValid);

	/**
	* Apply utility scorers to the query Actors.
	*
	* @param Actor - Input Actor of query.
	* @param Scorers  - The output scorers to use for sorting the query locations.
	*/
	UFUNCTION(BlueprintNativeEvent, Category = "Query")
	void GetScorers(AActor* Actor, TArray<FUtilityScore>& Scorers);

	/**
	* Gets the Array of Actors that should be scored. A Valid input would be 'Get Actors of Class'.
	*
	* @param ActorsToScore - Output Actor Array that should be scored.
	*/
	UFUNCTION(BlueprintNativeEvent, Category = "Query")
	void GetActors(TArray<AActor*>& ActorsToScore);

};
