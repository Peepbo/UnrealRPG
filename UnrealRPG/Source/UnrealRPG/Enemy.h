// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MeleeCharacter.h"
#include "DamageState.h"
#include "Enemy.generated.h"

UCLASS()
class UNREALRPG_API AEnemy : public AMeleeCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/* HealthBar 위젯을 숨긴다. */
	UFUNCTION(BlueprintImplementableEvent)
	void HideHealthBar();

	/* 탐색 시아 (범위 안에 들어오면 전투가 활성화된다.) */
	UFUNCTION()
	virtual void AgroSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	/* 공격 시작 범위 */
	UFUNCTION()
		void CombatRangeOverlap(
			UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult);

	/* 공격 범위 벗어남 */
	UFUNCTION()
		void CombatRangeEndOverlap(
			UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex);

	/* non-battle에서 battle로, 혹은 battle에서 non-battle로 */
	UFUNCTION(BlueprintCallable)
		void ChangeBattleMode();

	/* 캐릭터의 크기를 변경하는 함수 (LockOn에 필요) */
	void ChangeEnemySize(EEnemySize Size);

protected:
	class AEnemyAIController* EnemyAIController;

	/* 전투 이동속도 관련 변수 */
	float BattleWalkSpeed;
	float BattleRunSpeed;

	/* 플레이어를 저장하는 변수 */
	UPROPERTY(BlueprintReadOnly, Category = "Behavior Tree", meta = (AllowPrivateAccess = "true"))
	class APlayerCharacter* Target;

private:
	/* 데미지를 받을 수 있는 상태인지를 검사하기 위해 사용하는 변수 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	EDamageState DamageState;

	/* AI관련 행동을 저장하는 트리 */
	UPROPERTY(EditAnywhere, Category = "Behavior Tree", meta = (AllowPrivateAccess = "true"))
	class UBehaviorTree* BehaviorTree;

	/* 정찰 지점1 */
	UPROPERTY(EditAnywhere, Category = "Behavior Tree", meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
	FVector PatrolPoint;

	/* 정찰 지점2 */
	UPROPERTY(EditAnywhere, Category = "Behavior Tree", meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
	FVector PatrolPoint2;

	/* 적(플레이어) 인식 콜라이더 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Combat, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* AgroSphere;

	/* 공격 콜라이더 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Combat, meta = (AllowPrivateAccess = "true"))
	USphereComponent* CombatRangeSphere;

	/* 공격 범위안에 들어왔는지 */
	bool bInAttackRange;
	/* 달리기 상태인지 */
	bool bIsSprint;

	/* 락온되는 위치 및 이미지를 포함하는 위젯 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Combat, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* LockOnWidget;

	/* 몬스터 크기를 대략적으로 나타내는 enum 변수 */
	EEnemySize EnemySize;

	/* 락온 시 최소 Pitch값을 나타내는 변수, 크기에 따라 달라짐 */
	float LockOnMinimumPitchValue;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	FORCEINLINE EDamageState GetDamageState() const { return DamageState; }

	UFUNCTION()
	void ResetDamageState() { DamageState = EDamageState::EDS_Unoccupied; }

	FORCEINLINE UBehaviorTree* GetBehaviorTree() const { return BehaviorTree; }
	FORCEINLINE bool GetBattleMode() const { return bIsBattleMode; }
	FORCEINLINE bool GetSprinting() const { return bIsSprint; }
	FORCEINLINE UWidgetComponent* GetLockOnWidget() const { return LockOnWidget; }
	FORCEINLINE float GetMinimumLockOnPitchValue() const { return LockOnMinimumPitchValue; }
};
