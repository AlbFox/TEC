// Easy Utility AI - (C) 2022 by Michael Hegemann

#pragma once

#include "CoreMinimal.h"
#include "Tickable.h"
#include "AIUtilityData.h"
#include "UObject/NoExportTypes.h"
#include "EasyAIQueryObject.generated.h"


UCLASS(BlueprintType, abstract, hidecategories = Object)
class EASYUTILITYAI_API UEasyAIQueryObject : public UObject
{
	GENERATED_BODY()

public:


	/** 
	The Query Origin Actor. 
	If not set, it will try to cast the Querier from the Owning Object.
	To use it with Tasks/Task Manager, the Querier should be the associated Pawn or AIController.  
	*/
	UPROPERTY(BlueprintReadOnly, Category = "Easy AI Query")
	AActor* Querier;

	/** How much percent of results to get into the final output, mainly for optimization.*/
	UPROPERTY(EditDefaultsOnly, Category = "Results", meta = (ClampMin = "1.0", ClampMax = "100.0", UIMin = "1.0", UIMax = "100.0"))
	float OutputPercent = 100;

	/** How the query score is calculated.*/
	UPROPERTY(EditDefaultsOnly, Category = "Scoring")
	EScoringType ScoringType;

	/** If it should display the final locations or actors.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bDisplayDebugLocations;

	/** How long to display debug results.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	float DrawDebugDuration = 0.0f;

	/** If the debug location should have a Z offset. */
	UPROPERTY(EditDefaultsOnly, Category = "Debug")
	float DebugLocationZOffset = 0.0f;
	
protected:

	UWorld* GetWorld() const override;

	void DisplayDebug(const TArray<FVector>& DebugLocs);

	FORCEINLINE bool ValidateResultQuery() const
	{
		if (!Querier || !GetWorld())
		{
			return false;
		}

		if (!IsValid(Querier))
		{
			return false;
		}

		return true;
	}

	FORCEINLINE uint16 GetOutputPercent(const uint16& SortedScorerCount) const
	{
		const uint16 ScorerCount = FMath::Max(1, SortedScorerCount - 1);
		const float SortPct = ScorerCount * (OutputPercent / 100.0);

		if (SortPct < 1.0f)
		{
			return 1;
		}

		const uint16 ReducedEntries = FMath::Max(0, FPlatformMath::TruncToInt(SortPct));

		return ReducedEntries;
	}

private:

	UWorld* World;

public:

	/*Called when the Query is spawned.*/
	UFUNCTION(BlueprintImplementableEvent, Category = "Query", meta = (DisplayName = "Begin Play"))
	void OnBeginPlay();

	/*Called when GetQueryResults is called, but before the scoring process.*/
	UFUNCTION(BlueprintImplementableEvent, Category = "Query", meta = (DisplayName = "Pre Query"))
	void PreQuery();

	/**
	* Gets the origin of the query. Usually from the Querier.
	* If the owner is an object, it has to be set manually.
	*
	* @param OriginLocation Location of Query origin.
	* @param OriginRotation Rotation of Query origin.
	*/
	UFUNCTION(BlueprintPure, Category = "Query", BlueprintNativeEvent)
	void GetOrigin(FVector& OriginLocation, FRotator& OriginRotation);
	virtual void GetOrigin_Implementation(FVector& OriginLocation, FRotator& OriginRotation);


	void InitQuery(AActor* QueryInstigator);

};
