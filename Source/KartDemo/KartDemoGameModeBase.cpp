// Fill out your copyright notice in the Description page of Project Settings.


#include "KartDemoGameModeBase.h"
#include "Public/Kart.h"

AKartDemoGameModeBase::AKartDemoGameModeBase()
{
	DefaultPawnClass = AKart::StaticClass();
}
