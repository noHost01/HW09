// BAGameStateBase.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "BAGameStateBase.generated.h"

/**
 * 
 */
UCLASS()
class HW09_API ABAGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCBroadcastLoginMessage(const FString& InNameString = FString(TEXT("XXXXXXX")));

public:
	UPROPERTY(Replicated, BlueprintReadOnly)
	int32 RemainingTurnTime = 0;
};
