

#pragma once

#include "PaperCharacter.h"
#include "Character2D.generated.h"

/**
 * 
 */
UCLASS()
class SCROLLER2D_API ACharacter2D : public APaperCharacter
{
	GENERATED_UCLASS_BODY()

	/** Top View Camera*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	TSubobjectPtr<class UCameraComponent> TopViewCameraComponent;

	/** Camera boom positioning the camera on top of the character*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	TSubobjectPtr<class USpringArmComponent> CameraBoom;

protected:
	//Animation for running
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Animations)
	class UPaperFlipbook* RunAnimation;

	//Animation for idle
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* IdleAnimation;

	/** Called to choose the correct animation to play based on the charcter's movement state */
	void UpdateAnimation(bool bRunningUp);

	/** Called for the move right input */
	void MoveRight(float Value);

	/** Called for the move left input*/
	void MoveUp(float Value);

	//APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	
	
};
