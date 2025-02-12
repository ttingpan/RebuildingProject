#include "SpartaGameInstance.h"

USpartaGameInstance::USpartaGameInstance()
{
	TotalScore = 0;
	CurrentWaveIndex = 0;
}

void USpartaGameInstance::AddToScore(int32 Amount)
{
	TotalScore += Amount;
}
