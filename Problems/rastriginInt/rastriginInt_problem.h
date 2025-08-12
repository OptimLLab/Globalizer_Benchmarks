#ifndef __RASTRIGINPROBLEM_H__
#define __RASTRIGINPROBLEM_H__

#include "ProblemInterface.h"

class RastriginIntProblem : public IIntegerProgrammingProblem
{


protected:
  int countContinuousVariables;

  double* A;
  double* B;
  double* optPoint;
  double multKoef;
  double optMultKoef;

  int mDimension;
  bool mIsInitialized;
  int mMaxDimension;

  int mMinDimension;
  int mNumberOfCriterions;
  int mNumberOfConstraints;
  double mLeftBorder;
  double mRightBorder;

  int mDefNumberOfValues;

  bool IsMultInt;

  std::vector<int> mNumberOfValues;

  /// Очищает номер текущего значения для дискретного параметра
  virtual void ClearCurrentDiscreteValueIndex(int** mCurrentDiscreteValueIndex)
  {
    if (GetNumberOfDiscreteVariable() > 0)
    {
      if (*mCurrentDiscreteValueIndex != 0)
        delete[] * mCurrentDiscreteValueIndex;

      *mCurrentDiscreteValueIndex = new int[GetNumberOfDiscreteVariable()];
      for (int i = 0; i < GetNumberOfDiscreteVariable(); i++)
        (*mCurrentDiscreteValueIndex)[i] = 0;
    }
  }

  int DiscreteVariable() const
  {
    return   mDimension - countContinuousVariables;
  }

public:

  RastriginIntProblem();

  virtual int GetOptimumValue(double& value) const;
  virtual int GetOptimumPoint(double* x) const;

  virtual double CalculateFunctionals(const double* x, int fNumber);


  virtual int SetConfigPath(const std::string& configPath);
  virtual int SetDimension(int dimension);
  virtual int GetDimension() const;


  virtual void GetBounds(double* lower, double* upper);

  virtual int GetNumberOfFunctions() const;
  virtual int GetNumberOfConstraints() const;
  virtual int GetNumberOfCriterions() const;

  ~RastriginIntProblem();



  double MultFunc(const double* x);

  ///Инициализация задачи
  virtual int Initialize();

  // Унаследовано через IIntegerProgrammingProblem
  virtual int GetNumberOfDiscreteVariable();
  virtual int GetNumberOfValues(int discreteVariable);
  virtual int GetAllDiscreteValues(int discreteVariable, double* values);
  virtual int GetNextDiscreteValues(int* mCurrentDiscreteValueIndex, double& value, int discreteVariable, int previousNumber = -2);
  virtual bool IsPermissibleValue(double value, int discreteVariable);
};

extern "C" LIB_EXPORT_API IProblem* create();
extern "C" LIB_EXPORT_API void destroy(IProblem* ptr);

#endif
// - end of file ----------------------------------------------------------------------------------
