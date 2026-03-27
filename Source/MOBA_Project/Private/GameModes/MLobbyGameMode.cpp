#include "GameModes/MLobbyGameMode.h"

#include "Network/MGameSession.h"

AMLobbyGameMode::AMLobbyGameMode()
{
	bUseSeamlessTravel = true;
	GameSessionClass = AMGameSession::StaticClass();
}
