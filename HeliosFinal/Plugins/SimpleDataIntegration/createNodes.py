import json

INPUT_FILE_NAME = "input.json"

def generate_nodes():
	file_h = open("Source/Public/ExampleInterfaceComponent.h", "w+")
	file_cpp = open("Source/Private/ExampleInterfaceComponent.cpp", "w+")

	file_h.write("""// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "ExampleInterfaceComponent.generated.h"

/**
 * A component that behaves as mix-in functionality for interfacing with the singleton.  It contains
 * both functions and delegates for interfacing with any important functionality of the singleton, while
 * hiding the direct implementation.
 *
 * Any Actor in the world that would need to interact with the device should access it through this component
 */
UCLASS(ClassGroup = Utility, HideCategories = (Activation, "Components|Activation", Collision), meta = (BlueprintSpawnableComponent))
class EXAMPLEPLUGIN_API UExampleInterfaceComponent : public UActorComponent
{
	GENERATED_UCLASS_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FExampleCallback, FString, MyPayload);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSuccess, FString, MyPayload);

	void RequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	void HelioRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	FString generateNonce();
	FString generateTimestamp();
	FString UExampleInterfaceComponent::GenerateBearerToken();

	// --- UComponentInterface --------------------------
	// These are called when the delegate is added or removed from an actor.  We override these to hook up our delegate listeners to the ExampleSingleton
	void OnRegister() override;
	void OnUnregister() override;

	/**
	 * Simple example function which will request that the FExampleSingleton to issue a callback.  This is analogous to something like making a server request in a real-world example.
	 * It's callable either from C++ or a Blueprint
	 */
	UFUNCTION(BlueprintCallable, Category = "ExamplePlugin")
	void RequestFakeCallback(FString Query, FString ConsumerKey, FString ConsumerSecret);

	UFUNCTION(BlueprintCallable, Category = "ExamplePlugin")
	void RequestMostRecentTweet(FString Query, FString ConsumerKey, FString ConsumerSecret);

	UFUNCTION(BlueprintCallable, Category = "ExamplePlugin")
	void RequestUrl(FString Url);

	UPROPERTY(BlueprintAssignable, Category = "ExamplePlugin|Event")
	FOnSuccess OnSuccess;

	/**
	 * This is a delegate that will get triggered when the FExampleSingleton fires off it's broadcast after RequestFakeCallback() is called.  This is analogous to a server response in a real-world example.
	 * It's able to be bound from either C++ to a native function, or in Blueprints as an "Event"
	 */
	UPROPERTY(BlueprintAssignable, Category = "ExamplePlugin")
	FExampleCallback OnFakeCallback;

	/**
	 * Native handler to route from the FExampleSingleton to the OnFakeCallback delegate.
	 * This is essentially plumbing from FExampleSingleton -> UExampleComponent -> Whatever is listening to our OnFakeFallback delegate, and allows us to hide the delegate from other modules.
	 * In OnRegister, we bind this handler to FExampleSingleton's delegate callback, and when we receive a callback from that, we forward it on to our OnFakeCallback
	 */
	void OnFakeCallback_Handler(FString& MyPayload) { OnFakeCallback.Broadcast(MyPayload); }

	// UNIVERSAL

	void SetRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	// GENERATION

""")


	file_cpp.write(
	"""#include "ExamplePluginPCH.h"
#include "ExampleInterfaceComponent.h"
#include <string>
#include "json.hpp"
//=============================================================================
//	ExampleInterfaceComponent
//=============================================================================

using json = nlohmann::json;

UExampleInterfaceComponent::UExampleInterfaceComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

FString UExampleInterfaceComponent::generateNonce()
{
	FString ALPHANUMERIC = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789%";
	FString nonce;

	for (int i = 0; i <= 32; ++i)
	{
		nonce += ALPHANUMERIC[rand() % (ALPHANUMERIC.Len() - 1)]; // don't count null terminator in array
	}
	return nonce;
}


FString UExampleInterfaceComponent::generateTimestamp()
{
	FString time = FString::FromInt(abs(FDateTime().UtcNow().ToUnixTimestamp()));
	return time;
}


FString UExampleInterfaceComponent::GenerateBearerToken()
{
	FString bearerToken;
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb("GET");
	return bearerToken;
}


void UExampleInterfaceComponent::OnRegister()
{
	Super::OnRegister();
	UE_LOG(LogTemp, Warning, TEXT("registering..."));
	FExampleSingleton* Singleton = FExampleSingleton::Get();
	if (Singleton)
	{
		// Register our OnFakeCallback_Handler to be notified when the Singleton fires FakeCallbackDelegate
		Singleton->FakeCallbackDelegate.RemoveAll(this);
		Singleton->FakeCallbackDelegate.AddUObject(this, &UExampleInterfaceComponent::OnFakeCallback_Handler);
	}
}

void UExampleInterfaceComponent::OnUnregister()
{
	Super::OnUnregister();

	FExampleSingleton* Singleton = FExampleSingleton::Get();
	if (Singleton)
	{
		// Clean up when we're no longer listening for delegates
		Singleton->FakeCallbackDelegate.RemoveAll(this);
	}
}


void UExampleInterfaceComponent::RequestFakeCallback(FString Query, FString ConsumerKey, FString ConsumerSecret)
{
	UE_LOG(LogTemp, Warning, TEXT("requesting fake callback..."));
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetHeader("Content-Type", TEXT("application/json"));

	// attempting to generate nonce and timestamp
	//FString authHeader = "OAuth oauth_consumer_key =\"X47DSa0KwaU9cLJ6jd9cSX1Lh\", oauth_nonce =\"";
	//authHeader.Append(UExampleInterfaceComponent::generateNonce()).Append("\",oauth_signature = \"nT5rDM%2BV7jAjfzZYFJ8eK3ETO8Q%3D\", oauth_signature_method =\"HMAC-SHA1\", oauth_timestamp =\"").Append(UExampleInterfaceComponent::generateTimestamp()).Append("\", oauth_version = \"1.0\"");
	//HttpRequest->SetHeader("Authorization", authHeader);

	//HttpRequest->SetHeader("Authorization", "OAuth oauth_consumer_key =\"X47DSa0KwaU9cLJ6jd9cSX1Lh\", oauth_nonce =\"b861ff628a7855d67c50d5176387d1e8\", oauth_signature =\"nT5rDM%2BV7jAjfzZYFJ8eK3ETO8Q%3D\", oauth_signature_method =\"HMAC-SHA1\", oauth_timestamp =\"1457417396\", oauth_version = \"1.0\"");
	/*HttpRequest->SetHeader("Authorization", "OAuth oauth_consumer_key =\"X47DSa0KwaU9cLJ6jd9cSX1Lh\", oauth_nonce =\"f7d10f3cd3ad22d04a969dcd29448ae0\", oauth_signature =\"zOD34oxgps5hsGY9K%2FPXyNaZcmY%3D\", oauth_signature_method =\"HMAC-SHA1\", oauth_timestamp =\"1457487480\", oauth_version = \"1.0\"");*/
	/*HttpRequest->SetHeader("Authorization", "OAuth oauth_consumer_key =\"X47DSa0KwaU9cLJ6jd9cSX1Lh\", oauth_nonce =\"b02d0d76fe57a534bbac357a8f7221e9\", oauth_signature =\"xXqi5nKc%2BgRfCEwAjpVWVe%2FvbIo%3D\", oauth_signature_method =\"HMAC-SHA1\", oauth_timestamp =\"1457506238\", oauth_version = \"1.0\"");*/
	//HttpRequest->SetHeader("Authorization", "OAuth oauth_consumer_key =\"X47DSa0KwaU9cLJ6jd9cSX1Lh\", oauth_nonce =\"" + nonce + "\", oauth_signature =\"" + signature + "\", oauth_signature_method =\"HMAC-SHA1\", oauth_timestamp =\"" + timestamp + "\", oauth_version = \"1.0\"");
	UE_LOG(LogTemp, Warning, TEXT("signature for query %s"), *Query);

	//HttpRequest->SetURL("https://api.twitter.com/1.1/trends/available.json");

	////HttpRequest->SetURL("https://api.twitter.com/1.1/trends/place.json?id=1");
	//HttpRequest->SetURL("https://api.twitter.com/1.1/search/tweets.json?q=%23" + query + "&result_type=recent&count=1");
	HttpRequest->SetURL("https://shrouded-fjord-55764.herokuapp.com/twitter/search/tweets/popular?q=%23" + Query + "&consumer_key=" + ConsumerKey + "&consumer_secret=" + ConsumerSecret);

	HttpRequest->SetVerb("GET");
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UExampleInterfaceComponent::RequestComplete);

	////debugging code
	//UE_LOG(LogTemp, Warning, TEXT("generated nonce: %s"), *UExampleInterfaceComponent::generateNonce());
	//UE_LOG(LogTemp, Warning, TEXT("generated timestamp: %s"), *UExampleInterfaceComponent::generateTimestamp());

	//TArray<FString> headers = HttpRequest->GetAllHeaders();
	//UE_LOG(LogTemp, Warning, TEXT("headers:"));
	//for (FString header : headers) {
	//	UE_LOG(LogTemp, Warning, TEXT("%s"), *header);
	//}
	////debugging code end

	HttpRequest->ProcessRequest();
}

void UExampleInterfaceComponent::RequestMostRecentTweet(FString Query, FString ConsumerKey, FString ConsumerSecret)
{
	UE_LOG(LogTemp, Warning, TEXT("Requesting most recent tweet: %s"), *Query);
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetHeader("Content-Type", TEXT("application/json"));
	HttpRequest->SetURL("https://shrouded-fjord-55764.herokuapp.com/twitter/search/tweets?q=%23" + Query + "&consumer_key=" + ConsumerKey + "&consumer_secret=" + ConsumerSecret);
	HttpRequest->SetVerb("GET");
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UExampleInterfaceComponent::HelioRequestComplete);
	HttpRequest->ProcessRequest();
}

void UExampleInterfaceComponent::RequestUrl(FString Url)
{
	UE_LOG(LogTemp, Warning, TEXT("Requesting url: %s"), *Url);
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetHeader("Content-Type", TEXT("application/json"));
	HttpRequest->SetURL(Url);
	HttpRequest->SetVerb("GET");
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UExampleInterfaceComponent::HelioRequestComplete);
	HttpRequest->ProcessRequest();
}

void UExampleInterfaceComponent::HelioRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded) {
	if (!bSucceeded) {
		UE_LOG(LogTemp, Error, TEXT("Helios > request failed: %s"), *HttpRequest->GetURL());
		return;
	}
	if (!HttpResponse.IsValid()) {
		UE_LOG(LogTemp, Warning, TEXT("Helios > invalid response for request: %s"), *HttpRequest->GetURL());
		return;
	}

	FString responseContent = HttpResponse->GetContentAsString();
	UE_LOG(LogTemp, Warning, TEXT("valid response: %s"), *responseContent);
	OnSuccess.Broadcast(responseContent);
}


void UExampleInterfaceComponent::RequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded) {
	FString result("invalid");
	if (!HttpResponse.IsValid()) {
		UE_LOG(LogTemp, Warning, TEXT("invalid http response"));
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Response is: %s"), *HttpResponse->GetContentAsString());

		// TRENDS
		/*json trends = json::parse(TCHAR_TO_UTF8(*HttpResponse->GetContentAsString()))[0]["trends"];

		for (auto& trend : trends) {
			FString trendName(trend["name"].dump().c_str());
			UE_LOG(LogTemp, Warning, TEXT("Trend: %s"), *trendName);
		}
		json tempJson = json::parse(TCHAR_TO_UTF8(*HttpResponse->GetContentAsString()))[0]["trends"][0]["name"];*/

		// STATUSES
		json firstStatus = json::parse(TCHAR_TO_UTF8(*HttpResponse->GetContentAsString()))["statuses"][0]["text"];
		FString statusString(firstStatus.dump().c_str());
		result = statusString;
		UE_LOG(LogTemp, Warning, TEXT("Result is: %s"), *result);
	}

	FExampleSingleton* Singleton = FExampleSingleton::Get();
	if (Singleton)
	{
		// Call the simple function on the Singleton, which will cause the FakeCallbackDelegate to be fired.
		Singleton->RequestFakeCallback(result);
	}
}

// UNIVERSAL

void UExampleInterfaceComponent::SetRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded) {
	if (!bSucceeded) {
		UE_LOG(LogTemp, Error, TEXT("Helios > request failed: %s"), *HttpRequest->GetURL());
		return;
	}
	if (!HttpResponse.IsValid()) {
		UE_LOG(LogTemp, Warning, TEXT("Helios > invalid response for request: %s"), *HttpRequest->GetURL());
		return;
	}

	FString ContentAsString = HttpResponse->GetContentAsString();
	UE_LOG(LogTemp, Warning, TEXT("SetIsLightOnRequestComplete: %s"), *ContentAsString);
}

// GENERATION""");

	# JSON

	with open(INPUT_FILE_NAME) as json_file:
		json_data = json.load(json_file)

	multiple_instance_variables = json_data["multiple_instance_variables"]
	single_instance_variables = json_data["single_instance_variables"]

	# Multiple Instance Node Generation

	# for instance in multiple_instance_variables:
	# 	hInstanceString = """

	# 	UFUNCTION(BlueprintCallable, Category = "Example Plugin")
	# 	void """ + instance["name"] + "("

	# 	cppInstanceString = """

	# void UExampleInterfaceComponent::""" + instance["name"] + "("

	# 	subString = ""
	# 	for x in range(0, len(instance["variables"])):
	# 		var = instance["variables"][x]
	# 		subString += var["type"]
	# 		subString += " "
	# 		subString += var["keyname"]
	# 		if x != len(instance["variables"])-1:
	# 			subString += ", "

	# 	hInstanceString += subString + ");"
	# 	cppInstanceString += subString + """)
	# {
	# 	// fill in spectacular magic functionality here
	# 	UE_LOG(LogTemp, Warning, TEXT("sparkles"));

	# } """
	# 	file_h.write(hInstanceString)
	# 	file_cpp.write(cppInstanceString)

	# Single Instance Node Generation

	for instance in single_instance_variables:

		instance_name = instance["name"]
		instance_type = instance["type"]

		# Header File (.h)

		# Getter

		hInstanceString = """

	// Getter

	UFUNCTION(BlueprintCallable, Category = "Example Plugin")
	void Get"""
		hInstanceString += instance["name"] + "();"

		hInstanceString += """

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(F"""
		hInstanceString += instance["name"] + "Returned, " + instance["type"] + ", " + instance["name"] + ");"

		hInstanceString += """

	UPROPERTY(BlueprintAssignable, Category = "ExamplePlugin|Event")
	F"""
		hInstanceString += instance["name"] + "Returned " + instance["name"] + "Returned;"

		hInstanceString += """

	void Get"""
		hInstanceString += instance["name"] + "RequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);"

		# Setter

		hInstanceString += """

	// Setter

	UFUNCTION(BlueprintCallable, Category = "Example Plugin")
	void Set"""
		hInstanceString += instance["name"] + "(" + instance["type"] + " " + instance["name"] + "); "

		file_h.write(hInstanceString)


		# Implementation File (.cpp)

		# Getter

		cppInstanceString = """

// Getter

void UExampleInterfaceComponent::Get{name}()
{{
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb("GET");
	HttpRequest->SetURL("http://ec2-54-183-240-19.us-west-1.compute.amazonaws.com/helios/{lowercase_name}");
	HttpRequest->SetHeader("Content-Type", TEXT("application/json"));
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UExampleInterfaceComponent::Get{name}RequestComplete);
	HttpRequest->ProcessRequest();
}}

void UExampleInterfaceComponent::Get{name}RequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded) {{
	if (!bSucceeded) {{
		UE_LOG(LogTemp, Error, TEXT("Helios > request failed: %s"), *HttpRequest->GetURL());
		return;
	}}
	if (!HttpResponse.IsValid()) {{
		UE_LOG(LogTemp, Warning, TEXT("Helios > invalid response for request: %s"), *HttpRequest->GetURL());
		return;
	}}

	FString ContentAsString = HttpResponse->GetContentAsString();
	UE_LOG(LogTemp, Warning, TEXT("Get{name}RequestComplete: %s"), *ContentAsString);
	json Content = json::parse(TCHAR_TO_UTF8(*ContentAsString));
	FString JsonContent(Content.dump().c_str());
	UE_LOG(LogTemp, Warning, TEXT("json content: %s"), *JsonContent);
	if (Content.size() != 1) {{
		UE_LOG(LogTemp, Warning, TEXT("Helios > invalid json returned"));
		return;
	}}
	if (!Content["value"].is_{type_method}()) {{
		UE_LOG(LogTemp, Warning, TEXT("Helios > invalid type returned"));
		return;
	}}"""

		if instance_type == "FString":
			cppInstanceString += """
	std::string ResultString = Content["value"];
	FString Result(ResultString.c_str());"""
		else:
			cppInstanceString += """
	{type} Result = Content["value"];"""

		cppInstanceString += """
	{name}Returned.Broadcast(Result);
}}

// Setter

void UExampleInterfaceComponent::Set{name}({type} {name}) {{
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb("POST");
	HttpRequest->SetURL("http://ec2-54-183-240-19.us-west-1.compute.amazonaws.com/helios/{lowercase_name}");
	HttpRequest->SetHeader("Content-Type", TEXT("application/json"));

	json Content;"""

		if instance_type == "FString":
			cppInstanceString += """
	std::string ContentString(TCHAR_TO_UTF8(*{name}));
	Content["value"] = ContentString;"""
		else:
			cppInstanceString += """
	Content["value"] = {name};"""

		cppInstanceString += """
	FString ContentAsString(Content.dump().c_str());
	HttpRequest->SetContentAsString(ContentAsString);

	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UExampleInterfaceComponent::SetRequestComplete);
	HttpRequest->ProcessRequest();
}}"""

		instance_type_method = None
		if instance_type == "bool":
			instance_type_method = "boolean"
		elif instance_type == "FString":
			instance_type_method = "string"
		elif instance_type == "int":
			instance_type_method = "number_integer"
		elif instance_type == "float":
			instance_type_method = "number_float"
		if instance_type_method is None:
			print "TYPE ERROR"
			return

		cppInstanceString = cppInstanceString.format(
			name = instance_name,
			lowercase_name = instance_name.lower(),
			type = instance_type,
			type_method = instance_type_method)

		file_cpp.write(cppInstanceString)

	file_h.write("\n};")

	file_h.close()
	file_cpp.close()

if __name__ == '__main__':
	generate_nodes()
