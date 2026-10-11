#include "aether/AetherCore.hpp"
#include <cmath>
#include <stdexcept>
#include <string>
using namespace aether;
static void require(bool ok, const char* message) {
  if(!ok) throw std::runtime_error(message);
}
int main() {
  require(flowerOfLife(0,10).size()==1,"center");
  require(flowerOfLife(1,10).size()==7,"ring one");
  require(flowerOfLife(2,10).size()==19,"ring two");
  require(geometrySvg(1,10).find("<svg")!=std::string::npos,"svg");
  bool invalid=false;
  try { (void)flowerOfLife(99,10); } catch(const std::invalid_argument&) {invalid=true;}
  require(invalid,"reject bad geometry");
  HardStop stop;
  auto samples=synthesizeSine(528,1,48000,0.1,stop);
  require(samples.size()==48000,"wave sample count");
  for(float x:samples) require(std::abs(x)<=0.101f,"bounded audio");
  stop.engage();
  require(synthesizeSine(528,1,48000,0.1,stop).empty(),"hardstop gates oscillator");
  stop.resetByOperator();
  require(!stop.engaged(),"explicit reset");
  invalid=false;
  try { (void)synthesizeSine(528,1,48000,0.9,stop); }
  catch(const std::invalid_argument&) {invalid=true;}
  require(invalid,"reject excessive gain");
  require(formatNote({EvidenceKind::Synthesized,"geometry","generated"})
    .find("[SYNTHESIZED]")!=std::string::npos,"evidence type");
  require(promptForLocalGuff("inspect code").find("USER REQUEST:")!=std::string::npos,
          "guff prompt boundary");
  return 0;
}
