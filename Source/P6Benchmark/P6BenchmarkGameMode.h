// (C) 2025 Kimmo Kotajärvi <kimmo.kotajarvi@gmail.com>

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "P6BenchmarkResult.h"

#include "P6BenchmarkGameMode.generated.h"

class UP6BenchmarkDefinition;
class ULevelSequencePlayer;
class ALevelSequenceActor;
class UCameraComponent;

/**
 * Runs the benchmark, once the level has been loaded
 */
UCLASS()
class P6BENCHMARK_API AP6BenchmarkGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AP6BenchmarkGameMode();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaSeconds) override;

	// Call if automatic startup is disabled
	UFUNCTION(BlueprintCallable)
	void StartBenchmark();

	// Override this for custom handling results. By default this shows the default ResultsWidget class.
	UFUNCTION(BlueprintNativeEvent)
	void PresentResults(const FP6BenchmarkResult& Result);
	virtual void PresentResults_Implementation(const FP6BenchmarkResult& Result);

public:
	// Result of the last benchmark
	UPROPERTY(BlueprintReadOnly)
	FP6BenchmarkResult BenchmarkResult;

protected:
	bool LoadAssets();

	UFUNCTION()
	void OnSequenceFinished();

	UFUNCTION()
	void OnCameraCut(UCameraComponent* CameraComponent);

protected:
	UPROPERTY(transient)
	TSoftObjectPtr<UP6BenchmarkDefinition> BenchmarkDefinition;

	UPROPERTY(transient);
	ULevelSequencePlayer* SequencePlayer = nullptr;

	UPROPERTY(transient);
	ALevelSequenceActor* SequenceActor = nullptr;

	UPROPERTY(transient);
	APlayerController* PlayerController = nullptr;

	int32 CurrentSection = 0;
	bool bRunning        = false;
	float FPSSum         = 0.f;
	int32 SampleCount    = 0;

private:
	void StartFPSChart();
	void StopFPSChart();

	FString GetFPSChartName() const;
};
