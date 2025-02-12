#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "SpartaGameState.h"
#include "StatusEffectItem.generated.h"

UCLASS()
class CH3_1_API AStatusEffectItem : public ABaseItem
{
	GENERATED_BODY()

public:
	AStatusEffectItem();

	virtual void ActivateItem(AActor* Activator) override;

	// 효과 타입
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|StatusEffectItem")
	EStatusEffectType StatusEffectType;
	// 효과 지속 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|StatusEffectItem", meta = (ClampMin = "0.0", ClampMax = "10.0"))
	float Duration;
};
