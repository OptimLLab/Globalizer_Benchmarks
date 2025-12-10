
if(GLOBALIZER_BENCHMARKS_PYTHON_PATH)

  set(VENV_PATH ${GLOBALIZER_BENCHMARKS_PYTHON_PATH})
    message("VENV_PATH" "${VENV_PATH}")
    
    # Устанавливаем переменные Python
  set(Python3_EXECUTABLE "${VENV_PATH}/bin/python")
  set(Python3_ROOT_DIR "${VENV_PATH}")
  
  set(Python_EXECUTABLE "${VENV_PATH}/bin/python")
  set(Python_ROOT_DIR "${VENV_PATH}")
 endif()
  
find_package (Python COMPONENTS Interpreter Development NumPy)

set(EXTRA_DEFINES "")
#if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  set(EXTRA_DEFINES PYLIBNAME="${Python_LIBRARIES}")
  message("Python pname ${Python_LIBRARIES}")
  message("Python end")
  
  message(STATUS "Python interpreter: ${Python_EXECUTABLE}")
message(STATUS "Python version: ${Python_VERSION}")
message(STATUS "Python include dir: ${Python_INCLUDE_DIRS}")
message(STATUS "Python library: ${Python_LIBRARIES}")

  #message(${PYLIBNAME})
#endif()

set(COMPILER_FLAGS "")
#if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  set(COMPILER_FLAGS -std=c++17 -march=native -fopenmp)
#endif()

if(Python_Development_FOUND AND Python_NumPy_FOUND)
  GLOBALIZER_BENCHMARKS_define_problem(iOptProblem LINK_LIBS ${Python_LIBRARIES}
                                         INCLUDE_DIRS ${Python_INCLUDE_DIRS} ${Python_NumPy_INCLUDE_DIRS}
                                         EXTRA_DEFINITIONS ${EXTRA_DEFINES}
                                         COMPILE_OPTIONS ${COMPILER_FLAGS})
endif()


if (Python_Development_FOUND)
  message("Python Development FOUND")
endif()

if (Python_NumPy_FOUND)
  message("Python NumPy FOUND")
endif()