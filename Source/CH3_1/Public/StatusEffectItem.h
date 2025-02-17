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

	// ȿ�� Ÿ��
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|StatusEffectItem")
	EStatusEffectType StatusEffectType;
	// ȿ�� ���� �ð�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|StatusEffectItem", meta = (ClampMin = "0.0", ClampMax = "10.0"))
	float Duration;
};
