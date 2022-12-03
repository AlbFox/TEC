// Easy Utility AI - (C) 2022 by Michael Hegemann

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameplayTagContainer.h"
#include "Curves/CurveFloat.h"
#include "AIUtilityData.generated.h"

class UEasyAITask;

#define SCORE_ROUND	1000.0f
const FName DefaultChannelTag = "EasyUtilityAI.Default.DefaultTaskChannel";

/*
 *
 * Scoring System
 *
*/

USTRUCT(BlueprintType)
struct FUtilityScore
{
	GENERATED_BODY()

	FUtilityScore()
	{
		Curve = nullptr;
	}

public:

	/** The condition for a scorer to be considered. False means it returns 0. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scoring")
	bool bCondition = true;

	/** The input value. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scoring")
	float Input = 1.0f;

	/** The input denominator. Input/Base = Output. Clamped to 0-1.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scoring", meta = (DisplayName = "Base"))
	float InputMax = 1.0f;

	/** The output is scaled by the Y value of the curve. Unclamped.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scoring")
	UCurveFloat* Curve;

	FORCEINLINE float MakeUtilityScore()
	{
		InputMax = FMath::Max(Input, InputMax);
		Input = FMath::Clamp(Input / InputMax, 0.0f, 1.0f);

		if (Curve)
		{
			Input = Curve->GetFloatValue(Input);
		}

		Input = FMath::RoundToFloat(Input * SCORE_ROUND) / SCORE_ROUND;

		return Input;
	}
};

/** How scoring is calculated */
UENUM()
enum class EScoringType: uint8
{
	/** Scores are multiplied with each other. */
	ST_Default	UMETA(DisplayName = "Default"),

	/** The result is the average of all scores. */
	ST_Average	UMETA(DisplayName = "Average"),

	/** Scores are multiplied with added compensation factor based on number of scorers. */
	ST_Balanced	UMETA(DisplayName = "Balanced"),
};


/*
 *
 * Task System
 *
*/


/** How tasks are sorted if scores are equal */
UENUM()
enum class EEqualitySorting : uint8
{
	/** Pick a random one from the equals. */
	ES_Random	UMETA(DisplayName = "Random"),

	/** Pick the latest task with the equal score. */
	ES_Newest	UMETA(DisplayName = "Newest"),
};


/*
 *
 * Query System 
 *
*/

UENUM()
enum class EQueryPatternType : uint8
{
	Grid,
	Torus,
};

USTRUCT(BlueprintType)
struct FQueryGridData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Query")
	float Extent = 300.0f;
		
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Query")
	float Spacing = 100.0f;
};

USTRUCT(BlueprintType)
struct FQueryTorusData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Query")
	float InnerRadius = 200.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Query")
	float OuterRadius = 800.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Query")
	float NumRings = 5.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Query")
	float NumPoints = 20.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Query")
	float ConstraintAngle = 0.0f;
};

struct FUtilityScoreActor
{
	AActor* Actor;
	TArray<FUtilityScore> Scorers;
	float Score;
};

struct FUtilityScoreVector
{
	FVector Location;
	TArray<FUtilityScore> Scorers;
	float Score;
};

struct FDrawDebugLocationData
{
	TArray<FVector> Locations;
	float Duration;
	float Size = 15.0f;
	float ZOffset = 0;
};
