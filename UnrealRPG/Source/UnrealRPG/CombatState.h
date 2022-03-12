#pragma once

UENUM(BlueprintType)
enum class ECombatState : uint8
{
	// Common State
	ECS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	ECS_Attack UMETA(DisplayName = "Attack"),
	ECS_Guard UMETA(DisplayName = "Guard"),

	// Player State
	ECS_Roll UMETA(DisplayName = "Roll"),
	ECS_Impact UMETA(DisplayName = "Impact"),
	ECS_MovableInteraction UMETA(DisplayName = "Movable Interaction"),
	ECS_NonMovingInteraction UMETA(DisplayName = "Non-Moving Interaction"),

	// Enemy State
	ECS_FaceOff UMETA(DisplayName = "FaceOff"),
	ECS_Chase UMETA(DisplayName = "Chase"),
	ECS_MAX UMETA(DisplayName = "DefaultMax")
};

UENUM(BlueprintType)
enum class EEnemySize : uint8
{
	EES_Small UMETA(DisplayName = "Small"),
	EES_Medium UMETA(DisplayName = "Medium"),
	EES_Large UMETA(DisplayName = "Large"),

	EES_MAX UMETA(DisplayName = "DefaultMax")
};