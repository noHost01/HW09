// BAPawn.cpp


#include "Player/BAPawn.h"
#include "HW09.h"

// Called when the game starts or when spawned
void ABAPawn::BeginPlay()
{
	Super::BeginPlay();
	
	FString NetRoleString = HW09FunctionLibrary::GetRoleString(this);
	FString CombinedString = FString::Printf(TEXT("BAPawn::BeginPlay() %s [%s]"), *HW09FunctionLibrary::GetNetModeString(this), *NetRoleString);
	HW09FunctionLibrary::MyPrintString(this, CombinedString, 10.f);
}

void ABAPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	FString NetRoleString = HW09FunctionLibrary::GetRoleString(this);
	FString CombinedString = FString::Printf(TEXT("BAPawn::PossessedBy() %s [%s]"), *HW09FunctionLibrary::GetNetModeString(this), *NetRoleString);
	HW09FunctionLibrary::MyPrintString(this, CombinedString, 10.f);
}

