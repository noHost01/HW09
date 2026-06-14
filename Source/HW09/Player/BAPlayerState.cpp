// BAPlayerState.cpp

#include "Player/BAPlayerState.h"

#include "Net/UnrealNetwork.h"

ABAPlayerState::ABAPlayerState()
	: PlayerNameString(TEXT("None"))
	, CurrentGuessCount(0)
	, MaxGuessCount(3)
{
	bReplicates = true;
}

void ABAPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, PlayerNameString);
	DOREPLIFETIME(ThisClass, CurrentGuessCount);
	DOREPLIFETIME(ThisClass, MaxGuessCount); // Q. MaxGuessCount가 프로퍼티 레플리케이션될 필요가 있을까?
}

FString ABAPlayerState::GetPlayerInfoString()
{
	int32 RemainingGuessCount = MaxGuessCount - CurrentGuessCount;

	return FString::Printf(
		TEXT("%s(%d/%d)"),
		*PlayerNameString,
		RemainingGuessCount,
		MaxGuessCount
	);
}
