// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectileImpactFX.generated.h"

UCLASS()
class TWODSIDESCROLLER_API AProjectileImpactFX : public AActor
{
	GENERATED_BODY()

	/** Paper Flipbook Component  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class UPaperFlipbookComponent* PaperFlipbookComponent;
	
protected:
	
	AProjectileImpactFX();	

	virtual void BeginPlay() override;

	/** calls to destroy this actor when animation is finished [by OnFinishedPlaying delegate] */
	UFUNCTION()
	void DestroyOnFinishPlay();
	
};
