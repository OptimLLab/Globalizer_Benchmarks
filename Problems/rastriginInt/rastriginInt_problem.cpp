#include "rastriginInt_problem.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include "rastriginInt_problem.h"

// ------------------------------------------------------------------------------------------------
RastriginIntProblem::RastriginIntProblem()
{
  this->mIsInitialized = false;
  mMaxDimension = 100;
  mMinDimension = 2;
  mNumberOfCriterions = 1;
  mNumberOfConstraints = 0;
  mLeftBorder = -2.2;
  mRightBorder = 1.8;
  countContinuousVariables = 1;
  mDimension = 2;
  mDefNumberOfValues = 2;

  multKoef = 0;
  optMultKoef = 0;
  IsMultInt = true;
}

// ------------------------------------------------------------------------------------------------
int RastriginIntProblem::GetOptimumValue(double& value) const
{
  if (!this->mIsInitialized)
    return IGlobalOptimizationProblem::UNDEFINED;

  value = -mRightBorder;

  if (IsMultInt == true)
  {
    value = value * optMultKoef;
  }

  return IGlobalOptimizationProblem::OK;
}

// ------------------------------------------------------------------------------------------------
int RastriginIntProblem::GetOptimumPoint(std::vector<double>& point) const
{
  if (!this->mIsInitialized)
    return IGlobalOptimizationProblem::UNDEFINED;

  for (int i = 0; i < countContinuousVariables; i++)
    point[i] = 0.0;

  auto ndv = GetNumberOfDiscreteVariable();

  for (int i = 0; i < ndv; i++)
    point[i + countContinuousVariables] = mRightBorder;
  return IGlobalOptimizationProblem::OK;
}

// ------------------------------------------------------------------------------------------------
double RastriginIntProblem::CalculateFunctionals(const std::vector<double>& x, int fNumber)
{
  double sum = 0.;
  int j = 0;
  for (; j < (mDimension - GetNumberOfDiscreteVariable()); j++)
    sum += x[j] * x[j] - 10. * cos(2.0 * M_PI * x[j]) + 10.0;
  for (; j < mDimension; j++)
  { 
      sum = sum - x[j];
  }

  sum = sum * (MultFunc(x) + multKoef);

  return sum;
}

int RastriginIntProblem::SetDimension(int dimension)
{
  if (dimension > 0 && dimension <= mMaxDimension)
  {
    mDimension = dimension;
    Initialize();
    return IGlobalOptimizationProblem::OK;
  }
  else
    return IGlobalOptimizationProblem::ERROR;
}

int RastriginIntProblem::GetDimension() const
{
  return mDimension;
}

void RastriginIntProblem::GetBounds(std::vector<double>& lower, std::vector<double>& upper)
{
  for (int i = 0; i < mDimension; i++)
  {
    lower[i] = mLeftBorder;
    upper[i] = mRightBorder;
  }
}

int RastriginIntProblem::GetNumberOfFunctions() const
{
  return mNumberOfConstraints + mNumberOfCriterions;
}

int RastriginIntProblem::GetNumberOfConstraints() const
{
  return mNumberOfConstraints;
}

int RastriginIntProblem::GetNumberOfCriterions() const
{
  return mNumberOfCriterions;
}

// ------------------------------------------------------------------------------------------------
RastriginIntProblem::~RastriginIntProblem()
{

}


// ------------------------------------------------------------------------------------------------
double RastriginIntProblem::MultFunc(const std::vector<double>& x)
{
  double res = 0;

  double a;
  double d;
  int dim = 0;

  for (int j = 0; j < this->mDimension; j++)
  {
    d = (B[j] - A[j]) / 2;
    a = (x[j] - optPoint[j]) / d;
    a = a * a;
    res = res + a;
    dim++;
  }

  res = -res;

  if (IsMultInt == true)
    return res;
  else
    return 1.0;
}

// ------------------------------------------------------------------------------------------------
int RastriginIntProblem::Initialize()
{
  this->mIsInitialized = true;
  std::vector<double> x(this->mDimension);
  A.resize(this->mDimension);
  B.resize(this->mDimension);
  this->GetBounds(A, B);

  optPoint.resize(this->mDimension);
  this->GetOptimumPoint(optPoint);

  int count = (int)pow(2.0, this->mDimension);
  for (int i = 0; i < count; i++)
  {
    for (int j = 0; j < this->mDimension; j++)
    {
      x[j] = (((i >> j) & 1) == 0) ? A[j] : B[j];
    }

    double v = fabs(MultFunc(x));
    if (v > multKoef)
      multKoef = v;
  }
  multKoef += 4;
  optMultKoef = (MultFunc(optPoint) + multKoef);

  mNumberOfValues.resize(GetNumberOfDiscreteVariable());
  for (int i = 0; i < GetNumberOfDiscreteVariable(); i++)
  {
    mNumberOfValues[i] = mDefNumberOfValues;
  }

  return IGlobalOptimizationProblem::OK;
}

int RastriginIntProblem::GetNumberOfDiscreteVariable() const
{
  return mDimension - countContinuousVariables;
}

int RastriginIntProblem::SetNumberOfDiscreteVariable(int numberOfDiscreteVariable)
{
  countContinuousVariables = mDimension - numberOfDiscreteVariable;

  return Initialize();
}

// ------------------------------------------------------------------------------------------------
inline int RastriginIntProblem::GetDiscreteVariableValues(std::vector< std::vector<double>> values) const
{
  values.resize(GetNumberOfDiscreteVariable());

  for (int i = 0; i < GetNumberOfDiscreteVariable(); i++)
  {
    double d = (mRightBorder - mLeftBorder) / (mNumberOfValues[i] - 1);

    for (int j = 0; j < mNumberOfValues[i]; j++)
    {
      values[i][j] = mLeftBorder + d * j;
    }
  }
  return IGlobalOptimizationProblem::OK;
}

 

// ------------------------------------------------------------------------------------------------
LIB_EXPORT_API IGlobalOptimizationProblem* create()
{
  return new RastriginIntProblem();
}

// ------------------------------------------------------------------------------------------------
LIB_EXPORT_API void destroy(IGlobalOptimizationProblem* ptr)
{
  delete ptr;
}
// - end of file ----------------------------------------------------------------------------------
