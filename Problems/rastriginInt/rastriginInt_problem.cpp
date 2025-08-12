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

  A = 0;
  B = 0;
  optPoint = 0;
  multKoef = 0;
  optMultKoef = 0;
  IsMultInt = true;
}

// ------------------------------------------------------------------------------------------------
int RastriginIntProblem::GetOptimumValue(double& value) const
{
  if (!this->mIsInitialized)
    return IProblem::UNDEFINED;

  value = -mRightBorder;

  if (IsMultInt == true)
  {
    value = value * optMultKoef;
  }

  return IProblem::OK;
}

// ------------------------------------------------------------------------------------------------
int RastriginIntProblem::GetOptimumPoint(double* point) const
{
  if (!this->mIsInitialized)
    return IProblem::UNDEFINED;

  for (int i = 0; i < countContinuousVariables; i++)
    point[i] = 0.0;

  auto ndv = DiscreteVariable();

  for (int i = 0; i < ndv; i++)
    point[i + countContinuousVariables] = mRightBorder;
  return IProblem::OK;
}

// ------------------------------------------------------------------------------------------------
double RastriginIntProblem::CalculateFunctionals(const double* x, int fNumber)
{
  double sum = 0.;
  int j = 0;
  for (; j < (mDimension - GetNumberOfDiscreteVariable()); j++)
    sum += x[j] * x[j] - 10. * cos(2.0 * M_PI * x[j]) + 10.0;
  for (; j < mDimension; j++)
  {
    if (IsPermissibleValue(x[j], j))
      sum = sum - x[j];
  }

  sum = sum * (MultFunc(x) + multKoef);

  return sum;
}

int RastriginIntProblem::SetConfigPath(const std::string& configPath)
{
  return IProblem::OK;
}

int RastriginIntProblem::SetDimension(int dimension)
{
  if (dimension > 0 && dimension <= mMaxDimension)
  {
    mDimension = dimension;
    Initialize();
    return IProblem::OK;
  }
  else
    return IProblem::ERROR;
}

int RastriginIntProblem::GetDimension() const
{
  return mDimension;
}

void RastriginIntProblem::GetBounds(double* lower, double* upper)
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
double RastriginIntProblem::MultFunc(const double* x)
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
  double* x = new double[this->mDimension];
  A = new double[this->mDimension];
  B = new double[this->mDimension];
  this->GetBounds(A, B);

  optPoint = new double[this->mDimension];
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

  return IProblem::OK;
}

int RastriginIntProblem::GetNumberOfDiscreteVariable()
{
  //return 0;
  return   DiscreteVariable();
}

int RastriginIntProblem::GetNumberOfValues(int discreteVariable)
{
  if ((discreteVariable > GetDimension()) ||
    (discreteVariable < (GetDimension() - GetNumberOfDiscreteVariable())))
    return -1;
  return mNumberOfValues[discreteVariable - (GetDimension() - GetNumberOfDiscreteVariable())];
}

int RastriginIntProblem::GetAllDiscreteValues(int discreteVariable, double* values)
{
  if ((discreteVariable > GetDimension()) ||
    (discreteVariable < (GetDimension() - GetNumberOfDiscreteVariable())))
    return IIntegerProgrammingProblem::ERROR_DISCRETE_VALUE;
  int* mCurrentDiscreteValueIndex = 0;
  ClearCurrentDiscreteValueIndex(&mCurrentDiscreteValueIndex);

  // сбрасываем значение индекса текущего значения и задаем левую границу
  GetNextDiscreteValues(mCurrentDiscreteValueIndex, values[0], discreteVariable, -1);
  int numVal = GetNumberOfValues(discreteVariable);
  // определяем все остальные значения
  for (int i = 1; i < numVal; i++)
  {
    GetNextDiscreteValues(mCurrentDiscreteValueIndex, values[i], discreteVariable);
  }
  return IProblem::OK;
}

int RastriginIntProblem::GetNextDiscreteValues(int* mCurrentDiscreteValueIndex, double& value, int discreteVariable, int previousNumber)
{
  if ((discreteVariable > GetDimension()) ||
    (discreteVariable < (GetDimension() - GetNumberOfDiscreteVariable())) ||
    (mCurrentDiscreteValueIndex == 0) ||
    (mDefNumberOfValues == 0))
    return IIntegerProgrammingProblem::ERROR_DISCRETE_VALUE;
  // если -1 то сбрасываем значение текущего номера
  if (previousNumber == -1)
  {
    mCurrentDiscreteValueIndex[discreteVariable - GetNumberOfDiscreteVariable()] = 0;
    value = mLeftBorder;
    return IProblem::OK;
  }
  else if (previousNumber == -2)
  {
    double d = (mRightBorder - mLeftBorder) /
      (mNumberOfValues[discreteVariable - (GetDimension() - GetNumberOfDiscreteVariable())] - 1);
    mCurrentDiscreteValueIndex[discreteVariable - GetNumberOfDiscreteVariable()]++;
    value = mLeftBorder + d *
      mCurrentDiscreteValueIndex[discreteVariable - GetNumberOfDiscreteVariable()];
    return IProblem::OK;
  }
  else
  {
    double d = (mRightBorder - mLeftBorder) /
      (mNumberOfValues[discreteVariable - (GetDimension() - GetNumberOfDiscreteVariable())] - 1);
    mCurrentDiscreteValueIndex[discreteVariable - GetNumberOfDiscreteVariable()] =
      previousNumber;
    mCurrentDiscreteValueIndex[discreteVariable - GetNumberOfDiscreteVariable()]++;
    value = mLeftBorder + d * mCurrentDiscreteValueIndex[discreteVariable -
      GetNumberOfDiscreteVariable()];
    return IProblem::OK;
  }

}

bool RastriginIntProblem::IsPermissibleValue(double value, int discreteVariable)
{
#ifndef AccuracyDouble
#define AccuracyDouble 0.00000001
#endif

  if ((discreteVariable > GetDimension()) ||
    (discreteVariable < (GetDimension() - GetNumberOfDiscreteVariable())))
    return false;
  double d = (mRightBorder - mLeftBorder) /
    (mNumberOfValues[discreteVariable - (GetDimension() - GetNumberOfDiscreteVariable())] - 1);
  double v = 0;
  for (int i = 0; i < mNumberOfValues[discreteVariable - (GetDimension() - GetNumberOfDiscreteVariable())]; i++)
  {
    v = mLeftBorder + d * i;
    if (fabs(v - value) < AccuracyDouble)
    {
      return true;
    }
  }
  return false;
}


// ------------------------------------------------------------------------------------------------
LIB_EXPORT_API IProblem* create()
{
  return new RastriginIntProblem();
}

// ------------------------------------------------------------------------------------------------
LIB_EXPORT_API void destroy(IProblem* ptr)
{
  delete ptr;
}
// - end of file ----------------------------------------------------------------------------------
