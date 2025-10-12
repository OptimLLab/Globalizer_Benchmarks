#ifndef WIN32
  #include <dlfcn.h>
#endif

#include "python_function.h"

// ------------------------------------------------------------------------------------------------
PythonProblem::PythonProblem()
{
  mIsInitialized = false;
  mDimension = 0;
  //mPyFilePath = "F:/OptimLLab/Globalizer_Benchmarks/Problems/python_objective/objective_simple.py";
  mPyFilePath = "F:/OptimLLab/Globalizer_Benchmarks/Problems/python_objective/";
}

// ------------------------------------------------------------------------------------------------
int PythonProblem::SetConfigPath(const std::string& configPath)
{
  mPyFilePath = std::string(configPath);
  return IGlobalOptimizationProblem::PROBLEM_OK;
}

// ------------------------------------------------------------------------------------------------
int PythonProblem::SetDimension(int dimension)
{
    return IGlobalOptimizationProblem::PROBLEM_OK;
}

// ------------------------------------------------------------------------------------------------
int PythonProblem::GetDimension() const
{
  return mDimension;
}

// ------------------------------------------------------------------------------------------------
int PythonProblem::Initialize()
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
void PythonProblem::GetBounds(std::vector<double>& lower, std::vector<double>& upper)
{
  if (mIsInitialized)
  {
    mFunction->GetBounds(lower, upper);
  }
}

// ------------------------------------------------------------------------------------------------
int PythonProblem::GetOptimumValue(double& value) const
{
  return IGlobalOptimizationProblem::PROBLEM_UNDEFINED;
}

// ------------------------------------------------------------------------------------------------
int PythonProblem::GetOptimumPoint(std::vector<double>& x, std::vector<std::string>& u) const
{
  return IGlobalOptimizationProblem::PROBLEM_UNDEFINED;
}

// ------------------------------------------------------------------------------------------------
int PythonProblem::GetNumberOfFunctions() const
{
  return 1;
}

// ------------------------------------------------------------------------------------------------
int PythonProblem::GetNumberOfConstraints() const
{
  return 0;
}

// ------------------------------------------------------------------------------------------------
int PythonProblem::GetNumberOfCriterions() const
{
  return 1;
}

// ------------------------------------------------------------------------------------------------
double PythonProblem::CalculateFunctionals(const std::vector<double>& y, std::vector<std::string>& u, int fNumber)
{
  return mFunction->EvaluateFunction(y);
}

// ------------------------------------------------------------------------------------------------
PythonProblem::~PythonProblem()
{
#ifndef WIN32
  if (mLibpython_handle)
    dlclose(mLibpython_handle);
#endif
}

// ------------------------------------------------------------------------------------------------
LIB_EXPORT_API IGlobalOptimizationProblem* create()
{
  return new PythonProblem();
}

// ------------------------------------------------------------------------------------------------
LIB_EXPORT_API void destroy(IGlobalOptimizationProblem* ptr)
{
  delete ptr;
}
// - end of file ----------------------------------------------------------------------------------
