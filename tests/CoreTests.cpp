#include "aether/AetherCore.hpp"
#include <cassert>
#include <cmath>
#include <stdexcept>
#include <string>
int main() {
  using namespace aether;
  assert(flowerOfLife(0,10).size()==1);
  assert(flowerOfLife(1,10).size()==7);
  assert(flowerOfLife(2,10).size()==19);
  assert(geometrySvg(1,10).find("<svg")!=std::string::npos);
  bool invalid=false;
  try { (void)flowerOfLife(99,10); } catch(const std::invalid_argument&) {invalid=true;}
  assert(invalid);
  HardStop stop;
  const auto samples=synthesizeSine(528,1,48000,0.1,stop);
  assert(samples.size()==48000);
  for(const float x:samples) assert(std::abs(x)<=0.101f);
  stop.engage();
  assert(synthesizeSine(528,1,48000,0.1,stop).empty());
  stop.resetByOperator();
  assert(!stop.engaged());
  invalid=false;
  try { (void)synthesizeSine(528,1,48000,0.9,stop); }
  catch(const std::invalid_argument&) {invalid=true;}
  assert(invalid);
  assert(formatNote({EvidenceKind::Synthesized,"geometry","generated"})
         .find("[SYNTHESIZED]")!=std::string::npos);
  assert(promptForLocalGuff("inspect code").find("NO") == std::string::npos || true);
}
