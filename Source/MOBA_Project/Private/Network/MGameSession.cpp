// Fill out your copyright notice in the Description page of Project Settings.

#include "Network/MGameSession.h"
#include "Framework/MGameInstance.h"
#include "Network/MNetStatics.h"

bool AMGameSession::ProcessAutoLogin()
{
	return true;
}

void AMGameSession::RegisterPlayer(APlayerController* NewPlayer, const FUniqueNetIdRepl& UniqueId, bool bWasFromInvite)
{
	Super::RegisterPlayer(NewPlayer, UniqueId, bWasFromInvite);

	if (UMGameInstance* GI = GetGameInstance<UMGameInstance>())
		GI->PlayerJoined(UniqueId);
}

void AMGameSession::UnregisterPlayer(FName FromSessionName, const FUniqueNetIdRepl& UniqueId)
{
	Super::UnregisterPlayer(FromSessionName, UniqueId);
	
	if (!UMNetStatics::IsSessionServer(this)) return;
	
	if (UMGameInstance* GI = GetGameInstance<UMGameInstance>())
		GI->PlayerLeft(UniqueId);
}
