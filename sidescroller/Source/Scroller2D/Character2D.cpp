

#include "Scroller2D.h"
#include "Character2D.h"
#include "PaperFlipbookComponent.h"


ACharacter2D::ACharacter2D(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	//Setup the assets
	struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> RunAnimationAsset;
		ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> IdleAnimationAsset;
		FConstructorStatics()
			: RunAnimationAsset(TEXT("/Game/Character/Flipbooks/LinkWalkRight.LinkWalkRight"))
			, IdleAnimationAsset(TEXT("/Game/Character/Flipbooks/Idle.Idle"))
		{
		}

	};
	static FConstructorStatics ConstructorStatics;

	RunAnimation = ConstructorStatics.RunAnimationAsset.Get();
	IdleAnimation = ConstructorStatics.IdleAnimationAsset.Get();
	Sprite->SetFlipbook(IdleAnimation);
	

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	//set the size of our collision capsule
	CapsuleComponent->SetCapsuleHalfHeight(16.0f);
	CapsuleComponent->SetCapsuleRadius(16.0f);

	//Create a camera boom attached to the root
	CameraBoom = PCIP.CreateDefaultSubobject<USpringArmComponent>(this, TEXT("CameraBoom"));
	CameraBoom->AttachTo(RootComponent);
	CameraBoom->TargetArmLength = 20.0f;
	CameraBoom->SocketOffset = FVector(0.0f, 0.0f, 0.0f);
	CameraBoom->bAbsoluteRotation = true;
	CameraBoom->RelativeRotation = FRotator(0.0f, -90.0f, 0.0f);

	//Create an orthographic camera (no perspective) and attach it to the boom
	TopViewCameraComponent = PCIP.CreateDefaultSubobject<UCameraComponent>(this, TEXT("TopViewCamera"));
	TopViewCameraComponent->ProjectionMode = ECameraProjectionMode::Orthographic;
	TopViewCameraComponent->OrthoWidth = 128.0f;
	TopViewCameraComponent->AttachTo(CameraBoom, USpringArmComponent::SocketName);

	CameraBoom->bAbsoluteRotation = true;
	TopViewCameraComponent->bUseControllerViewRotation = false;
	CharacterMovement->bOrientRotationToMovement = false;

	CharacterMovement->GravityScale = 0.0f;
	CharacterMovement->GroundFriction = MAX_FLT;
	CharacterMovement->MaxWalkSpeed = 20.0f;
	CharacterMovement->MaxFlySpeed = 20.0f;
	
	//Lock character motion onto the XZ plane, so the character can't move in or out of the screen
	CharacterMovement->bConstrainToPlane = true;
	CharacterMovement->SetPlaneConstraintNormal(FVector(0.0f, 1.0f, 0.0f));


	CharacterMovement->bUseFlatBaseForFloorChecks = true;
	CharacterMovement->DefaultLandMovementMode = MOVE_Flying;

	Sprite->SetIsReplicated(true);
	bReplicates = true;
	Sprite->SetEnableGravity(false);
	CapsuleComponent->SetEnableGravity(false);


}

void ACharacter2D::UpdateAnimation(bool bRunningUp)
{
	const FVector PlayerVelocity = GetVelocity();
	const float PlayerSpeed = PlayerVelocity.Size();

	UPaperFlipbook* DesiredAnimation = IdleAnimation;
	
	DesiredAnimation = (PlayerSpeed > 0.0f) ? RunAnimation : IdleAnimation;
	Sprite->SetFlipbook(DesiredAnimation);
}

void ACharacter2D::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	InputComponent->BindAxis("MoveRight", this, &ACharacter2D::MoveRight);
	InputComponent->BindAxis("MoveUp", this, &ACharacter2D::MoveUp);

}

void ACharacter2D::MoveRight(float Value)
{
	UpdateAnimation(false);

	if (Controller != nullptr)
	{
		if (Value < 0.0f)
		{
			Controller->SetControlRotation(FRotator(0.0f, 180.0f, 0.0f));
		}
		else if (Value > 0.0f)
		{
			Controller->SetControlRotation(FRotator(0.0f, 0.0f, 0.0f));
		}
	}

	AddMovementInput(FVector(1.0f, 0.0f, 0.0f), Value);
}

void ACharacter2D::MoveUp(float Value)
{
	UpdateAnimation(true);

	AddMovementInput(FVector(0.0f, 0.0f, 1.0f), Value);
}

