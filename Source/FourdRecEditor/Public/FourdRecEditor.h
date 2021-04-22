// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FFourdRecEditorModule : public IModuleInterface
{
	void AddMenuBarExtension(FMenuBarBuilder& MenuBarBuilder);
	void AddMenuExtension(FMenuBuilder& Builder);
	void ImportCallback();

public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
