include(tools4tests)

if(WITH_${COMPONENT}_TESTING)

  # If WITH_SYSTEM_SUITESPARSE, suite sparse is an imported target
  # that must sometimes be taken into account by tests.
  if(WITH_SYSTEM_SUITESPARSE)
    set(suitesparse SuiteSparse::CXSparse)
  else()
    set(suitesparse)
  endif()
      

  begin_tests(src/tools/test)

  new_test(SOURCES test_op3x3.c DEPS "externals;LAPACK::LAPACK")

  new_test(SOURCES test_timers_interf.c)

  new_test(SOURCES test_blas_lapack.c DEPS "externals;LAPACK::LAPACK")

  if(HAS_LAPACK_dgesvd) # Some lapack versions (e.g. Atlas) miss dgesvd
    new_test(SOURCES test_pinv.c)# DEPS "externals")
  endif()
  
  new_test(NAME tools_projection SOURCES test_projection.c)

  new_test(SOURCES NumericsArrays.c)

  #  tests for NumericsMatrix
  new_test(SOURCES NM_test.c DEPS "externals;BLAS::BLAS;${suitesparse}")

  #  tests for JordanAlgebra
  NEW_TEST(tools_test_JordanAlgebra JordanAlgebra_test.c)

  # MUMPS interface tests
  if(WITH_MUMPS)
    new_test(SOURCES NM_MUMPS_test.c)
  endif()
  
  # Specfic tests for SBM matrices 
  new_test(SOURCES SBM_test.c DEPS "externals;BLAS::BLAS;${suitesparse}")
  new_test(SOURCES SBCM_to_SBM.c)

  # Specfic tests for sparse matrices 
  new_test(SOURCES SparseMatrix_test.c DEPS "externals;${suitesparse}")

  if(HAS_ONE_LP_SOLVER)
    new_test(SOURCES vertex_problem.c)
  endif(HAS_ONE_LP_SOLVER)

  # ----------- LCP solvers tests -----------
  # Start tests for LCP dir.
  begin_tests(src/LCP/test)

  # Two kinds of tests :
  # * those with existing source file ('standards') and those where sources
  #  --> use new_test function (see details in cmake/tools4tests.cmake)
  #   new_test(NAME <test_name> SOURCES <source file name>)
  # * those generated from a global driver for a collection of solvers and data : run a test
  #  for each couple (data file, solver name).
  #  Usually, solvers list is defined in test_solvers_collection* files and data files list
  #  in data_collection* files.
  #  Use new_tests_collection function as below.
  
  new_test(NAME lcp_test_DefaultSolverOptions SOURCES LinearComplementarity_DefaultSolverOptions_test.c)

  new_tests_collection(
    DRIVER lcp_test_collection.c.in FORMULATION lcp COLLECTION TEST_LCP_COLLECTION_1
    EXTRA_SOURCES data_collection_1.c test_solvers_collection_1.c)
  new_tests_collection(
    DRIVER lcp_test_collection.c.in FORMULATION lcp COLLECTION TEST_LCP_COLLECTION_2
    EXTRA_SOURCES data_collection_2.c test_solvers_collection_1.c)
  new_tests_collection(
    DRIVER lcp_test_collection.c.in FORMULATION lcp COLLECTION TEST_LCP_COLLECTION_3
    EXTRA_SOURCES data_collection_3.c test_solvers_collection_2.c)
  new_tests_collection(
    DRIVER lcp_test_collection.c.in FORMULATION lcp COLLECTION TEST_LCP_COLLECTION_4
    EXTRA_SOURCES data_collection_4.c test_solvers_collection_3.c)

  # ----------- Relay solvers tests -----------
  # Start tests for Relay dir.
  begin_tests(src/Relay/test)

  new_test(SOURCES relay_test20.c)
  new_test(SOURCES step_test1.c)
  new_test(SOURCES step_test2.c)
  new_test(SOURCES step_test3.c)
  new_test(SOURCES step_test4.c)

  new_tests_collection(
    DRIVER relay_test_collection.c.in FORMULATION relay COLLECTION TEST_RELAY_COLLECTION_1
    EXTRA_SOURCES data_collection_1.c test_solvers_collection_1.c)

  # ----------- MLCP solvers tests -----------
  begin_tests(src/MLCP/test)
  
  if(HAVE_SYSTIMES_H AND WITH_CXX)
    new_test(NAME MLCPtest SOURCES main_mlcp.cpp)
  endif()
  new_test(SOURCES MixedLinearComplementarity_ReadWrite_test.c DEPS "externals")

  # ----------- MCP solvers tests -----------
  begin_tests(src/MCP/test)
  new_test(SOURCES MCP_test.c)
  new_test(SOURCES MCP_test1.c)
  new_test(SOURCES MCP_test2.c)
  new_test(SOURCES MCP_2_test1.c)

  # ----------- NCP solvers tests -----------
  begin_tests(src/NCP/test)
  # Generated tests
  # -- List of data files used in NCP tests --
  #  -- List of solvers for NCPs --
  set(SICONOS_NCP_SOLVERS
    SICONOS_NCP_NEWTON_FB_FBLSA
    SICONOS_NCP_NEWTON_MIN_FBLSA
    SICONOS_NCP_PATHSEARCH
    )
  if(HAVE_PATHFERRIS)
    list(APPEND SICONOS_NCP_SOLVERS "SICONOS_NCP_PATH")
  endif()
  
  if(WITH_UNSTABLE_TEST)
  
    # -- Declare the tests --
    foreach(SOLVER IN LISTS SICONOS_NCP_SOLVERS)
      new_tests_collection(DRIVER NCP_ZI1.c.in FORMULATION NCP COLLECTION ${SOLVER})
      if(DEV_MODE)
        new_tests_collection(DRIVER NCP_ZIT1.c.in FORMULATION NCP COLLECTION ${SOLVER} SUFFIX _UNSTABLE)
      endif()
    endforeach()
    
  endif()

  #===========================================
  # 3D Friction Contact tests
  #===========================================

  begin_tests(src/FrictionContact/test DEPS "${suitesparse}")
  new_tests_collection(
    DRIVER fc_test_collection.c.in FORMULATION fc3d COLLECTION TEST_NSGS_COLLECTION_1
    EXTRA_SOURCES data_collection_1.c test_nsgs_1.c)
  new_tests_collection(
    DRIVER fc_test_collection.c.in FORMULATION fc3d COLLECTION TEST_NSGS_COLLECTION_2
    EXTRA_SOURCES data_collection_2.c test_nsgs_2.c)
  new_tests_collection(
    DRIVER fc_test_collection.c.in FORMULATION fc3d COLLECTION TEST_NSGS_COLLECTION_3
    EXTRA_SOURCES data_collection_3.c test_nsgs_3.c)
  new_tests_collection(
    DRIVER fc_test_collection.c.in FORMULATION fc3d COLLECTION TEST_NSGS_COLLECTION_4
    EXTRA_SOURCES data_collection_4.c test_nsgs_4.c)
  new_tests_collection(
    DRIVER fc_test_collection.c.in FORMULATION fc3d COLLECTION TEST_NSGS_COLLECTION_5
    EXTRA_SOURCES data_collection_3.c test_nsgs_5.c)
  
  new_tests_collection(
    DRIVER fc_test_collection.c.in FORMULATION fc3d COLLECTION TEST_ADMM_COLLECTION_1
    EXTRA_SOURCES data_collection_1.c test_admm_1.c)
  new_tests_collection(
    DRIVER fc_test_collection.c.in FORMULATION fc3d COLLECTION TEST_ADMM_COLLECTION_2
    EXTRA_SOURCES data_collection_2.c test_admm_2.c)
  new_tests_collection(
    DRIVER fc_test_collection.c.in FORMULATION fc3d COLLECTION TEST_NSN_COLLECTION_1
    EXTRA_SOURCES data_collection_1.c test_nsn_1.c)
  new_tests_collection(
    DRIVER fc_test_collection.c.in FORMULATION fc3d COLLECTION TEST_NSN_COLLECTION_2
    EXTRA_SOURCES data_collection_3.c test_nsn_2.c)
  new_tests_collection(
    DRIVER fc_test_collection.c.in FORMULATION fc3d COLLECTION TEST_NSN_COLLECTION_3
    EXTRA_SOURCES data_collection_3.c test_nsn_3.c)

  new_tests_collection(
    DRIVER fc_test_collection.c.in FORMULATION fc3d COLLECTION TEST_VI_BASED_COLLECTION_1
    EXTRA_SOURCES data_collection_1.c test_vi_based_1.c)

  new_tests_collection(
    DRIVER fc_test_collection.c.in FORMULATION fc3d COLLECTION TEST_FP_COLLECTION_0
    EXTRA_SOURCES data_collection_3.c test_fp_0.c)
  new_tests_collection(
    DRIVER fc_test_collection.c.in FORMULATION fc3d COLLECTION TEST_FP_COLLECTION_1
    EXTRA_SOURCES data_collection_3.c test_fp_1.c)
  new_tests_collection(
    DRIVER fc_test_collection.c.in FORMULATION fc3d COLLECTION TEST_FP_COLLECTION_2
    EXTRA_SOURCES data_collection_3.c test_fp_2.c)

  new_tests_collection(
    DRIVER fc_test_collection.c.in FORMULATION fc3d COLLECTION TEST_PROX_COLLECTION_1
    EXTRA_SOURCES data_collection_3.c test_prox_1.c)

  new_tests_collection(
    DRIVER fc_test_collection.c.in FORMULATION fc3d COLLECTION TEST_QUARTIC_COLLECTION_1
    EXTRA_SOURCES data_collection_5.c test_quartic_1.c)
  
  #  IF (WITH_UNSTABLE_TEST)
  #    NEW_FC_3D_TEST(BoxesStack1-i100000-32.hdf5.dat SICONOS_FRICTION_3D_NSN_AC 1e-5 5000
  #      0 0 0
  #      IPARAM 1 1)
  #    NEW_FC_3D_TEST(BoxesStack1-i100000-32.hdf5.dat SICONOS_FRICTION_3D_NSN_AC_TEST 1e-5 500
  #      0 0 0
  #      IPARAM 1 1
  #      DPARAM SICONOS_DPARAM_LSA_ALPHA_MIN 0.0) # alpha_min needs to be equal to zero for convergence
  #    NEW_FC_3D_TEST(BoxesStack1-i100000-32.hdf5.dat SICONOS_FRICTION_3D_NSN_AC_TEST 1e-3 1000
  #      0 0 0
  #      DPARAM SICONOS_DPARAM_LSA_ALPHA_MIN 0.0) # alpha_min needs to be equal to zero for convergence
  #  ENDIF()
  
  # --- LMGC driver ---
  new_test(SOURCES fc3d_newFromFortranData.c)
  new_test(SOURCES fc3d_LmgcDriver_test1.c)
  new_test(SOURCES fc3d_LmgcDriver_test2.c)
  new_test(SOURCES fc3d_LmgcDriver_test3.c)
  new_test(SOURCES fc3d_LmgcDriver_test4.c)
  new_test(SOURCES fc3d_LmgcDriver_test5.c)

  # # --- Quartic ---
  # # NEW_FC_3D_TEST(FrictionContact3D_1c.dat SICONOS_FRICTION_3D_ONECONTACT_QUARTIC)
  # # NEW_FC_3D_TEST(FrictionContact3D_RR_1c.dat SICONOS_FRICTION_3D_ONECONTACT_QUARTIC)

  # ---------------------------------------------------
  # --- Global friction contact problem formulation ---
  # ---------------------------------------------------

  new_tests_collection(
    DRIVER gfc3d_test_collection.c.in FORMULATION gfc3d COLLECTION TEST_NSGS_COLLECTION_1
    EXTRA_SOURCES data_collection_gfc3d_1.c test_first_order_gfc3d_1.c)
  new_tests_collection(
    DRIVER gfc3d_test_collection.c.in FORMULATION gfc3d COLLECTION TEST_WR_COLLECTION_1
    EXTRA_SOURCES data_collection_gfc3d_1.c test_solvers_wr_gfc3d_1.c)
  new_tests_collection(
    DRIVER gfc3d_test_collection.c.in FORMULATION gfc3d COLLECTION TEST_NSN_COLLECTION_1
    EXTRA_SOURCES data_collection_gfc3d_1.c test_nsn_gfc3d_1.c)

  #  IF (WITH_UNSTABLE_TEST)
  #    NEW_GFC_3D_TEST(GFC3D_TwoRods1.dat SICONOS_GLOBAL_FRICTION_3D_NSN_AC 0 0
  #      0 0 0
  #      WILL_FAIL) # pass with mumps only
  #  ENDIF()

  # Alart Curnier functions
  new_test(NAME AlartCurnierFunctions_test SOURCES fc3d_AlartCurnierFunctions_test.c)
  
  if(WITH_FCLIB)

    new_test(NAME FCLIB_test1 SOURCES fc3d_writefclib_local_test.c DEPS FCLIB::fclib)

    new_tests_collection(
      DRIVER fc_test_collection.c.in FORMULATION fc3d COLLECTION TEST_NSGS_COLLECTION_6
      EXTRA_SOURCES data_collection_6.c test_nsgs_1.c DEPS FCLIB::fclib
      HDF5 ON
      )
    #target_compile_definitions(fc3d_TEST_NSGS_COLLECTION_6 PUBLIC TEST_HDF5)

    new_tests_collection(
      DRIVER fc_test_collection.c.in FORMULATION fc3d COLLECTION TEST_ADMM_COLLECTION_3
      EXTRA_SOURCES data_collection_6.c test_admm_1.c DEPS FCLIB::fclib
      HDF5 ON
      )

    #    # NEW_FC_3D_TEST_HDF5(Capsules-i125-1213.hdf5 SICONOS_FRICTION_3D_NSGS)
    #    # NEW_FC_3D_TEST_HDF5(Capsules-i125-1213.hdf5 SICONOS_FRICTION_3D_ADMM 1e-10 0
    #    #   0 0 0
    #    #   IPARAM SICONOS_FRICTION_3D_ADMM_IPARAM_RHO_STRATEGY  SICONOS_FRICTION_3D_ADMM_RHO_STRATEGY_RESIDUAL_BALANCING)
    
    #    # NEW_FC_3D_TEST_HDF5(LMGC_100_PR_PerioBox-i00361-60-03000.hdf5 SICONOS_FRICTION_3D_ADMM 1e-08 100000
    #    #   0 0 0
    #    #   IPARAM SICONOS_FRICTION_3D_ADMM_IPARAM_RHO_STRATEGY  SICONOS_FRICTION_3D_ADMM_RHO_STRATEGY_RESIDUAL_BALANCING)

    #    # NEW_FC_3D_TEST_HDF5(LMGC_100_PR_PerioBox-i00361-60-03000.hdf5 SICONOS_FRICTION_3D_ADMM 1e-08 100000
    #    #   0 0 0
    #    #   IPARAM SICONOS_FRICTION_3D_ADMM_IPARAM_RHO_STRATEGY  SICONOS_FRICTION_3D_ADMM_RHO_STRATEGY_RESIDUAL_BALANCING
    #    #   IPARAM SICONOS_FRICTION_3D_ADMM_IPARAM_SPARSE_STORAGE  SICONOS_FRICTION_3D_ADMM_FORCED_SPARSE_STORAGE)

    new_tests_collection(
      DRIVER gfc3d_test_collection.c.in FORMULATION gfc3d COLLECTION TEST_NSGS_COLLECTION_2
      EXTRA_SOURCES data_collection_gfc3d_2.c test_first_order_gfc3d_1.c DEPS FCLIB::fclib
      HDF5 ON
      )
    new_tests_collection(
      DRIVER gfc3d_test_collection.c.in  FORMULATION gfc3d COLLECTION TEST_WR_COLLECTION_2
      EXTRA_SOURCES data_collection_gfc3d_2.c test_solvers_wr_gfc3d_1.c DEPS FCLIB::fclib
      HDF5 ON
      )
    
    new_tests_collection(
      DRIVER gfc3d_test_collection.c.in  FORMULATION gfc3d COLLECTION TEST_NSN_COLLECTION_2
      EXTRA_SOURCES data_collection_gfc3d_2.c test_nsn_gfc3d_1.c DEPS FCLIB::fclib
      HDF5 ON
      )

    # ---------------------------------------------------
    # --- Rolling friction contact problem formulation ---
    # ---------------------------------------------------
    
    new_tests_collection(
      DRIVER rfc3d_test_collection.c.in  FORMULATION rolling_fc3d COLLECTION TEST_NSGS_COLLECTION_1
      EXTRA_SOURCES data_collection_rfc3d_1.c test_first_order_rfc3d_1.c DEPS FCLIB::fclib)
    
  endif()

  #===========================================
  # 2D Friction Contact tests
  #===========================================
  # test 2D dense on two differents files
  new_tests_collection(
    DRIVER fc_test_collection.c.in FORMULATION fc2d COLLECTION TEST_FC2D_COLLECTION_1
    EXTRA_SOURCES data_collection_fc2d_1.c test_fc2d_1.c)

  #===========================================
  # Generic mechanical tests
  #===========================================
  begin_tests(src/GenericMechanical/test)

  new_tests_collection(
    DRIVER gmp_test_collection.c.in FORMULATION gmp COLLECTION TEST_NSGS_COLLECTION_1
    EXTRA_SOURCES data_collection_1.c test_solvers_1.c)

  # new_test(NAME GMP_FAILED SOURCES GenericMechanical_test1.c)

  # ----------- Variationnal inequalities solvers tests -----------
  begin_tests(src/VI/test)

  new_test(SOURCES VI_test_collection_1.c)
  new_test(SOURCES VI_fc3d_test_collection_1.c DEPS "externals;BLAS::BLAS")

  set(SICONOS_VI_SOLVERS
    SICONOS_VI_BOX_QI
    SICONOS_VI_BOX_AVI_LSA
    )
  if(HAVE_PATHFERRIS)
    list(APPEND SICONOS_VI_SOLVERS "SICONOS_VI_BOX_PATH")
  endif()
  
  if(DEV_MODE)
    foreach(SOLVER IN LISTS SICONOS_VI_SOLVERS)
      new_tests_collection(DRIVER VI_ZI1.c.in FORMULATION vi COLLECTION ${SOLVER} SUFFIX I1 )
      new_tests_collection(DRIVER VI_ZIT1.c.in FORMULATION vi COLLECTION ${SOLVER} SUFFIX IT1 )
    endforeach()
  endif()

  # ----------- QP solvers tests -----------
  begin_tests(src/QP/test)

  new_test(NAME ConvexQP_test_collection SOURCES ConvexQP_test.c)
  new_test(NAME ConvexQP_FC3D_test_collection SOURCES  ConvexQP_FC3D_test.c DEPS "externals;BLAS::BLAS")
  
  # ----------- AVI solvers tests -----------
  begin_tests(src/AVI/test)

  if(HAS_ONE_LP_SOLVER)
    new_test(NAME AVI_twisting SOURCES implicit_twisting.c)
  endif()

  # ----------- SOCP solvers tests -----------
  begin_tests(src/SOCP/test)
  new_test(SOURCES soclcp_test1.c)
  new_test(SOURCES soclcp_test2.c)
  new_test(SOURCES soclcp_test3.c)
  # timeout on all machines, see
  # http://cdash-bipop.inrialpes.fr/testSummary.php?project=1&name=SOCLCP_test4&date=2015-09-03
  # Feel free to remove this once it is fixed --xhub
  #new_test(SOURCES soclcp_test4.c)
  #new_test(SOURCES soclcp_test5.c)
  new_test(SOURCES fc3d_to_soclcp.c DEPS LAPACK::LAPACK)

  # ---- Extra conf for ${COMPONENT}-test library ---
  if(TARGET numerics-test)
    if(MSVC)
      # This part should be reviewed by a windows expert ...
      include(WindowsLibrarySetup)
      windows_library_extra_setup("numerics-test" "numerics-test")
    endif()
  endif()

  # For SiconosLapack.h 
  target_link_libraries(numerics-test PRIVATE externals)
  target_link_libraries(numerics-test PUBLIC BLAS::BLAS)
  target_include_directories(numerics-test PUBLIC ${CMAKE_SOURCE_DIR}/externals/blas_lapack)

endif()
