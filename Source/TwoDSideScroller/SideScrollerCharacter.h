// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperCharacter.h"
#include "SideScrollerCharacter.generated.h"

/** describes all available character's states  */
UENUM(BlueprintType)
enum class EAnimState : uint8
{
	Idle,
	Jumping,
	Walking,
	Shooting,
	Dying,
	Dead,
	Hit
};

/**
 * 
 */
UCLASS()
class TWODSIDESCROLLER_API ASideScrollerCharacter : public APaperCharacter
{
	GENERATED_BODY()

	/** camera  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AAA", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* CameraComponent;
	
	/** camera boom  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AAA", meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* SpringArmComponent;

	/** projectile spawn point  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AAA", meta = (AllowPrivateAccess = "true"))
	class UArrowComponent* ProjectileSpawnPoint;
	
public:

	/** calls to handle dead or hit state according current health  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void GetHit();

	/** Apply damage to this actor.*/
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	
protected:

	ASideScrollerCharacter();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	/**
	* Called upon landing when falling, to perform actions based on the Hit result. Triggers the OnLanded event.
	* Note that movement mode is still "Falling" during this event. Current Velocity value is the velocity at the time of landing.
	* Consider OnMovementModeChanged() as well, as that can be used once the movement mode changes to the new mode (most likely Walking).
	*
	* @param Hit Result describing the landing that resulted in a valid landing spot.
	* @see OnMovementModeChanged()
	*/
	virtual void Landed(const FHitResult& Hit) override;
	
	// -----------------------------------------------------------------------------------
	
	/** sets character state to NewState  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void SetState(EAnimState NewState) { AnimState = NewState; }

	/** call to move character left or right  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void MoveRight(float Value);

	/** calls to start firing  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void ShootStart();

	/** call to stop firing  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void ShootStop();

	/** calls to start jump  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void StartJump();

	/** call per tick to set animation according character's state  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void UpdateAnimation();

	/**   */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void UpdateCharacterDirection();

	/** calls to change sprite color during hit
	* intended to using timeline color animation and when the timeline will finished 
	* call call PostChangeSpriteColor() to handle exit from hit state
	*/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "AAA")
	void ChangeSpriteColor();

	/** handle exit from hit state. Intended to call after character was hit  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void PostChangeSpriteColor();

	/** fire if can  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void SpawnProjectile();

	/** call by timer when cooldown ends  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void ResetCanFire();

	/** projectile class template  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AAA")
	TSubclassOf<class AProjectile> ProjectileTemplate;
	
private:

	/** current character's velocity  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float Velocity;
	
	/** timer to cooldown player's firing ability  */
	FTimerHandle FireCooldownTimer;

	/** initial color  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	FLinearColor ColorNormal = FLinearColor::White;

	/** hit color  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	FLinearColor ColorHit = FLinearColor::Red;
	
	/** time between shoots  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float FireCooldown = 0.25f;

	/** number of sprite at firing(shoot) animation to spawn projectile at  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	int32 FiringSpriteNumber = 3;

	/** shows whether player can fire now or not  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	uint32 bCanFire : 1;

	/** player controller  */
	class APlayerController* PlayerController;
	
	/** current animation state  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	EAnimState AnimState = EAnimState::Idle;
	
	/** shows whether player is facing right  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	uint32 bMovingRight : 1;

	/** force to launch a character when it hit  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float HitForceDirection;
	
	/** current character's health  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float Health = 100.f;

	/** how much (in sec) player will be inactive after hit  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float ProjectileLifeSpan = 5.f;
	
	//~ ------------- F L I P B O O K S -----------------------------------------------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class UPaperFlipbook* DyingFlipbook;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class UPaperFlipbook* DeadFlipbook;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class UPaperFlipbook* HitFlipbook;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class UPaperFlipbook* JumpFlipbook;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class UPaperFlipbook* WalkFlipbook;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class UPaperFlipbook* ShootFlipbook;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class UPaperFlipbook* IdleFlipbook;
	//~ ------------- F L I P B O O K S ----------------------------------------------------------------------------

public:

	/** returns projectile spawn point **/
	FORCEINLINE class UArrowComponent* GetProjectileSpawnPoint() const { return ProjectileSpawnPoint; }
	/** shows whether character is dead or not  */
	FORCEINLINE EAnimState GetState() const { return AnimState; }
};