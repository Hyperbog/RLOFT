#include "games/ravenloft/data/AnmMovie.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
using namespace ravenloft::data;
static void ppm(const std::filesystem::path&p,const AnmDecoder&d){std::ofstream o(p,std::ios::binary);o<<"P6\n"<<AnmDecoder::Width<<" "<<AnmDecoder::Height<<"\n255\n";for(auto idx:d.pixels()){auto c=d.palette()[idx];o.put((char)c.r);o.put((char)c.g);o.put((char)c.b);}}
int main(int argc,char**argv){if(argc<3){std::cerr<<"usage: ravenloft_anm_dump CINE00.ANM output_dir [frame...]\n";return 2;}AnmMovie m(argv[1]);std::filesystem::create_directories(argv[2]);std::vector<int> wanted;if(argc>3)for(int i=3;i<argc;++i)wanted.push_back(std::stoi(argv[i]));else wanted={0,25,75,125,200,300,(int)m.frameCount()-1};AnmDecoder d;for(int i=0;i<(int)m.frameCount();++i){d.apply(m.frames()[i]);for(int f:wanted)if(i==f){auto p=std::filesystem::path(argv[2])/("frame_"+std::to_string(i)+".ppm");ppm(p,d);std::cout<<p.string()<<"\n";}}}
