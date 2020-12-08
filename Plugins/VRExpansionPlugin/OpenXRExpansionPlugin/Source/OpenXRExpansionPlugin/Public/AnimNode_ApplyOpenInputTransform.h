// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "HeadMountedDisplayTypes.h"
#include "OpenInputFunctionLibrary.h"
#include "Runtime/AnimGraphRuntime/Public/BoneControllers/AnimNode_SkeletalControlBase.h"
//#include "Skeleton/BodyStateSkeleton.h"
//#include "BodyStateAnimInstance.h"

#include "AnimNode_ApplyOpenInputTransform.generated.h"

UENUM(BlueprintType)
enum class EVROpenVRSkeletonType : uint8
{
	OVR_SkeletonType_UE4Default_Left,
	OVR_SkeletonType_UE4Default_Right,
	OVERSkeletonType_Custom
};

USTRUCT(BlueprintType, Category = "VRExpansionFunctions|SteamVR|HandSkeleton")
struct OPENINPUTPLUGIN_API FBPOpenVRSkeletalPair
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
		EHandKeypoint OpenVRBone;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
		FName BoneToTarget;

	FBoneReference ReferenceToConstruct;
	FCompactPoseBoneIndex ParentReference;

	FBPOpenVRSkeletalPair() :
		ParentReference(INDEX_NONE)
	{
		OpenVRBone = EEHandKeypoint::Wrist;
		BoneToTarget = NAME_None;
	}

	FBPOpenVRSkeletalPair(EHandKeypoint Bone, FString TargetBone) :
		ParentReference(INDEX_NONE)
	{
		OpenVRBone = Bone;
		BoneToTarget = FName(*TargetBone);
		ReferenceToConstruct.BoneName = BoneToTarget;
	}

	FORCEINLINE bool operator==(const int32 &Other) const
	{
		return ReferenceToConstruct.CachedCompactPoseIndex.GetInt() == Other;
		//return ReferenceToConstruct.BoneIndex == Other;
	}
};

USTRUCT(BlueprintType, Category = "VRExpansionFunctions|SteamVR|HandSkeleton")
struct OPENINPUTPLUGIN_API FBPSkeletalMappingData
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
		TArray<FBPOpenVRSkeletalPair> BonePairs;

	// Merge the transforms of bones that are missing from the OpenVR skeleton to the UE4 one.
	// This should be always enabled for UE4 skeletons generally.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	bool bMergeMissingBonesUE4;

	// The hand data to get, if not using a custom bone mapping then this value will be auto filled
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
		EVRActionHand TargetHand;

	FQuat AdjustmentQuat;
	bool bInitialized;

	FName LastInitializedName;

	void ConstructDefaultMappings(EVROpenVRSkeletonType SkeletonType, bool bSkipRootBone)
	{
		switch (SkeletonType)
		{
		case EVROpenVRSkeletonType::OVR_SkeletonType_UE4Default_Left:
		case EVROpenVRSkeletonType::OVR_SkeletonType_UE4Default_Right:
		{
			bMergeMissingBonesUE4 = true;
			SetDefaultUE4Inputs(SkeletonType, bSkipRootBone);
		}break;
		}
	}

	void SetDefaultUE4Inputs(EVROpenVRSkeletonType cSkeletonType, bool bSkipRootBone)
	{
		// Don't map anything if the end user already has
		if (BonePairs.Num())
			return;

		bool bIsRightHand = cSkeletonType != EVROpenVRSkeletonType::OVR_SkeletonType_UE4Default_Left;
		FString HandDelimiterS = !bIsRightHand ? "l" : "r";
		const TCHAR* HandDelimiter = *HandDelimiterS;

		TargetHand = bIsRightHand ? EVRActionHand::EActionHand_Right : EVRActionHand::EActionHand_Left;

		// Default ue4 skeleton hand to the OpenVR bones, skipping the extra joint and the aux joints
		if (!bSkipRootBone)
		{
			BonePairs.Add(FBPOpenVRSkeletalPair(EHandKeypoint::Wrist, FString::Printf(TEXT("hand_%s"), HandDelimiter)));
		}

		
		/*
UENUM(BlueprintType)
enum class EHandKeypoint : uint8
{
	Palm,
	Wrist,
	ThumbMetacarpal,
	ThumbProximal,
	ThumbDistal,
	ThumbTip,
	IndexMetacarpal,
	IndexProximal,
	IndexIntermediate,
	IndexDistal,
	IndexTip,
	MiddleMetacarpal,
	MiddleProximal,
	MiddleIntermediate,
	MiddleDistal,
	MiddleTip,
	RingMetacarpal,
	RingProximal,
	RingIntermediate,
	RingDistal,
	RingTip,
	LittleMetacarpal,
	LittleProximal,
	LittleIntermediate,
	LittleDistal,
	LittleTip
};
		*/
		
		BonePairs.Add(FBPOpenVRSkeletalPair(EHandKeypoint::ThumbMetacarpal, FString::Printf(TEXT("thumb_01_%s"), HandDelimiter)));
		BonePairs.Add(FBPOpenVRSkeletalPair(EHandKeypoint::ThumbProximal, FString::Printf(TEXT("thumb_02_%s"), HandDelimiter)));
		BonePairs.Add(FBPOpenVRSkeletalPair(EHandKeypoint::ThumbDistal, FString::Printf(TEXT("thumb_03_%s"), HandDelimiter)));
		
		// OpenInput skeleton only
		//BonePairs.Add(FBPOpenVRSkeletalPair(EHandKeypoint::IndexMetacarpal, FString::Printf(TEXT("index_01_%s"), HandDelimiter)));
		BonePairs.Add(FBPOpenVRSkeletalPair(EHandKeypoint::IndexProximal, FString::Printf(TEXT("index_01_%s"), HandDelimiter)));
		BonePairs.Add(FBPOpenVRSkeletalPair(EHandKeypoint::IndexIntermediate, FString::Printf(TEXT("index_02_%s"), HandDelimiter)));
		BonePairs.Add(FBPOpenVRSkeletalPair(EHandKeypoint::IndexDistal, FString::Printf(TEXT("index_03_%s"), HandDelimiter)));
		
		// Not used for skeletal positioning, though it could be for tip placement
		//BonePairs.Add(FBPOpenVRSkeletalPair(EHandKeypoint::IndexTip, FString::Printf(TEXT("index_03_%s"), HandDelimiter)));

		BonePairs.Add(FBPOpenVRSkeletalPair(EHandKeypoint::MiddleProximal, FString::Printf(TEXT("middle_01_%s"), HandDelimiter)));
		BonePairs.Add(FBPOpenVRSkeletalPair(EHandKeypoint::MiddleIntermediate, FString::Printf(TEXT("middle_02_%s"), HandDelimiter)));
		BonePairs.Add(FBPOpenVRSkeletalPair(EHandKeypoint::MiddleDistal, FString::Printf(TEXT("middle_03_%s"), HandDelimiter)));

		BonePairs.Add(FBPOpenVRSkeletalPair(EHandKeypoint::RingProximal, FString::Printf(TEXT("ring_01_%s"), HandDelimiter)));
		BonePairs.Add(FBPOpenVRSkeletalPair(EHandKeypoint::RingIntermediate, FString::Printf(TEXT("ring_02_%s"), HandDelimiter)));
		BonePairs.Add(FBPOpenVRSkeletalPair(EHandKeypoint::RingDistal, FString::Printf(TEXT("ring_03_%s"), HandDelimiter)));
		
		BonePairs.Add(FBPOpenVRSkeletalPair(EHandKeypoint::LittleProximal, FString::Printf(TEXT("pinky_01_%s"), HandDelimiter)));
		BonePairs.Add(FBPOpenVRSkeletalPair(EHandKeypoint::LittleIntermediate, FString::Printf(TEXT("pinky_02_%s"), HandDelimiter)));
		BonePairs.Add(FBPOpenVRSkeletalPair(EHandKeypoint::LittleDistal, FString::Printf(TEXT("pinky_03_%s"), HandDelimiter)));
	}

	FBPSkeletalMappingData()
	{
		AdjustmentQuat = FQuat::Identity;
		bInitialized = false;
		bMergeMissingBonesUE4 = false;
		TargetHand = EVRActionHand::EActionHand_Right;
		LastInitializedName = NAME_None;
	}
};

USTRUCT()
struct OPENINPUTPLUGIN_API FAnimNode_ApplyOpenInputTransform : public FAnimNode_SkeletalControlBase
{
	GENERATED_USTRUCT_BODY()

public:

	// Generally used when not passing in custom bone mappings, defines the auto mapping style
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Skeletal, meta = (PinShownByDefault))
		EVROpenVRSkeletonType SkeletonType;

	// If your hand is part of a full body or arm skeleton and you don't have a proxy bone to retain the position enable this
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Skeletal, meta = (PinShownByDefault))
		bool bSkipRootBone;

	// If you only want to use the wrist transform part of this
	// This will also automatically add the deform to the wrist as it doesn't make much sense without it
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Skeletal, meta = (PinShownByDefault))
		bool bOnlyApplyWristTransform;
	
	// Generally used when not passing in custom bone mappings, defines the auto mapping style
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Skeletal, meta = (PinShownByDefault))
		FBPOpenVRActionSkeletalData OptionalStoredActionInfo;

	// MappedBonePairs, if you leave it blank then they will auto generate based off of the SkeletonType
	// Otherwise, fill out yourself.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Skeletal, meta = (PinHiddenByDefault))
		FBPSkeletalMappingData MappedBonePairs;

	bool bIsOpenInputAnimationInstance;

	FTransform GetRefBoneInCS(TArray<FTransform>& RefBones, TArray<FMeshBoneInfo>& RefBonesInfo, int32 BoneIndex)
	{
		FTransform BoneTransform;

		if (BoneIndex >= 0)
		{
			BoneTransform = RefBones[BoneIndex];
			if (RefBonesInfo[BoneIndex].ParentIndex >= 0)
			{
				BoneTransform *= GetRefBoneInCS(RefBones, RefBonesInfo, RefBonesInfo[BoneIndex].ParentIndex);
			}
		}

		return BoneTransform;
	}

	void SetVectorToMaxElement(FVector& vec)
	{
		float aX = FMath::Abs(vec.X);
		float aY = FMath::Abs(vec.Y);

		if (aY < aX)
		{
			vec.Y = 0.f;
			if (FMath::Abs(vec.Z) < aX)
				vec.Z = 0.f;
			else
				vec.X = 0.f;
		}
		else
		{
			vec.X = 0.f;
			if (FMath::Abs(vec.Z) < aY)
				vec.Z = 0;
			else
				vec.Y = 0;
		}
	}

	// FAnimNode_SkeletalControlBase interface
	//virtual void UpdateInternal(const FAnimationUpdateContext& Context) override;
	virtual void EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms) override;
	virtual bool IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones) override;
	// End of FAnimNode_SkeletalControlBase interface
	virtual void OnInitializeAnimInstance(const FAnimInstanceProxy* InProxy, const UAnimInstance* InAnimInstance) override;
	virtual bool NeedsOnInitializeAnimInstance() const override { return true; }
	virtual void InitializeBoneReferences(const FBoneContainer& RequiredBones) override;

	// Constructor 
	FAnimNode_ApplyOpenInputTransform();

protected:
	bool WorldIsGame;
	AActor* OwningActor;

private:
};