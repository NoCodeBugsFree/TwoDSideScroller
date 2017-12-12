// Fill out your copyright notice in the Description page of Project Settings.

#include "Spikes.h"
#include "PaperSpriteComponent.h"
#include "PaperSprite.h"
#include "ConstructorHelpers.h"
#include "Components/BoxComponent.h"
#include "SideScrollerCharacter.h"

// Sets default values
ASpikes::ASpikes()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	/** Paper Sprite Component  */
	PaperSpriteComponent = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Paper Sprite Component"));
	SetRootComponent(PaperSpriteComponent);

	/** set sprite  */
	static ConstructorHelpers::FObjectFinder<UPaperSprite> PaperSpriteTemplate(TEXT("/Game/TwoDSideScroller/Sprites/Environment/Env_Tile__65"));
	if (PaperSpriteTemplate.Succeeded())
	{
		PaperSpriteComponent->SetSprite(PaperSpriteTemplate.Object);
	}

	/* Box Collision Component  */
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetupAttachment(RootComponent);
	BoxComponent->SetBoxExtent(FVector(16.f, 4.f, 7.f));
	BoxComponent->SetRelativeLocation(FVector(0.f, 0.f, 16.f));
}

// Called when the game starts or when spawned
void ASpikes::BeginPlay()
{
	Super::BeginPlay();
	
	/** bind box overlap delegate  */
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ASpikes::OnOverlapBegin);	
}

void ASpikes::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	// Other Actor is the actor that triggered the event. Check that is not ourself. 
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && !IsPendingKill())
	{
		/** hit character  */
		if(ASideScrollerCharacter* SideScrollerCharacter = Cast<ASideScrollerCharacter>(OtherActor))
		{
			/** only if he is alive  */
			EAnimState State = SideScrollerCharacter->GetState();
			if(State != EAnimState::Dead)
			{
				/** simple decrease health  */
				SideScrollerCharacter->TakeDamage(Damage, FDamageEvent(), nullptr, this);

				/** handle dead or hit state according current health  */
				SideScrollerCharacter->GetHit();
			}
		}
	}
}
