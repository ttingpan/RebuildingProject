#include "SpartaGameState.h"
#include "Kismet/GameplayStatics.h"
#include "SpawnVolume.h"
#include "CoinItem.h"
#include "SpartaGameInstance.h"
#include "SpartaPlayerController.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Blueprint/UserWidget.h"
#include "HUDWidget.h"
#include "SpartaCharacter.h"

ASpartaGameState::ASpartaGameState()
{
	Score = 0;
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;
	WaveDuration = 30.0f;
	NextWaveStartDelay = 3.0f;
	CurrentWaveIndex = 0;
	MaxWaves = 0;

	SpartaPlayerController = nullptr;
	PlayerCharacter = nullptr;

	CharacterStatus.Emplace(EStatusEffectType::SLOW, FTimerHandle());
	CharacterStatus.Emplace(EStatusEffectType::REVERSECONTROL, FTimerHandle());
	CharacterStatus.Emplace(EStatusEffectType::BLIND, FTimerHandle());
}

void ASpartaGameState::BeginPlay()
{
	Super::BeginPlay();

	PrepareWave();

	GetWorldTimerManager().SetTimer(
		HUDUpdateTimerHandle,
		this,
		&ASpartaGameState::UpdateHUD,
		0.1f,
		true
	);
}

int32 ASpartaGameState::GetScore() const
{
	return Score;
}

void ASpartaGameState::AddScore(int32 Amount)
{
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance))
		{
			SpartaGameInstance->AddToScore(Amount);
		}
	}
}

void ASpartaGameState::OnGameOver()
{
	// 웨이브 시작 알림
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Magenta, FString::Printf(TEXT("GAME OVER!!!")));

	if (SpartaPlayerController)
	{
		SpartaPlayerController->SetPause(true);
		SpartaPlayerController->ShowGameOver();
	}
}

void ASpartaGameState::ActivateCharacterStatus(EStatusEffectType Type, float Duration)
{
	if (!PlayerCharacter) return;

	FTimerHandle& CharacterStatusTimerHandle = CharacterStatus[Type];

	FTimerManager& TimerManager = GetWorld()->GetTimerManager();

	FTimerDelegate TimerDel;

	// 이전 지속시간 초기화
	TimerManager.ClearTimer(CharacterStatusTimerHandle);

	// 새로 적용
	switch (Type)
	{
	case EStatusEffectType::SLOW:
	{
		PlayerCharacter->ActivateSlowing(true);
		TimerDel.BindUFunction(PlayerCharacter, FName("ActivateSlowing"), false);
	}
	break;
	case EStatusEffectType::REVERSECONTROL:
	{
		PlayerCharacter->ActivateReverseControl(true);
		TimerDel.BindUFunction(PlayerCharacter, FName("ActivateReverseControl"), false);
	}
	break;
	case EStatusEffectType::BLIND:
	{
		if (SpartaPlayerController)
		{
			if (UUserWidget* BlindEffectWidget = SpartaPlayerController->GetBlindEffectWidget())
			{
				UFunction* ActivateAnimFunc = BlindEffectWidget->FindFunction(FName("ActivateBlindEffect"));
				
				if (ActivateAnimFunc)
				{
					BlindEffectWidget->ProcessEvent(ActivateAnimFunc, nullptr);

					TimerDel.BindLambda(
						[BlindEffectWidget]()
						{
							UFunction* DeactivateAnimFunc = BlindEffectWidget->FindFunction(FName("DeactivateBlindEffect"));
							BlindEffectWidget->ProcessEvent(DeactivateAnimFunc, nullptr);
						});
				}
			}
		}
	}
	break;
	case EStatusEffectType::NONE:
		// falls through(통과)
	default:
		break;
	}

	// 바운드 여부 확인
	if (TimerDel.IsBound())
	{
		// 종료 타이머 실행
		TimerManager.SetTimer(
			CharacterStatusTimerHandle,
			TimerDel,
			Duration,
			false
		);
	}
}

void ASpartaGameState::PrepareWave()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController);
		if (SpartaPlayerController)
		{
			SpartaPlayerController->ShowGameHUD();
			PlayerCharacter = Cast<ASpartaCharacter>(SpartaPlayerController->GetPawn());
		}
	}


	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance))
		{
			CurrentWaveIndex = SpartaGameInstance->CurrentWaveIndex;
		}
	}

	// 다음 웨이브 시작 딜레이
	GetWorld()->GetTimerManager().SetTimer(
		NextWaveStartTimerHadle,
		this,
		&ASpartaGameState::StartWave,
		NextWaveStartDelay,
		false
	);
}

void ASpartaGameState::StartWave()
{
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);

	if (FoundVolumes.Num() > 0)
	{
		// 웨이브 시작 알림
		//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, FString::Printf(TEXT("Wave %d 시작!"), CurrentWaveIndex + 1));

		ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[0]);

		const int32 ItemToSpawn = SpawnVolume->ItemDataTables[CurrentWaveIndex].SpawnCount;

		if (SpawnVolume)
		{
			MaxWaves = SpawnVolume->ItemDataTables.Num();

			for (int32 i = 0; i < ItemToSpawn; i++)
			{
				AActor* SpawnedActor = SpawnVolume->SpawnRandomItem();
				if (SpawnedActor && SpawnedActor->IsA(ACoinItem::StaticClass()))
				{
					SpawnedCoinCount++;
				}

				SpawnedActors.Add(SpawnedActor);
			}
		}

		GetWorld()->GetTimerManager().SetTimer(
			WaveTimerHandle,
			this,
			&ASpartaGameState::OnWaveTimeUp,
			WaveDuration,
			false
		);
	}
}

void ASpartaGameState::OnWaveTimeUp()
{
	EndWave();
}

void ASpartaGameState::EndWave()
{
	GetWorldTimerManager().ClearTimer(WaveTimerHandle);

	for (auto& [Type, TimerHandle] : CharacterStatus)
	{
		GetWorldTimerManager().ClearTimer(TimerHandle);
	}

	PlayerCharacter->ActivateSlowing(false);
	PlayerCharacter->ActivateReverseControl(false);

	if (UUserWidget* BlindEffectWidget = SpartaPlayerController->GetBlindEffectWidget())
	{
		UFunction* DeactivateAnimFunc = BlindEffectWidget->FindFunction(FName("DeactivateBlindEffect"));

		if (DeactivateAnimFunc)
		{
			BlindEffectWidget->ProcessEvent(DeactivateAnimFunc, nullptr);
		}
	}

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
		if (SpartaGameInstance)
		{
			// 웨이브 종료 알림
			/*GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Magenta, FString::Printf(TEXT("Wave %d 종료!"), CurrentWaveIndex + 1));*/

			AddScore(Score);
			CurrentWaveIndex++;
			SpartaGameInstance->CurrentWaveIndex = CurrentWaveIndex;
		}
	}

	DestroyAllSpawnedActors();

	if (CurrentWaveIndex == MaxWaves)
	{
		OnGameOver();
		return;
	}

	// 다음 웨이브 준비
	PrepareWave();
}

void ASpartaGameState::OnCoinCollected()
{
	CollectedCoinCount++;

	if (SpawnedCoinCount > 0 && CollectedCoinCount >= SpawnedCoinCount)
	{
		EndWave();
	}
}

void ASpartaGameState::UpdateHUD()
{
	if (SpartaPlayerController)
	{
		if (UHUDWidget* HUDWidget = SpartaPlayerController->GetHUDWidget())
		{
			// 점수
			if (UGameInstance* GameInstance = GetGameInstance())
			{
				USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
				if (SpartaGameInstance)
				{
					HUDWidget->ScoreText->SetText(FText::FromString(FString::Printf(TEXT("%d"), SpartaGameInstance->TotalScore)));
				}
			}

			// 웨이브
			HUDWidget->WaveText->SetText(FText::FromString(FString::Printf(TEXT("%d"), CurrentWaveIndex + 1)));
			
			FString PrintString = "";
			// 시간
			if (GetWorldTimerManager().IsTimerActive(NextWaveStartTimerHadle))
			{
				float RemainingNextWaveStartTime = GetWorldTimerManager().GetTimerRemaining(NextWaveStartTimerHadle);

				PrintString = FString::Printf(TEXT("다음 웨이브 시작까지 %.0f초"), RemainingNextWaveStartTime);
			}
			else if(GetWorldTimerManager().IsTimerActive(WaveTimerHandle))
			{
				float RemainingWaveTime = GetWorldTimerManager().GetTimerRemaining(WaveTimerHandle);

				PrintString = FString::Printf(TEXT("남은 시간 : %.1f"), RemainingWaveTime);
			}

			HUDWidget->TimeText->SetText(FText::FromString(PrintString));

			// 프로그래스 위젯 업데이트
			HUDWidget->SlowProgress->SetPercent(GetStatusEffectRemainingPercent(EStatusEffectType::SLOW));
			HUDWidget->ReverseControlProgress->SetPercent(GetStatusEffectRemainingPercent(EStatusEffectType::REVERSECONTROL));
			HUDWidget->BlindProgress->SetPercent(GetStatusEffectRemainingPercent(EStatusEffectType::BLIND));
		}
	}
}

float ASpartaGameState::GetStatusEffectRemainingPercent(EStatusEffectType Type) const
{
	FTimerHandle StatusEffectTimerHandle = CharacterStatus[Type];

	// 타이머가 작동중이지 않으면 0.0f반환
	if (!GetWorldTimerManager().IsTimerActive(StatusEffectTimerHandle)) return 0.0f;

	float RemainingStatusEffectTime = GetWorldTimerManager().GetTimerRemaining(StatusEffectTimerHandle);
	float InitialDelay = GetWorldTimerManager().GetTimerRate(StatusEffectTimerHandle);
	return RemainingStatusEffectTime / InitialDelay;
}

void ASpartaGameState::DestroyAllSpawnedActors()
{
	// 스폰된 엑터가 없을 경우
	if (SpawnedActors.IsEmpty()) return;

	for (AActor* Actor : SpawnedActors)
	{
		// 엑터가 있을 경우
		if (Actor)
		{
			Actor->Destroy();
		}
	}

	// TArray 비우기
	SpawnedActors.Empty();
}


