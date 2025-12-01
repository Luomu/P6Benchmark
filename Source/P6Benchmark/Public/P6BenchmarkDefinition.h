
// (C) 2025 Kimmo Kotajärvi <kimmo.kotajarvi@gmail.com>

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LevelSequence.h"

#include "P6BenchmarkDefinition.generated.h"

/**
 * Defines which level to load for a benchmark
 */
UCLASS(Blueprintable, BlueprintType)
class P6BENCHMARK_API UP6BenchmarkDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// Level to load for benchmarking
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Benchmark")
	TSoftObjectPtr<UWorld> Level;

	// List of level sequences to load and run
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Benchmark")
	TArray<TSoftObjectPtr<ULevelSequence>> LevelSequences;

	// Game mode override, leave empty unless your project uses a custom benchmark GM.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Benchmark", meta = (AllowedClasses = "/Script/P6Benchmark.P6BenchmarkGameMode"))
	FString GameModeOverride;
};
