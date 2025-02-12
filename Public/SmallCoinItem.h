#pragma once

#include "CoreMinimal.h"
#include "CoinItem.h"
#include "SmallCoinItem.generated.h"

UCLASS()
class CH3_1_API ASmallCoinItem : public ACoinItem
{
	GENERATED_BODY()

public:
	ASmallCoinItem();

protected:

	virtual void ActivateItem(AActor* Activator) override;
};
