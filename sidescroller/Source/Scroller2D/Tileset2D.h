

#pragma once

#include "PaperSpriteActor.h"
#include "Tile2D.h"
#include "Graph.h"
#include "Tileset2D.generated.h"

/**
 * 
 */
UCLASS()
class SCROLLER2D_API ATileset2D : public APaperSpriteActor
{
	GENERATED_UCLASS_BODY()


private:
	TArray<FString> MapStrings;


	int32 rows;
	int32 columns;

public:
	
	bool bPathfindingInUse;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Navigation)
	bool bShowGrid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Navigation)
	FColor GridColor;

	virtual void Tick(float DeltaSeconds) override;

	/**Top left corner boundary tile*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = BPClasses)
	UClass* TopLeftBoundary;

	/**Top boundary*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = BPClasses)
	UClass* TopBoundary;

	/**Top right corner boundary*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = BPClasses)
	UClass* TopRightBoundary;

	/**Left Boundary*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = BPClasses)
	UClass* LeftBoundary;

	/**Maze boundary*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = BPClasses)
	UClass* MazeBoundary;

	/**Right boundary*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = BPClasses)
	UClass* RightBoundary;

	/**Bottom left corner boundary*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = BPClasses)
	UClass* BottomLeftBoundary;

	/**Bottom boundary*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = BPClasses)
	UClass* BottomBoundary;

	/**Bottom right boundary*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = BPClasses)
	UClass* BottomRightBoundary;

	/**Maze floor*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = BPClasses)
	UClass* MazeFloor;

	/**The name of the map file to load*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Files)
	FString MapFileName;
	
	/**Offset for tiles to be placed*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Tiles)
	float Offset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Map)
	bool IsGenerated;

	UFUNCTION(BlueprintCallable, Category=Map)
	void LoadMapFile();
	
	UFUNCTION()
	void CreateMap();

	UFUNCTION()
	void AddEdges();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Navigation)
	bool bTrimPath;

	//trims the path of unnecessary nodes
	void TrimPath(TArray<FVector> & path);
	
	void AddVertex(FVector Loc);

	/**Function to find best path on the navigation mesh*/
	UFUNCTION(BlueprintCallable, Category = Navigation)
	void FindPath(FVector start, FVector end, TArray<FVector> & path);



	void ShowPath(FVector start, FVector end);

	UFUNCTION()
	void CreateBorders();

	//Graph<Graph<FVector, FString>*, FString> Grid;
	Graph Grid;

};
