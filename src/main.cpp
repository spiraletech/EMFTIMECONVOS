#include "aether/AetherCore.hpp"
#include <exception>
#include <fstream>
#include <iostream>
#include <string>
int main(int argc, char** argv) {
  aether::HardStop stop;
  try {
    if(argc<2 || std::string(argv[1])=="--help") {
      std::cout << "AETHER OS " << aether::version << " | An App for the Future\n"
        << "Commands:\n  geometry <output.svg> [rings=2]\n"
        << "  tone <output.wav> [frequency=528]\n"
        << "  prompt <request>\n  status\n";
      return 0;
    }
    const std::string command=argv[1];
    if(command=="status") {
      std::cout << "AETHER alpha: command-line instrument; no GUI, no microphone, "
                   "no connected GUFF model, no external contact receiver.\n";
      return 0;
    }
    if(command=="geometry" && argc>=3) {
      const unsigned rings=argc>=4?static_cast<unsigned>(std::stoul(argv[3])):2;
      std::ofstream f(argv[2],std::ios::binary);
      if(!f) return 2;
      f << aether::geometrySvg(rings,40.0);
      if(!f) return 2;
      std::cout << "SYNTHESIZED SVG: " << argv[2] << "\n";
      return 0;
    }
    if(command=="tone" && argc>=3) {
      const double hz=argc>=4?std::stod(argv[3]):528.0;
      const auto samples=aether::synthesizeSine(hz,2.0,48000,0.1,stop);
      if(!aether::writeWavMono16(argv[2],samples,48000)) return 2;
      std::cout << "SYNTHESIZED WAV: " << argv[2] << " (not transmitted)\n";
      return 0;
    }
    if(command=="prompt" && argc>=3) {
      std::cout << aether::promptForLocalGuff(argv[2])
                << "\n[NO LOCAL MODEL CONNECTED; THIS IS A PROMPT PREVIEW]\n";
      return 0;
    }
    std::cerr << "Invalid command; see --help\n";
    return 2;
  } catch(const std::exception& e) {
    std::cerr << "AETHER ERROR: " << e.what() << "\n";
    return 2;
  }
}
