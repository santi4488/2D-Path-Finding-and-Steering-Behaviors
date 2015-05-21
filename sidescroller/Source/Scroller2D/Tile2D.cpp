

#include "Scroller2D.h"
#include "Tile2D.h"


#if WITH_EDITOR
bool APaperSpriteActor::GetReferencedContentObjects(TArray<UObject*>& Objects) const
{
	if (UPaperSprite* SourceSprite = RenderComponent->GetSprite())
	{
		Objects.Add(SourceSprite);
	}
	return true;
}
#endif

ATile2D::ATile2D(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	BoxComponent = PCIP.CreateDefaultSubobject<UBoxComponent>(this, "BoxComponent");
	BoxComponent->AttachTo(RootComponent);
	BoxComponent->InitBoxExtent(FVector(1, 32, 32));
}
