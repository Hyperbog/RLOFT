#!/usr/bin/env python3
"""Source-based contract for Level22 relic event -> real Level23 continuity.

The user-facing 22-step palette fade is explicitly native PC presentation;
source assets for original DOS 2E510 mist frames remain future work.
"""
from pathlib import Path

ROOT=Path(__file__).resolve().parents[2]

def check(playback:str,runtime:str,host:str,tests:str,save:str,
          cmake:str,workflow:str)->None:
    for marker in (
        'frame<AssassinRelicTransition::kTransitionFrameCount;',
        'if(!present(frame))return false;',
        'if(frame==AssassinRelicTransition::kLevelChangeFrame)',
        'enter(AssassinRelicTransition::kDestinationLevel);',
        'for(auto relic:AssassinRelicTransition::kConsumedAfterTransition)',
    ):
        if marker not in playback:
            raise ValueError('authentic 22-frame event ordering changed: '+marker)
    if not (playback.index('if(!present(frame))return false;') <
            playback.index('if(frame==AssassinRelicTransition::kLevelChangeFrame)') <
            playback.index('for(auto relic:AssassinRelicTransition::kConsumedAfterTransition)')):
        raise ValueError('relics removed before original frame21 playback')

    for marker in (
        'ExplorationRuntime staged=*this;',
        'AssassinRelicTransition::kStagingColumn',
        'AssassinRelicTransition::kStagingRow',
        'AssassinRelicTransition::kStagingFacing',
        'for(const auto relic:AssassinRelicTransition::kRelics)',
        'inventory_.has_object(session_.party,object_catalog_,',
        'const bool completed=Level22QuestPlayback::execute(',
        'staged.transition_to_level(',
        'AssassinRelicTransition::kDestinationCoordinateA',
        'AssassinRelicTransition::kDestinationCoordinateB',
        'AssassinRelicTransition::kDestinationFacing',
        'staged.inventory_.remove_object(',
        'if(!completed)return false;',
        '*this=std::move(staged);',
        '++world_scene_revision_;',
    ):
        if marker not in runtime:
            raise ValueError('Level22 runtime incomplete or not transactional: '+marker)
    transition=runtime[runtime.index(
        'bool ExplorationRuntime::complete_level22_relic_transition('):
        runtime.index('ExplorationRuntime::Level22PickupProgress')]
    if 'ExplorationRuntime staged=*this;' not in transition:
        raise ValueError('mist transition is mutating authoritative runtime')
    if transition.index('*this=std::move(staged);') <
            transition.index('if(!completed)return false;'):
        raise ValueError('mist quest committed before frame playback finished')
    # The original 21704 pickup and 21F70 event MUST commit as
    # one persistent transaction: an aborted frame must not consume the
    # fourth trigger relic and make a retry impossible.
    transaction=runtime[runtime.index(
        'ExplorationRuntime::pickup_and_advance_level22_at('):
        runtime.index('InventoryPartyOrderResult ExplorationRuntime::exchange_party_order(')]
    for marker in (
        'ExplorationRuntime staged=*this;',
        'auto pickup=staged.pickup_level22_loose_item_at(',
        'if(!pickup)return {};',
        'staged.complete_level22_relic_transition(',
        'return {std::nullopt,true,false};',
        '*this=std::move(staged);',
        'return {std::move(pickup),false,warped};',
    ):
        if marker not in transaction:
            raise ValueError('fourth pickup no longer atomic: '+marker)
    if not (transaction.index('ExplorationRuntime staged=*this;') <
            transaction.index('auto pickup=staged.pickup_level22_loose_item_at(') <
            transaction.index('staged.complete_level22_relic_transition(') <
            transaction.index('return {std::nullopt,true,false};') <
            transaction.index('*this=std::move(staged);')):
        raise ValueError('world item removed before transaction completes')

    beginning=host.index('bool cinematic_input_detached=false;')
    ending=host.index('} catch(const std::logic_error&)',beginning)
    quest_host=host[beginning:ending]
    for marker in (
        'runtime.pickup_and_advance_level22_at(',
        'assets.source(),',
        'staged.build_frame();',
        'presenter.present(',
        'rloft::StartupPresenter::fade_factor(',
        'frame==21U?4:',
        'host.pause_frame(',
        'host.set_exploration_input(nullptr);',
        'host.set_exploration_input(&input);',
        'if(progress.pickup)',
        'if(progress.entered_level23)',
        'else if(progress.transition_cancelled)',
        'refresh_world_palette();',
        'world_music_state.invalidate();',
        'start_authored_world_music();',
        'displayed_scene_frame.reset();',
    ):
        if marker not in quest_host:
            raise ValueError('live original quest progression unwired: '+marker)
    if quest_host.index('host.set_exploration_input(nullptr);') >= quest_host.index(
            'host.pause_frame('):
        raise ValueError('mist playback captured player input before frame')
    if quest_host.index('host.set_exploration_input(&input);') <= quest_host.index(
            'host.pause_frame('):
        raise ValueError('mist input restored before effect finishes')
    if 'mist transition pending' in quest_host:
        raise ValueError('placeholder transition still exposed')
    for marker in (
        'events.size()==26U',
        'events.at(21)=="load 23"',
        'events.at(22)=="frame 21"',
        'events.at(23)=="consume 135"',
        'events.at(24)=="consume 136"',
        'events.at(25)=="consume 116"',
        'interrupted<22U',
        'assert(!completed && !consumed)',
    ):
        if marker not in tests:
            raise ValueError('source-order and abort tests missing: '+marker)
    for marker in (
        'kDestinationCoordinateA',
        'kDestinationCoordinateB',
        'kDestinationFacing',
        'old22.world.npcs[0].current_hp==0',
        'warped.inventory.held.object_id==0xE9',
        'warped.visited_levels.count(22)==1U',
    ):
        if marker not in save:
            raise ValueError('native save cannot continue original quest: '+marker)
    for marker in (
        'src/level22_quest_playback.cpp',
        'add_executable(level22_quest_playback_tests',
        'add_test(NAME level22_quest_playback_tests',
        'NAME enhanced_level22_quest_transition_contract_tests',
    ):
        if marker not in cmake:
            raise ValueError('native game missing real quest dispatcher: '+marker)
    for marker in (
        'level22_mist_timeline_tests level22_quest_playback_tests',
        '|level22_mist_timeline_tests|level22_quest_playback_tests|',
        'python DemoV1/tools/verify_game_level22_transition.py',
        '|enhanced_level22_quest_transition_contract_tests|',
    ):
        if marker not in workflow:
            raise ValueError('normal Windows build cannot gate Level23: '+marker)

def main()->None:
    paths=(
        'DemoV1/src/level22_quest_playback.cpp',
        'DemoV1/src/exploration_runtime.cpp',
        'DemoV1/src/win32_main.cpp',
        'DemoV1/tests/level22_quest_playback_tests.cpp',
        'DemoV1/tests/enhanced_game_save_tests.cpp',
        'DemoV1/CMakeLists.txt',
        '.github/workflows/demo-v1-build.yml',
    )
    sources=[(ROOT/p).read_text(encoding='utf-8') for p in paths]
    check(*sources)
    mutations=(
        (0,'if(!present(frame))return false;','/* ignore cancelled playback */'),
        (0,'if(frame==AssassinRelicTransition::kLevelChangeFrame)',
           'if(frame==21U)'),
        (0,'for(auto relic:AssassinRelicTransition::kConsumedAfterTransition)',
           'for(auto relic:AssassinRelicTransition::kRelics)'),
        (1,'ExplorationRuntime staged=*this;','ExplorationRuntime& staged=*this;'),
        (1,'if(!completed)return false;','/* partial event commits */'),
        (1,'staged.inventory_.remove_object(','staged.inventory_.has_object('),
        (1,'auto pickup=staged.pickup_level22_loose_item_at(',
           'auto pickup=this->pickup_level22_loose_item_at('),
        (1,'return {std::nullopt,true,false};',
           'return {std::move(pickup),false,false};'),
        (2,'runtime.pickup_and_advance_level22_at(',
           'runtime.pickup_level22_loose_item_at('),
        (2,'host.set_exploration_input(nullptr);\n                                        host.set_gpu_world_visible(false);',
           '/* no mist input lock */;\n                                        host.set_gpu_world_visible(false);'),
        (2,'refresh_world_palette();\n                                world_music_state.invalidate();\n                                start_authored_world_music();',
           'refresh_world_palette();\n                                /* destination music omitted */;'),
        (3,'events.at(22)=="frame 21"','events.at(22)=="frame 20"'),
        (4,'warped.visited_levels.count(22)==1U',
           'warped.visited_levels.count(22)==0U'),
        (5,'src/level22_quest_playback.cpp','src/missing_quest_playback.cpp'),
        (6,'level22_mist_timeline_tests level22_quest_playback_tests',
           'level22_mist_timeline_tests'),
    )
    for index,old,new in mutations:
        if old not in sources[index]:
            raise ValueError('negative test fixture absent: '+old)
        mutated=list(sources)
        mutated[index]=mutated[index].replace(old,new,1)
        try:check(*mutated)
        except ValueError:continue
        raise ValueError('unsafe Level22 quest regression escaped: '+old)
    print('ENHANCED_LEVEL22_QUEST_TRANSITION_PASS (15 mutations rejected)')

if __name__=='__main__':
    main()
