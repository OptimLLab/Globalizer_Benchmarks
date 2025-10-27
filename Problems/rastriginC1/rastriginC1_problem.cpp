#include "rastriginC1_problem.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>

// ------------------------------------------------------------------------------------------------
RastriginC1Problem::RastriginC1Problem()
{
  mIsInitialized = false;
  mDimension = 50;
}

// ------------------------------------------------------------------------------------------------
int RastriginC1Problem::SetDimension(int dimension)
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
int RastriginC1Problem::GetDimension() const
{
  return mDimension;
}

// ------------------------------------------------------------------------------------------------
int RastriginC1Problem::Initialize()
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
void RastriginC1Problem::GetBounds(std::vector<double>& lower, std::vector<double>& upper)
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
int RastriginC1Problem::GetOptimumValue(double& value) const
{
  if (!mIsInitialized)
    return IGlobalOptimizationProblem::PROBLEM_UNDEFINED;

  value = 0.0;
  return IGlobalOptimizationProblem::PROBLEM_OK;
}

// ------------------------------------------------------------------------------------------------
int RastriginC1Problem::GetOptimumPoint(std::vector<double>& point, std::vector<std::string>& u) const
{
  if (!mIsInitialized)
    return IGlobalOptimizationProblem::PROBLEM_UNDEFINED;

  point.resize(mDimension);

  for (int i = 0; i < mDimension; i++)
    point[i] = 0.0;
  return IGlobalOptimizationProblem::PROBLEM_OK;
}

// ------------------------------------------------------------------------------------------------
int RastriginC1Problem::GetNumberOfFunctions() const
{
  return GetNumberOfConstraints() + GetNumberOfCriterions();
}

// ------------------------------------------------------------------------------------------------
int RastriginC1Problem::GetNumberOfConstraints() const
{
  return 1;
}

// ------------------------------------------------------------------------------------------------
int RastriginC1Problem::GetNumberOfCriterions() const
{
  return 1;
}

// ------------------------------------------------------------------------------------------------
double RastriginC1Problem::CalculateFunctionals(const std::vector<double>& x, std::vector<std::string>& u, int fNumber)
{
  double sum = 0.;
  if (fNumber == 0)
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
RastriginC1Problem::~RastriginC1Problem()
{

}

// ------------------------------------------------------------------------------------------------
inline int RastriginC1Problem::GetStartTrial(std::vector<double>& y, std::vector<std::string>& u, std::vector<double>& values)
{
  if (!mIsInitialized)
    return IGlobalOptimizationProblem::PROBLEM_UNDEFINED;

  y.resize(mDimension);

  for (int i = 0; i < mDimension; i++)
    y[i] = 0.1;

  values.resize(GetNumberOfFunctions());

  for (int j = 0; j < GetNumberOfFunctions(); j++)
    values[j] = CalculateFunctionals(y, u, j);

  return IGlobalOptimizationProblem::PROBLEM_OK;
}

// ------------------------------------------------------------------------------------------------
LIB_EXPORT_API IGlobalOptimizationProblem* create()
{
  return new RastriginC1Problem();
}

// ------------------------------------------------------------------------------------------------
LIB_EXPORT_API void destroy(IGlobalOptimizationProblem* ptr)
{
  delete ptr;
}

// - end of file ----------------------------------------------------------------------------------
