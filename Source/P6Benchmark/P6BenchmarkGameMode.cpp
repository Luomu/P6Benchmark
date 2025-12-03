// (C) 2025 Kimmo Kotajärvi <kimmo.kotajarvi@gmail.com>

#include "P6BenchmarkGameMode.h"

#include "Blueprint/UserWidget.h"
// #include "Engine/AssetManager.h"
// #include "Engine/StreamableManager.h"
#include "Kismet/GameplayStatics.h"
// #include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"
#include "P6BenchmarkDefinition.h"
#include "P6BenchmarkSettings.h"
#include "P6BenchmarkStatics.h"
#include "Scalability.h"

#define GET_CONFIG_VALUE(a) (GetDefault<UP6BenchmarkSettings>()->a)

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
		const bool bHideHUD         = false; // true;
		const bool bPreventMovement = true;
		const bool bPreventTurning  = true;
		PlayerController->SetCinematicMode(bCinematicMode, bHidePlayer, bHideHUD, bPreventMovement, bPreventTurning);
	}

	if (LoadAssets())
	{
		StartBenchmark();
	}
}

void AP6BenchmarkGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// early exit?
	if (bRunning)
	{
		StopFPSChart();
	}

	Super::EndPlay(EndPlayReason);
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
	BenchmarkResult = FP6BenchmarkResult();

	// Run the built in scalability benchmark to get a cpu/gpu perf index
	const Scalability::FQualityLevels ScalabilityBenchmark = Scalability::BenchmarkQualityLevels();
	BenchmarkResult.CPUBenchmarkIndex                      = ScalabilityBenchmark.CPUBenchmarkResults;
	BenchmarkResult.GPUBenchmarkIndex                      = ScalabilityBenchmark.GPUBenchmarkResults;

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

		StartFPSChart();
	}
}

void AP6BenchmarkGameMode::PresentResults_Implementation(const FP6BenchmarkResult& Result)
{
	if (PlayerController)
	{
		PlayerController->SetShowMouseCursor(true);
		PlayerController->SetInputMode(FInputModeUIOnly());
	}
	// By default, show the default results widget
	const FSoftClassPath WidgetClassName = GET_CONFIG_VALUE(ResultsWidgetClass);
	TSubclassOf<UUserWidget> WidgetClass = WidgetClassName.TryLoadClass<UUserWidget>();
	if (WidgetClass && PlayerController)
	{
		UUserWidget* Widget = CreateWidget(PlayerController, WidgetClass);
		Widget->AddToPlayerScreen(0);
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

	// Load level sequence
	const FString LevelSequenceName = BenchmarkDefinition->LevelSequence.ToString();
	if (LevelSequenceName.IsEmpty())
	{
		FFrame::KismetExecutionMessage(TEXT("No level sequence"), ELogVerbosity::Warning);
		return false;
	}

	ULevelSequence* TargetSequence = LoadObject<ULevelSequence>(NULL, *LevelSequenceName, NULL, LOAD_None, NULL);
	if (TargetSequence == nullptr)
	{
		FFrame::KismetExecutionMessage(TEXT("Could not load sequence"), ELogVerbosity::Warning);
		return false;
	}

	// Create sequence player
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
	StopFPSChart();
	SequencePlayer->OnFinished.RemoveAll(this);
	SequencePlayer->OnCameraCut.RemoveAll(this);

	bRunning = false;

	const float AverageFPS     = (SampleCount > 0) ? (FPSSum / SampleCount) : 0.f;
	BenchmarkResult.AverageFPS = AverageFPS;

	// Report results
	GEngine->AddOnScreenDebugMessage(
		-1,             // Key: -1 means add a new message each call
		5.0f,           // Duration in seconds
		FColor::Yellow, // Text color
		FString::Printf(TEXT("Average FPS %.2f"), AverageFPS));

	PresentResults(BenchmarkResult);

	// Scrub back to beginning
	FMovieSceneSequencePlaybackParams PlaybackParams = FMovieSceneSequencePlaybackParams();
	FMovieSceneSequencePlayToParams PlayToParams     = FMovieSceneSequencePlayToParams();
	PlaybackParams.Time                              = 0.0;
	PlaybackParams.UpdateMethod                      = EUpdatePositionMethod::Scrub;
	SequencePlayer->PlayTo(PlaybackParams, PlayToParams);
	SequencePlayer->Stop();
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
	if (PlayerController)
	{
		PlayerController->ConsoleCommand("stat detailed");
	}
	GEngine->StopFPSChart(*GetFPSChartName());
}

FString AP6BenchmarkGameMode::GetFPSChartName() const
{
	return TEXT("Benchmark");
}
