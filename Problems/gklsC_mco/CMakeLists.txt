# GCGen входит в Globalizer; при отдельной сборке путь можно задать вручную.
set(GKLS_MCO_GCGEN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../GCGen" CACHE PATH "GCGen source directory")
if(EXISTS "${GKLS_MCO_GCGEN_DIR}/sample_src/GKLS/GKLSProblem.cpp" OR gklsC_mco_build)
  GLOBALIZER_BENCHMARKS_define_problem(gklsC_mco)
  if(TARGET gklsC_mco)
    target_include_directories(gklsC_mco PRIVATE "${GKLS_MCO_GCGEN_DIR}/src" "${GKLS_MCO_GCGEN_DIR}/sample_src")
    target_sources(gklsC_mco PRIVATE
      "${GKLS_MCO_GCGEN_DIR}/src/IGeneralOptProblem.cpp"
      "${GKLS_MCO_GCGEN_DIR}/src/IOptProblem.cpp"
      "${GKLS_MCO_GCGEN_DIR}/sample_src/GKLS/GKLSProblem.cpp"
      "${GKLS_MCO_GCGEN_DIR}/sample_src/GKLS/GKLSOption.cpp"
      "${GKLS_MCO_GCGEN_DIR}/sample_src/GKLS/gkls_random.cpp")
  endif()
endif()
