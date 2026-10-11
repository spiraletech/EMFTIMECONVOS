#pragma once
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace aether {
inline constexpr const char* version = "0.1.0-alpha";
enum class EvidenceKind { Measured, Derived, Interpreted, Synthesized };
const char* evidenceLabel(EvidenceKind kind) noexcept;

class HardStop {
public:
  void engage() noexcept { stopped_.store(true, std::memory_order_release); }
  void resetByOperator() noexcept { stopped_.store(false, std::memory_order_release); }
  bool engaged() const noexcept { return stopped_.load(std::memory_order_acquire); }
private:
  std::atomic<bool> stopped_{false};
};

struct Point { double x{}, y{}; };
struct Circle { Point center; double radius{}; };
std::vector<Circle> flowerOfLife(unsigned rings, double radius);
std::string geometrySvg(unsigned rings, double radius);
std::vector<float> synthesizeSine(double hz, double seconds, unsigned sampleRate,
                                  double gain, const HardStop& stop);
bool writeWavMono16(const std::string& path, const std::vector<float>& samples,
                    unsigned sampleRate);
struct ResearchNote {
  EvidenceKind kind;
  std::string subject;
  std::string summary;
};
std::string formatNote(const ResearchNote& note);
std::string promptForLocalGuff(const std::string& request);
}
