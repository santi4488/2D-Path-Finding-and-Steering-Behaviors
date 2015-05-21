

#pragma once

#include "PaperSpriteActor.h"
#include "Graph.h"
#include "Tile2D.generated.h"

/**
 * 
 */



UCLASS()
class SCROLLER2D_API ATile2D : public APaperSpriteActor
{
	GENERATED_UCLASS_BODY()

	/**Set to true if tile is walkable*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=NavData)
	bool IsWalkable;

	/**Weight of the edge*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=NavData)
	float Weight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = NavData)
	TSubobjectPtr<UBoxComponent> BoxComponent;

	
};
