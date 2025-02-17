#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "SpartaGameState.generated.h"

class ASpartaCharacter;
class ASpartaPlayerController;

UENUM()
enum class EStatusEffectType
{
	NONE,
	SLOW,
	REVERSECONTROL,
	BLIND
};

UCLASS()
class CH3_1_API ASpartaGameState : public AGameState
{
	GENERATED_BODY()
	
public:
	ASpartaGameState();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintPure, Category = "Score")
	int32 GetScore() const;
	UFUNCTION(BlueprintCallable, Category = "Score")
	void AddScore(int32 Amount);
	UFUNCTION(BlueprintCallable, Category = "Wave")
	void OnGameOver();

	UFUNCTION(BlueprintCallable, Category = "CharacterStatus")
	void ActivateCharacterStatus(EStatusEffectType Type, float Duration);

	void PrepareWave();
	void StartWave();
	void OnWaveTimeUp();
	void EndWave();
	void OnCoinCollected();
	void UpdateHUD();
	float GetStatusEffectRemainingPercent(EStatusEffectType Type) const;

	// 스폰된 액터들 모두 파괴(아이템 및 함정 등)
	void DestroyAllSpawnedActors();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Score")
	int32 Score;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coin")
	int32 SpawnedCoinCount;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coin")
	int32 CollectedCoinCount;

	// Wave
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave")
	float WaveDuration;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave")
	float NextWaveStartDelay;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave")
	int32 CurrentWaveIndex;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave")
	int32 MaxWaves;

	FTimerHandle WaveTimerHandle;
	FTimerHandle HUDUpdateTimerHandle;
	FTimerHandle NextWaveStartTimerHadle;

	TArray<AActor*> FoundVolumes;
	// 스폰 볼륨에 의해 스폰된 액터들(아이템 및 함정 등)
	TArray<AActor*> SpawnedActors;

	ASpartaPlayerController* SpartaPlayerController;
	ASpartaCharacter* PlayerCharacter;

	TMap<EStatusEffectType, FTimerHandle> CharacterStatus;
};
