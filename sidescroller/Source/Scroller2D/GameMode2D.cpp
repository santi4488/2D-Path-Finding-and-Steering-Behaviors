

#include "Scroller2D.h"
#include "GameMode2D.h"
#include "Character2D.h"


AGameMode2D::AGameMode2D(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	//set default to blueprint character
	static ConstructorHelpers::FObjectFinder<UBlueprint> PlayerPawnObject(TEXT("/Game/Character/CharacterBlueprints/BP_Link.BP_Link"));
	DefaultPawnClass = (UClass*)PlayerPawnObject.Object->GeneratedClass;
}


