#include "pymodule_wrapper.h"

#include <stdexcept>
#include <iostream>

using namespace std;

PyObject* makeFloatList(const double* array, int size)
{
  PyObject *l = PyList_New(size);
  for (int i = 0; i != size; i++)
      PyList_SET_ITEM(l, i, PyFloat_FromDouble(array[i]));
  return l;
}

std::vector<double> floatListToVectorDouble(PyObject* incoming)
{
  std::vector<double> data;
  if (PyList_Check(incoming))
  {
    data.resize(PyList_Size(incoming));
    for(Py_ssize_t i = 0; i < PyList_Size(incoming); i++)
    {
      auto value = PyList_GetItem(incoming, i);
      data[i] = PyFloat_AsDouble(value);
      Py_DECREF(value);
    }
  }
  else
    throw std::logic_error("Passed PyObject pointer was not a list!");
  return data;
}

#ifdef WIN32
int setenv(const char* name, const char* value, int overwrite)
{
  int errcode = 0;
  if (!overwrite) {
    size_t envsize = 0;
    errcode = getenv_s(&envsize, NULL, 0, name);
    if (errcode || envsize) return errcode;
  }
  return _putenv_s(name, value);
}
#endif

std::vector<double> py_list_to_vector_double(PyObject* list_obj) 
{
  std::vector<double> result;

  if (!list_obj || !PyList_Check(list_obj)) 
  {
    throw - 1;
    return result;
  }

  Py_ssize_t size = PyList_Size(list_obj);
  result.reserve(size);

  for (Py_ssize_t i = 0; i < size; ++i) 
  {
    PyObject* item = PyList_GetItem(list_obj, i);

    if (PyFloat_Check(item)) 
    {
      result.push_back(PyFloat_AsDouble(item));
    }
    else if (PyLong_Check(item)) 
    {
      result.push_back(static_cast<double>(PyLong_AsLong(item)));
    }
    else 
    {
      result.push_back(0.0);
    }
  }

  return result;
}

TPythonModuleWrapper::TPythonModuleWrapper(const std::string& module_path)
{
  setenv("PYTHONPATH", module_path.c_str(), true);
  Py_Initialize();

  PyErr_Print();

  pModule = PyImport_ImportModule("Globalizer_problem");
  if (pModule == nullptr)
  {
    PyErr_Print();
    std::exit(1);
  }
  assert(pModule != nullptr);

  // Получаем класс из модуля
  pClass = PyObject_GetAttrString(pModule, "GlobalizerProblem");
  if (!pClass || !PyCallable_Check(pClass)) 
  {
    PyErr_Print();
    std::cerr << "Cannot find class" << std::endl;
    Py_DECREF(pModule);
    std::exit(1);
  }

  // Создаем экземпляр класса
  pInstance = PyObject_CallObject(pClass, NULL);
  if (!pInstance) 
  {
    PyErr_Print();
    std::cerr << "Failed to create instance" << std::endl;
    Py_DECREF(pClass);
    Py_DECREF(pModule);
    std::exit(1);
  }

  // Получаем размерность
  PyObject* pResultDim = PyObject_CallMethod(pInstance, "get_dimension", NULL);
  mDimension = PyFloat_AsDouble(pResultDim);
  if (pResultDim)
    Py_DECREF(pResultDim);
  else
    PyErr_Print();

  // Получаем границы
  PyObject* pResultLB = PyObject_CallMethod(pInstance, "get_lower_bounds", NULL);
  mLowerBound = py_list_to_vector_double(pResultLB);
  if (pResultLB)
    Py_DECREF(pResultLB);
  else
    PyErr_Print();

  PyObject* pResultUB = PyObject_CallMethod(pInstance, "get_upper_bounds", NULL);
  mUpperBound = py_list_to_vector_double(pResultUB);
  if (pResultUB)
    Py_DECREF(pResultUB);
  else
    PyErr_Print();


}

int TPythonModuleWrapper::GetDimension() const
{
  return mDimension;
}

double TPythonModuleWrapper::EvaluateFunction(const std::vector<double>& y) const
{
  double retval = 0;
#pragma omp critical
{

  auto py_arg = makeFloatList(y.data(), mDimension);
  auto arglist = PyTuple_Pack(1, py_arg);
  auto result = PyObject_CallMethod(pInstance, "calculate", "O", arglist);
  PyErr_Print();
  retval = PyFloat_AsDouble(result);
  Py_DECREF(py_arg);
  Py_DECREF(arglist);
  Py_DECREF(result);
}
  return retval;
}

std::vector<double> TPythonModuleWrapper::EvaluateAllFunction(const std::vector<double>& y) const
{
  std::vector<double> retval;
#pragma omp critical
  {

    auto py_arg = makeFloatList(y.data(), mDimension);
    auto arglist = PyTuple_Pack(1, py_arg);
    auto result = PyObject_CallMethod(pInstance, "calculate_all_functionals", "O", arglist);
    PyErr_Print();
    retval = py_list_to_vector_double(result);
    Py_DECREF(py_arg);
    Py_DECREF(arglist);
    Py_DECREF(result);
  }
  return retval;
}

TPythonModuleWrapper::~TPythonModuleWrapper()
{
  // Очистка
  Py_DECREF(pInstance);
  Py_DECREF(pClass);
  Py_DECREF(pModule);
  Py_Finalize();
}

void TPythonModuleWrapper::GetBounds(std::vector<double>& lower, std::vector<double>& upper) const
{
  for (int i = 0; i < mDimension; i++)
  {
    lower[i] = mLowerBound[i];
    upper[i] = mUpperBound[i];
  }
}

// ------------------------------------------------------------------------------------------------
int TPythonModuleWrapper::GetNumberOfFunctions() const
{
  try
  {
    PyObject* pNumberOfFunctions = PyObject_CallMethod(pInstance, "get_number_of_functions", NULL);
    if (pNumberOfFunctions)
    {
      int numberOfFunctions = PyLong_AsLong(pNumberOfFunctions);

      Py_DECREF(pNumberOfFunctions);
      return numberOfFunctions;
    }
    else
    {
      PyErr_Print();
      return 1;
    }
    
  }
  catch (...)
  {
    return 1;
  }
}

// ------------------------------------------------------------------------------------------------
int TPythonModuleWrapper::GetNumberOfConstraints() const
{
  try
  {
    PyObject* pNumberOfConstraints = PyObject_CallMethod(pInstance, "get_number_of_constraints", NULL);
    if (pNumberOfConstraints)
    {
      int numberOfConstraints = PyLong_AsLong(pNumberOfConstraints);

      Py_DECREF(pNumberOfConstraints);
      return numberOfConstraints;
    }
    else
    {
      PyErr_Print();
      return 1;
    }

  }
  catch (...)
  {
    return 1;
  }
}

// ------------------------------------------------------------------------------------------------
int TPythonModuleWrapper::GetNumberOfCriterions() const
{
  try
  {
    PyObject* pNumberOfCriterions = PyObject_CallMethod(pInstance, "get_number_of_criterions", NULL);
    if (pNumberOfCriterions)
    {
      int numberOfCriterions = PyLong_AsLong(pNumberOfCriterions);

      Py_DECREF(pNumberOfCriterions);
      return numberOfCriterions;
    }
    else
    {
      PyErr_Print();
      return 1;
    }

  }
  catch (...)
  {
    return 1;
  }
}

// ------------------------------------------------------------------------------------------------
inline int TPythonModuleWrapper::GetStartTrial(std::vector<double>& y, std::vector<std::string>& u, std::vector<double>& values)
{
  try
  {

    PyObject* pStartY = PyObject_CallMethod(pInstance, "get_start_y", NULL);
    if (pStartY)
    {
      y = py_list_to_vector_double(pStartY);

      Py_DECREF(pStartY);
    }
    else
    {
      PyErr_Print();
      return -1;
    }

    PyObject* pValues = PyObject_CallMethod(pInstance, "get_start_y", NULL);
    if (pValues)
    {
      values = py_list_to_vector_double(pValues);

      Py_DECREF(pValues);
    }
    else
    {
      PyErr_Print();
      return -1;
    }

  }
  catch (...)
  {
    return -1;
  }
  return 0;
}