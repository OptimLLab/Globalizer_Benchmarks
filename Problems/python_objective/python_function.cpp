#ifndef WIN32
  #include <dlfcn.h>
#endif

#include "python_function.h"

// ------------------------------------------------------------------------------------------------
TPythonProblem::TPythonProblem()
{
  mIsInitialized = false;
  mDimension = 0;
  //mPyFilePath = "F:/OptimLLab/Globalizer_Benchmarks/Problems/python_objective/objective_simple.py";
  mPyFilePath = "F:/OptimLLab/Globalizer_Benchmarks/Problems/python_objective/";
}

// ------------------------------------------------------------------------------------------------
int TPythonProblem::SetConfigPath(const std::string& configPath)
{
  mPyFilePath = std::string(configPath);
  return IGlobalOptimizationProblem::OK;
}

// ------------------------------------------------------------------------------------------------
int TPythonProblem::SetDimension(int dimension)
{
    return IGlobalOptimizationProblem::OK;
}

// ------------------------------------------------------------------------------------------------
int TPythonProblem::GetDimension() const
{
  return mDimension;
}

// ------------------------------------------------------------------------------------------------
int TPythonProblem::Initialize()
{
  if (!mIsInitialized)
  {
#ifndef WIN32
    mLibpython_handle = dlopen("/home/lebedev_i/miniconda3/lib/libpython3.9.so", RTLD_LAZY | RTLD_GLOBAL);
#endif
    mFunction = std::make_shared<TPythonModuleWrapper>(mPyFilePath);
    mDimension = mFunction->GetDimension();

    mIsInitialized = true;
    return IGlobalOptimizationProblem::OK;
  }
  else
    return IGlobalOptimizationProblem::ERROR;
}

// ------------------------------------------------------------------------------------------------
void TPythonProblem::GetBounds(std::vector<double>& lower, std::vector<double>& upper)
{
  if (mIsInitialized)
  {
    mFunction->GetBounds(lower, upper);
  }
}

// ------------------------------------------------------------------------------------------------
int TPythonProblem::GetOptimumValue(double& value) const
{
  return IGlobalOptimizationProblem::UNDEFINED;
}

// ------------------------------------------------------------------------------------------------
int TPythonProblem::GetOptimumPoint(std::vector<double>& x, std::vector<std::string>& u) const
{
  return IGlobalOptimizationProblem::UNDEFINED;
}

// ------------------------------------------------------------------------------------------------
int TPythonProblem::GetNumberOfFunctions() const
{
  return 1;
}

// ------------------------------------------------------------------------------------------------
int TPythonProblem::GetNumberOfConstraints() const
{
  return 0;
}

// ------------------------------------------------------------------------------------------------
int TPythonProblem::GetNumberOfCriterions() const
{
  return 1;
}

// ------------------------------------------------------------------------------------------------
double TPythonProblem::CalculateFunctionals(const std::vector<double>& y, std::vector<std::string>& u, int fNumber)
{
  return mFunction->EvaluateFunction(y);
}

// ------------------------------------------------------------------------------------------------
TPythonProblem::~TPythonProblem()
{
#ifndef WIN32
  if (mLibpython_handle)
    dlclose(mLibpython_handle);
#endif
}

// ------------------------------------------------------------------------------------------------
LIB_EXPORT_API IGlobalOptimizationProblem* create()
{
  return new TPythonProblem();
}

// ------------------------------------------------------------------------------------------------
LIB_EXPORT_API void destroy(IGlobalOptimizationProblem* ptr)
{
  delete ptr;
}
// - end of file ----------------------------------------------------------------------------------
