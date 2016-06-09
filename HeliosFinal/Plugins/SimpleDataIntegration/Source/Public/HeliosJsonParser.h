#pragma once

#include "SimpleDataIntegrationPCH.h"
#include "K2Node_Helios.h"

struct HeliosNodeProperties {
	FName name;
	EHeliosClassType type;
};

class HeliosJsonParser
{

public:
	static TArray<HeliosNodeProperties> *getVariableNames();
	static FName getServerUrl();

//private:
	static bool getInputFileContents(FString &fileContents);
	static bool parseJson(FString jsonString);

};
