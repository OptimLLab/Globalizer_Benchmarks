#include "IGlobalOptimizationProblem.h"
#include "GKLS/GKLSProblem.hpp"

#include <algorithm>
#include <charconv>
#include <cmath>
#include <memory>
#include <stdexcept>

namespace
{
class Constraint : public TGKLSProblem
{
public:
  Constraint(int id, const std::vector<double>& firstMinimum, const std::vector<double>& lastMinimum)
    : TGKLSProblem(id, 2, Simple, TD)
  {
    // Масштаб рассчитывается до переноса минимума ограничения к первому критерию.
    const double farthest = 1 + std::max(std::abs(lastMinimum[0]), std::abs(lastMinimum[1]));
    const double nearest = 1 - std::max(std::abs(mOptimumPoint[0]), std::abs(mOptimumPoint[1]));
    zoom = farthest / nearest;
    mOptimumPoint = firstMinimum;
    option.GKLS_free();
    if (option.GKLS_arg_generate(id, mOptimumPoint.data(), 2, mLoBound, mUpBound) != GKLS_OK)
      throw std::runtime_error("Cannot generate shifted GKLS constraint");
  }

  double Value(const std::vector<double>& x) const
  {
    const double scaled[] = { x[0] / zoom, x[1] / zoom };
    return CalculateDFunction(scaled);
  }

private:
  double zoom;
};

int FunctionId(int instance, int offset)
{
  return (instance - 1 + offset) % 100 + 1;
}

struct Instance
{
  TGKLSProblem f1, f2;
  Constraint h1, h2;
  double threshold;

  explicit Instance(int id)
    : f1(id, 2, Simple, TD), f2(FunctionId(id, 1), 2, Simple, TD),
      h1(FunctionId(id, 2), f1.GetOptimumPoint(), f2.GetOptimumPoint()),
      h2(FunctionId(id, 3), f1.GetOptimumPoint(), f2.GetOptimumPoint()),
      threshold(CalculateThreshold())
  {}

  double MaxConstraint(const std::vector<double>& x) const
  {
    return std::max(h1.Value(x), h2.Value(x));
  }

  double CalculateThreshold() const
  {
    // Общая delta=0.5, сетка 201x201, 100 интервалов исходной гистограммы.
    std::vector<double> samples;
    samples.reserve(201 * 201);
    std::vector<double> x(2);
    double upper = -1.0;
    for (int i = 0; i <= 200; ++i)
      for (int j = 0; j <= 200; ++j)
      {
        x[0] = -1.0 + i * 0.01;
        x[1] = -1.0 + j * 0.01;
        samples.push_back(MaxConstraint(x));
        upper = std::max(upper, samples.back());
      }

    auto median = samples.begin() + samples.size() / 2;
    std::nth_element(samples.begin(), median, samples.end());
    const double step = (upper + 1.0) / 100;
    double q = upper;
    for (int bin = 1; bin <= 100; ++bin)
      if (-1.0 + bin * step >= *median)
      {
        q = -1.0 + bin * step;
        break;
      }

    // В исходной задаче последняя функция задаёт контрольную точку для RHS.
    const double critical = MaxConstraint(f2.GetOptimumPoint());
    return q < critical ? critical + 0.5 * (upper + 1.0) : q;
  }
};

class GKLSMultiCriteriaProblem : public IGlobalOptimizationProblem
{
public:
  int SetDimension(int dimension) override { return dimension == 2 ? PROBLEM_OK : PROBLEM_ERROR; }
  int GetDimension() const override { return 2; }
  int GetNumberOfFunctions() const override { return 4; }
  int GetNumberOfConstraints() const override { return 2; }
  int GetNumberOfCriterions() const override { return 2; }
  int GetOptimumValue(double&) const override { return PROBLEM_UNDEFINED; }
  int GetOptimumPoint(std::vector<double>&, std::vector<std::string>&) const override { return PROBLEM_UNDEFINED; }

  void GetBounds(std::vector<double>& lower, std::vector<double>& upper) override
  {
    lower.assign(2, -1.0);
    upper.assign(2, 1.0);
  }

  int SetParameter(std::string name, std::string value) override
  {
    if (name != "problemIndex")
      return PROBLEM_UNDEFINED;
    int id = 0;
    const auto end = value.data() + value.size();
    const auto parsed = std::from_chars(value.data(), end, id);
    if (parsed.ec != std::errc() || parsed.ptr != end || id < 1 || id > 100)
      return PROBLEM_ERROR;
    if (id != instanceId)
      instance.reset();
    instanceId = id;
    return PROBLEM_OK;
  }

  int Initialize() override
  {
    if (!instance)
      instance = std::make_unique<Instance>(instanceId);
    return PROBLEM_OK;
  }

  void GetParameters(std::vector<std::string>& names, std::vector<std::string>& values) override
  {
    names = { "problemIndex" };
    values = { std::to_string(instanceId) };
  }

  int SetParameter(std::string name, IOptVariantType value) override
  {
    if (auto id = std::get_if<int>(&value))
      return SetParameter(name, std::to_string(*id));
    if (auto text = std::get_if<std::string>(&value))
      return SetParameter(name, *text);
    return PROBLEM_ERROR;
  }

  double CalculateFunctionals(const std::vector<double>& x, std::vector<std::string>&, int index) override
  {
    if (!instance || x.size() != 2)
      throw std::invalid_argument("GKLS MCO requires an initialized instance and two coordinates");
    switch (index)
    {
    case 0: return instance->h1.Value(x) - instance->threshold;
    case 1: return instance->h2.Value(x) - instance->threshold;
    case 2: return instance->f1.ComputeFunction(x);
    case 3: return instance->f2.ComputeFunction(x);
    default: throw std::out_of_range("GKLS MCO function index");
    }
  }

private:
  int instanceId = 1;
  std::unique_ptr<Instance> instance;
};
}

extern "C" LIB_EXPORT_API IGlobalOptimizationProblem* create() { return new GKLSMultiCriteriaProblem(); }
extern "C" LIB_EXPORT_API void destroy(IGlobalOptimizationProblem* problem) { delete problem; }
