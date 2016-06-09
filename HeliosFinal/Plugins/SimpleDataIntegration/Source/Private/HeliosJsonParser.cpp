#include "SimpleDataIntegrationPCH.h"
#include "HeliosJsonParser.h"

#include "json.hpp"
#include "CoreMisc.h"

#include "Paths.h"

const FString INPUT_FILE_PATH = FPaths::GamePluginsDir() + FString(TEXT("SimpleDataIntegration/input.json"));

const char *SINGLE_VARS = "single_instance_variables";
const char *SERVER_URL = "server_url";

using json = nlohmann::json;


bool HeliosJsonParser::getInputFileContents(FString &fileContents) {
	if (!FPaths::FileExists(*INPUT_FILE_PATH)) {

		UE_LOG(LogTemp, Error, TEXT("Helios > Input File Does Not Exist"));
		UE_LOG(LogTemp, Error, TEXT("  --  Please Add input.js JSON File in Helios Directory"));

		return false;
	}

	FFileHelper::LoadFileToString(fileContents, *INPUT_FILE_PATH, 0);
	return true;
}

FName HeliosJsonParser::getServerUrl() {
	FString jsonString;

	bool fileLoadedSuccessfully = getInputFileContents(jsonString);
	if (!fileLoadedSuccessfully) {
		return FName("");
	}

	auto j = json::parse(TCHAR_TO_UTF8(*jsonString));

	if (j.count(SERVER_URL) == 1) {
		auto urlJson = j.find(SERVER_URL).value();
		return FName(UTF8_TO_TCHAR(urlJson.get<std::string>().c_str()));
	} else {
		// Server URL not found in JSON.
		UE_LOG(LogTemp, Error, TEXT("Helios > Server URL Value Does Not Exist"));
		UE_LOG(LogTemp, Error, TEXT("  --  Please add 'server_url' value to input.js"));
		return FName("");

	}
}

TArray<HeliosNodeProperties> *HeliosJsonParser::getVariableNames() {

	FString jsonString;

	bool fileLoadedSuccessfully = getInputFileContents(jsonString);
	if (!fileLoadedSuccessfully) {
		return new TArray<HeliosNodeProperties>();
	}

	auto j = json::parse(TCHAR_TO_UTF8(*jsonString));

	TArray<HeliosNodeProperties> *variableNames = new TArray<HeliosNodeProperties>();

	if (j.count(SINGLE_VARS) == 1) {
		auto s = j.find(SINGLE_VARS);
		for (auto& var : s.value()) {
			auto nameJson = var.find("name").value();
			auto typeJson = var.find("type").value();
			HeliosNodeProperties newNode;

			auto nameStr = UTF8_TO_TCHAR(nameJson.get<std::string>().c_str());
			newNode.name = FName(nameStr);

			std::string typeStr = typeJson.get<std::string>();
			if (typeStr == "FString") {
				newNode.type = EHeliosClassType::HeliosFString;
			}
			else if (typeStr == "bool") {
				newNode.type = EHeliosClassType::HeliosBool;
			}
			else if (typeStr == "int") {
				newNode.type = EHeliosClassType::HeliosInt;
			}
			else if (typeStr == "float") {
				newNode.type = EHeliosClassType::HeliosFloat;
			}
			else {
				UE_LOG(LogTemp, Error, TEXT("Helios > %s Has Invalid Type"), nameStr);
				UE_LOG(LogTemp, Error, TEXT("  -- Accepted Types: FString, bool, int, and float"));
			}

			variableNames->Add(newNode);
		}
	}

	return variableNames;
}