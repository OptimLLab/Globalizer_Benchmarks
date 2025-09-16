#ifndef WIN32
  #include <dlfcn.h>
#endif

#include "iOptProblem.h"

#include <iostream>
#include <filesystem>

// ------------------------------------------------------------------------------------------------
iOptProblem::iOptProblem()
{
  mIsInitialized = false;
  mDimension = 0;

  const char* filename = __FILE__;
  std::filesystem::path file_path(filename);
  mPyFilePath = file_path.parent_path().string();
}

// ------------------------------------------------------------------------------------------------
int iOptProblem::SetConfigPath(const std::string& configPath)
{
  mPyFilePath = std::string(configPath);
  return IGlobalOptimizationProblem::PROBLEM_OK;
}

// ------------------------------------------------------------------------------------------------
int iOptProblem::SetDimension(int dimension)
{
    return IGlobalOptimizationProblem::PROBLEM_OK;
}

// ------------------------------------------------------------------------------------------------
int iOptProblem::GetDimension() const
{
  return mDimension;
}

// ------------------------------------------------------------------------------------------------
int iOptProblem::Initialize()
{
  if (!mIsInitialized)
  {
#ifndef WIN32
    mLibpython_handle = dlopen("/home/lebedev_i/miniconda3/lib/libpython3.9.so", RTLD_LAZY | RTLD_GLOBAL);
#endif
    mFunction = std::make_shared<TPythonModuleWrapper>(mPyFilePath);
    mDimension = mFunction->GetDimension();

    mIsInitialized = true;
    return IGlobalOptimizationProblem::PROBLEM_OK;
  }
  else
    return IGlobalOptimizationProblem::PROBLEM_ERROR;
}

// ------------------------------------------------------------------------------------------------
void iOptProblem::GetBounds(std::vector<double>& lower, std::vector<double>& upper)
{
  if (mIsInitialized)
  {
    mFunction->GetBounds(lower, upper);
  }
}

// ------------------------------------------------------------------------------------------------
int iOptProblem::GetOptimumValue(double& value) const
{
  return IGlobalOptimizationProblem::PROBLEM_UNDEFINED;
}

// ------------------------------------------------------------------------------------------------
int iOptProblem::GetOptimumPoint(std::vector<double>& x, std::vector<std::string>& u) const
{
  return IGlobalOptimizationProblem::PROBLEM_UNDEFINED;
}

// ------------------------------------------------------------------------------------------------
int iOptProblem::GetNumberOfFunctions() const
{
  return 1;
}

// ------------------------------------------------------------------------------------------------
int iOptProblem::GetNumberOfConstraints() const
{
  return 0;
}

// ------------------------------------------------------------------------------------------------
int iOptProblem::GetNumberOfCriterions() const
{
  return 1;
}

// ------------------------------------------------------------------------------------------------
double iOptProblem::CalculateFunctionals(const std::vector<double>& y, std::vector<std::string>& u, int fNumber)
{
  return mFunction->EvaluateFunction(y);
}

// ------------------------------------------------------------------------------------------------
inline std::vector<double> iOptProblem::CalculateAllFunctionals(const std::vector<double>& y, std::vector<std::string>& u)
{
  return mFunction->EvaluateAllFunction(y);
}

// ------------------------------------------------------------------------------------------------
iOptProblem::~iOptProblem()
{
#ifndef WIN32
  if (mLibpython_handle)
    dlclose(mLibpython_handle);
#endif
}

// ------------------------------------------------------------------------------------------------
LIB_EXPORT_API IGlobalOptimizationProblem* create()
{
  return new iOptProblem();
}

// ------------------------------------------------------------------------------------------------
LIB_EXPORT_API void destroy(IGlobalOptimizationProblem* ptr)
{
  delete ptr;
}
// - end of file ----------------------------------------------------------------------------------
