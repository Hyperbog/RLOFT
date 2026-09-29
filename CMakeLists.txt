cmake_minimum_required(VERSION 3.20)
project(ssi_runtime_demo VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Keep all MSVC targets on the same runtime model. The alpha is intended to be
# portable as a test build, so Release uses the static CRT consistently across
# the executable and its static libraries.
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

add_library(ssi_core
    src/engine/core/FileSystem.cpp
    src/engine/render/IndexedFrameBuffer.cpp
)
target_include_directories(ssi_core PUBLIC src)

add_library(ravenloft_data
    src/games/ravenloft/data/Binary.cpp
    src/games/ravenloft/data/ResourceArchive.cpp
    src/games/ravenloft/data/Iso9660Image.cpp
    src/games/ravenloft/data/RavenloftDataSource.cpp
    src/games/ravenloft/data/LevelDatabase.cpp
    src/games/ravenloft/data/TextDatabase.cpp
    src/games/ravenloft/data/DefinitionDatabases.cpp
    src/games/ravenloft/data/InitDatabase.cpp
    src/games/ravenloft/data/PaletteDatabase.cpp
    src/games/ravenloft/data/Lzss.cpp
    src/games/ravenloft/data/GraphicsDecode.cpp
    src/games/ravenloft/data/SaveGame.cpp
    src/games/ravenloft/data/WorkLevel.cpp
    src/games/ravenloft/data/TrigTable.cpp
    src/games/ravenloft/data/AnmMovie.cpp
    src/games/ravenloft/data/ResourceStore.cpp
)
target_include_directories(ravenloft_data PUBLIC src)
target_link_libraries(ravenloft_data PUBLIC ssi_core)

add_library(ravenloft_game
    src/games/ravenloft/game/OpeningSequence.cpp
    src/games/ravenloft/game/OpeningDemoSession.cpp
    src/games/ravenloft/game/CinematicSession.cpp
    src/games/ravenloft/game/MistTransition.cpp
    src/games/ravenloft/game/AutomapState.cpp
    src/games/ravenloft/game/AutomapGlyphSelector.cpp
    src/games/ravenloft/game/AutomapView.cpp
    src/games/ravenloft/game/OpeningDemoFlow.cpp
    src/games/ravenloft/game/WorldScene.cpp
    src/games/ravenloft/game/CollisionSystem.cpp
    src/games/ravenloft/game/SpriteViewSelector.cpp
    src/games/ravenloft/game/ProjectionSystem.cpp
    src/games/ravenloft/game/SceneProjector.cpp
    src/games/ravenloft/game/SpriteRasterGeometry.cpp
    src/games/ravenloft/game/OriginalRenderQueue.cpp
    src/games/ravenloft/game/SpriteRasterizer.cpp
    src/games/ravenloft/game/SpritePassRenderer.cpp
    src/games/ravenloft/game/SkyRenderer.cpp
    src/games/ravenloft/game/FloorPlaneRenderer.cpp
    src/games/ravenloft/game/LightingSystem.cpp
    src/games/ravenloft/game/WorldRenderer.cpp
    src/games/ravenloft/game/PlayerController.cpp
    src/games/ravenloft/game/OriginalKeyboardMovement.cpp
    src/games/ravenloft/game/AdventureHudModel.cpp
    src/games/ravenloft/game/AdventureInterfaceTheme.cpp
    src/games/ravenloft/game/AdventureScreenRenderer.cpp
    src/games/ravenloft/game/CellInteractionSystem.cpp
    src/games/ravenloft/game/CellInteractionRuntimeIndex.cpp
    src/games/ravenloft/game/CellInteractionOccupancySystem.cpp
    src/games/ravenloft/game/CellInteractionScheduler.cpp
    src/games/ravenloft/game/CellInteractionSoundSystem.cpp
    src/games/ravenloft/game/NpcCellInteractionSystem.cpp
    src/games/ravenloft/game/NpcCandidateCellScanSystem.cpp
    src/games/ravenloft/game/NpcCandidateCollisionSystem.cpp
    src/games/ravenloft/game/CandidateCollisionSystem2D5D0.cpp
    src/games/ravenloft/game/PlayerCollisionTrigger2D004.cpp
    src/games/ravenloft/game/PortalCollisionTransition2D3D8.cpp
    src/games/ravenloft/game/SceneryCollisionSystem.cpp
    src/games/ravenloft/game/NpcOverlapSystem.cpp
    src/games/ravenloft/game/FilteredNpcOverlapSystem.cpp
    src/games/ravenloft/game/PartyOverlapSystem.cpp
    src/games/ravenloft/game/CandidateCollisionTailSystem.cpp
    src/games/ravenloft/game/MovementVector2DF2C.cpp
    src/games/ravenloft/game/NpcMovementTransaction1BBD8.cpp
    src/games/ravenloft/game/OriginalRng30CA2.cpp
    src/games/ravenloft/game/OriginalGameplayClock19C80.cpp
    src/games/ravenloft/game/OriginalWorldClock1E75C.cpp
    src/games/ravenloft/game/NpcDirectionSelector1AF90.cpp
    src/games/ravenloft/game/PartyNpcPredicate1B6EC.cpp
    src/games/ravenloft/game/NpcType8Relocate1B1C4.cpp
    src/games/ravenloft/game/NpcType8Decision1B27C.cpp
    src/games/ravenloft/game/NpcDecision1B760.cpp
    src/games/ravenloft/game/NpcTargetExclusionD4E8.cpp
    src/games/ravenloft/game/NpcHitTestC9A0.cpp
    src/games/ravenloft/game/CharacterDefenseHelpers.cpp
    src/games/ravenloft/game/ContainerWeightF5F8.cpp
    src/games/ravenloft/game/CharacterDefenseEFA0.cpp
    src/games/ravenloft/game/HandItemPredicates.cpp
    src/games/ravenloft/game/HandItemClassifier240A8.cpp
    src/games/ravenloft/game/HandItemUsability24478.cpp
    src/games/ravenloft/game/CharacterDamageCD44.cpp
    src/games/ravenloft/game/CharacterNameLookup14A60.cpp
    src/games/ravenloft/game/OriginalDice1C328.cpp
    src/games/ravenloft/game/NpcAttackDispatcherD550.cpp
    src/games/ravenloft/game/NpcScheduler1BDAC.cpp
    src/games/ravenloft/game/ObjectSpawn2195C.cpp
    src/games/ravenloft/game/NpcDamageE284.cpp
    src/games/ravenloft/game/PlayerHitTestC80C.cpp
    src/games/ravenloft/game/PlayerDamageModifierC910.cpp
    src/games/ravenloft/game/PlayerAttackStateE578.cpp
    src/games/ravenloft/game/PlayerAttackCooldown24B48.cpp
    src/games/ravenloft/game/PlayerAttackTimers1E4A0.cpp
    src/games/ravenloft/game/PlayerNpcAttackE5F0.cpp
    src/games/ravenloft/game/PlayerHandAttack26018.cpp
    src/games/ravenloft/game/PlayerTargetSelectorDB94.cpp
    src/games/ravenloft/game/PlayerHandClick214B8.cpp
    src/games/ravenloft/game/ObjectOwnershipF390F4B0.cpp
    src/games/ravenloft/game/ObjectRemovalF40CF540.cpp
    src/games/ravenloft/game/InventorySlotExchange117A0.cpp
    src/games/ravenloft/game/InventoryGridClick11FB0.cpp
    src/games/ravenloft/game/GroundObjectPickup21704.cpp
    src/games/ravenloft/game/OpeningGroundPickup223F4.cpp
    src/games/ravenloft/game/OpeningGroundObjectHit223F4.cpp
)
target_include_directories(ravenloft_game PUBLIC src)
target_link_libraries(ravenloft_game PUBLIC ravenloft_data)

add_executable(ravenloft_probe tools/ravenloft_probe/main.cpp)
target_link_libraries(ravenloft_probe PRIVATE ravenloft_data)

add_executable(ravenloft_data_tests tests/data_tests.cpp)
target_link_libraries(ravenloft_data_tests PRIVATE ravenloft_game)

enable_testing()
add_test(NAME ravenloft_data_tests COMMAND ravenloft_data_tests)

add_executable(ravenloft_anm_dump tools/anm_dump/main.cpp)
target_link_libraries(ravenloft_anm_dump PRIVATE ravenloft_data)

add_executable(ravenloft_render_probe tools/render_probe/main.cpp)
target_link_libraries(ravenloft_render_probe PRIVATE ravenloft_game)

if(WIN32)
    add_executable(RavenloftAlpha WIN32 apps/win32_alpha/main.cpp)
    target_compile_definitions(RavenloftAlpha PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX UNICODE _UNICODE)
    target_link_libraries(RavenloftAlpha PRIVATE ravenloft_game shell32 user32 gdi32 ole32)
endif()
