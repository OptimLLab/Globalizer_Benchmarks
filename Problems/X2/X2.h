#ifndef __RASTRIGINPROBLEM_H__
#define __RASTRIGINPROBLEM_H__

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <fstream>

#include "ProblemInterface.h"

class X2Problem : public IProblem
{
protected:
protected:

  int mDimension;
  bool mIsInitialized;
  static const int mMaxDimension = 100;
  int function_number;

  double mLeftBorder;
  double mRightBorder;

public:

  X2Problem();

  virtual int SetConfigPath(const std::string& configPath);
  virtual int SetDimension(int dimension);
  virtual int GetDimension() const;
  virtual int Initialize();

  virtual void GetBounds(double* lower, double* upper);
  virtual int GetOptimumValue(double& value) const;
  virtual int GetOptimumPoint(double* x) const;

  virtual int GetNumberOfFunctions() const;
  virtual int GetNumberOfConstraints() const;
  virtual int GetNumberOfCriterions() const;

  virtual double CalculateFunctionals(const double* x, int fNumber);

  ~X2Problem();
};

extern "C" LIB_EXPORT_API IProblem* create();
extern "C" LIB_EXPORT_API void destroy(IProblem* ptr);

#endif
// - end of file ----------------------------------------------------------------------------------
