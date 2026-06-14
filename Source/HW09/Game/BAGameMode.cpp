// BAGameMode.cpp


#include "Game/BAGameMode.h"

#include "BAGameStateBase.h"
#include "Player/BAPlayerController.h"
#include "EngineUtils.h"
#include "Player/BAPlayerState.h"

void ABAGameMode::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);

	ABAPlayerController* BAPlayerController = Cast<ABAPlayerController>(NewPlayer);
	if (IsValid(BAPlayerController) == true)
	{
		BAPlayerController->NotificationText = FText::FromString(TEXT("Connected to the game server."));

		AllPlayerControllers.Add(BAPlayerController);

		ABAPlayerState* BAPS = BAPlayerController->GetPlayerState<ABAPlayerState>();
		if (IsValid(BAPS) == true)
		{
			BAPS->PlayerNameString = TEXT("Player") + FString::FromInt(AllPlayerControllers.Num());
		}

		ABAGameStateBase* BAGameStateBase = GetGameState<ABAGameStateBase>();
		if (IsValid(BAPS) == true && IsValid(BAGameStateBase) == true)
		{
			BAGameStateBase->MulticastRPCBroadcastLoginMessage(BAPS->PlayerNameString);
		}

		CurrentTurnPlayerIndex = 0;
		ResetGame();
		StartTurnTimer();
	}
}

FString ABAGameMode::GenerateSecretNumber()
{
	TArray<int32> Numbers;
	for (int32 i = 1; i <= 9; ++i)
	{
		Numbers.Add(i);
	}

	FMath::RandInit(FDateTime::Now().GetTicks());
	Numbers = Numbers.FilterByPredicate([](int32 Num) { return Num > 0; });

	FString Result;
	for (int32 i = 0; i < 3; ++i)
	{
		int32 Index = FMath::RandRange(0, Numbers.Num() - 1);
		Result.Append(FString::FromInt(Numbers[Index]));
		Numbers.RemoveAt(Index);
	}

	return Result;
}

bool ABAGameMode::IsGuessNumberString(const FString& InNumberString, FString& OutErrorMessage)
{
	OutErrorMessage.Empty();

	if (InNumberString.Len() != 3)
	{
		OutErrorMessage = TEXT("3자리 숫자를 입력해주세요.");
		return false;
	}

	TSet<TCHAR> UniqueDigits;

	for (TCHAR C : InNumberString)
	{
		if (FChar::IsDigit(C) == false)
		{
			OutErrorMessage = TEXT("숫자만 입력해주세요.");
			return false;
		}

		if (C == TEXT('0'))
		{
			OutErrorMessage = TEXT("1~9 사이의 숫자만 입력해주세요.");
			return false;
		}

		if (UniqueDigits.Contains(C) == true)
		{
			OutErrorMessage = TEXT("중복되지 않은 숫자를 입력해주세요.");
			return false;
		}

		UniqueDigits.Add(C);
	}

	return true;
}

FString ABAGameMode::JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString)
{
	int32 StrikeCount = 0, BallCount = 0;

	for (int32 i = 0; i < 3; ++i)
	{
		if (InSecretNumberString[i] == InGuessNumberString[i])
		{
			StrikeCount++;
		}
		else
		{
			FString PlayerGuessChar = FString::Printf(TEXT("%c"), InGuessNumberString[i]);
			if (InSecretNumberString.Contains(PlayerGuessChar))
			{
				BallCount++;
			}
		}
	}

	if (StrikeCount == 0 && BallCount == 0)
	{
		return TEXT("OUT");
	}

	return FString::Printf(TEXT("%dS%dB"), StrikeCount, BallCount);
}

void ABAGameMode::BeginPlay()
{
	Super::BeginPlay();

	SecretNumberString = GenerateSecretNumber();
	UE_LOG(LogTemp, Error, TEXT("%s"), *SecretNumberString);

	StartTurnTimer();
}

void ABAGameMode::PrintChatMessageString(ABAPlayerController* InChattingPlayerController, const FString& InChatMessageString)
{
	ABAGameStateBase* BAGameStateBase = GetGameState<ABAGameStateBase>();
	if (IsValid(BAGameStateBase) == true && BAGameStateBase->RemainingTurnTime <= 0)
	{
		if (IsValid(InChattingPlayerController) == true)
		{
			InChattingPlayerController->ClientRPCPrintChatMessageString(TEXT("Time over. You cannot enter a number."));
		}

		return;
	}

	if (IsCurrentTurnPlayer(InChattingPlayerController) == false)
	{
		if (IsValid(InChattingPlayerController) == true)
		{
			InChattingPlayerController->ClientRPCPrintChatMessageString(TEXT("It is not your turn."));
		}

		return;
	}

	FString ChatMessageString = InChatMessageString;
	int Index = InChatMessageString.Len() - 3;
	FString GuessNumberString = InChatMessageString.RightChop(Index);
	FString ErrorMessageString;

	if (IsGuessNumberString(GuessNumberString, ErrorMessageString) == true)
	{
		FString JudgeResultString = JudgeResult(SecretNumberString, GuessNumberString);

		IncreaseGuessCount(InChattingPlayerController);

		for (TActorIterator<ABAPlayerController> It(GetWorld()); It; ++It)
		{
			ABAPlayerController* BAPlayerController = *It;
			if (IsValid(BAPlayerController) == true)
			{
				FString CombinedMessageString = InChatMessageString + TEXT(" -> ") + JudgeResultString;
				BAPlayerController->ClientRPCPrintChatMessageString(CombinedMessageString);
			}
		}

		int32 StrikeCount = FCString::Atoi(*JudgeResultString.Left(1));
		JudgeGame(InChattingPlayerController, StrikeCount);
		if (StrikeCount != 3)
		{
			MoveToNextTurn();
		}
	}
	else
	{
		if (IsValid(InChattingPlayerController) == true)
		{
			InChattingPlayerController->ClientRPCPrintChatMessageString(ErrorMessageString);
		}
	}
}

void ABAGameMode::IncreaseGuessCount(ABAPlayerController* InChattingPlayerController)
{
	ABAPlayerState* BAPS = InChattingPlayerController->GetPlayerState<ABAPlayerState>();
	if (IsValid(BAPS) == true)
	{
		BAPS->CurrentGuessCount++;
	}
}

void ABAGameMode::ResetGame()
{
	SecretNumberString = GenerateSecretNumber();
	UE_LOG(LogTemp, Warning, TEXT("SecretNumberString: %s"), *SecretNumberString);

	for (const auto& CXPlayerController : AllPlayerControllers)
	{
		ABAPlayerState* BAPS = CXPlayerController->GetPlayerState<ABAPlayerState>();
		if (IsValid(BAPS) == true)
		{
			BAPS->CurrentGuessCount = 0;
		}
	}
}

void ABAGameMode::JudgeGame(ABAPlayerController* InChattingPlayerController, int InStrikeCount)
{
	if (3 == InStrikeCount)
	{
		ABAPlayerState* BAPS = InChattingPlayerController->GetPlayerState<ABAPlayerState>();
		for (const auto& BAPlayerController : AllPlayerControllers)
		{
			if (IsValid(BAPS) == true)
			{
				FString CombinedMessageString = BAPS->PlayerNameString + TEXT(" has won the game.");
				BAPlayerController->NotificationText = FText::FromString(CombinedMessageString);

				ResetGame();
			}
		}
	}
	else
	{
		bool bIsDraw = true;
		for (const auto& BAPlayerController : AllPlayerControllers)
		{
			ABAPlayerState* BAPS = BAPlayerController->GetPlayerState<ABAPlayerState>();
			if (IsValid(BAPS) == true)
			{
				if (BAPS->CurrentGuessCount < BAPS->MaxGuessCount)
				{
					bIsDraw = false;
					break;
				}
			}
		}

		if (true == bIsDraw)
		{
			for (const auto& BAPlayerController : AllPlayerControllers)
			{
				BAPlayerController->NotificationText = FText::FromString(TEXT("Draw..."));

				ResetGame();
			}
		}
	}
}

void ABAGameMode::StartTurnTimer()
{
	ABAGameStateBase* BAGameStateBase = GetGameState<ABAGameStateBase>();
	if (IsValid(BAGameStateBase) == true)
	{
		BAGameStateBase->RemainingTurnTime = TurnTimeLimit;
	}

	GetWorldTimerManager().ClearTimer(TurnTimerHandle);
	GetWorldTimerManager().SetTimer(
		TurnTimerHandle,
		this,
		&ThisClass::DecreaseTurnTimer,
		1.0f,
		true
	);
}

void ABAGameMode::DecreaseTurnTimer()
{
	ABAGameStateBase* BAGameStateBase = GetGameState<ABAGameStateBase>();
	if (IsValid(BAGameStateBase) == false)
	{
		return;
	}

	BAGameStateBase->RemainingTurnTime--;

	if (BAGameStateBase->RemainingTurnTime <= 0)
	{
		BAGameStateBase->RemainingTurnTime = 0;
		GetWorldTimerManager().ClearTimer(TurnTimerHandle);

		HandleTurnTimeExpired();
	}
}

void ABAGameMode::HandleTurnTimeExpired()
{
	if (AllPlayerControllers.IsValidIndex(CurrentTurnPlayerIndex) == true)
	{
		ABAPlayerController* CurrentTurnPlayerController = AllPlayerControllers[CurrentTurnPlayerIndex];
		if (IsValid(CurrentTurnPlayerController) == true)
		{
			IncreaseGuessCount(CurrentTurnPlayerController);
		}
	}

	for (const auto& BAPlayerController : AllPlayerControllers)
	{
		if (IsValid(BAPlayerController) == true)
		{
			BAPlayerController->ClientRPCPrintChatMessageString(TEXT("Time over. Turn changed."));
		}
	}

	MoveToNextTurn();
}

void ABAGameMode::MoveToNextTurn()
{
	if (AllPlayerControllers.Num() <= 0)
	{
		return;
	}

	CurrentTurnPlayerIndex++;

	if (CurrentTurnPlayerIndex >= AllPlayerControllers.Num())
	{
		CurrentTurnPlayerIndex = 0;
	}

	StartTurnTimer();

	ABAPlayerController* CurrentTurnPlayerController = AllPlayerControllers[CurrentTurnPlayerIndex];
	if (IsValid(CurrentTurnPlayerController) == true)
	{
		ABAPlayerState* BAPS = CurrentTurnPlayerController->GetPlayerState<ABAPlayerState>();
		if (IsValid(BAPS) == true)
		{
			FString TurnMessageString = BAPS->PlayerNameString + TEXT("'s turn.");

			for (const auto& BAPlayerController : AllPlayerControllers)
			{
				if (IsValid(BAPlayerController) == true)
				{
					BAPlayerController->ClientRPCPrintChatMessageString(TurnMessageString);
				}
			}
		}
	}
}

bool ABAGameMode::IsCurrentTurnPlayer(ABAPlayerController* InPlayerController) const
{
	if (IsValid(InPlayerController) == false)
	{
		return false;
	}

	if (AllPlayerControllers.IsValidIndex(CurrentTurnPlayerIndex) == false)
	{
		return false;
	}

	return AllPlayerControllers[CurrentTurnPlayerIndex] == InPlayerController;
}
