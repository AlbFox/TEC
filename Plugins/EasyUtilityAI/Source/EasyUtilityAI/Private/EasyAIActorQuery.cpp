// Easy Utility AI - (C) 2022 by Michael Hegemann


#include "EasyAIActorQuery.h"
#include "AIUtilityFunctions.h"

void UEasyAIActorQuery::GetScorers_Implementation(AActor* Actor, TArray<FUtilityScore>& Scorers)
{
	FUtilityScore TempScore = FUtilityScore();
	Scorers.Add(TempScore);
}

void UEasyAIActorQuery::GetActors_Implementation(TArray<AActor*>& ActorsToScore)
{
}

void UEasyAIActorQuery::GetQueryResults(TArray<AActor*>& Actors, bool& bIsValid)
{
	if (!ValidateResultQuery())
	{
		return;
	}

	PreQuery();

	TArray<AActor*> ActorsToScore;
	GetActors(ActorsToScore);

	if (ActorsToScore.Num() <= 0)
	{
		return;
	}

	FUtilityScoreActor UtilityScoreActor;
	TArray<FUtilityScoreActor> FinalScoreActors;
	TArray<FUtilityScore> ActorScorers;

	for (AActor* ScoreActor : ActorsToScore)
	{
		GetScorers(ScoreActor, ActorScorers);

		if (ActorScorers.Num() > 0)
		{
			UtilityScoreActor.Actor = ScoreActor;
			UtilityScoreActor.Score = UAIUtilityFunctions::CreateUtilityScore(ActorScorers, ScoringType);
			FinalScoreActors.Add(UtilityScoreActor);
		}
		else
		{
			return;
		}
	}

	if (FinalScoreActors.Num() > 0)
	{
		const TArray<FUtilityScoreActor> SortedScoreActors = UAIUtilityFunctions::SortScoredActors(FinalScoreActors);
		const uint16 ReducedEntries = GetOutputPercent(SortedScoreActors.Num()); 

		if (ReducedEntries <= 0)
		{
			return;
		}

		for (const FUtilityScoreActor& SortedActor : SortedScoreActors)
		{
			if (Actors.Num() <= ReducedEntries)
			{
				Actors.Add(SortedActor.Actor);
			}
		}
	}

	if (Actors.Num() > 0)
	{
		bIsValid = true;

		if (bDisplayDebugLocations)
		{
			TArray<FVector> DebugLocs;
			for (const AActor* ScoreActor : Actors)
			{
				if (ScoreActor)
				{
					DebugLocs.Add(ScoreActor->GetActorLocation());
				}
			}

			if (DebugLocs.Num() > 0)
			{
				DisplayDebug(DebugLocs);
			}
		}
	}
}

