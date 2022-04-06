// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAnimInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "Enemy.h"

void UEnemyAnimInstance::InitializeAnimationProperties()
{
	Super::InitializeAnimationProperties();

	if (Enemy == nullptr)
	{
		Enemy = Cast<AEnemy>(TryGetPawnOwner());
	}
}

void UEnemyAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	Super::UpdateAnimationProperties(DeltaTime);

	if (Enemy) 
	{
		bIsBattleMode = Enemy->GetBattleMode();
		bIsSprint = Enemy->GetSprinting();
		
		WalkDirection = Enemy->GetWalkDirection();
		
		bWalkDirectionZero = UKismetMathLibrary::NearlyEqual_FloatFloat(WalkDirection, 0.f);
		
		bTurnInPlace = Enemy->GetTurnInPlace();
		
		bTurnLeft = Enemy->GetTurnLeft();
	}
}