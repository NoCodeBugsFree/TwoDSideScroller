// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Spikes.generated.h"

UCLASS()
class TWODSIDESCROLLER_API ASpikes : public AActor
{
	GENERATED_BODY()

	/**  Paper Sprite Component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class UPaperSpriteComponent* PaperSpriteComponent;
	
	/* Box Collision Component  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* BoxComponent;
	
protected:

	// Sets default values for this actor's properties
	ASpikes();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

	/** calls when overlap event happens  */
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	/** damage to cause  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float Damage = 20.f;
	
};
