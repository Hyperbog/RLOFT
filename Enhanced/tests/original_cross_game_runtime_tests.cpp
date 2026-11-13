#include "rloft/original_launcher.hpp"
#include "rloft/original_bound_image.hpp"
#include <cassert>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>

using namespace rloft;
using A=LauncherAction;
using M=LauncherModule;

static void expect(const LauncherStep& actual,M next,std::vector<A> actions) {
    assert(actual.next==next && actual.actions==actions);
}
static void verify_supervisors() {
    for (auto game : {GameId::StrahdsPossession,GameId::Menzoberranzan,
                      GameId::StoneProphet}) {
        OriginalLauncher host(game);
        const auto setup=host.begin();
        assert(setup.next==M::Code2);
        assert(setup.actions.front()==A::CreateWork);
        assert(setup.actions.back()==A::StartCode2);
        assert((setup.actions.size()==3)==(game==GameId::StrahdsPossession));
        assert((setup.actions.size()==4)==(game==GameId::Menzoberranzan));
        assert((setup.actions.size()==5)==(game==GameId::StoneProphet));
        bool rejected=false;
        try {(void)host.begin();} catch(const std::logic_error&) {rejected=true;}
        assert(rejected);
        rejected=false;
        try {(void)host.complete(M::Code1,10);}
        catch(const std::logic_error&) {rejected=true;}
        assert(rejected);
        const bool stone=game==GameId::StoneProphet;
        expect(host.complete(M::Code2,10),M::Code1,
               stone?std::vector<A>{A::DeleteChars,A::StartCode1}
                    :std::vector<A>{A::StartCode1});
        expect(host.complete(M::Code1,10),M::Code2,{A::StartCode2});
        expect(host.complete(M::Code2,12),M::Code2,
               stone?std::vector<A>{A::DeleteChars,A::DeleteChars,A::StartCode2}
                    :std::vector<A>{A::DeleteChars,A::StartCode2});
        if(game==GameId::StrahdsPossession) {
            expect(host.complete(M::Code2,13),M::Stopped,
                   {A::DeleteChars,A::Stop});
        } else {
            expect(host.complete(M::Code2,13),M::Code2,
                   stone?std::vector<A>{A::DeleteChars,A::StartCode2}
                        :std::vector<A>{A::StartCode2});
            expect(host.complete(M::Code2,0),M::Stopped,
                   stone?std::vector<A>{A::DeleteChars,A::DeleteChars,
                       A::RemoveWork,A::RemoveAutomaps,A::RemoveGsr,A::Stop}
                        :std::vector<A>{A::DeleteChars,A::RemoveWork,A::Stop});
        }
        rejected=false;
        try {(void)host.complete(M::Code2,0);}
        catch(const std::logic_error&) {rejected=true;}
        assert(rejected);
    }
    for(const auto game:{GameId::StrahdsPossession,
                         GameId::Menzoberranzan,GameId::StoneProphet}) {
        for(int status:{0,12,13}) {
            OriginalLauncher host(game);
            (void)host.begin();
            (void)host.complete(M::Code2,10);
            const auto step=host.complete(M::Code1,status);
            assert(step.next==M::Stopped && step.actions.back()==A::Stop);
        }
    }
}
static std::vector<std::uint8_t> read_file(const std::string& path) {
    std::ifstream f(path,std::ios::binary);
    if(!f)throw std::runtime_error("cannot open original media: "+path);
    return {std::istreambuf_iterator<char>(f),std::istreambuf_iterator<char>()};
}
static void verify_original(const std::string& path,
                            const OriginalExecutableSpec& spec) {
    const auto bytes=read_file(path);
    const OriginalBoundImage image(bytes);
    assert(matches_original_executable_structure(spec,bytes.size(),image.layout()));
    assert(image.module_origin()==0x25d24);
    assert(image.layout().objects.size()==2);
    const auto code=image.object_bytes(0);
    const auto data=image.object_bytes(1);
    assert(code.size()==spec.code_object_size);
    assert(data.size()==image.layout().objects[1].virtual_size);
    assert(code.size()>3 && code[0]==0xcc && code[1]==0xeb && code[2]==0xfd);
    assert(data.size()>4 && data[0]==1 && data[1]==1 && data[2]==1);
    if(spec.game==GameId::StrahdsPossession) {
        if(spec.path=="CODE.1") {
            assert(image.object_physical_start(0)==0x2c724);
            assert(image.object_physical_start(1)==0x3e724);
            assert(data.size()>0x99 &&
                   std::string(data.begin()+0x94,data.begin()+0x99)=="CHARS");
        } else {
            assert(image.object_physical_start(0)==0x3af24);
            assert(image.object_physical_start(1)==0x75f24);
            assert(data.size()>0x715 &&
                   std::string(data.begin()+0x710,data.begin()+0x715)=="CHARS");
        }
    }
    std::cout << path << ": LE map PASS; code="<<code.size()
              <<"; data="<<data.size()<<'\n';
}
int main(int argc,char** argv) {
    verify_supervisors();
    if(argc==4) {
        const auto game=game_id_from_key(argv[1]);
        if(!game)throw std::invalid_argument("unknown game key");
        for(int i=0;i<2;++i) {
            bool found=false;
            for(const auto& spec:kOriginalExecutables)
                if(spec.game==*game &&
                   spec.path.find(i==0?"CODE.1":"CODE.2")!=std::string_view::npos) {
                    verify_original(argv[i+2],spec);
                    found=true;
                }
            assert(found);
        }
    } else if(argc!=1) {
        throw std::invalid_argument(
            "usage: original_cross_game_runtime_tests [strahd|menzo|stone-prophet CODE.1 CODE.2]");
    }
    std::cout<<"original_cross_game_runtime_tests: PASS\n";
}
