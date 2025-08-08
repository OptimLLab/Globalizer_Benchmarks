#include "rastrigin_problem.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>

// ------------------------------------------------------------------------------------------------
TRastriginProblem::TRastriginProblem()
{
  mIsInitialized = false;
  mDimension = 1;
}

// ------------------------------------------------------------------------------------------------
int TRastriginProblem::SetConfigPath(const std::string& configPath)
{
  return IProblem::OK;
}

// ------------------------------------------------------------------------------------------------
int TRastriginProblem::SetDimension(int dimension)
{
  if(dimension > 0 && dimension <= mMaxDimension)
  {
    mDimension = dimension;
    return IProblem::OK;
  }
  else
    return IProblem::ERROR;
}

// ------------------------------------------------------------------------------------------------
int TRastriginProblem::GetDimension() const
{
  return mDimension;
}

// ------------------------------------------------------------------------------------------------
int TRastriginProblem::Initialize()
{
  if (mDimension > 0)
  {
    mIsInitialized = true;
    return IProblem::OK;
  }
  else
    return IProblem::ERROR;
}

// ------------------------------------------------------------------------------------------------
void TRastriginProblem::GetBounds(double* lower, double *upper)
{
  if (mIsInitialized)
    for (int i = 0; i < mDimension; i++)
    {
      lower[i] = -2.2;
      upper[i] = 1.8;
    }
}

// ------------------------------------------------------------------------------------------------
int TRastriginProblem::GetOptimumValue(double& value) const
{
  if (!mIsInitialized)
    return IProblem::UNDEFINED;

  value = 0.0;
  return IProblem::OK;
}

// ------------------------------------------------------------------------------------------------
int TRastriginProblem::GetOptimumPoint(double* point) const
{
  if (!mIsInitialized)
    return IProblem::UNDEFINED;

  for (int i = 0; i < mDimension; i++)
    point[i] = 0.0;
  return IProblem::OK;
}

// ------------------------------------------------------------------------------------------------
int TRastriginProblem::GetNumberOfFunctions() const
{
  return 1;
}

// ------------------------------------------------------------------------------------------------
int TRastriginProblem::GetNumberOfConstraints() const
{
  return 0;
}

// ------------------------------------------------------------------------------------------------
int TRastriginProblem::GetNumberOfCriterions() const
{
  return 1;
}

// ------------------------------------------------------------------------------------------------
double TRastriginProblem::CalculateFunctionals(const double* x, int fNumber)
{
  double sum = 0.;
  for (int j = 0; j < mDimension; j++)
    sum += x[j] * x[j] - 10. * cos(2.0 * M_PI * x[j]) + 10.0;
  return sum;
}

// ------------------------------------------------------------------------------------------------
TRastriginProblem::~TRastriginProblem()
{

}

// ------------------------------------------------------------------------------------------------
LIB_EXPORT_API IProblem* create()
{
  return new TRastriginProblem();
}

// ------------------------------------------------------------------------------------------------
LIB_EXPORT_API void destroy(IProblem* ptr)
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

  TRastriginProblem rastr;
  rastr.SetDimension(2);
  rastr.Initialize();
  double point[2] = { x, y };
  result = rastr.CalculateFunctionals(point, 0);

  //std::cout << "Calculation\n" << std::endl;
  return result;
}

// ------------------------------------------------------------------------------------------------
LIB_EXPORT_API double GetUpperBounds()
{
  double result = 0;

  TRastriginProblem rastr;
  rastr.SetDimension(2);
  rastr.Initialize();
  double Upper[2] = { 0.0, 0.0 };
  double Lower[2] = { 0.0, 0.0 };
  rastr.GetBounds(Lower, Upper);

  result = Upper[0];

  return result;
}

// ------------------------------------------------------------------------------------------------
LIB_EXPORT_API double GetLowerBounds()
{
  double result = 0;

  TRastriginProblem rastr;
  rastr.SetDimension(2);
  rastr.Initialize();
  double Upper[2] = { 0.0, 0.0 };
  double Lower[2] = { 0.0, 0.0 };
  rastr.GetBounds(Lower, Upper);

  result = Lower[0];
  return result;
}
// - end of file ----------------------------------------------------------------------------------
