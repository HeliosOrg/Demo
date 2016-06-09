#include "SimpleDataIntegrationPCH.h"
#include "HeliosGetterProxy_FString.h"

UHeliosGetterProxy_FString::UHeliosGetterProxy_FString(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Empty.
}

void UHeliosGetterProxy_FString::SendHeliosRequest(const FName HeliosClass, const FName ServerUrl)
{
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb("GET");
	HttpRequest->SetURL(ServerUrl.ToString() + HeliosClass.ToString().ToLower());
	HttpRequest->SetHeader("Content-Type", TEXT("application/json"));
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UHeliosGetterProxy_FString::OnHeliosRequestComplete);
	HttpRequest->ProcessRequest();
}

void UHeliosGetterProxy_FString::OnHeliosRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded) {
	if (!bSucceeded) {
		UE_LOG(LogTemp, Error, TEXT("Helios > request failed: %s"), *HttpRequest->GetURL());
		OnFail.Broadcast(FString(TEXT("error")));
		return;
	}
	if (!HttpResponse.IsValid()) {
		UE_LOG(LogTemp, Warning, TEXT("Helios > invalid response for request: %s"), *HttpRequest->GetURL());
		OnFail.Broadcast(FString(TEXT("error")));
		return;
	}

	FString ContentAsString = HttpResponse->GetContentAsString();
	json Content = json::parse(TCHAR_TO_UTF8(*ContentAsString));
	if (Content.size() != 1) {
		UE_LOG(LogTemp, Warning, TEXT("Helios > invalid json returned"));
		OnFail.Broadcast(FString(TEXT("error")));
		return;
	}
	if (!Content["value"].is_string()) {
		UE_LOG(LogTemp, Warning, TEXT("Helios > invalid type returned"));
		OnFail.Broadcast(FString(TEXT("error")));
		return;
	}
	std::string ResultString = Content["value"];
	FString Result(ResultString.c_str());
	OnSuccess.Broadcast(Result);
}