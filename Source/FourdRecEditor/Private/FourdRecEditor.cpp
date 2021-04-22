// Copyright Epic Games, Inc. All Rights Reserved.
#include "FourdRecEditor.h"
#include "LevelEditor.h"

#define LOCTEXT_NAMESPACE "FFourdRecEditorModule"

void FFourdRecEditorModule::AddMenuBarExtension(FMenuBarBuilder& MenuBarBuilder)
{
	MenuBarBuilder.AddPullDownMenu(
		FText::FromString("4DREC"),
		FText::FromString("4DREC moonshine import plugin"),
		FNewMenuDelegate::CreateRaw(this, &FFourdRecEditorModule::AddMenuExtension),
		"AfterHelp"
	);
}

void FFourdRecEditorModule::AddMenuExtension(FMenuBuilder& Builder)
{
	Builder.BeginSection("CustomSection", LOCTEXT("CustomArea", "CustomArea"));
	Builder.AddMenuEntry(
		FText::FromString("Import 4D file"),
		FText::FromString("Import 4D file"),
		FSlateIcon(),
		FUIAction(FExecuteAction::CreateRaw(this, &FFourdRecEditorModule::ImportCallback)));
	Builder.EndSection();
}

void FFourdRecEditorModule::ImportCallback()
{
	UE_LOG(LogTemp, Warning, TEXT("Import YA!"));
}

void FFourdRecEditorModule::StartupModule()
{
	// Create the Extender that will add content to the menu
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor"); 
	TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());
	MenuExtender->AddMenuBarExtension(
	    "Help",
	    EExtensionHook::After,
	    nullptr,
	    FMenuBarExtensionDelegate::CreateRaw(this, &FFourdRecEditorModule::AddMenuBarExtension)
	);
	LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);
}

void FFourdRecEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FFourdRecEditorModule, FourdRecEditor)