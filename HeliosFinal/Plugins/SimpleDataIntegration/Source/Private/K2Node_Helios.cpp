#include "SimpleDataIntegrationPCH.h"
#include "CompilerResultsLog.h"
#include "HeliosHelperLibrary.h"
#include "Blueprint/AIAsyncTaskBlueprintProxy.h"
#include "K2Node_Helios.h"
#include "EditorCategoryUtils.h"

#include "BlueprintNodeSpawner.h"
#include "Engine/InputAxisDelegateBinding.h"
#include "BlueprintActionDatabaseRegistrar.h"

#include "HeliosJsonParser.h"

#define LOCTEXT_NAMESPACE "K2Node_Helios"

UK2Node_Helios::UK2Node_Helios(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ProxyFactoryClass = UHeliosHelperLibrary::StaticClass();
}

void UK2Node_Helios::Initialize(const FName HeliosClass, const FName ServerUrl, const ERequestVerb RequestVerb, const EHeliosClassType HeliosClassType)
{
	MyHeliosClass = HeliosClass;
	MyServerUrl = ServerUrl;
	MyRequestVerb = RequestVerb;
	MyHeliosClassType = HeliosClassType;
	
	switch (MyRequestVerb)
	{
		case ERequestVerb::Get:
			switch (MyHeliosClassType) {
				case EHeliosClassType::HeliosFString:
					ProxyFactoryFunctionName = GET_FUNCTION_NAME_CHECKED(UHeliosHelperLibrary, CreateHeliosGetterFStringProxyObject);
					ProxyClass = UHeliosGetterProxy_FString::StaticClass();
					break;
				case EHeliosClassType::HeliosBool:
					ProxyFactoryFunctionName = GET_FUNCTION_NAME_CHECKED(UHeliosHelperLibrary, CreateHeliosGetterBoolProxyObject);
					ProxyClass = UHeliosGetterProxy_Bool::StaticClass();
					break;
				case EHeliosClassType::HeliosInt:
					ProxyFactoryFunctionName = GET_FUNCTION_NAME_CHECKED(UHeliosHelperLibrary, CreateHeliosGetterIntProxyObject);
					ProxyClass = UHeliosGetterProxy_Int::StaticClass();
					break;
				case EHeliosClassType::HeliosFloat:
					ProxyFactoryFunctionName = GET_FUNCTION_NAME_CHECKED(UHeliosHelperLibrary, CreateHeliosGetterFloatProxyObject);
					ProxyClass = UHeliosGetterProxy_Float::StaticClass();
					break;
			}
			break;
		
		case ERequestVerb::Post:
			switch (MyHeliosClassType) {
				case EHeliosClassType::HeliosFString:
					ProxyFactoryFunctionName = GET_FUNCTION_NAME_CHECKED(UHeliosHelperLibrary, CreateHeliosSetterFStringProxyObject);
					ProxyClass = UHeliosSetterProxy_FString::StaticClass();
					break;
				case EHeliosClassType::HeliosBool:
					ProxyFactoryFunctionName = GET_FUNCTION_NAME_CHECKED(UHeliosHelperLibrary, CreateHeliosSetterBoolProxyObject);
					ProxyClass = UHeliosSetterProxy_Bool::StaticClass();
					break;
				case EHeliosClassType::HeliosInt:
					ProxyFactoryFunctionName = GET_FUNCTION_NAME_CHECKED(UHeliosHelperLibrary, CreateHeliosSetterIntProxyObject);
					ProxyClass = UHeliosSetterProxy_Int::StaticClass();
					break;
				case EHeliosClassType::HeliosFloat:
					ProxyFactoryFunctionName = GET_FUNCTION_NAME_CHECKED(UHeliosHelperLibrary, CreateHeliosSetterFloatProxyObject);
					ProxyClass = UHeliosSetterProxy_Float::StaticClass();
					break;
			}
			break;
	}
}

void UK2Node_Helios::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	TArray<HeliosNodeProperties> NodeProperties = *HeliosJsonParser::getVariableNames();
	FName ServerUrl = HeliosJsonParser::getServerUrl();

	auto CustomizeHeliosNodeLambda = [](UEdGraphNode* NewNode, bool bIsTemplateNode, FName HeliosClass, FName ServerUrl, ERequestVerb RequestVerb, EHeliosClassType HeliosClassType)
	{
		UK2Node_Helios* HeliosNode = CastChecked<UK2Node_Helios>(NewNode);
		HeliosNode->Initialize(HeliosClass, ServerUrl, RequestVerb, HeliosClassType);
	};

	// actions get registered under specific object-keys; the idea is that 
	// actions might have to be updated (or deleted) if their object-key is  
	// mutated (or removed)... here we use the node's class (so if the node 
	// type disappears, then the action should go with it)
	UClass* ActionKey = GetClass();

	// to keep from needlessly instantiating a UBlueprintNodeSpawner, first   
	// check to make sure that the registrar is looking for actions of this type
	// (could be regenerating actions for a specific asset, and therefore the 
	// registrar would only accept actions corresponding to that asset)
	if (ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		auto RefreshClassActions = []()
		{
			FBlueprintActionDatabase::Get().RefreshClassActions(StaticClass());
		};

		static bool bRegisterOnce = true;
		if (bRegisterOnce)
		{
			bRegisterOnce = false;
			FEditorDelegates::OnActionAxisMappingsChanged.AddStatic(RefreshClassActions);
		}

		for (HeliosNodeProperties const NodeProperty : NodeProperties)
		{

			// Getter
			UBlueprintNodeSpawner* GetterNodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
			check(GetterNodeSpawner != nullptr);

			GetterNodeSpawner->CustomizeNodeDelegate = UBlueprintNodeSpawner::FCustomizeNodeDelegate::CreateStatic(CustomizeHeliosNodeLambda, NodeProperty.name, ServerUrl, ERequestVerb::Get, NodeProperty.type);
			ActionRegistrar.AddBlueprintAction(ActionKey, GetterNodeSpawner);

			// Setter
			UBlueprintNodeSpawner* SetterNodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
			check(SetterNodeSpawner != nullptr);

			SetterNodeSpawner->CustomizeNodeDelegate = UBlueprintNodeSpawner::FCustomizeNodeDelegate::CreateStatic(CustomizeHeliosNodeLambda, NodeProperty.name, ServerUrl, ERequestVerb::Post, NodeProperty.type);
			ActionRegistrar.AddBlueprintAction(ActionKey, SetterNodeSpawner);
		}
	}
}

void UK2Node_Helios::AllocateDefaultPins()
{
	Super::AllocateDefaultPins();
	UEdGraphPin* HeliosClassPin = FindPin(TEXT("HeliosClass"));
	if (HeliosClassPin != NULL)
	{
		HeliosClassPin->DefaultValue = MyHeliosClass.ToString();
		HeliosClassPin->SafeSetHidden(true);
	}

	UEdGraphPin* ServerUrlPin = FindPin(TEXT("ServerUrl"));
	if (ServerUrlPin != NULL)
	{
		ServerUrlPin->DefaultValue = MyServerUrl.ToString();
		ServerUrlPin->SafeSetHidden(true);
	}

	UEdGraphPin* InputValuePin = FindPin(TEXT("InputValue"));
	if (InputValuePin != NULL)
	{
		InputValuePin->PinFriendlyName = FText::FromName(MyHeliosClass);
	}

	UEdGraphPin* OutputValuePin = FindPin(TEXT("OutputValue"));
	if (OutputValuePin != NULL)
	{
		switch (MyRequestVerb)
		{
		case ERequestVerb::Get:
			OutputValuePin->PinFriendlyName = FText::FromName(MyHeliosClass);
			break;
		case ERequestVerb::Post:
			OutputValuePin->SafeSetHidden(true);
			break;
		}
	}
}

FText UK2Node_Helios::GetMenuCategory() const
{
	return FEditorCategoryUtils::GetCommonCategory(FCommonEditorCategory::Networking);
}

FText UK2Node_Helios::GetTooltipText() const
{
	switch (MyRequestVerb)
	{
		case ERequestVerb::Get:
			UE_LOG(LogTemp, Warning, TEXT("Helios > getter node title"));
			return FText::Format(LOCTEXT("HeliosTooltipText", "{0} {1}"), LOCTEXT("HeliosGet", "Get"), FText::FromName(MyHeliosClass));
		case ERequestVerb::Post:
			UE_LOG(LogTemp, Warning, TEXT("Helios > setter node title"));
			return FText::Format(LOCTEXT("HeliosTooltipText", "{0} {1}"), LOCTEXT("HeliosSet", "Set"), FText::FromName(MyHeliosClass));
	}
	return LOCTEXT("HeliosTooltipText", "Invalid Helios Node");
}

FText UK2Node_Helios::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	switch (MyRequestVerb)
	{
		case ERequestVerb::Get:
			UE_LOG(LogTemp, Warning, TEXT("Helios > getter node title"));
			return FText::Format(LOCTEXT("HeliosGetterNodeTitle", "{0} {1}"), LOCTEXT("HeliosGet", "Get"), FText::FromName(MyHeliosClass));
		case ERequestVerb::Post:
			UE_LOG(LogTemp, Warning, TEXT("Helios > setter node title"));
			return FText::Format(LOCTEXT("HeliosSetterNodeTitle", "{0} {1}"), LOCTEXT("HeliosSet", "Set"), FText::FromName(MyHeliosClass));
	}
	return LOCTEXT("HeliosInvalidNodeTitle", "Invalid Helios Node");
}

#undef LOCTEXT_NAMESPACE
