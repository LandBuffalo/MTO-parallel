set(CMAKE_CXX_COMPILER "/apps/intel-ct/cc-wrapper/icpc")
set(CMAKE_CXX_COMPILER_ARG1 "")
set(CMAKE_CXX_COMPILER_ID "Intel")
set(CMAKE_CXX_COMPILER_VERSION "12.1.0.20120212")
set(CMAKE_CXX_COMPILER_WRAPPER "")
set(CMAKE_CXX_STANDARD_COMPUTED_DEFAULT "98")
set(CMAKE_CXX_COMPILE_FEATURES "cxx_std_98;cxx_binary_literals;cxx_std_11;cxx_alias_templates;cxx_attributes;cxx_auto_type;cxx_decltype;cxx_default_function_template_args;cxx_defaulted_functions;cxx_deleted_functions;cxx_extended_friend_declarations;cxx_extern_templates;cxx_func_identifier;cxx_lambdas;cxx_local_type_template_args;cxx_long_long_type;cxx_nullptr;cxx_right_angle_brackets;cxx_rvalue_references;cxx_static_assert;cxx_template_template_parameters;cxx_trailing_return_types;cxx_variadic_macros;cxx_variadic_templates")
set(CMAKE_CXX98_COMPILE_FEATURES "cxx_std_98;cxx_binary_literals")
set(CMAKE_CXX11_COMPILE_FEATURES "cxx_std_11;cxx_alias_templates;cxx_attributes;cxx_auto_type;cxx_decltype;cxx_default_function_template_args;cxx_defaulted_functions;cxx_deleted_functions;cxx_extended_friend_declarations;cxx_extern_templates;cxx_func_identifier;cxx_lambdas;cxx_local_type_template_args;cxx_long_long_type;cxx_nullptr;cxx_right_angle_brackets;cxx_rvalue_references;cxx_static_assert;cxx_template_template_parameters;cxx_trailing_return_types;cxx_variadic_macros;cxx_variadic_templates")
set(CMAKE_CXX14_COMPILE_FEATURES "")
set(CMAKE_CXX17_COMPILE_FEATURES "")

set(CMAKE_CXX_PLATFORM_ID "Linux")
set(CMAKE_CXX_SIMULATE_ID "")
set(CMAKE_CXX_SIMULATE_VERSION "")

set(CMAKE_AR "/usr/bin/ar")
set(CMAKE_RANLIB "/usr/bin/ranlib")
set(CMAKE_LINKER "/opt/bin/ld")
set(CMAKE_COMPILER_IS_GNUCXX )
set(CMAKE_CXX_COMPILER_LOADED 1)
set(CMAKE_CXX_COMPILER_WORKS TRUE)
set(CMAKE_CXX_ABI_COMPILED TRUE)
set(CMAKE_COMPILER_IS_MINGW )
set(CMAKE_COMPILER_IS_CYGWIN )
if(CMAKE_COMPILER_IS_CYGWIN)
  set(CYGWIN 1)
  set(UNIX 1)
endif()

set(CMAKE_CXX_COMPILER_ENV_VAR "CXX")

if(CMAKE_COMPILER_IS_MINGW)
  set(MINGW 1)
endif()
set(CMAKE_CXX_COMPILER_ID_RUN 1)
set(CMAKE_CXX_IGNORE_EXTENSIONS inl;h;hpp;HPP;H;o;O;obj;OBJ;def;DEF;rc;RC)
set(CMAKE_CXX_SOURCE_FILE_EXTENSIONS C;M;c++;cc;cpp;cxx;mm;CPP)
set(CMAKE_CXX_LINKER_PREFERENCE 30)
set(CMAKE_CXX_LINKER_PREFERENCE_PROPAGATES 1)

# Save compiler ABI information.
set(CMAKE_CXX_SIZEOF_DATA_PTR "8")
set(CMAKE_CXX_COMPILER_ABI "ELF")
set(CMAKE_CXX_LIBRARY_ARCHITECTURE "")

if(CMAKE_CXX_SIZEOF_DATA_PTR)
  set(CMAKE_SIZEOF_VOID_P "${CMAKE_CXX_SIZEOF_DATA_PTR}")
endif()

if(CMAKE_CXX_COMPILER_ABI)
  set(CMAKE_INTERNAL_PLATFORM_ABI "${CMAKE_CXX_COMPILER_ABI}")
endif()

if(CMAKE_CXX_LIBRARY_ARCHITECTURE)
  set(CMAKE_LIBRARY_ARCHITECTURE "")
endif()

set(CMAKE_CXX_CL_SHOWINCLUDES_PREFIX "")
if(CMAKE_CXX_CL_SHOWINCLUDES_PREFIX)
  set(CMAKE_CL_SHOWINCLUDES_PREFIX "${CMAKE_CXX_CL_SHOWINCLUDES_PREFIX}")
endif()





set(CMAKE_CXX_IMPLICIT_LINK_LIBRARIES "imf;svml;m;ipgo;decimal;cilkrts;stdc++;irc;c;irc_s;dl;c")
set(CMAKE_CXX_IMPLICIT_LINK_DIRECTORIES "/apps/openmpi/1.6.3/lib/Intel;/apps/openmpi/1.6.3/lib;/apps/openmpi/1.6.3/lib/profilers;/apps/intel-ct/12.1.9.293/cc/lib/intel64;/apps/intel-ct/12.1.9.293/fc/lib/intel64;/apps/intel-ct/composer_xe_2011_sp1.9.293/compiler/lib/intel64;/plush/dugong/usr/lib/gcc/x86_64-redhat-linux/4.4.7;/plush/dugong/usr/lib/gcc;/usr/lib/gcc/x86_64-redhat-linux/4.4.7;/plush/dugong/usr/lib64;/lib64;/usr/lib64;/apps/openmpi/1.6.3/lib/GNU;/plush/dugong/usr/lib;/lib;/usr/lib")
set(CMAKE_CXX_IMPLICIT_LINK_FRAMEWORK_DIRECTORIES "")
