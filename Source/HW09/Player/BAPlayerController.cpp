// BAPlayerController.cpp

#include "Player/BAPlayerController.h"

#include "UI/BAChatInput.h"
#include "Kismet/KismetSystemLibrary.h"
#include "HW09.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Game/BAGameMode.h"
#include "BAPlayerState.h"
#include "Net/UnrealNetwork.h"

ABAPlayerController::ABAPlayerController()
{
	bReplicates = true;
}

void ABAPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController() == false)
	{
		return;
	}

	bShowMouseCursor = true;

	if (IsValid(ChatInputWidgetClass) == true)
	{
		ChatInputWidgetInstance = CreateWidget<UBAChatInput>(this, ChatInputWidgetClass);
		if (IsValid(ChatInputWidgetInstance) == true)
		{
			ChatInputWidgetInstance->AddToViewport();

			FInputModeUIOnly InputModeUIOnly;
			InputModeUIOnly.SetWidgetToFocus(ChatInputWidgetInstance->TakeWidget());
			SetInputMode(InputModeUIOnly);

			ChatInputWidgetInstance->SetKeyboardFocus();
		}
	}

	if (IsValid(TimerWidgetClass) == true)
	{
		TimerWidgetInstance = CreateWidget<UUserWidget>(this, TimerWidgetClass);
		if (IsValid(TimerWidgetInstance) == true)
		{
			TimerWidgetInstance->AddToViewport();
		}
	}

	if (IsValid(NotificationTextWidgetClass) == true)
	{
		NotificationTextWidgetInstance = CreateWidget<UUserWidget>(this, NotificationTextWidgetClass);
		if (IsValid(NotificationTextWidgetInstance) == true)
		{
			NotificationTextWidgetInstance->AddToViewport();
		}
	}
}

void ABAPlayerController::SetChatMessageString(const FString& InChatMessageString)
{
	ChatMessageString = InChatMessageString;

	if (IsLocalController() == true)
	{
		ABAPlayerState* BAPS = GetPlayerState<ABAPlayerState>();
		if (IsValid(BAPS) == true)
		{
			FString CombinedMessageString = BAPS->GetPlayerInfoString() + TEXT(": ") + InChatMessageString;

			ServerRPCPrintChatMessageString(CombinedMessageString);
		}
	}
}

void ABAPlayerController::PrintChatMessageString(const FString& InChatMessageString)
{
	HW09FunctionLibrary::MyPrintString(this, InChatMessageString, 10.f);
}

void ABAPlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, NotificationText);
}

void ABAPlayerController::ServerRPCPrintChatMessageString_Implementation(const FString& InChatMessageString)
{
	AGameModeBase* GM = UGameplayStatics::GetGameMode(this);
	if (IsValid(GM) == true)
	{
		ABAGameMode* BAGM = Cast<ABAGameMode>(GM);
		if (IsValid(BAGM) == true)
		{
			BAGM->PrintChatMessageString(this, InChatMessageString);
		}
	}
}

void ABAPlayerController::ClientRPCPrintChatMessageString_Implementation(const FString& InChatMessageString)
{
	PrintChatMessageString(InChatMessageString);
}
