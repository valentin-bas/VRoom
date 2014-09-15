

#include "VRoom.h"
#include "VRoomGameMode.h"
#include "VRoomPlayerController.h"


AVRoomGameMode::AVRoomGameMode(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	//backup
	PlayerControllerClass = AVRoomPlayerController::StaticClass();
	
	//Blueprinted Version, relies on the "Copy Reference" asset path you get from Editor
	static ConstructorHelpers::FObjectFinder<UBlueprint> VRoomPCOb(TEXT("Blueprint'/Game/MyAssets/Blueprints/VRoomPlayerController_BP.VRoomPlayerController_BP'"));
	if (VRoomPCOb.Object != NULL)
	{
		PlayerControllerClass = (UClass*)VRoomPCOb.Object->GeneratedClass;
	}
}