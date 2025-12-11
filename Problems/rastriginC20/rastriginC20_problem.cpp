#include "rastriginC20_problem.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>

// ------------------------------------------------------------------------------------------------
rastriginC20Problem::rastriginC20Problem()
{
  mIsInitialized = false;
  mDimension = 1;
}

// ------------------------------------------------------------------------------------------------
int rastriginC20Problem::SetDimension(int dimension)
{
  if(dimension > 0 && dimension <= mMaxDimension)
  {
    mDimension = dimension;
    return IGlobalOptimizationProblem::PROBLEM_OK;
  }
  else
    return IGlobalOptimizationProblem::PROBLEM_ERROR;
}

// ------------------------------------------------------------------------------------------------
int rastriginC20Problem::GetDimension() const
{
  return mDimension;
}

// ------------------------------------------------------------------------------------------------
int rastriginC20Problem::Initialize()
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
void rastriginC20Problem::GetBounds(std::vector<double>& lower, std::vector<double>& upper)
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
int rastriginC20Problem::GetOptimumValue(double& value) const
{
  if (!mIsInitialized)
    return IGlobalOptimizationProblem::PROBLEM_UNDEFINED;

  value = 0.0;
  return IGlobalOptimizationProblem::PROBLEM_OK;
}

// ------------------------------------------------------------------------------------------------
int rastriginC20Problem::GetOptimumPoint(std::vector<double>& point, std::vector<std::string>& u) const
{
  if (!mIsInitialized)
    return IGlobalOptimizationProblem::PROBLEM_UNDEFINED;

  point.resize(mDimension);

  for (int i = 0; i < mDimension; i++)
    point[i] = 0.0;

  return IGlobalOptimizationProblem::PROBLEM_OK;
}

// ------------------------------------------------------------------------------------------------
int rastriginC20Problem::GetNumberOfFunctions() const
{
  return GetNumberOfConstraints() + GetNumberOfCriterions();
}

// ------------------------------------------------------------------------------------------------
int rastriginC20Problem::GetNumberOfConstraints() const
{
  return 20;
}

// ------------------------------------------------------------------------------------------------
int rastriginC20Problem::GetNumberOfCriterions() const
{
  return 1;
}

// ------------------------------------------------------------------------------------------------
double rastriginC20Problem::CalculateFunctionals(const std::vector<double>& x, std::vector<std::string>& u, int fNumber)
{
  
  if (fNumber == 21)
  {
    double sum = 0.;
    for (int j = 0; j < mDimension; j++)
      sum += x[j] * x[j] - 10. * cos(2.0 * M_PI * x[j]) + 10.0;
    return sum;
  }
  else if (fNumber == 0)
  {
    double sum = 0.;
    for (int j = 0; j < mDimension; j++)
      sum += x[j] * x[j];

    sum += -pow(1.25, mDimension);
    return sum;
  }
  else if (fNumber == 1)
  {
    double sum = 0.;
    for (int j = 0; j < mDimension; j++) {
      sum += x[j] * x[j];
    }
    return sum - pow(1.2, mDimension);
  }
  else if (fNumber == 2)
  {
    double sum = 0.;
    for (int j = 0; j < mDimension; j++) {
      sum += x[j];
    }
    return sum - 1.5;
  }
  else if (fNumber == 3)
  {
    double sum = 0.;
    for (int j = 0; j < mDimension; j++) {
      sum += -x[j];
    }
    return sum - 1.5;
  }
  else if (fNumber == 4)
  {
    double sum = 0.;
    for (int j = 0; j < mDimension; j++) {
      sum += fabs(x[j]);
    }
    return sum - 1.5;
  }
  else if (fNumber == 5)
  {
    double sum = 0.;
    for (int j = 0; j < mDimension; j++) {
      sum += exp(fabs(x[j])) - 1.0;
    }
    return sum -1.1;
  }
  else if (fNumber == 6)
  {
    double sum = 0.;
    for (int j = 0; j < mDimension; j++) {
      sum += sin(x[j]) * sin(x[j]);
    }
    return sum - 1.1;
  }
  else if (fNumber == 7)
  {
    double sum = 0.;
    for (int j = 0; j < mDimension; j++) {
      sum += x[j] * x[j] * x[j] * x[j];
    }
    return sum - 1.01;
  }
  else if (fNumber == 8)
  {
    double sum = 0.;
    for (int j = 0; j < mDimension; j++) {
      sum += x[j] * x[j] * x[j] + x[j] * x[j];
    }
    return sum - 1.01;
  }
  else if (fNumber == 9)
  {
    double sum = 0.;
    for (int j = 0; j < mDimension; j++) {
      sum += cos(x[j]);
    }
    return mDimension - sum - 1.1;
  }
  else if (fNumber == 10)
  {
    double sum = 0.;
    for (int j = 0; j < mDimension; j++) {
      sum += x[j] * sin(x[j]);
    }
    return sum - 1.1;
  }
  else if (fNumber == 11)
  {
    double sum = 0.;
    for (int j = 0; j < mDimension; j++) {
      sum += log(1.0 + x[j] * x[j]);
    }
    return sum - 1.1;
  }
  else if (fNumber == 12)
  {
    double sum = 0.;
    for (int j = 0; j < mDimension; j++) {
      sum += tanh(x[j] * x[j]);
    }
    return sum - 1.1;
  }
  else if (fNumber == 13)
  {
    double sum = 0.;
    for (int j = 0; j < mDimension; j++) {
      sum += x[j];
    }
    return sum * sum - 1.01;
  }
  else if (fNumber == 14)
  {
    if (mDimension < 2) return -0.1; // Всегда выполняется для n=1

    double sum = 0.;
    for (int j = 0; j < mDimension - 1; j++) {
      double diff = x[j + 1] - x[j];
      sum += diff * diff;
    }
    return sum - 1.1;
  }
  else if (fNumber == 15)
  {
    double sum = 0.;
    for (int j = 0; j < mDimension; j++) {
      sum += x[j] * x[j] * (1.0 + cos(x[j]));
    }
    return sum - 1.1;
  }
  else if (fNumber == 16)
  {
    double sum = 0.;
    for (int j = 0; j < mDimension; j++) {
      sum += pow(fabs(x[j]), 1.5);
    }
    return sum - 1.1;
  }
  else if (fNumber == 17)
  {
    double sum = 0.;
    for (int j = 0; j < mDimension; j++) {
      sum += (x[j] * x[j]) / (1.0 + x[j] * x[j]);
    }
    return sum - 1.1;
  }
  else if (fNumber == 18)
  {
    double sum = 0.;
    for (int j = 0; j < mDimension; j++) {
      double sh = sinh(x[j]);
      sum += sh * sh;
    }
    return sum - 1.1;
  }
  else if (fNumber == 19)
  {
    double prod = 1.0;
    for (int j = 0; j < mDimension; j++) {
      prod *= (1.0 + x[j] * x[j]);
    }
    return prod - 1.0 - 1.1;
  }
  else if (fNumber == 20)
  {
    double sum = 0.;
    for (int j = 0; j < mDimension; j++) {
      sum += fabs(x[j]) - x[j] * x[j];
    }
    return sum - 1.1;
  }

}

inline int rastriginC20Problem::GetStartTrial(std::vector<double>& y, std::vector<std::string>& u, std::vector<double>& values)
{
  if (mIsInitialized)
  {
    y.resize(mDimension);
    u.resize(0);
    std::vector<double>lower;
    std::vector<double> upper;
    GetBounds(lower, upper);
    for (int j = 0; j < mDimension; j++)
      y[j] = (lower[j] + upper[j]) / 2.0;

    values = std::vector<double>(1, CalculateFunctionals(y, u, 0));
    return IGlobalOptimizationProblem::PROBLEM_OK;
  }
  return IGlobalOptimizationProblem::PROBLEM_ERROR;
}

// ------------------------------------------------------------------------------------------------
rastriginC20Problem::~rastriginC20Problem()
{

}

// ------------------------------------------------------------------------------------------------
LIB_EXPORT_API IGlobalOptimizationProblem* create()
{
  return new rastriginC20Problem();
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

  rastriginC20Problem rastr;
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

  rastriginC20Problem rastr;
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

  rastriginC20Problem rastr;
  rastr.SetDimension(2);
  rastr.Initialize();
  std::vector<double> Upper = { 0.0, 0.0 };
  std::vector<double> Lower = { 0.0, 0.0 };
  rastr.GetBounds(Lower, Upper);

  result = Lower[0];
  return result;
}
// - end of file ----------------------------------------------------------------------------------
