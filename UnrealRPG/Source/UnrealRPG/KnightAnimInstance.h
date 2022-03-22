// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MeleeAnimInstance.h"
#include "KnightAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class UNREALRPG_API UKnightAnimInstance : public UMeleeAnimInstance
{
	GENERATED_BODY()

public:

public:
	virtual void InitializeAnimationProperties() override;

	virtual void UpdateAnimationProperties(float DeltaTime) override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class ADarkKnight* DarkKnight;
	//class AEnemy* Enemy;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= Movement, meta = (AllowPrivateAccess = "true"))
	float WalkDirection;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= Movement, meta = (AllowPrivateAccess = "true"))
	bool bTurnInPlace;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= Movement, meta = (AllowPrivateAccess = "true"))
	bool bTurnLeft;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bWalkDirectionZero;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float MoveValue;
};
