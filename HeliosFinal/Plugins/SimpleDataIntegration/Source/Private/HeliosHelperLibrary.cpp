#include "SimpleDataIntegrationPCH.h"
#include "HeliosHelperLibrary.h"
#include "HeliosGetterProxy_FString.h"
#include "HeliosGetterProxy_Bool.h"
#include "HeliosGetterProxy_Int.h"
#include "HeliosGetterProxy_Float.h"
#include "HeliosSetterProxy_FString.h"
#include "HeliosSetterProxy_Bool.h"
#include "HeliosSetterProxy_Int.h"
#include "HeliosSetterProxy_Float.h"

UHeliosHelperLibrary::UHeliosHelperLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Empty.
}

UHeliosGetterProxy_FString* UHeliosHelperLibrary::CreateHeliosGetterFStringProxyObject(UObject* WorldContextObject, FName HeliosClass, FName ServerUrl)
{
	check(WorldContextObject);

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject);
	UHeliosGetterProxy_FString* MyObj = NewObject<UHeliosGetterProxy_FString>(World);

	MyObj->SendHeliosRequest(HeliosClass, ServerUrl);

	return MyObj;
}

UHeliosGetterProxy_Bool* UHeliosHelperLibrary::CreateHeliosGetterBoolProxyObject(UObject* WorldContextObject, FName HeliosClass, FName ServerUrl)
{
	check(WorldContextObject);

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject);
	UHeliosGetterProxy_Bool* MyObj = NewObject<UHeliosGetterProxy_Bool>(World);

	MyObj->SendHeliosRequest(HeliosClass, ServerUrl);

	return MyObj;
}

UHeliosGetterProxy_Int* UHeliosHelperLibrary::CreateHeliosGetterIntProxyObject(UObject* WorldContextObject, FName HeliosClass, FName ServerUrl)
{
	check(WorldContextObject);

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject);
	UHeliosGetterProxy_Int* MyObj = NewObject<UHeliosGetterProxy_Int>(World);

	MyObj->SendHeliosRequest(HeliosClass, ServerUrl);

	return MyObj;
}

UHeliosGetterProxy_Float* UHeliosHelperLibrary::CreateHeliosGetterFloatProxyObject(UObject* WorldContextObject, FName HeliosClass, FName ServerUrl)
{
	check(WorldContextObject);

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject);
	UHeliosGetterProxy_Float* MyObj = NewObject<UHeliosGetterProxy_Float>(World);

	MyObj->SendHeliosRequest(HeliosClass, ServerUrl);

	return MyObj;
}

UHeliosSetterProxy_FString* UHeliosHelperLibrary::CreateHeliosSetterFStringProxyObject(UObject* WorldContextObject, FName HeliosClass, FName ServerUrl, FString InputValue)
{
	check(WorldContextObject);

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject);
	UHeliosSetterProxy_FString* MyObj = NewObject<UHeliosSetterProxy_FString>(World);

	MyObj->SendHeliosRequest(HeliosClass, ServerUrl, InputValue);

	return MyObj;
}

UHeliosSetterProxy_Bool* UHeliosHelperLibrary::CreateHeliosSetterBoolProxyObject(UObject* WorldContextObject, FName HeliosClass, FName ServerUrl, bool InputValue)
{
	check(WorldContextObject);

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject);
	UHeliosSetterProxy_Bool* MyObj = NewObject<UHeliosSetterProxy_Bool>(World);

	MyObj->SendHeliosRequest(HeliosClass, ServerUrl, InputValue);

	return MyObj;
}

UHeliosSetterProxy_Int* UHeliosHelperLibrary::CreateHeliosSetterIntProxyObject(UObject* WorldContextObject, FName HeliosClass, FName ServerUrl, int InputValue)
{
	check(WorldContextObject);

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject);
	UHeliosSetterProxy_Int* MyObj = NewObject<UHeliosSetterProxy_Int>(World);

	MyObj->SendHeliosRequest(HeliosClass, ServerUrl, InputValue);

	return MyObj;
}

UHeliosSetterProxy_Float* UHeliosHelperLibrary::CreateHeliosSetterFloatProxyObject(UObject* WorldContextObject, FName HeliosClass, FName ServerUrl, float InputValue)
{
	check(WorldContextObject);

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject);
	UHeliosSetterProxy_Float* MyObj = NewObject<UHeliosSetterProxy_Float>(World);

	MyObj->SendHeliosRequest(HeliosClass, ServerUrl, InputValue);

	return MyObj;
}