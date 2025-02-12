#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HUDWidget.generated.h"

class UTextBlock;
class UProgressBar;

UCLASS()
class CH3_1_API UHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	UTextBlock* ScoreText;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	UTextBlock* WaveText;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	UTextBlock* TimeText;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	UProgressBar* SlowProgress;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	UProgressBar* ReverseControlProgress;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	UProgressBar* BlindProgress;

};
