// Fill out your copyright notice in the Description page of Project Settings.


#include "MeleeAnimInstance.h"
#include "MeleeCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UMeleeAnimInstance::InitializeAnimationProperties()
{
	Character = Cast<ACharacter>(TryGetPawnOwner());
}

void UMeleeAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (Character == nullptr) {
		Character = Cast<ACharacter>(TryGetPawnOwner());
	}

	if (Character) {
		// 캐릭터의 속도를 구함
		FVector Velocity{ Character->GetVelocity() };
		Velocity.Z = 0;
		Speed = Velocity.Size();

		// 캐릭터가 공중에 있는지 확인
		bIsInAir = Character->GetCharacterMovement()->IsFalling();

		// 캐릭터가 움직이고 있는지 확인
		if (Character->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0) {
			bIsAccelerating = true;
		}
		else {
			bIsAccelerating = false;
		}

		//bIsSprint = Character->GetSprinting();

		//bIsGuard = Character->GetGuarding();

		//bIsBattleMode = Character->GetBattleMode();
	}
}
