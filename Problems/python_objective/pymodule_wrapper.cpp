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

// ------------------------------------------------------------------------------------------------
/// <summary>
/// Создает вектор double по python list
/// </summary>
/// <param name="list_obj">
/// Создает вектор double по python list</param>
/// <returns> вектор double</returns>
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

// ------------------------------------------------------------------------------------------------
/// <summary>
/// Добавляет переменную окружения
/// </summary>
/// <param name="name"></param>
/// <param name="value"></param>
/// <param name="overwrite"></param>
/// <returns></returns>
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

// ------------------------------------------------------------------------------------------------
TPythonModuleWrapper::TPythonModuleWrapper(const std::string& module_path)
{
  setenv("PYTHONPATH", module_path.c_str(), true);

  // Надёжный на весь процесс флаг: Py_IsInitialized() ненадёжен,
  // когда несколько DLL имеют своё состояние Python C-API.
  const char* alreadyInit = getenv("GLOBALIZER_PYTHON_INITIALIZED");

  if (!Py_IsInitialized())
  {
    Py_Initialize();
    PyErr_Print();

    // PyEval_InitThreads() устарел и НЕ НУЖЕН начиная с Python 3.7 —
    // Py_Initialize() уже инициализирует GIL. Удаляем этот вызов.

    // Помечаем, что интерпретатор инициализирован в этом процессе.
    setenv("GLOBALIZER_PYTHON_INITIALIZED", "1", true);

    // Освобождаем GIL один раз после инициализации.
    if (!alreadyInit)
      // Освобождаем GIL один раз после инициализации.
      main_ts = PyEval_SaveThread();
  }
  else
  {
    // Интерпретатор уже поднят другой задачей/DLL — ничего не инициализируем.
    main_ts = nullptr;
  }

  // Дальше — весь код с импортами, но ОБЯЗАТЕЛЬНО под GIL:
  PyGILState_STATE gstate = PyGILState_Ensure();


  auto pName = PyUnicode_FromString("objective_simple");
  PyErr_Print();
  auto pModule = PyImport_Import(pName);
  if (pModule == nullptr)
  {
    PyErr_Print();
    std::exit(1);
  }
  assert(pModule != nullptr);
  Py_DECREF(pName);
  auto pDict = PyModule_GetDict(pModule);

  mDimension = 6;


  mLowerBound.resize(mDimension);
  mUpperBound.resize(mDimension);


  mLowerBound = {90,  10, 30, 4, 0, 0};
  mUpperBound = {150, 13, 60, 7, 2, 5};


  mPFunc = PyDict_GetItemString(pDict, "objective");
  Py_DECREF(pDict);
  assert(mPFunc != nullptr);
  assert(PyCallable_Check(mPFunc));
  Py_DECREF(pModule);
  PyGILState_Release(gstate);
}

// ------------------------------------------------------------------------------------------------
int TPythonModuleWrapper::GetDimension() const
{
  return mDimension;
}

// ------------------------------------------------------------------------------------------------
double TPythonModuleWrapper::EvaluateFunction(const std::vector<double>& y) const
{
  double retval = 0;
#pragma omp critical
{
  PyGILState_STATE gstate = PyGILState_Ensure();
  auto py_arg = makeFloatList(y.data(), mDimension);
  auto arglist = PyTuple_Pack(1, py_arg);
  auto result = PyObject_CallObject(mPFunc, arglist);
  retval = PyFloat_AsDouble(result);
  Py_DECREF(py_arg);
  Py_DECREF(arglist);
  Py_DECREF(result);
  PyGILState_Release(gstate);
}
  return retval;
}

// ------------------------------------------------------------------------------------------------
TPythonModuleWrapper::~TPythonModuleWrapper()
{
  PyGILState_STATE gstate = PyGILState_Ensure();
  Py_DECREF(mPFunc);
  PyGILState_Release(gstate);
  //Py_Finalize();
}

// ------------------------------------------------------------------------------------------------
void TPythonModuleWrapper::GetBounds(std::vector<double>& lower, std::vector<double>& upper) const
{
  for (int i = 0; i < mDimension; i++)
  {
    lower[i] = mLowerBound[i];
    upper[i] = mUpperBound[i];
  }
}
