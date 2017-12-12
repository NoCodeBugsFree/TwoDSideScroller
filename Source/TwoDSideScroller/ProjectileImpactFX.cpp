// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectileImpactFX.h"
#include "PaperFlipbookComponent.h"
#include "ConstructorHelpers.h"
#include "PaperFlipbook.h"

// Sets default values
AProjectileImpactFX::AProjectileImpactFX()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	/**  Paper Flipbook Component */
	PaperFlipbookComponent = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("PaperFlipbookComponent"));
	SetRootComponent(PaperFlipbookComponent);
	
	/** set flipbook  */
	static ConstructorHelpers::FObjectFinder<UPaperFlipbook> ProjectileFlipbookTemplate(TEXT("/Game/TwoDSideScroller/MySprites/Anims/Impact"));
	if (ProjectileFlipbookTemplate.Succeeded())
	{
		PaperFlipbookComponent->SetFlipbook(ProjectileFlipbookTemplate.Object);
	}
}

void AProjectileImpactFX::BeginPlay()
{
	Super::BeginPlay();

	/** disable looping for valid delegate work  */
	PaperFlipbookComponent->SetLooping(false);

	/** bind  OnFinishedPlaying delegate to destroy this actor after animation ends */
	PaperFlipbookComponent->OnFinishedPlaying.AddDynamic(this, &AProjectileImpactFX::DestroyOnFinishPlay);
}

void AProjectileImpactFX::DestroyOnFinishPlay()
{
	Destroy();
}
