#ifndef CMP_PARAM_LIST_HPP
#define CMP_PARAM_LIST_HPP

#include <map>

// depth: 1 - 16
// logN: 10 - 16
constexpr std::size_t kMaxDepth = 10;
constexpr std::size_t kMaxLogRingDim = 16;
constexpr std::size_t kMaxRelWindow = 60; // 0 or 30

constexpr std::size_t GetKey(const std::size_t depth, const std::size_t logN, const std::size_t rel_window) {
  return depth + kMaxDepth * logN + kMaxDepth * kMaxLogRingDim * rel_window;
}

// constexpr seems invalid
const std::map<std::size_t, double> kParamsDcrt30
{
    // logN = 12, RelWindow = 30
    std::make_pair(GetKey(1, 12, 30), 1.0030),
    std::make_pair(GetKey(2, 12, 30), 1.0040),
    std::make_pair(GetKey(3, 12, 30), 1.0060),
    std::make_pair(GetKey(4, 12, 30), 1.0070),
    std::make_pair(GetKey(5, 12, 30), 1.0080),
    // logN = 13, RelWindow = 30
    std::make_pair(GetKey(1, 13, 30), 1.0020),
    std::make_pair(GetKey(2, 13, 30), 1.0020),
    std::make_pair(GetKey(3, 13, 30), 1.0030),
    std::make_pair(GetKey(4, 13, 30), 1.0040),
    std::make_pair(GetKey(5, 13, 30), 1.0040),
    // logN = 14, RelWindow = 30
    std::make_pair(GetKey(1, 14, 30), 1.0010),
    std::make_pair(GetKey(2, 14, 30), 1.0020),
    std::make_pair(GetKey(3, 14, 30), 1.0020),
    std::make_pair(GetKey(4, 14, 30), 1.0020),
    std::make_pair(GetKey(5, 14, 30), 1.0030),
    std::make_pair(GetKey(6, 14, 30), 1.0030),
    std::make_pair(GetKey(7, 14, 30), 1.0030),
    std::make_pair(GetKey(8, 14, 30), 1.0040),
    std::make_pair(GetKey(9, 14, 30), 1.0040),
    std::make_pair(GetKey(10, 14, 30), 1.0040),
    // logN = 12, RelWindow = 0
    std::make_pair(GetKey(1, 12, 0), 1.003),
    std::make_pair(GetKey(2, 12, 0), 1.004),
    std::make_pair(GetKey(3, 12, 0), 1.006),
    std::make_pair(GetKey(4, 12, 0), 1.007),
    std::make_pair(GetKey(5, 12, 0), 1.008),
    // logN = 13, RelWindow = 0
    std::make_pair(GetKey(0, 13, 0), 1.001),
    std::make_pair(GetKey(1, 13, 0), 1.002),
    std::make_pair(GetKey(2, 13, 0), 1.002),
    std::make_pair(GetKey(3, 13, 0), 1.003),
    std::make_pair(GetKey(4, 13, 0), 1.004),
    std::make_pair(GetKey(5, 13, 0), 1.004),
    std::make_pair(GetKey(6, 13, 0), 1.005),
    // logN = 14, RelWindow = 0
    std::make_pair(GetKey(0, 14, 0), 1.001),
    std::make_pair(GetKey(1, 14, 0), 1.001),
    std::make_pair(GetKey(2, 14, 0), 1.002),
    std::make_pair(GetKey(3, 14, 0), 1.002),
    std::make_pair(GetKey(4, 14, 0), 1.002),
    std::make_pair(GetKey(5, 14, 0), 1.003),
    std::make_pair(GetKey(6, 14, 0), 1.003),
    std::make_pair(GetKey(7, 14, 0), 1.003),
    std::make_pair(GetKey(8, 14, 0), 1.004),
    std::make_pair(GetKey(9, 14, 0), 1.004),
    std::make_pair(GetKey(10, 14, 0), 1.004),
};

// constexpr seems invalid
const std::map<std::size_t, double> kParamsDcrt60
{
    // logN = 12, RelWindow = 0
    std::make_pair(GetKey(0, 12, 0), 1.006),
    std::make_pair(GetKey(2, 12, 0), 1.006),
    std::make_pair(GetKey(3, 12, 0), 1.008),
    std::make_pair(GetKey(4, 12, 0), 1.008),
    std::make_pair(GetKey(5, 12, 0), 1.011),
    // logN = 13, RelWindow = 0
    std::make_pair(GetKey(0, 13, 0), 1.003),
    std::make_pair(GetKey(1, 13, 0), 1.003),
    std::make_pair(GetKey(2, 13, 0), 1.003),
    std::make_pair(GetKey(3, 13, 0), 1.004),
    std::make_pair(GetKey(4, 13, 0), 1.004),
    std::make_pair(GetKey(5, 13, 0), 1.006),
    std::make_pair(GetKey(6, 13, 0), 1.006),
    std::make_pair(GetKey(7, 13, 0), 1.009),
    // logN = 14, RelWindow = 0
    std::make_pair(GetKey(0, 14, 0), 1.001),
    std::make_pair(GetKey(1, 14, 0), 1.002),
    std::make_pair(GetKey(2, 14, 0), 1.002),
    std::make_pair(GetKey(3, 14, 0), 1.002),
    std::make_pair(GetKey(4, 14, 0), 1.003),
    std::make_pair(GetKey(5, 14, 0), 1.003),
    std::make_pair(GetKey(6, 14, 0), 1.003),
    std::make_pair(GetKey(7, 14, 0), 1.006),
    std::make_pair(GetKey(8, 14, 0), 1.006),
};

// Check parameter range (and assert if a param is out of range)
static void GetRootHermit(
    const std::size_t mul_depth,
    const std::size_t logN,
    const std::size_t rel_window,
    const std::size_t dcrt_bit,
    double& root_hermit)
{
    if (dcrt_bit == 30) {
        root_hermit = kParamsDcrt30.at(GetKey(mul_depth, logN, rel_window));
    } else {
        root_hermit = kParamsDcrt60.at(GetKey(mul_depth, logN, rel_window));
    }
}

#endif // CMP_PARAM_LIST_HPP
