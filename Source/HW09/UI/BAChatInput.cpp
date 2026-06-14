// BAChatInput.cpp

#include "UI/BAChatInput.h"

#include "Components/EditableTextBox.h"
#include "Player/BAPlayerController.h"

void UBAChatInput::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(EditableTextBox_ChatInput) == true)
	{
		if (EditableTextBox_ChatInput->OnTextCommitted.IsAlreadyBound(this, &ThisClass::OnChatInputTextCommitted) == false)
		{
			EditableTextBox_ChatInput->OnTextCommitted.AddDynamic(this, &ThisClass::OnChatInputTextCommitted);
		}

		EditableTextBox_ChatInput->SetKeyboardFocus();
	}
}

void UBAChatInput::NativeDestruct()
{
	Super::NativeDestruct();

	if (IsValid(EditableTextBox_ChatInput) == true)
	{
		if (EditableTextBox_ChatInput->OnTextCommitted.IsAlreadyBound(this, &ThisClass::OnChatInputTextCommitted) == true)
		{
			EditableTextBox_ChatInput->OnTextCommitted.RemoveDynamic(this, &ThisClass::OnChatInputTextCommitted);
		}
	}
}

void UBAChatInput::OnChatInputTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		APlayerController* OwningPlayerController = GetOwningPlayer();
		if (IsValid(OwningPlayerController) == true)
		{
			ABAPlayerController* OwningBAPlayerController = Cast<ABAPlayerController>(OwningPlayerController);
			if (IsValid(OwningBAPlayerController) == true)
			{
				OwningBAPlayerController->SetChatMessageString(Text.ToString());

				if (IsValid(EditableTextBox_ChatInput) == true)
				{
					EditableTextBox_ChatInput->SetText(FText());
					EditableTextBox_ChatInput->SetKeyboardFocus();
				}
			}
		}
	}
}
