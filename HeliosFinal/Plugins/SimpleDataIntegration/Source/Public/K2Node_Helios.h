#pragma once
#include "K2Node_BaseAsyncTask.h"
#include "K2Node_Helios.generated.h"

UENUM(BlueprintType)
enum class ERequestVerb : uint8 { Get, Post };
// Get = 0, Post = 1

//
UENUM(BlueprintType)
enum class EHeliosClassType : uint8 { HeliosFString, HeliosBool, HeliosInt, HeliosFloat };
// FString = 0, Bool = 1, Int = 2, Float = 3

UCLASS()
class SIMPLEDATAINTEGRATION_API UK2Node_Helios : public UK2Node_BaseAsyncTask
{
	GENERATED_UCLASS_BODY()

	UPROPERTY()
	FName MyHeliosClass;

	UPROPERTY()
	FName MyServerUrl;

	UPROPERTY()
	ERequestVerb MyRequestVerb;

	UPROPERTY()
	EHeliosClassType MyHeliosClassType;

	//~ Begin UEdGraphNode Interface
	virtual FText GetTooltipText() const override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual void AllocateDefaultPins() override;
	//~ End UEdGraphNode Interface

	//~ Begin UK2Node Interface
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual FText GetMenuCategory() const override;
	//~ End UK2Node Interface

	void Initialize(const FName HeliosClass, const FName ServerUrl, const ERequestVerb RequestVerb, const EHeliosClassType HeliosClassType);
};
