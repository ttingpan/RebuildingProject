#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SpartaPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class UHUDWidget;
class UGameOverWidget;

UCLASS()
class CH3_1_API ASpartaPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ASpartaPlayerController();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputMappingContext* InputMappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* JumpAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* LookAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* SprintAction;
	
	// HUD
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
	TSubclassOf<UHUDWidget> HUDWidgetClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HUD")
	UHUDWidget* HUDWidgetInstance;

	// Blind Effect
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlindEffect")
	TSubclassOf<UUserWidget> BlindEffectWidgetClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "BlindEffect")
	UUserWidget* BlindEffectWidgetInstance;

	// MainMenu
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MainMenu")
	TSubclassOf<UUserWidget> MainMenuWidgetClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MainMenu")
	UUserWidget* MainMenuWidgetInstance;

	// GameOver
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameOver")
	TSubclassOf<UGameOverWidget> GameOverWidgetClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "GameOver")
	UGameOverWidget* GameOverWidgetInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
	FName MainLevelName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
	FName MainMenuLevelName;

	// HUD
	UFUNCTION(BlueprintPure, Category = "HUD")
	UHUDWidget* GetHUDWidget() const;
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void ShowGameHUD();

	// Blind Effect
	UFUNCTION(BlueprintPure, Category = "HUD")
	UUserWidget* GetBlindEffectWidget() const;

	// MainMenu
	UFUNCTION(BlueprintCallable, Category = "MainMenu")
	void ShowMainMenu();
	UFUNCTION(BlueprintCallable, Category = "MainMenu")
	void StartGame();
	
	// GameOver
	UFUNCTION(BlueprintCallable, Category = "GameOver")
	void ShowGameOver();
	UFUNCTION(BlueprintCallable, Category = "GameOver")
	void GoMainMenu();

	virtual void BeginPlay() override;
	
	void RemoveWidget();
};
