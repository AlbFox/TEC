// Easy Utility AI - (C) 2022 by Michael Hegemann


#include "EasyAILocationQuery.h"
#include "AIUtilityFunctions.h"

void UEasyAILocationQuery::GetScorers_Implementation(const FVector& Location, TArray<FUtilityScore>& Scorers)
{
	FUtilityScore TempScore = FUtilityScore();
	Scorers.Add(TempScore);
}

void UEasyAILocationQuery::GetLocations_Implementation(TArray<FVector>& LocationsToScore)
{
}

void UEasyAILocationQuery::GetQueryResults(TArray<FVector>& Locations, bool& bIsValid)
{
	if (!ValidateResultQuery())
	{
		return;
	}

	PreQuery();

	TArray<FVector> LocationsToScore;
	GetLocations(LocationsToScore);

	if (LocationsToScore.Num() <= 0)
	{
		return;
	}

	FUtilityScoreVector UtilityScoreLoc;
	TArray<FUtilityScoreVector> FinalScoreLocations;
	TArray<FUtilityScore> LocScorers;

	for (const FVector& ScoreLoc : LocationsToScore)
	{
		GetScorers(ScoreLoc, LocScorers);

		if (LocScorers.Num() > 0)
		{
			UtilityScoreLoc.Location = ScoreLoc;
			UtilityScoreLoc.Score = UAIUtilityFunctions::CreateUtilityScore(LocScorers, ScoringType);
			FinalScoreLocations.Add(UtilityScoreLoc);
		}
		else
		{
			return;
		}
	}

	if (FinalScoreLocations.Num() > 0)
	{
		const TArray<FUtilityScoreVector> SortedScoreLocs = UAIUtilityFunctions::SortScoredVectors(FinalScoreLocations);
		const uint16 ReducedEntries = GetOutputPercent(FinalScoreLocations.Num());

		if (ReducedEntries <= 0)
		{
			return;
		}

		for (const FUtilityScoreVector& SortedLoc : SortedScoreLocs)
		{
			if (Locations.Num() <= ReducedEntries)
			{
				Locations.Add(SortedLoc.Location);
			}
		}
	}

	if (Locations.Num() > 0)
	{
		bIsValid = true;

		if (bDisplayDebugLocations)
		{
			DisplayDebug(Locations);
		}
	}
}