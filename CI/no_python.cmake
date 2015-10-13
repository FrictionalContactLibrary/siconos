include(CI/default.cmake)
set(WITH_PYTHON_WRAPPER OFF CACHE BOOL "Python" FORCE)
set(WITH_XML OFF CACHE BOOL "Xml" FORCE)
foreach(component "numerics;kernel;control;mechanics;io")
  set(WITH_${component}_PYTHON_WRAPPER OFF CACHE BOOL "Python" FORCE)
endforeach()
