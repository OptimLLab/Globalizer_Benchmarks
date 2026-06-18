#include "rastriginUC_problem.h"
#include "Common.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>

// ------------------------------------------------------------------------------------------------
RastriginUCProblem::RastriginUCProblem()
{
  mIsInitialized = false;
  mDimension = 50;
}

// ------------------------------------------------------------------------------------------------
int RastriginUCProblem::SetDimension(int dimension)
{
  if(dimension >= 50 && dimension <= mMaxDimension)
  {
    mDimension = dimension;
    return IGlobalOptimizationProblem::PROBLEM_OK;
  }
  else
    return IGlobalOptimizationProblem::PROBLEM_ERROR;
}

// ------------------------------------------------------------------------------------------------
int RastriginUCProblem::GetDimension() const
{
  return mDimension;
}

// ------------------------------------------------------------------------------------------------
int RastriginUCProblem::Initialize()
{
  if (mDimension > 0)
  {
    mIsInitialized = true;
    return IGlobalOptimizationProblem::PROBLEM_OK;
  }
  else
    return IGlobalOptimizationProblem::PROBLEM_ERROR;
}

// ------------------------------------------------------------------------------------------------
void RastriginUCProblem::GetBounds(std::vector<double>& lower, std::vector<double>& upper)
{
  if (mIsInitialized)
  {
    lower.resize(mDimension);
    upper.resize(mDimension);

    for (int i = 0; i < mDimension; i++)
    {
      lower[i] = -2.2;
      upper[i] = 1.8;
    }
  }
}

// ------------------------------------------------------------------------------------------------
int RastriginUCProblem::GetOptimumValue(double& value) const
{
  if (!mIsInitialized)
    return IGlobalOptimizationProblem::PROBLEM_UNDEFINED;

  value = 0.0;
  return IGlobalOptimizationProblem::PROBLEM_OK;
}

// ------------------------------------------------------------------------------------------------
int RastriginUCProblem::GetOptimumPoint(std::vector<double>& point, std::vector<std::string>& u) const
{
  if (!mIsInitialized)
    return IGlobalOptimizationProblem::PROBLEM_UNDEFINED;

  point.resize(mDimension);

  for (int i = 0; i < mDimension; i++)
    point[i] = 0.0;
  return IGlobalOptimizationProblem::PROBLEM_OK;
}

// ------------------------------------------------------------------------------------------------
int RastriginUCProblem::GetNumberOfFunctions() const
{
  return GetNumberOfConstraints() + GetNumberOfCriterions();
}

// ------------------------------------------------------------------------------------------------
int RastriginUCProblem::GetNumberOfConstraints() const
{
  return 1;
}

// ------------------------------------------------------------------------------------------------
int RastriginUCProblem::GetNumberOfCriterions() const
{
  return 1;
}

// ------------------------------------------------------------------------------------------------
double RastriginUCProblem::CalculateFunctionals(const std::vector<double>& x, std::vector<std::string>& u, int fNumber)
{
  double sum = 0.;
  bool is_uncalc = false;

  // невычислимые точки
  for (int i = 0; i < mDimension; i++) {
      if (std::fabs(x[i] - 0.23) < 0.15 && std::fabs(x[i] - 0.23) < 0.15) {
          is_uncalc = true;
          break;
      }
  }

  if (is_uncalc) {
      if (rand() % 2)
          sum = MaxDouble;
      else
          throw std::logic_error("Error of calculations");
  }
  else if (fNumber == 0)
  {
    for (int j = 0; j < mDimension; j++)
      sum += x[j] * x[j];

    sum += -1.5;
  }
  else if (fNumber == 1)
  {
    for (int j = 0; j < mDimension; j++)
      sum += x[j] * x[j] - 10. * cos(2.0 * M_PI * x[j]) + 10.0;
  }
  return sum;
}

// ------------------------------------------------------------------------------------------------
RastriginUCProblem::~RastriginUCProblem()
{

}

// ------------------------------------------------------------------------------------------------
inline int RastriginUCProblem::GetStartTrial(std::vector<double>& y, std::vector<std::string>& u, std::vector<double>& values)
{
  if (!mIsInitialized)
    return IGlobalOptimizationProblem::PROBLEM_UNDEFINED;

  y.resize(mDimension);

  for (int i = 0; i < mDimension; i++)
    y[i] = 0.1;

  values.resize(GetNumberOfFunctions());

  for (int j = 0; j < GetNumberOfFunctions(); j++) {
      try {
          values[j] = CalculateFunctionals(y, u, j);
      }
      catch (...) {
          values[j] = MaxDouble;
      }
  }

  return IGlobalOptimizationProblem::PROBLEM_OK;
}

// ------------------------------------------------------------------------------------------------
LIB_EXPORT_API IGlobalOptimizationProblem* create()
{
  return new RastriginUCProblem();
}

// ------------------------------------------------------------------------------------------------
LIB_EXPORT_API void destroy(IGlobalOptimizationProblem* ptr)
{
  delete ptr;
}

// - end of file ----------------------------------------------------------------------------------
