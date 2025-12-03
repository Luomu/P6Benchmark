// (C) 2025 Kimmo Kotajärvi <kimmo.kotajarvi@gmail.com>

#pragma once

#include "CoreMinimal.h"
#include "P6BenchmarkResult.generated.h"

// Result for one section (camera cut) of the benchmark sequence
USTRUCT(BlueprintType)
struct FP6BenchmarkSectionResult
{
	GENERATED_BODY();

	UPROPERTY(BlueprintReadOnly)
	FString SectionName;

	UPROPERTY(BlueprintReadOnly)
	float AverageFrameTime;

	FP6BenchmarkSectionResult()
		: AverageFrameTime(0.f)
	{
	}
};

// Overall result for the benchmark
USTRUCT(BlueprintType)
struct FP6BenchmarkResult
{
	GENERATED_BODY();

	// Results for individual section (camera cuts)
	UPROPERTY(BlueprintReadOnly)
	TArray<FP6BenchmarkSectionResult> Sections;

	UPROPERTY(BlueprintReadOnly)
	float CPUBenchmarkIndex;

	UPROPERTY(BlueprintReadOnly)
	float GPUBenchmarkIndex;

	UPROPERTY(BlueprintReadOnly)
	float AverageFPS;

	FP6BenchmarkResult()
		: CPUBenchmarkIndex(0.f)
		, GPUBenchmarkIndex(0.f)
		, AverageFPS(0.f)
	{
	}
};
