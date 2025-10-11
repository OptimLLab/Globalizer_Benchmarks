#include "stronginc3_problem.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>

// ------------------------------------------------------------------------------------------------
StronginC3::StronginC3()
{
  mIsInitialized = false;
  mDimension = 1;
}

// ------------------------------------------------------------------------------------------------
int StronginC3::SetDimension(int dimension)
{
  if(dimension == 2)
  {
    mDimension = dimension;
    return IGlobalOptimizationProblem::PROBLEM_OK;
  }
  else
    return IGlobalOptimizationProblem::PROBLEM_ERROR;
}

// ------------------------------------------------------------------------------------------------
int StronginC3::GetDimension() const
{
  return mDimension;
}

// ------------------------------------------------------------------------------------------------
int StronginC3::Initialize()
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
void StronginC3::GetBounds(std::vector<double>& lower, std::vector<double>& upper)
{
  lower.resize(mDimension);
  upper.resize(mDimension);
  if (mIsInitialized)
  {
    lower[0] = 0.;
    upper[0] = 4.;
    lower[1] = -1.;
    upper[1] = 3.;
  }
}

// ------------------------------------------------------------------------------------------------
int StronginC3::GetOptimumValue(double& value) const
{
  if (!mIsInitialized)
    return IGlobalOptimizationProblem::PROBLEM_UNDEFINED;

  value = -1.489444;
  return IGlobalOptimizationProblem::PROBLEM_OK;
}

// ------------------------------------------------------------------------------------------------
int StronginC3::GetOptimumPoint(std::vector<double>& point, std::vector<std::string>& u) const
{
  if (!mIsInitialized)
    return IGlobalOptimizationProblem::PROBLEM_UNDEFINED;
  point.resize(mDimension);
  point[0] = 0.941176;
  point[1] = 0.941176;
  return IGlobalOptimizationProblem::PROBLEM_OK;
}

// ------------------------------------------------------------------------------------------------
int StronginC3::GetNumberOfFunctions() const
{
  return 4;
}

// ------------------------------------------------------------------------------------------------
int StronginC3::GetNumberOfConstraints() const
{
  return 3;
}

// ------------------------------------------------------------------------------------------------
int StronginC3::GetNumberOfCriterions() const
{
  return 1;
}

// ------------------------------------------------------------------------------------------------
double StronginC3::CalculateFunctionals(const std::vector<double>& x, std::vector<std::string>& u, int fNumber)
{
  double res = 0.0;
  double x1 = x[0], x2 = x[1];
  switch (fNumber)
  {
  case 0: // constraint 1
    res = 0.01 * ((x1 - 2.2) * (x1 - 2.2) + (x2 - 1.2) * (x2 - 1.2) - 2.25);
    break;
  case 1: // constraint 2
    res = 100.0 * (1.0 - ((x1 - 2.0) / 1.2) * ((x1 - 2.0) / 1.2) -
      (x2 / 2.0) * (x2 / 2.0));
    break;
  case 2: // constraint 3
    res = 10.0 * (x2 - 1.5 - 1.5 * sin(6.283 * (x1 - 1.75)));
    break;
  case 3: // criterion
  {
    double t1 = pow(0.5 * x1 - 0.5, 4.0);
    double t2 = pow(x2 - 1.0, 4.0);
    res = 1.5 * x1 * x1 * exp(1.0 - x1 * x1 - 20.25 * (x1 - x2) * (x1 - x2));
    res = res + t1 * t2 * exp(2.0 - t1 - t2);
    res = -res;
  }
  break;
  }

  return res;
}

// ------------------------------------------------------------------------------------------------
StronginC3::~StronginC3()
{

}

// ------------------------------------------------------------------------------------------------
LIB_EXPORT_API IGlobalOptimizationProblem* create()
{
  return new StronginC3();
}

// ------------------------------------------------------------------------------------------------
LIB_EXPORT_API void destroy(IGlobalOptimizationProblem* ptr)
{
  delete ptr;
}

// ------------------------------------------------------------------------------------------------
LIB_EXPORT_API double Calculation1D(double x, int fType, int fNum)
{
  std::cout << "Calculation\n" << std::endl;
  return x + fType + fNum;
}

// ------------------------------------------------------------------------------------------------
LIB_EXPORT_API double Calculation(double x, double y)
{
  double result = 0;

  StronginC3 rastr;
  rastr.SetDimension(2);
  rastr.Initialize();
  std::vector<double> point = { x, y };
  std::vector<std::string> u;
  result = rastr.CalculateFunctionals(point, u, 0);

  //std::cout << "Calculation\n" << std::endl;
  return result;
}

// ------------------------------------------------------------------------------------------------
LIB_EXPORT_API double GetUpperBounds()
{
  double result = 0;

  StronginC3 rastr;
  rastr.SetDimension(2);
  rastr.Initialize();
  std::vector<double> Upper = { 0.0, 0.0 };
  std::vector<double> Lower = { 0.0, 0.0 };
  rastr.GetBounds(Lower, Upper);

  result = Upper[0];

  return result;
}

// ------------------------------------------------------------------------------------------------
LIB_EXPORT_API double GetLowerBounds()
{
  double result = 0;

  StronginC3 rastr;
  rastr.SetDimension(2);
  rastr.Initialize();
  std::vector<double> Upper = { 0.0, 0.0 };
  std::vector<double> Lower = { 0.0, 0.0 };
  rastr.GetBounds(Lower, Upper);

  result = Lower[0];
  return result;
}
// - end of file ----------------------------------------------------------------------------------
