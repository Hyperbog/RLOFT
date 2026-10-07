#include "rloft/exploration_presenter.hpp"

#include <cassert>

int main() {
    rloft::ExplorationRuntimeFrame frame{};

    rloft::ViewSweepRun fallback{};
    fallback.pass = rloft::SweepRenderPass::Main;
    fallback.first_ray = 0;
    fallback.ray_count = 1;
    fallback.depth = 1024; // extent 512
    fallback.resource_id = 1;

    rloft::ViewSweepRun generated = fallback;
    generated.first_ray = 1;
    generated.depth = 128; // extent 64

    rloft::ViewSweepRun prepass = generated;
    prepass.first_ray = 2;
    prepass.pass = rloft::SweepRenderPass::Prepass;

    rloft::ViewSweepRun zero = generated;
    zero.first_ray = 3;
    zero.depth = 0;

    frame.sweep_runs = {
        fallback,
        generated,
        prepass,
        zero,
    };

    rloft::RendererTables tables{};
    tables.runtime_lookup_max = 508;

    const auto coverage =
        rloft::ExplorationPresenter::surface_coverage(
            frame,
            tables);

    assert(coverage.fallback_main_runs == 1);
    assert(coverage.sintab_main_runs_pending == 1);
    assert(coverage.sintab_prepass_runs_pending == 1);
    assert(!coverage.complete());

    frame.sweep_runs = {fallback};
    const auto complete =
        rloft::ExplorationPresenter::surface_coverage(
            frame,
            tables);
    assert(complete.fallback_main_runs == 1);
    assert(complete.sintab_main_runs_pending == 0);
    assert(complete.sintab_prepass_runs_pending == 0);
    assert(complete.complete());
}
