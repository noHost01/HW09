// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BAPlayerController.generated.h"

class UBAChatInput;

/**
 * 
 */
UCLASS()
class HW09_API ABAPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	void SetChatMessageString(const FString& InChatMessageString);

	void PrintChatMessageString(const FString& InChatMessageString);

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UBAChatInput> ChatInputWidgetClass;

	UPROPERTY()
	TObjectPtr<UBAChatInput> ChatInputWidgetInstance;
	
	FString ChatMessageString;
};
