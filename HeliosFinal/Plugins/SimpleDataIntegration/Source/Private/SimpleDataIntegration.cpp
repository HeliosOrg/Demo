#include "SimpleDataIntegrationPCH.h"
#include "ISimpleDataIntegration.h"

//=============================================================================
//	Plugin Boilerplate
//	------------------
//	This contains an actual implementation of the ISimpleDataIntegration interface.
//	Nothing special, other than any init / shutdown code needs to happen here.
//=============================================================================

class FSimpleDataIntegration : public ISimpleDataIntegration
{
public:
	FSimpleDataIntegration()
	{
	}

	/**
	* Called right after the module DLL has been loaded and the module object has been created
	*/
	virtual void StartupModule() override
	{
		// Do some startup here!
	}

	/**
	* Called before the module is unloaded, right before the module object is destroyed.
	*/
	virtual void ShutdownModule()
	{
		// Do some shutdown stuff here!
	}
};

// Special Macro!  This needs to be implemented somewhere in your module, to allow the engine to implement the thunk and loading code
IMPLEMENT_MODULE(FSimpleDataIntegration, SimpleDataIntegration);