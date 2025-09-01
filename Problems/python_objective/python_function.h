#ifndef __PYTHONPROBLEM_H__
#define __PYTHONPROBLEM_H__

#include <memory>

#include "IGlobalOptimizationProblem.h"
#include "pymodule_wrapper.h"

class TPythonProblem : public IGlobalOptimizationProblem
{
protected:

  int mDimension;
  bool mIsInitialized;
  std::string mPyFilePath;
  std::shared_ptr<TPythonModuleWrapper> mFunction;
  void* mLibpython_handle;

public:

  TPythonProblem();

  virtual int SetConfigPath(const std::string& configPath);
  virtual int SetDimension(int dimension);
  virtual int GetDimension() const;
  virtual int Initialize();

  virtual void GetBounds(std::vector<double>& upper, std::vector<double>& lower);
  virtual int GetOptimumValue(double& value) const;
  virtual int GetOptimumPoint(std::vector<double>& x, std::vector<std::string>& u) const;

  virtual int GetNumberOfFunctions() const;
  virtual int GetNumberOfConstraints() const;
  virtual int GetNumberOfCriterions() const;

  virtual double CalculateFunctionals(const std::vector<double>& y, std::vector<std::string>& u, int fNumber);

  ~TPythonProblem();
};

extern "C" LIB_EXPORT_API IGlobalOptimizationProblem* create();
extern "C" LIB_EXPORT_API void destroy(IGlobalOptimizationProblem* ptr);

#endif
// - end of file ----------------------------------------------------------------------------------
