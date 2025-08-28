#include "rastrigin_problem.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>

// ------------------------------------------------------------------------------------------------
RastriginProblem::RastriginProblem()
{
  mIsInitialized = false;
  mDimension = 1;
}

// ------------------------------------------------------------------------------------------------
int RastriginProblem::SetDimension(int dimension)
{
  if(dimension > 0 && dimension <= mMaxDimension)
  {
    mDimension = dimension;
    return IGlobalOptimizationProblem::OK;
  }
  else
    return IGlobalOptimizationProblem::ERROR;
}

// ------------------------------------------------------------------------------------------------
int RastriginProblem::GetDimension() const
{
  return mDimension;
}

// ------------------------------------------------------------------------------------------------
int RastriginProblem::Initialize()
{
  if (mDimension > 0)
  {
    mIsInitialized = true;
    return IGlobalOptimizationProblem::OK;
  }
  else
    return IGlobalOptimizationProblem::ERROR;
}

// ------------------------------------------------------------------------------------------------
void RastriginProblem::GetBounds(std::vector<double>& lower, std::vector<double>& upper)
{
  if (mIsInitialized)
    for (int i = 0; i < mDimension; i++)
    {
      lower[i] = -2.2;
      upper[i] = 1.8;
    }
}

// ------------------------------------------------------------------------------------------------
int RastriginProblem::GetOptimumValue(double& value) const
{
  if (!mIsInitialized)
    return IGlobalOptimizationProblem::UNDEFINED;

  value = 0.0;
  return IGlobalOptimizationProblem::OK;
}

// ------------------------------------------------------------------------------------------------
int RastriginProblem::GetOptimumPoint(std::vector<double>& point) const
{
  if (!mIsInitialized)
    return IGlobalOptimizationProblem::UNDEFINED;

  for (int i = 0; i < mDimension; i++)
    point[i] = 0.0;
  return IGlobalOptimizationProblem::OK;
}

// ------------------------------------------------------------------------------------------------
int RastriginProblem::GetNumberOfFunctions() const
{
  return 1;
}

// ------------------------------------------------------------------------------------------------
int RastriginProblem::GetNumberOfConstraints() const
{
  return 0;
}

// ------------------------------------------------------------------------------------------------
int RastriginProblem::GetNumberOfCriterions() const
{
  return 1;
}

// ------------------------------------------------------------------------------------------------
double RastriginProblem::CalculateFunctionals(const std::vector<double>& x, int fNumber)
{
  double sum = 0.;
  for (int j = 0; j < mDimension; j++)
    sum += x[j] * x[j] - 10. * cos(2.0 * M_PI * x[j]) + 10.0;
  return sum;
}

// ------------------------------------------------------------------------------------------------
RastriginProblem::~RastriginProblem()
{

}

// ------------------------------------------------------------------------------------------------
LIB_EXPORT_API IGlobalOptimizationProblem* create()
{
  return new RastriginProblem();
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

  RastriginProblem rastr;
  rastr.SetDimension(2);
  rastr.Initialize();
  std::vector<double> point = { x, y };
  result = rastr.CalculateFunctionals(point, 0);

  //std::cout << "Calculation\n" << std::endl;
  return result;
}

// ------------------------------------------------------------------------------------------------
LIB_EXPORT_API double GetUpperBounds()
{
  double result = 0;

  RastriginProblem rastr;
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

  RastriginProblem rastr;
  rastr.SetDimension(2);
  rastr.Initialize();
  std::vector<double> Upper = { 0.0, 0.0 };
  std::vector<double> Lower = { 0.0, 0.0 };
  rastr.GetBounds(Lower, Upper);

  result = Lower[0];
  return result;
}
// - end of file ----------------------------------------------------------------------------------
