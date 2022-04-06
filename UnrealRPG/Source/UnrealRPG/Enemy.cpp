// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "EnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "PlayerCharacter.h"
#include "Components/WidgetComponent.h"
#include "Components/CapsuleComponent.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AEnemy::AEnemy() :
	bInAttackRange(false),
	BattleWalkSpeed(130.f),
	BattleRunSpeed(400.f),
	bIsSprint(false),
	EnemySize(EEnemySize::EES_MAX),
	bLockOnEnemy(false),
	bRestTime(false),
	bAvoidImpactState(false),
	bPatrolEnemy(false),
	bTurnInPlace(false),
	bAttackable(true),
	WalkDirection(0.f),
	bMove(false),
	InterpSpeed(5.f),
	AttackIndex(0),
	LastAttackIndex(0)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AgroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AgroSphere"));
	AgroSphere->SetupAttachment(GetRootComponent());
	AgroSphere->ComponentTags.Add(TEXT("Agro"));

	CombatRangeSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatRangeSphere"));
	CombatRangeSphere->SetupAttachment(GetRootComponent());
	CombatRangeSphere->ComponentTags.Add(TEXT("CombatRange"));

	MaximumWalkSpeed = 170.f;
	GetCharacterMovement()->MaxWalkSpeed = MaximumWalkSpeed;
	GetCharacterMovement()->bAllowPhysicsRotationDuringAnimRootMotion = true;

	LockOnWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("LockOnWidget"));
	LockOnWidget->SetupAttachment(GetMesh(), TEXT("Hips"));
	LockOnWidget->SetWidgetSpace(EWidgetSpace::Screen);
	LockOnWidget->SetDrawSize({ 18.f,18.f });
	LockOnWidget->SetVisibility(false);
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	AgroSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::AgroSphereOverlap);
	AgroSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::AgroSphereEndOverlap);

	CombatRangeSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatRangeOverlap);
	CombatRangeSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatRangeEndOverlap);
	// 플레이어를 찾게 되면 activate 해준다.
	CombatRangeSphere->Deactivate();

	EnemyAIController = Cast<AEnemyAIController>(GetController());
	if (EnemyAIController && BehaviorTree)
	{
		if (bPatrolEnemy)
		{
			const FVector WorldPatrolPoint = UKismetMathLibrary::TransformLocation(GetActorTransform(), PatrolPoint);
			const FVector WorldPatrolPoint2 = UKismetMathLibrary::TransformLocation(GetActorTransform(), PatrolPoint2);

			DrawDebugSphere(
				GetWorld(),
				WorldPatrolPoint,
				25.f,
				12,
				FColor::Red,
				true
			);

			DrawDebugSphere(
				GetWorld(),
				WorldPatrolPoint2,
				25.f,
				12,
				FColor::Red,
				true
			);

			EnemyAIController->GetBlackboardComponent()->SetValueAsVector(TEXT("PatrolPoint"), WorldPatrolPoint);
			EnemyAIController->GetBlackboardComponent()->SetValueAsVector(TEXT("PatrolPoint2"), WorldPatrolPoint2);
		}

		EnemyAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("bDying"), false);

		EnemyAIController->RunBehaviorTree(BehaviorTree);
	}

	AnimInstance = GetMesh()->GetAnimInstance();
}

void AEnemy::AgroSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bIsBattleMode)return;
	if (OtherActor == nullptr)return;

	auto Character = Cast<APlayerCharacter>(OtherActor);
	if (Character) {
		if (EnemyAIController) {
			Target = Character;
			EnemyAIController->GetBlackboardComponent()->SetValueAsObject(TEXT("Target"), Character);
		}
	}
}

void AEnemy::AgroSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void AEnemy::CombatRangeOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == nullptr)return;

	auto Character = Cast<APlayerCharacter>(OtherActor);

	if (Character) {
		bInAttackRange = true;

		if (EnemyAIController) {
			EnemyAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("InAttackRange"), true);
		}
	}
}

void AEnemy::CombatRangeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == nullptr)return;

	auto Character = Cast<APlayerCharacter>(OtherActor);

	if (Character) {
		bInAttackRange = false;

		if (EnemyAIController) {
			EnemyAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("InAttackRange"), false);
		}
	}
}

void AEnemy::ChangeBattleMode()
{
	bIsBattleMode = !bIsBattleMode;

	GetCharacterMovement()->MaxWalkSpeed = (bIsBattleMode ? BattleWalkSpeed : MaximumWalkSpeed);

	if (EnemyAIController) {
		EnemyAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("IsBattleMode"), bIsBattleMode);
	}
}

void AEnemy::ChangeCombatState(ECombatState NextCombatState)
{
	CombatState = NextCombatState;
}

void AEnemy::PlayAttackMontage()
{
	AnimInstance->Montage_Play(AttackMontage[0]);
}

void AEnemy::ChangeEnemySize(EEnemySize Size)
{
	EnemySize = Size;

	switch (EnemySize)
	{
	case EEnemySize::EES_Small:
		LockOnMinimumPitchValue = -30.f;
		break;
	case EEnemySize::EES_Medium:
		LockOnMinimumPitchValue = -20.f;
		break;
	case EEnemySize::EES_Large:
		LockOnMinimumPitchValue = -10.f;
		break;
	}
}

void AEnemy::ChangeColliderSetting(bool bBattle)
{
	if (bBattle) {
		AgroSphere->Deactivate();
		CombatRangeSphere->Activate();
	}
	else {
		AgroSphere->Activate();
		CombatRangeSphere->Deactivate();
	}
}

void AEnemy::Die()
{
	// RagDoll로 바꿔준다.
	GetMesh()->SetCollisionProfileName(FName("Ragdoll"));
	GetMesh()->SetSimulatePhysics(true);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy::SaveTargetRotator()
{
	FVector TargetLoc{ Target->GetActorLocation() };
	FVector ActorLoc{ GetActorLocation() };
	TargetLoc.Z = ActorLoc.Z = 0.f;

	const FVector DirectionToTarget{ TargetLoc - ActorLoc };
	const FVector DirectionToTargetNormal{ UKismetMathLibrary::Normal(DirectionToTarget) };
	const float RotateDegree = UKismetMathLibrary::DegAtan2(DirectionToTargetNormal.Y, DirectionToTargetNormal.X);

	const FRotator RotationToTarget{ 0,RotateDegree,0 };
	LastSaveRotate = RotationToTarget;
}

void AEnemy::StartRotate()
{
	bTurnInPlace = true;
	EnemyAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("bRotate"), bTurnInPlace);
}

void AEnemy::StopRotate()
{
	bTurnInPlace = false;
	EnemyAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("bRotate"), bTurnInPlace);
}

void AEnemy::TracingAttackSphere()
{
	if (!bAttackable)return;

	/* 아이템의 TopSocket과 BottomSocket의 위치를 받아온다. */
	const FVector TopSocketLoc{ WeaponMesh->GetSocketLocation("TopSocket") };
	const FVector BottomSocketLoc{ WeaponMesh->GetSocketLocation("BottomSocket") };

	FHitResult HitResult;
	// SphereTraceSingle로 원을 그리고 원에 겹치는 오브젝트를 HitResult에 담는다.
	bool bHit = UKismetSystemLibrary::SphereTraceSingle(
		this,
		BottomSocketLoc,
		TopSocketLoc,
		50.f,
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		{ this },
		bVisibleTraceSphere ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
		HitResult,
		true
	);

	if (bHit) {
		if (HitResult.Actor != nullptr) {
			auto Player = Cast<APlayerCharacter>(HitResult.Actor);

			if (Player) {
				// attack point를 플레이어한테 전달
				Player->SetHitPoint(HitResult.Location);

				UGameplayStatics::ApplyDamage(
					Player,
					AD,
					GetController(),
					this,
					UDamageType::StaticClass());

				bAttackable = false;
			}
		}
	}
}

void AEnemy::StartAttackCheckTime()
{
	GetWorldTimerManager().SetTimer(
		AttackCheckTimer,
		this,
		&AEnemy::TracingAttackSphere,
		0.005f,
		true);
}

void AEnemy::EndAttackCheckTime()
{
	GetWorldTimerManager().ClearTimer(AttackCheckTimer);

	bAttackable = true;
}

void AEnemy::FaceOff(float NextWalkDirection)
{
	GetCharacterMovement()->bUseControllerDesiredRotation = false;

	WalkDirection = NextWalkDirection;
}

void AEnemy::EndFaceOff()
{
	GetCharacterMovement()->bUseControllerDesiredRotation = true;

	WalkDirection = 0.f;
}

void AEnemy::StartRestTimer()
{
	bRestTime = true;

	GetWorldTimerManager().SetTimer(
		RestTimer,
		this,
		&AEnemy::EndRestTimer,
		1.5f,
		false);
}

void AEnemy::EndRestTimer()
{
	bRestTime = false;

	EnemyAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("IsAttack"), false);
	ChangeCombatState(ECombatState::ECS_Unoccupied);

	//GetCharacterMovement()->bUseControllerDesiredRotation = true;
}

void AEnemy::ChangeSprinting(bool IsSprinting)
{
	SetSprinting(IsSprinting);
	if (IsSprinting) {
		GetCharacterMovement()->MaxWalkSpeed = BattleRunSpeed;
	}
	else {
		GetCharacterMovement()->MaxWalkSpeed = MaximumWalkSpeed;
	}
}

void AEnemy::FindCharacter()
{
	// 캐릭터가 전방 160도 안에 있는지 확인한다. (dot product)
	FVector ActorForward{ GetActorForwardVector() };
	ActorForward.Z = 0.f;

	FVector ActorToCharacter{ OverlapCharacter->GetActorLocation() - GetActorLocation() };
	ActorToCharacter.Z = 0.f;
	ActorToCharacter = UKismetMathLibrary::Normal(ActorToCharacter);

	float DotRadian{ UKismetMathLibrary::Dot_VectorVector(ActorForward, ActorToCharacter) };
	float DotDegree{ UKismetMathLibrary::DegAcos(DotRadian) };
	// 전방 160도면 -80~+80 사이 (dot product는 어차피 -가 나오질 않으므로 80이하인지 검사)
	const bool bFind{ DotDegree <= 80.f };

	if (bFind) {
		if (EnemyAIController) {
			GetWorldTimerManager().ClearTimer(SearchTimer);
			ChangeColliderSetting(true);

			StopRotate();

			Target = OverlapCharacter;
			EnemyAIController->GetBlackboardComponent()->SetValueAsObject(TEXT("Target"), OverlapCharacter);
		}
	}
}

float AEnemy::GetDegreeForwardToTarget()
{
	if (Target == nullptr)return 0.f;

	const FVector ActorForward{ GetActorForwardVector() };
	const FVector ActorToTarget{ UKismetMathLibrary::Normal(Target->GetActorLocation() - GetActorLocation()) };
	const FVector2D ActorForward2D{ ActorForward };
	const FVector2D ActorToTarget2D{ ActorToTarget };

	const float DotProductRadian{ UKismetMathLibrary::DotProduct2D(ActorForward2D,ActorToTarget2D) };
	float ResultDegree{ UKismetMathLibrary::DegAcos(DotProductRadian) };

	const float CrossProduct{ UKismetMathLibrary::CrossProduct2D(ActorForward2D,ActorToTarget2D) };
	if (CrossProduct < 0.f) {
		ResultDegree = -(ResultDegree);
	}

	return ResultDegree;
}

void AEnemy::StartAttack()
{
	ChangeCombatState(ECombatState::ECS_Attack);

	GetCharacterMovement()->bUseControllerDesiredRotation = false;
}

void AEnemy::EndAttack()
{
	EnemyAIController->ClearFocus(EAIFocusPriority::Gameplay);

	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	StartRestTimer();
}

void AEnemy::GetWeaponMesh(USkeletalMeshComponent* ItemMesh)
{
	if (ItemMesh) {
		WeaponMesh = ItemMesh;
	}
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetAttacking() && bTurnInPlace) {
		const FRotator LookRot{ UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target->GetActorLocation()) };

		if (UKismetMathLibrary::EqualEqual_RotatorRotator(GetActorRotation(), { 0.f,LookRot.Yaw,0.f }, 0.5f)) {
			bTurnInPlace = false;
		}
		else {
			SetActorRotation(UKismetMathLibrary::RInterpTo(GetActorRotation(), { 0.f,LookRot.Yaw,0.f }, DeltaTime, InterpSpeed));
		}
	}
}

float AEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("Enemy TakeDamage call"));
	if (bDying)return DamageAmount;

	// Attack이 아니거나 Attack Rest Time일 경우 Impact로 바뀐다.
	if (!bAvoidImpactState && (CombatState != ECombatState::ECS_Attack || bRestTime)) {
		CombatState = ECombatState::ECS_Impact;
	}

	UE_LOG(LogTemp, Warning, TEXT("Enemy Super(MeleeCharacter) TakeDamage call"));
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	UE_LOG(LogTemp, Warning, TEXT("Enemy Change DamageState"));
	DamageState = EDamageState::EDS_invincibility;
	if (bDying) {
		HideHealthBar();
		EnemyAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("bDying"), true);

		//UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && DeathMontage) {
			AnimInstance->Montage_Play(DeathMontage);
		}
	}

	return DamageAmount;
}