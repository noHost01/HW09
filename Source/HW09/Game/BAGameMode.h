// BAGameMode.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BAGameMode.generated.h"

class ABAPlayerController;

/**
 * 
 */
UCLASS()
class HW09_API ABAGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void OnPostLogin(AController* NewPlayer) override;

	FString GenerateSecretNumber();

	bool IsGuessNumberString(const FString& InNumberString, FString& OutErrorMessage);

	FString JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString);

	virtual void BeginPlay() override;

	void PrintChatMessageString(ABAPlayerController* InChattingPlayerController, const FString& InChatMessageString);

	void IncreaseGuessCount(ABAPlayerController* InChattingPlayerController);

	void ResetGame();

	void JudgeGame(ABAPlayerController* InChattingPlayerController, int InStrikeCount);
	
protected:
	FString SecretNumberString;

	TArray<TObjectPtr<ABAPlayerController>> AllPlayerControllers;

	UPROPERTY(EditDefaultsOnly)
	int32 TurnTimeLimit = 20;

	FTimerHandle TurnTimerHandle;

	void StartTurnTimer();

	void DecreaseTurnTimer();

	void HandleTurnTimeExpired();

	int32 CurrentTurnPlayerIndex = 0;

	void MoveToNextTurn();

	bool IsCurrentTurnPlayer(ABAPlayerController* InPlayerController) const;
};
