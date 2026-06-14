// BAGameStateBase.cpp

#include "Game/BAGameStateBase.h"

#include "Kismet/GameplayStatics.h"
#include "Player/BAPlayerController.h"
#include "Net/UnrealNetwork.h"

void ABAGameStateBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, RemainingTurnTime);
}

void ABAGameStateBase::MulticastRPCBroadcastLoginMessage_Implementation(const FString& InNameString)
{
	if (HasAuthority() == false)
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (IsValid(PC) == true)
		{
			ABAPlayerController* BAPC = Cast<ABAPlayerController>(PC);
			if (IsValid(BAPC) == true)
			{
				FString NotificationString = InNameString + TEXT(" has joined the game.");
				BAPC->PrintChatMessageString(NotificationString);
			}
		}
	}
}
