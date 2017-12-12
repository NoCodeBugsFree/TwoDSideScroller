// Fill out your copyright notice in the Description page of Project Settings.

#include "Projectile.h"
#include "Components/SphereComponent.h"
#include "PaperFlipbookComponent.h"
#include "ConstructorHelpers.h"
#include "PaperFlipbook.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "ProjectileImpactFX.h"

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	/** root  */
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ROOT"));
	SetRootComponent(SceneComponent);

	/** sphere collision  */
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->SetupAttachment(RootComponent);
	SphereComponent->SetSphereRadius(4.f);
	
	/** paper flipbook component  */
	PaperFlipbookComponent = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("PaperFlipbookComponent"));
	PaperFlipbookComponent->SetupAttachment(RootComponent);
	PaperFlipbookComponent->SetRelativeLocation(FVector(-3.f, 0.f, 0.f));

	/** ProjectileMovementComponent  */
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->InitialSpeed = 300.f;
	ProjectileMovementComponent->MaxSpeed = 300.f;
	ProjectileMovementComponent->ProjectileGravityScale = 0.f;

	/** set flipbook  */
	static ConstructorHelpers::FObjectFinder<UPaperFlipbook> ProjectileFlipbookTemplate(TEXT("/Game/TwoDSideScroller/MySprites/Anims/Projectile"));
	if (ProjectileFlipbookTemplate.Succeeded())
	{
		PaperFlipbookComponent->SetFlipbook(ProjectileFlipbookTemplate.Object);
	}

	/**  Explosion FX Template */
	static ConstructorHelpers::FClassFinder<AProjectileImpactFX> ExplosionFXTemplateClass(TEXT("/Game/TwoDSideScroller/BP/BP/BP_ProjectileImpactFX"));
	if (ExplosionFXTemplateClass.Class != NULL)
	{
		ExplosionFXTemplate = ExplosionFXTemplateClass.Class;
	}

	/** projectiles not intended to live forever  */
	InitialLifeSpan = 10.f;
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnOverlapBegin);
}

void AProjectile::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	/** simple explode when ends play  */
	SpawnImpactFX();
}

void AProjectile::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	// Other Actor is the actor that triggered the event. Check that is not ourself. 
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && !IsPendingKill())
	{		
		// TODO damage logic
		OtherActor->TakeDamage(ProjectileDamage, FDamageEvent(), nullptr, nullptr);

		SpawnImpactFX();

		Destroy();
	}
}

void AProjectile::SpawnImpactFX()
{
	if (ExplosionFXTemplate)
	{
		UWorld* const World = GetWorld();
		if (World)
		{
			AProjectileImpactFX* Spawned = World->SpawnActor<AProjectileImpactFX>(ExplosionFXTemplate, GetActorTransform());
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ExplosionFXTemplate == NULL"));
	}
}

