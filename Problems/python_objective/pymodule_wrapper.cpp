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

TPythonModuleWrapper::TPythonModuleWrapper(const std::string& module_path)
{
  setenv("PYTHONPATH", module_path.c_str(), true);
  //std::cout << "AAAAAAAAAAAAAAAAAAAAAAAAAAAA1!!!!\n";
  Py_Initialize();
  //std::cout << "AAAAAAAAAAAAAAAAAAAAAAAAAAAA2!!!!\n";
  auto pName = PyUnicode_FromString("objective_simple");
  //std::cout << "AAAAAAAAAAAAAAAAAAAAAAAAAAAA3!!!!\n";
  PyErr_Print();
  //std::cout << "AAAAAAAAAAAAAAAAAAAAAAAAAAAA4!!!!\n";  
  auto pModule = PyImport_Import(pName);
  //std::cout << "AAAAAAAAAAAAAAAAAAAAAAAAAAAA5!!!!\n";  
  if (pModule == nullptr)
  {
    PyErr_Print();
    std::exit(1);
  }
  //std::cout << "AAAAAAAAAAAAAAAAAAAAAAAAAAAA6!!!!\n";
  assert(pModule != nullptr);
  Py_DECREF(pName);
  //std::cout << "AAAAAAAAAAAAAAAAAAAAAAAAAAAA7!!!!\n";
  auto pDict = PyModule_GetDict(pModule);
  //std::cout << "AAAAAAAAAAAAAAAAAAAAAAAAAAAA8!!!!\n";
  //auto pDimension = PyDict_GetItemString(pDict, "dimension");
  //assert(pDimension != nullptr);
  //mDimension = PyLong_AsLong(pDimension);
  //Py_DECREF(pDimension);
  //assert(mDimension > 0 && mDimension < 50);
  mDimension = 6;

  //auto pUpper = PyDict_GetItemString(pDict, "upper");
  //assert(pUpper != nullptr);
  //mUpperBound = floatListToVectorDouble(pUpper);
  //Py_DECREF(pUpper);

  //auto pLower = PyDict_GetItemString(pDict, "lower");
  //assert(pLower != nullptr);
  //mLowerBound = floatListToVectorDouble(pLower);
  //Py_DECREF(pLower);

  mLowerBound.resize(mDimension);
  mUpperBound.resize(mDimension);


  mLowerBound = {90,  10, 30, 4, 0, 0};
  mUpperBound = {150, 13, 60, 7, 2, 5};


  mPFunc = PyDict_GetItemString(pDict, "objective");
  Py_DECREF(pDict);
  assert(mPFunc != nullptr);
  assert(PyCallable_Check(mPFunc));
  Py_DECREF(pModule);
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
  auto result = PyObject_CallObject(mPFunc, arglist);
  retval = PyFloat_AsDouble(result);
  Py_DECREF(py_arg);
  Py_DECREF(arglist);
  Py_DECREF(result);
}
  return retval;
}

TPythonModuleWrapper::~TPythonModuleWrapper()
{
  Py_DECREF(mPFunc);
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
