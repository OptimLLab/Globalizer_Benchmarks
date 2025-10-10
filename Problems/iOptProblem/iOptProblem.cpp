#ifndef WIN32
#include <dlfcn.h>
#endif

#include "iOptProblem.h"

#include <iostream>
#include <filesystem>
#include <variant>

// ------------------------------------------------------------------------------------------------
iOptProblem::iOptProblem()
{
  mIsInitialized = false;
  mDimension = 3;

  const char* filename = __FILE__;
  std::filesystem::path file_path(filename);
  mPyFilePath = file_path.parent_path().string();

  functionScriptName = "rastrigin";
  functionClassName = "Rastrigin";
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
    std::vector<IOptVariantType> param = {mDimension};
    mFunction = std::make_shared<TPythonModuleWrapper>(mPyFilePath, param, functionScriptName, functionClassName);
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
  if (mIsInitialized)
  {
    return mFunction->GetNumberOfFunctions();
  }
  return 0;
}

// ------------------------------------------------------------------------------------------------
int iOptProblem::GetNumberOfConstraints() const
{
  if (mIsInitialized)
  {
    return mFunction->GetNumberOfConstraints();
  }
  return 0;
}

// ------------------------------------------------------------------------------------------------
int iOptProblem::GetNumberOfCriterions() const
{
  if (mIsInitialized)
  {
    return mFunction->GetNumberOfCriterions();
  }
  return 0;
}

// ------------------------------------------------------------------------------------------------
double iOptProblem::CalculateFunctionals(const std::vector<double>& y, std::vector<std::string>& u, int fNumber)
{
  return mFunction->EvaluateFunction(y, fNumber);
}

// ------------------------------------------------------------------------------------------------
inline std::vector<double> iOptProblem::CalculateAllFunctionals(const std::vector<double>& y, std::vector<std::string>& u)
{
  return mFunction->EvaluateAllFunction(y);
}

inline int iOptProblem::GetStartTrial(std::vector<double>& y, std::vector<std::string>& u, std::vector<double>& values)
{
  if (mIsInitialized)
  {
    return mFunction->GetStartTrial(y, u, values);
  }
}


// ------------------------------------------------------------------------------------------------
inline int iOptProblem::SetParameter(std::string name, std::string value)
{
  if (name == "Dimension")
    mDimension = std::stoi(value);

  else if (name == "mPyFilePath")
    mPyFilePath = value;

  else if (name == "functionScriptName")
    functionScriptName = value;

  else   if (name == "functionClassName")
    functionClassName = value;

  return IGlobalOptimizationProblem::PROBLEM_OK;
}

// ------------------------------------------------------------------------------------------------
//inline int iOptProblem::SetParameter(std::string name, std::any value)
//{
//  return IGlobalOptimizationProblem::PROBLEM_UNDEFINED;
//}

// ------------------------------------------------------------------------------------------------
inline int iOptProblem::SetParameter(std::string name, void* value)
{
  return IGlobalOptimizationProblem::PROBLEM_UNDEFINED;
}

// ------------------------------------------------------------------------------------------------
inline void iOptProblem::GetParameters(std::vector<std::string>& names, std::vector<std::string>& values)
{
  names.resize(4);
  values.resize(4);

  names[0] = "Dimension";
  values[0] = std::to_string(mDimension);

  names[0] = "mPyFilePath";
  values[0] = mPyFilePath;

  names[0] = "functionScriptName";
  values[0] = functionScriptName;

  names[0] = "functionClassName";
  values[0] = functionClassName;

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
