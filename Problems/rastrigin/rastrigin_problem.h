#ifndef __RASTRIGINPROBLEM_H__
#define __RASTRIGINPROBLEM_H__

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "IGlobalOptimizationProblem.h"

class RastriginProblem : public IGlobalOptimizationProblem
{
protected:

  int mDimension;
  bool mIsInitialized;
  static const int mMaxDimension = 100;

public:

  RastriginProblem();

  virtual int SetDimension(int dimension);
  virtual int GetDimension() const;
  virtual int Initialize();

  virtual void GetBounds(std::vector<double>& lower, std::vector<double>& upper);
  virtual int GetOptimumValue(double& value) const;
  virtual int GetOptimumPoint(std::vector<double>&, std::vector<std::string>& u) const;

  virtual int GetNumberOfFunctions() const;
  virtual int GetNumberOfConstraints() const;
  virtual int GetNumberOfCriterions() const;

  virtual double CalculateFunctionals(const std::vector<double>& x, std::vector<std::string>& u, int fNumber);

  ~RastriginProblem();
};

extern "C" LIB_EXPORT_API IGlobalOptimizationProblem* create();
extern "C" LIB_EXPORT_API void destroy(IGlobalOptimizationProblem* ptr);
extern "C" LIB_EXPORT_API double Calculation1D(double x, int fType, int fNum);
extern "C" LIB_EXPORT_API double Calculation(double x, double y);
extern "C" LIB_EXPORT_API double GetUpperBounds();
extern "C" LIB_EXPORT_API double GetLowerBounds();
#endif
// - end of file ----------------------------------------------------------------------------------
