// Fill out your copyright notice in the Description page of Project Settings.

#include "SideScrollerCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "PaperFlipbookComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "ConstructorHelpers.h"
#include "PaperFlipbook.h"
#include "Projectile.h"

ASideScrollerCharacter::ASideScrollerCharacter()
{
	/** camera boom  */
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Boom"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->bDoCollisionTest = false;
	SpringArmComponent->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	
	/** camera  */
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent);

	// disable auto exposure
	CameraComponent->PostProcessSettings.bOverride_AutoExposureMaxBrightness = true;
	CameraComponent->PostProcessSettings.bOverride_AutoExposureMinBrightness = true;
	CameraComponent->PostProcessSettings.AutoExposureMinBrightness = 1.f;
	CameraComponent->PostProcessSettings.AutoExposureMaxBrightness = 1.f;
	
	//  disable motion blur
	CameraComponent->PostProcessSettings.bOverride_MotionBlurAmount = true;
	CameraComponent->PostProcessSettings.MotionBlurAmount = 0.f;
	
	/** projectile spawn point  */
	ProjectileSpawnPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("Projectile Spawn Point"));
	ProjectileSpawnPoint->SetupAttachment(GetSprite());
	ProjectileSpawnPoint->SetRelativeLocation(FVector(40.f, 0.f, 30.f));

	/** capsule  */
	GetCapsuleComponent()->SetCapsuleHalfHeight(32.f);
	GetCapsuleComponent()->SetCapsuleRadius(22.f);

	/** sprite  */
	GetSprite()->SetRelativeLocation(FVector(0.f, 0.f, -37.f));

	/** character movement  */
	GetCharacterMovement()->GravityScale = 2.f;
	GetCharacterMovement()->JumpZVelocity = 800.f;
	GetCharacterMovement()->AirControl = 0.8f;
	GetCharacterMovement()->GroundFriction = 3.f;
	GetCharacterMovement()->MaxWalkSpeed = 130.f;
	GetCharacterMovement()->MaxStepHeight = 3.f;
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->SetPlaneConstraintNormal(FVector(0.f, -1.f, 0.f));

	/** set all character flip book asset references  */
	// jump
	static ConstructorHelpers::FObjectFinder<UPaperFlipbook> JumpFlipbookTemplate(TEXT("/Game/TwoDSideScroller/MySprites/Anims/Jump"));
	if (JumpFlipbookTemplate.Succeeded())
	{
		JumpFlipbook = JumpFlipbookTemplate.Object;
	}
	// walk
	static ConstructorHelpers::FObjectFinder<UPaperFlipbook> WalkFlipbookTemplate(TEXT("/Game/TwoDSideScroller/MySprites/Anims/Walk"));
	if (WalkFlipbookTemplate.Succeeded())
	{
		WalkFlipbook = WalkFlipbookTemplate.Object;
	}

	// shoot
	static ConstructorHelpers::FObjectFinder<UPaperFlipbook> ShootFlipbookTemplate(TEXT("/Game/TwoDSideScroller/MySprites/Anims/Shoot"));
	if (ShootFlipbookTemplate.Succeeded())
	{
		ShootFlipbook = ShootFlipbookTemplate.Object;
	}

	// idle
	static ConstructorHelpers::FObjectFinder<UPaperFlipbook> IdleFlipbookTemplate(TEXT("/Game/TwoDSideScroller/MySprites/Anims/Idle"));
	if (IdleFlipbookTemplate.Succeeded())
	{
		IdleFlipbook = IdleFlipbookTemplate.Object;
	}

	// dead
	static ConstructorHelpers::FObjectFinder<UPaperFlipbook> DeadFlipbookTemplate(TEXT("/Game/TwoDSideScroller/MySprites/Anims/Dead"));
	if (DeadFlipbookTemplate.Succeeded())
	{
		DeadFlipbook = DeadFlipbookTemplate.Object;
	}

	// hit
	static ConstructorHelpers::FObjectFinder<UPaperFlipbook> HitFlipbookTemplate(TEXT("/Game/TwoDSideScroller/MySprites/Anims/Hit"));
	if (HitFlipbookTemplate.Succeeded())
	{
		HitFlipbook = HitFlipbookTemplate.Object;
	}

	// dying
	static ConstructorHelpers::FObjectFinder<UPaperFlipbook> DyingFlipbookTemplate(TEXT("/Game/TwoDSideScroller/MySprites/Anims/Dying"));
	if (DyingFlipbookTemplate.Succeeded())
	{
		DyingFlipbook = DyingFlipbookTemplate.Object;
	}

	/** set projectile BP template  */
	static ConstructorHelpers::FClassFinder<AProjectile> ProjectileClass(TEXT("/Game/TwoDSideScroller/BP/BP/BP_Projectile"));
	if (ProjectileClass.Class != NULL)
	{
		ProjectileTemplate = ProjectileClass.Class;
	}

	/** init states  */
	bMovingRight = true;
	bCanFire = true;
}

void ASideScrollerCharacter::BeginPlay()
{
	Super::BeginPlay();

	/** set the PC reference  */
	PlayerController = Cast<APlayerController>(GetController());
}

void ASideScrollerCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	if (!ensure(PlayerInputComponent)) { return; }

	/** Jump  */
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASideScrollerCharacter::StartJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	/** Fire  */
	PlayerInputComponent->BindAction("Shoot", IE_Pressed, this, &ASideScrollerCharacter::ShootStart);
	PlayerInputComponent->BindAction("Shoot", IE_Released, this, &ASideScrollerCharacter::ShootStop);

	/**  Move Right */
	PlayerInputComponent->BindAxis("MoveRight", this, &ASideScrollerCharacter::MoveRight);
}

float ASideScrollerCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	if (ActualDamage > 0.f)
	{
		Health -= ActualDamage;
	}

	return ActualDamage;
}

void ASideScrollerCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	
	if (AnimState == EAnimState::Hit)
	{
		return;
	}

	if(AnimState == EAnimState::Dead && DeadFlipbook)
	{
		GetSprite()->SetFlipbook(DeadFlipbook);
	} 
	else
	{
		SetState(EAnimState::Idle);
	}
}

void ASideScrollerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
		
	UpdateAnimation();

	UpdateCharacterDirection();
}

void ASideScrollerCharacter::UpdateAnimation()
{
	/** this states handles another place  */
	if (AnimState == EAnimState::Dead || AnimState == EAnimState::Dying || AnimState == EAnimState::Hit)
	{
		return;
	}

	Velocity = GetCharacterMovement()->Velocity.Size();
	if (Velocity > 0)
	{
		/** jumping  */
		if (AnimState == EAnimState::Jumping)
		{
			GetSprite()->SetFlipbook(JumpFlipbook);
			GetSprite()->SetLooping(true);
		} 
		else
		{
			GetSprite()->SetFlipbook(WalkFlipbook);
			GetSprite()->SetLooping(true);
		}
	} 
	else /**  Velocity == 0 */
	{
		if (AnimState == EAnimState::Shooting)
		{
			GetSprite()->SetFlipbook(ShootFlipbook);
			GetSprite()->SetLooping(true);

			/** shoot only once per shoot animation  */
			if (bCanFire)
			{
				if (GetSprite()->GetFlipbook() == ShootFlipbook)
				{
					if (GetSprite()->GetPlaybackPositionInFrames() == FiringSpriteNumber)
					{
						SpawnProjectile();
						bCanFire = false;
						GetWorldTimerManager().SetTimer(FireCooldownTimer, this, &ASideScrollerCharacter::ResetCanFire, FireCooldown, false);
					}
				}
			}
		} 
		else
		{
			GetSprite()->SetFlipbook(IdleFlipbook);
			GetSprite()->SetLooping(true);
		}
	}
}

void ASideScrollerCharacter::MoveRight(float Value)
{
	if (AnimState == EAnimState::Jumping)
	{
		AddMovementInput(FVector(1.f, 0.f, 0.f), Value);
	}

	if (AnimState == EAnimState::Idle || AnimState == EAnimState::Walking)
	{
		if (Value == 0.f)
		{
			SetState(EAnimState::Idle);
			return;
		}
		else if (Value > 0)
		{
			bMovingRight = true;
			SetState(EAnimState::Walking);
		}
		else
		{
			bMovingRight = false;
			SetState(EAnimState::Walking);
		}
		AddMovementInput(FVector(1.f, 0.f, 0.f), Value);
	}
}

void ASideScrollerCharacter::ShootStart()
{
	if (AnimState != EAnimState::Jumping && AnimState != EAnimState::Shooting)
	{
		SetState(EAnimState::Shooting);
	}
}

void ASideScrollerCharacter::ShootStop()
{
	SetState(EAnimState::Idle);
	GetWorldTimerManager().ClearTimer(FireCooldownTimer);
}

void ASideScrollerCharacter::StartJump()
{
	if(AnimState == EAnimState::Walking || AnimState == EAnimState::Idle)
	{
		Jump();
		SetState(EAnimState::Jumping);
	}
}

void ASideScrollerCharacter::UpdateCharacterDirection()
{
	float VelocityX = GetCharacterMovement()->Velocity.X;
	if (VelocityX < 0)
	{
		GetSprite()->SetRelativeRotation(FRotator(0.f, 180.f, 0.f));
	}
	else if (VelocityX == 0)
	{
		if (bMovingRight)
		{
			GetSprite()->SetRelativeRotation(FRotator(FRotator::ZeroRotator));
		} 
		else
		{
			GetSprite()->SetRelativeRotation(FRotator(0.f, 180.f, 0.f));
		}
	} 
	else // Velocity > 0
	{
		GetSprite()->SetRelativeRotation(FRotator(FRotator::ZeroRotator));
	}
}

void ASideScrollerCharacter::PostChangeSpriteColor()
{
	if (AnimState != EAnimState::Dead)
	{
		SetState(EAnimState::Idle);
		GetSprite()->SetLooping(true);
		EnableInput(PlayerController);
	}
}

void ASideScrollerCharacter::SpawnProjectile()
{
	if (ProjectileTemplate)
	{
		UWorld* const World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = Instigator; //  GetController();
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			FVector SpawnLocation = GetProjectileSpawnPoint()->GetComponentLocation();
			FRotator SpawnRotation = GetProjectileSpawnPoint()->GetComponentRotation();
			
			if (AProjectile* SpawnedProjectile = World->SpawnActor<AProjectile>(ProjectileTemplate, SpawnLocation, SpawnRotation, SpawnParams))
			{
				SpawnedProjectile->SetLifeSpan(ProjectileLifeSpan);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ProjectileTemplate == NULL"));
	}
}

void ASideScrollerCharacter::ResetCanFire()
{
	bCanFire = true;
	GetWorldTimerManager().ClearTimer(FireCooldownTimer);
}

void ASideScrollerCharacter::GetHit()
{
	/** if alive and not just hit  */
	if (AnimState != EAnimState::Dead || AnimState != EAnimState::Hit)
	{
		GetCharacterMovement()->StopMovementImmediately();
		
		float HitForceDirection = 0.f;

		/** Dead  */
		if (Health <= 0.f) 
		{
			SetState(EAnimState::Dead);
			DisableInput(PlayerController);
			if (DyingFlipbook)
			{
				GetSprite()->SetFlipbook(DyingFlipbook);
				GetSprite()->SetLooping(false);
				GetSprite()->PlayFromStart();
				HitForceDirection = -1500000.f;
				ChangeSpriteColor();
			}
		}
		else /** Hit  */
		{
			SetState(EAnimState::Hit);
			if (HitFlipbook)
			{
				GetSprite()->SetFlipbook(HitFlipbook);
				GetSprite()->SetLooping(true);
				GetSprite()->PlayFromStart();
				HitForceDirection = 1500000.f;
				DisableInput(PlayerController);
				ChangeSpriteColor();
			}
		}
		/** find character's direction  */
		bool bCharacterFacingRight = GetSprite()->GetComponentRotation() == FRotator::ZeroRotator;
		if (bCharacterFacingRight)
		{
			GetCharacterMovement()->AddForce(FVector(-HitForceDirection, 0.f, 4000000.f));
		} 
		else
		{
			GetCharacterMovement()->AddForce(FVector(HitForceDirection, 0.f, 4000000.f));
		}
	}
}
