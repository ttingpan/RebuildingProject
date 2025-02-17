#include "SpartaPlayerController.h"
#include "SpartaGameState.h"
#include "SpartaGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "HUDWidget.h"
#include "GameOverWidget.h"

ASpartaPlayerController::ASpartaPlayerController()
	: InputMappingContext(nullptr),
	MoveAction(nullptr),
	JumpAction(nullptr),
	LookAction(nullptr),
	SprintAction(nullptr),
	HUDWidgetClass(nullptr),
	HUDWidgetInstance(nullptr),
	MainMenuWidgetClass(nullptr),
	MainMenuWidgetInstance(nullptr),
	GameOverWidgetClass(nullptr),
	GameOverWidgetInstance(nullptr)
{
}

UHUDWidget* ASpartaPlayerController::GetHUDWidget() const
{
	return HUDWidgetInstance;
}

void ASpartaPlayerController::ShowGameHUD()
{
	// 기존 위젯 제거
	RemoveWidget();

	if (BlindEffectWidgetClass)
	{
		BlindEffectWidgetInstance = CreateWidget<UUserWidget>(this, BlindEffectWidgetClass);
		if (BlindEffectWidgetInstance)
		{
			BlindEffectWidgetInstance->AddToViewport();
		}
	}

	if (HUDWidgetClass)
	{
		HUDWidgetInstance = CreateWidget<UHUDWidget>(this, HUDWidgetClass);
		if (HUDWidgetInstance)
		{
			HUDWidgetInstance->AddToViewport();

			HUDWidgetInstance->SlowProgress->SetPercent(0.0f);
			HUDWidgetInstance->ReverseControlProgress->SetPercent(0.0f);
			HUDWidgetInstance->BlindProgress->SetPercent(0.0f);

			bShowMouseCursor = false;
			SetInputMode(FInputModeGameOnly());
		}

		ASpartaGameState* SpartaGameState = GetWorld() ? GetWorld()->GetGameState<ASpartaGameState>() : nullptr;
		if (SpartaGameState)
		{
			SpartaGameState->UpdateHUD();
		}
	}
}

UUserWidget* ASpartaPlayerController::GetBlindEffectWidget() const
{
	return BlindEffectWidgetInstance;
}

void ASpartaPlayerController::ShowGameOver()
{
	// 기존 위젯 제거
	RemoveWidget();

	if (GameOverWidgetClass)
	{
		GameOverWidgetInstance = CreateWidget<UGameOverWidget>(this, GameOverWidgetClass);
		if (GameOverWidgetInstance)
		{
			GameOverWidgetInstance->AddToViewport();

			bShowMouseCursor = true;
			SetInputMode(FInputModeUIOnly());

			if (USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(UGameplayStatics::GetGameInstance(this)))
			{
				GameOverWidgetInstance->TotalScoreText->SetText(FText::FromString(FString::Printf(TEXT("%d"), SpartaGameInstance->TotalScore)));
			}
		}

		UFunction* PlayAnimFunc = GameOverWidgetInstance->FindFunction(FName("PlayGameOverAnim"));
		if (PlayAnimFunc)
		{
			GameOverWidgetInstance->ProcessEvent(PlayAnimFunc, nullptr);
		}
	}
}

void ASpartaPlayerController::GoMainMenu()
{
	if (!MainMenuLevelName.IsNone())
	{
		UGameplayStatics::OpenLevel(GetWorld(), MainMenuLevelName);
		SetPause(false);
	}
}

void ASpartaPlayerController::ShowMainMenu()
{
	// 기존 위젯 제거
	RemoveWidget();

	if (MainMenuWidgetClass)
	{
		MainMenuWidgetInstance = CreateWidget<UUserWidget>(this, MainMenuWidgetClass);
		if (MainMenuWidgetInstance)
		{
			MainMenuWidgetInstance->AddToViewport();

			bShowMouseCursor = true;
			SetInputMode(FInputModeUIOnly());
		}
	}
}

void ASpartaPlayerController::StartGame()
{
	if (USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		SpartaGameInstance->CurrentWaveIndex = 0;
		SpartaGameInstance->TotalScore = 0;
	}

	if (!MainLevelName.IsNone())
	{
		UGameplayStatics::OpenLevel(GetWorld(), MainLevelName);
		SetPause(false);
	}
}

void ASpartaPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (InputMappingContext)
			{
				Subsystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}

	// 현재 레벨이 메인 메뉴 레벨인 경우
	if (!MainMenuLevelName.IsNone() && UGameplayStatics::GetCurrentLevelName(GetWorld()) == MainMenuLevelName)
	{
		ShowMainMenu();
	}
}

void ASpartaPlayerController::RemoveWidget()
{
	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->RemoveFromParent();
		HUDWidgetInstance = nullptr;
	}

	if (BlindEffectWidgetInstance)
	{
		BlindEffectWidgetInstance->RemoveFromParent();
		BlindEffectWidgetInstance = nullptr;
	}

	if (MainMenuWidgetInstance)
	{
		MainMenuWidgetInstance->RemoveFromParent();
		MainMenuWidgetInstance = nullptr;
	}

	if (GameOverWidgetInstance)
	{
		GameOverWidgetInstance->RemoveFromParent();
		GameOverWidgetInstance = nullptr;
	}
}
