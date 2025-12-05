#include "pymodule_wrapper.h"

#include <stdexcept>
#include <iostream>

using namespace std;

// ------------------------------------------------------------------------------------------------
/// <summary>
/// Создает python list из float
/// </summary>
/// <param name="array">входящий массив</param>
/// <param name="size">размер массива</param>
/// <returns>получившийся список</returns>
PyObject* makeFloatList(const double* array, int size)
{
  PyObject *l = PyList_New(size);
  for (int i = 0; i != size; i++)
      PyList_SET_ITEM(l, i, PyFloat_FromDouble(array[i]));
  return l;
}


PyObject* makeStrList(std::vector<std::string> array)
{
    PyObject* l = PyList_New(array.size());
    for (int i = 0; i != array.size(); i++)
        PyList_SET_ITEM(l, i, PyUnicode_FromString(array[i].c_str()));
    return l;
}



// ------------------------------------------------------------------------------------------------
#ifdef WIN32
/// <summary>
/// Добавляет переменную окружения
/// </summary>
/// <param name="name"></param>
/// <param name="value"></param>
/// <param name="overwrite"></param>
/// <returns></returns>
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


std::vector<std::vector<std::string>> py_list_of_list_to_vector_vectors_str(PyObject* list_obj) {
    std::vector<std::vector<std::string>> result;

    if (!list_obj || !PyList_Check(list_obj))
    {
        throw std::runtime_error("Invalid input: not a Python list");
    }

    Py_ssize_t outer_size = PyList_Size(list_obj);
    result.reserve(outer_size);

    for (Py_ssize_t i = 0; i < outer_size; ++i)
    {
        // Берем ссылку на внутренний список
        PyObject* inner_list_obj = PyList_GetItem(list_obj, i);
        Py_INCREF(inner_list_obj); // Увеличиваем счетчик ссылок

        std::unique_ptr<PyObject, decltype(&Py_DECREF)> inner_list_guard(
            inner_list_obj, Py_DECREF);

        if (!PyList_Check(inner_list_obj))
        {
            result.push_back(std::vector<std::string>());
            continue;
        }

        std::vector<std::string> inner_vec;
        Py_ssize_t inner_size = PyList_Size(inner_list_obj);
        inner_vec.reserve(inner_size);

        for (Py_ssize_t j = 0; j < inner_size; ++j)
        {
            PyObject* item = PyList_GetItem(inner_list_obj, j);
            Py_INCREF(item);

            std::unique_ptr<PyObject, decltype(&Py_DECREF)> item_guard(
                item, Py_DECREF);

            // Получаем строковое представление
            PyObject* py_str = PyObject_Str(item);
            if (!py_str)
            {
                inner_vec.push_back(std::string());
                continue;
            }

            std::unique_ptr<PyObject, decltype(&Py_DECREF)> str_guard(
                py_str, Py_DECREF);

            // Конвертируем в UTF-8
            const char* c_str = PyUnicode_AsUTF8(py_str);
            if (c_str)
            {
                inner_vec.push_back(std::string(c_str));
            }
            else
            {
                inner_vec.push_back(std::string());
                PyErr_Clear(); // Очищаем ошибку, если была
            }
        }

        result.push_back(std::move(inner_vec));
    }

    return result;
}

// ------------------------------------------------------------------------------------------------
/// <summary>
/// Создает вектор double по python list
/// </summary>
/// <param name="list_obj">
/// Создает вектор double по python list</param>
/// <returns> вектор double</returns>
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

// ------------------------------------------------------------------------------------------------
/// <summary>
/// Создает параметры для создания объекта класа
/// </summary>
/// <param name="param"></param>
/// <returns></returns>
PyObject* TPythonModuleWrapper::VectorToTuple(std::vector<IOptVariantType> param)
{

  std::vector<PyObject*> vec(param.size());
  for (int i = 0; i < param.size(); i++)
  {
    if (param[i].index() == 0)//int
      vec[i] = PyLong_FromLong(std::get<0>(param[i]));
    else if (param[i].index() == 1) //double
      vec[i] = PyFloat_FromDouble(std::get<1>(param[i]));
    else if (param[i].index() == 2) //string
      vec[i] = PyUnicode_FromString(std::get<2>(param[i]).c_str());
  }

  PyObject* tuple = PyTuple_New(vec.size());
  if (!tuple) return nullptr;

  for (size_t i = 0; i < vec.size(); ++i) {
    PyObject* item = vec[i];
    Py_INCREF(item); // Увеличиваем счетчик ссылок
    PyTuple_SET_ITEM(tuple, i, item);
  }

  return tuple;
}

// ------------------------------------------------------------------------------------------------
TPythonModuleWrapper::TPythonModuleWrapper(const std::string& module_path, std::vector<IOptVariantType> param,
  std::string functionScriptName, std::string functionClassName)
{
  setenv("PYTHONPATH", module_path.c_str(), true);
  Py_Initialize();

  PyErr_Print();

  ///////////////////////////////////////////////////////
  auto funcModule = PyImport_ImportModule(functionScriptName.c_str());
  if (funcModule == nullptr)
  {
    PyErr_Print();
    std::exit(1);
  }
  assert(funcModule != nullptr);

  // Получаем класс из модуля
  funcClass = PyObject_GetAttrString(funcModule, functionClassName.c_str());
  if (!pClass || !PyCallable_Check(funcClass))
  {
    PyErr_Print();
    std::cerr << "Cannot find class" << std::endl;
    Py_DECREF(funcModule);
    std::exit(1);
  }



  auto arglist = VectorToTuple(param);
  // Создаем экземпляр класса
  auto funcInstance = PyObject_CallObject(funcClass, arglist);
  if (!funcInstance)
  {
    PyErr_Print();
    std::cerr << "Failed to create instance" << std::endl;
    Py_DECREF(funcClass);
    Py_DECREF(funcModule);
    std::exit(1);
  }

  PyObject* tuple = PyTuple_New(1);
  if (!tuple) 
  {
    PyErr_Print();
    Py_DECREF(funcClass);
    Py_DECREF(funcModule);
    std::exit(1);
  }
  PyTuple_SET_ITEM(tuple, 0, funcInstance);
  Py_INCREF(funcInstance); 

  ///////////////////////////////////////////////////////


  ///////////////////////////////////////////////////////
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
  pInstance = PyObject_CallObject(pClass, tuple);
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


  PyObject* pResultDiscreteParams = PyObject_CallMethod(pInstance, "get_discrete_params", NULL);
  discreteParams = py_list_of_list_to_vector_vectors_str(pResultDiscreteParams);
  if (pResultDiscreteParams)
      Py_DECREF(pResultDiscreteParams);
  else
      PyErr_Print();


}

// ------------------------------------------------------------------------------------------------
int TPythonModuleWrapper::GetDimension() const
{
  return mDimension;
}

// ------------------------------------------------------------------------------------------------
double TPythonModuleWrapper::EvaluateFunction(const std::vector<double>& y, const std::vector<std::string>& categorys, int fNumber) const
{
  double retval = 0;
#pragma omp critical
  {

    auto py_arg1 = makeFloatList(y.data(), mDimension - discreteParams.size());
    auto py_arg2 = makeStrList(categorys);
    auto py_arg3 = PyLong_FromLong(fNumber);

    auto arglist = PyTuple_Pack(3, py_arg1, py_arg2, py_arg3);

    auto result = PyObject_CallMethod(pInstance, "calculate", "O", arglist);
    PyErr_Print();
    retval = PyFloat_AsDouble(result);
    Py_DECREF(py_arg1);
    Py_DECREF(py_arg2);
    Py_DECREF(py_arg3);
    Py_DECREF(arglist);
    Py_DECREF(result);
  }
  return retval;
}


// ------------------------------------------------------------------------------------------------
std::vector<double> TPythonModuleWrapper::EvaluateAllFunction(const std::vector<double>& y, const std::vector<std::string>& categorys) const
{
  std::vector<double> retval;
#pragma omp critical
  {

    auto py_arg1 = makeFloatList(y.data(), mDimension);
    auto py_arg2 = makeStrList(categorys);

    auto arglist = PyTuple_Pack(2, py_arg1, py_arg2);

    auto result = PyObject_CallMethod(pInstance, "calculate_all_functionals", "O", arglist);
    PyErr_Print();
    retval = py_list_to_vector_double(result);
    Py_DECREF(py_arg1);
    Py_DECREF(py_arg2);
    Py_DECREF(arglist);
    Py_DECREF(result);
  }
  return retval;
}

// ------------------------------------------------------------------------------------------------
TPythonModuleWrapper::~TPythonModuleWrapper()
{
  // Очистка
  Py_DECREF(pInstance);
  Py_DECREF(pClass);
  Py_DECREF(pModule);
  Py_Finalize();
}

// ------------------------------------------------------------------------------------------------
void TPythonModuleWrapper::GetBounds(std::vector<double>& lower, std::vector<double>& upper) const
{
  for (int i = 0; i < mDimension - discreteParams.size(); i++)
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

std::vector<std::vector<std::string>> TPythonModuleWrapper::GetDescreteParameters()
{
    return discreteParams;
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
      u[0] = GetDescreteParameters()[0][0]; // !!!! Bad
      Py_DECREF(pStartY);
    }
    else
    {
      PyErr_Print();
      return -1;
    }

    PyObject* pValues = PyObject_CallMethod(pInstance, "get_start_value", NULL);
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