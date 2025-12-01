// (C) 2025 Kimmo Kotajärvi <kimmo.kotajarvi@gmail.com>

#include "P6BenchmarkGameMode.h"

#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Kismet/GameplayStatics.h"
#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"
#include "P6BenchmarkDefinition.h"
#include "P6BenchmarkStatics.h"

AP6BenchmarkGameMode::AP6BenchmarkGameMode()
{
	PrimaryActorTick.bCanEverTick          = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.TickInterval          = 0.f;
}

void AP6BenchmarkGameMode::BeginPlay()
{
	Super::BeginPlay();

	PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (PlayerController)
	{
		const bool bCinematicMode   = true;
		const bool bHidePlayer      = true;
		const bool bHideHUD         = true;
		const bool bPreventMovement = true;
		const bool bPreventTurning  = true;
		PlayerController->SetCinematicMode(bCinematicMode, bHidePlayer, bHideHUD, bPreventMovement, bPreventTurning);
	}

	if (LoadAssets())
	{
		StartBenchmark();
	}
}

void AP6BenchmarkGameMode::Tick(float DeltaSeconds)
{
	if (bRunning)
	{
		float CurrentFPS = 1.f / DeltaSeconds;
		FPSSum += CurrentFPS;
		SampleCount++;
	}
}

void AP6BenchmarkGameMode::StartBenchmark()
{
	if (SequencePlayer)
	{
		bRunning       = true;
		CurrentSection = 1;
		FPSSum         = 0.f;
		SampleCount    = 0;

		// Play + trigger the first camera cut
		SequencePlayer->Play();
		OnCameraCut(SequencePlayer->GetActiveCameraComponent());

		SequencePlayer->OnFinished.AddDynamic(this, &AP6BenchmarkGameMode::OnSequenceFinished);
		SequencePlayer->OnCameraCut.AddDynamic(this, &AP6BenchmarkGameMode::OnCameraCut);
	}
}

bool AP6BenchmarkGameMode::LoadAssets()
{
	// Load the benchmark asset
	const FSoftObjectPath BenchmarkPath = FSoftObjectPath(UGameplayStatics::ParseOption(OptionsString, TEXT("benchmark")));
	if (!BenchmarkPath.IsValid())
	{
		FFrame::KismetExecutionMessage(TEXT("Benchmark asset not found"), ELogVerbosity::Warning);
		return false;
	}

	BenchmarkDefinition = TSoftObjectPtr<UP6BenchmarkDefinition>(BenchmarkPath);
	BenchmarkDefinition.LoadSynchronous();
	if (!BenchmarkDefinition.IsValid())
	{
		FFrame::KismetExecutionMessage(TEXT("Failed to load the benchmark definition"), ELogVerbosity::Warning);
		return false;
	}

	if (BenchmarkDefinition->LevelSequences.IsEmpty())
	{
		FFrame::KismetExecutionMessage(TEXT("No level sequences defined"), ELogVerbosity::Warning);
		return false;
	}

	// Level sequence (just the first one)
	const FString CurrentSequence  = BenchmarkDefinition->LevelSequences[0].ToString();
	ULevelSequence* TargetSequence = LoadObject<ULevelSequence>(NULL, *CurrentSequence, NULL, LOAD_None, NULL);
	if (TargetSequence == nullptr)
	{
		FFrame::KismetExecutionMessage(TEXT("Could not load sequence"), ELogVerbosity::Warning);
		return false;
	}

	UWorld* const World = GetWorld();
	SequencePlayer      = ULevelSequencePlayer::CreateLevelSequencePlayer(World, TargetSequence, FMovieSceneSequencePlaybackSettings(), SequenceActor);

	// Scrub to beginning
	FMovieSceneSequencePlaybackParams PlaybackParams = FMovieSceneSequencePlaybackParams();
	FMovieSceneSequencePlayToParams PlayToParams     = FMovieSceneSequencePlayToParams();
	PlaybackParams.Time                              = 0.0;
	PlaybackParams.UpdateMethod                      = EUpdatePositionMethod::Scrub;

	SequencePlayer->PlayTo(PlaybackParams, PlayToParams);

	return true;
}

void AP6BenchmarkGameMode::OnSequenceFinished()
{
	bRunning               = false;
	const float AverageFPS = (SampleCount > 0) ? (FPSSum / SampleCount) : 0.f;

	// Report results
	GEngine->AddOnScreenDebugMessage(
		-1,             // Key: -1 means add a new message each call
		5.0f,           // Duration in seconds
		FColor::Yellow, // Text color
		FString::Printf(TEXT("Average FPS %.2f"), AverageFPS));
}

void AP6BenchmarkGameMode::OnCameraCut(UCameraComponent* CameraComponent)
{
	UP6BenchmarkStatics::TraceBookmark(FString::Format(TEXT("Section #{0}"), { CurrentSection }));
	CurrentSection++;
}

void AP6BenchmarkGameMode::StartFPSChart()
{
	if (PlayerController)
	{
		PlayerController->ConsoleCommand("stat detailed");
	}
	GEngine->StartFPSChart(*GetFPSChartName(), false);
}

void AP6BenchmarkGameMode::StopFPSChart()
{
	GEngine->StopFPSChart(*GetFPSChartName());
}

FString AP6BenchmarkGameMode::GetFPSChartName() const
{
	return TEXT("Benchmark");
}
