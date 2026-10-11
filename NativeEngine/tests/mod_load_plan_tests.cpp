#include "rloft_native/mod_load_plan.hpp"
#include <cassert>
#include <stdexcept>
#include <string>
#include <vector>

using namespace rloft_native;
namespace {
template <typename F> void rejects(F f) {
    bool caught = false;
    try { f(); } catch (const std::exception&) { caught = true; }
    assert(caught);
}
void exercise() {
    assert(plan_mod_mounts({}).empty());
    const std::vector<ModManifest> manifests = {
        {"Quest", "1.0", 1, 1, {"Textures", "Core"}},
        {"Unrelated", "2.1"},
        {"Textures", "1.0", 1, 1, {"Core"}},
        {"Core", "1.0"}
    };
    const auto plan = plan_mod_mounts(manifests);
    assert(plan.size() == 4);
    // Stable current-input priority among ready mods, dependencies first.
    assert(plan[0].id == "unrelated");
    assert(plan[1].id == "core");
    assert(plan[2].id == "textures");
    assert(plan[3].id == "quest");
    assert(plan[3].original_priority == 0);
    assert(canonical_mod_id("MiXEd-1") == "mixed-1");
    rejects([] { canonical_mod_id("../outside"); });
    rejects([] { canonical_mod_id("original"); });
    rejects([] { canonical_mod_id("bad/name"); });
    rejects([] { canonical_mod_id("é"); });
    rejects([] { plan_mod_mounts({{"Same", "1"}, {"same", "1"}}); });
    rejects([] { plan_mod_mounts({{"missing", "1", 1, 1, {"unavailable"}}}); });
    rejects([] { plan_mod_mounts({{"self", "1", 1, 1, {"SELF"}}}); });
    rejects([] { plan_mod_mounts({{"A", "1", 1, 1, {"b"}},
                                  {"b", "1", 1, 1, {"a"}}}); });
    rejects([] { plan_mod_mounts({{"a", "1", 1, 1, {"B", "b"}}, {"b", "1"}}); });
    rejects([] { plan_mod_mounts({{"a", "", 1, 1, {}}}); });
    rejects([] { plan_mod_mounts({{"a", "1", 2, 3, {}}}); });
    rejects([] { plan_mod_mounts({{"a", "1", 3, 1, {}}}); });
    rejects([] { plan_mod_mounts({{"a", "../escape", 1, 1, {}}}); });
    rejects([] { plan_mod_mounts({{"a", "1", 1, 1, {}}}, 2); });
    // A low-priority base may be listed after its dependent; planning
    // reorders it ahead of the dependent so final mounts satisfy overrides.
    assert(plan_mod_mounts({{"dependent", "1", 1, 1, {"base"}},
                            {"base", "1"}})[0].id == "base");
    auto many = std::vector<ModManifest>(257);
    for (std::size_t i = 0; i < many.size(); ++i)
        many[i].id = "mod" + std::to_string(i);
    rejects([&] { plan_mod_mounts(many); });
}
}
int main() { exercise(); }
