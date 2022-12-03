// Easy Utility AI - (C) 2022 by Michael Hegemann

#pragma once

#include "CoreMinimal.h"
#include "EasyAIQueryObject.h"
#include "NavFilters/NavigationQueryFilter.h"
#include "EasyAINavigationQuery.generated.h"


UCLASS()
class EASYUTILITYAI_API UEasyAINavigationQuery : public UEasyAIQueryObject
{
	GENERATED_BODY()
	
public:

	/** The type of pattern to use for the query. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern")
	EQueryPatternType Pattern;

	/** The inputs for a Grid Pattern. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern", meta = (EditCondition = "Pattern == EQueryPatternType::Grid"))
	FQueryGridData GridData;

	/** The inputs for a Torus Pattern. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern", meta = (EditCondition = "Pattern == EQueryPatternType::Torus"))
	FQueryTorusData TorusData;

	/** How the navigation projection happens */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Navigation Mesh")
	FVector ProjectionExtent;

	/** How much Z offset to apply to the final projected location. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Navigation Mesh")
	float ProjectionZOffset = 0;

	/** Navigation filter to apply. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Navigation Mesh")
	TSubclassOf<UNavigationQueryFilter> FilterClass;

public:

	/**
	* This returns the results of the navigation Query.
	*
	* @param Locations The final query results.
	* @param bIsValid - If the Query returns anything at all.
	*/
	UFUNCTION(BlueprintCallable, Category = "Query")
	void GetQueryResults(TArray<FVector>& Locations, bool& bIsValid);

	/**
	* Apply utility scorers to the query locations.
	*
	* @param Location - Input location of query pattern.
	* @param Scorers - The output scorers to use for sorting the query locations.
	*/
	UFUNCTION(BlueprintNativeEvent, Category = "Query")
	void GetScorers(const FVector& Location, TArray<FUtilityScore>& Scorers);

	/**
	* This check is called on the reduced entries by Output Percent. Good for line-traces etc.
	*
	* @param Location - The input location.
	* @param OutLocation - The output location.
	* @return If the location is considered.
	*/
	UFUNCTION(BlueprintNativeEvent, Category = "Query")
	bool CheckLocation(const FVector& Location, FVector& OutLocation);

};
