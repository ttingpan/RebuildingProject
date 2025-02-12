#include "StatusEffectItem.h"

AStatusEffectItem::AStatusEffectItem() :
	StatusEffectType(EStatusEffectType::NONE),
	Duration(0.0f)
{
}

void AStatusEffectItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);

	if (Activator && Activator->ActorHasTag("Player"))
	{
		if (UWorld* World = GetWorld())
		{
			if (ASpartaGameState* GameState = World->GetGameState<ASpartaGameState>())
			{
				GameState->ActivateCharacterStatus(StatusEffectType, Duration);
			}
		}
		DestroyItem();
	}
}
