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
    return IGlobalOptimizationProblem::PROBLEM_UNDEFINED;

  value = -mRightBorder;

  if (IsMultInt == true)
  {
    value = value * optMultKoef;
  }

  return IGlobalOptimizationProblem::PROBLEM_OK;
}

// ------------------------------------------------------------------------------------------------
int RastriginIntProblem::GetOptimumPoint(std::vector<double>& point, std::vector<std::string>& u) const
{
  if (!this->mIsInitialized)
    return IGlobalOptimizationProblem::PROBLEM_UNDEFINED;

  for (int i = 0; i < countContinuousVariables; i++)
    point[i] = 0.0;

  auto ndv = GetNumberOfDiscreteVariable();
  std::vector< std::vector<std::string>> values;
  GetDiscreteVariableValues(values);

  for (int i = 0; i < ndv; i++)
    u[i] = values[i][values[i].size() - 1];
  return IGlobalOptimizationProblem::PROBLEM_OK;
}

// ------------------------------------------------------------------------------------------------
double RastriginIntProblem::CalculateFunctionals(const std::vector<double>& x, std::vector<std::string>& u, int fNumber)
{
  double sum = 0.;
  int j = 0;
  int i = 0;
  std::vector<double> y(mDimension);
  for (; i < countContinuousVariables; i++)
    y[i] = x[i];
  for (; i < mDimension; i++)
    y[i] = discreteValues[u[i - countContinuousVariables][0] - 'A'];
  for (; j < (mDimension - GetNumberOfDiscreteVariable()); j++)
    sum += y[j] * y[j] - 10. * cos(2.0 * M_PI * y[j]) + 10.0;
  for (; j < mDimension; j++)
  {
    sum = sum - y[j];
  }

  sum = sum * (MultFunc(y) + multKoef);

  return sum;
}

int RastriginIntProblem::SetDimension(int dimension)
{
  if (dimension > 0 && dimension <= mMaxDimension)
  {
    mDimension = dimension;
    Initialize();
    return IGlobalOptimizationProblem::PROBLEM_OK;
  }
  else
    return -1;//IGlobalOptimizationProblem::PROBLEM_ERROR;
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
  std::vector<std::string> u(this->mDimension);

  mNumberOfValues.resize(GetNumberOfDiscreteVariable());
  for (int i = 0; i < GetNumberOfDiscreteVariable(); i++)
  {
    mNumberOfValues[i] = mDefNumberOfValues;
  }

  this->GetOptimumPoint(optPoint, u);


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



  discreteValues.resize(mDefNumberOfValues);
  double d = (mRightBorder - mLeftBorder) / (mDefNumberOfValues - 1);
  for (int i = 0; i < mDefNumberOfValues; i++)
  {
    discreteValues[i] = mLeftBorder + d * i;
  }

  return IGlobalOptimizationProblem::PROBLEM_OK;
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
inline int RastriginIntProblem::GetDiscreteVariableValues(std::vector< std::vector<std::string>>& values) const
{
  values.resize(GetNumberOfDiscreteVariable());

  for (int i = 0; i < GetNumberOfDiscreteVariable(); i++)
  {
    values[i].resize(mNumberOfValues[i]);
    for (int j = 0; j < mNumberOfValues[i]; j++)
    {
      values[i][j] = std::string(1, 'A' + j);
    }
  }
  return IGlobalOptimizationProblem::PROBLEM_OK;
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
