cmake_minimum_required(VERSION 3.6)
project(Linux)

set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_LINKER g++)
set(CMAKE_BUILD_TYPE "Release")

set (SGX_DIR /opt/intel/sgxsdk)

set (SGX_LIBRARY_PATH ${SGX_DIR}/lib64)
set (SGX_COMMON_CFLAGS -m64 -O2 -fPIC -Wno-attributes -DEDEBUG -UDEBUG)
set (SGX_EDGER8R ${SGX_DIR}/bin/x64/sgx_edger8r)
set (SGX_ENCLAVE_SIGNER ${SGX_DIR}/bin/x64/sgx_sign)

find_package (Boost REQUIRED COMPONENTS thread system date_time chrono)

set(SP_SOURCE_FILES
        ServiceProvider/isv_app/isv_app.cpp
        ServiceProvider/isv_app/VerificationManager.cpp
        Util/Base64.cpp
        Util/LogBase.cpp
        Util/UtilityFunctions.cpp
        Networking/AbstractNetworkOps.cpp
        Networking/Client.cpp
        Networking/NetworkManager.cpp
        Networking/NetworkManagerClient.cpp
        Networking/Server.cpp
        Networking/Session.cpp
        GoogleMessages/Messages.pb.cc
        WebService/WebService.cpp
        ServiceProvider/service_provider/ecp.cpp
        ServiceProvider/service_provider/ias_ra.cpp
        ServiceProvider/service_provider/ServiceProvider.cpp
)

add_executable(isv_app ${SP_SOURCE_FILES})

target_include_directories (isv_app
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/ServiceProvider/service_provider
        ${SGX_DIR}/include
        headers
        ${CMAKE_CURRENT_SOURCE_DIR}/Util
        ${CMAKE_CURRENT_SOURCE_DIR}/Networking
        ${CMAKE_CURRENT_SOURCE_DIR}/ServiceProvider/isv_app
        ${CMAKE_CURRENT_SOURCE_DIR}/GoogleMessages
        ${CMAKE_CURRENT_SOURCE_DIR}
        /usr/local/include
        ${CMAKE_CURRENT_SOURCE_DIR}/WebService
        ${CMAKE_CURRENT_SOURCE_DIR}/ServiceProvider/sample_libcrypto)

target_compile_options(isv_app PUBLIC ${SGX_COMMON_CFLAGS} -DEnableClient)
target_compile_options(isv_app PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-std=c++11>)

find_library(SGX_URTS_LIB NAMES sgx_urts PATHS ${SGX_LIBRARY_PATH})
find_library(SGX_UAE_LIB NAMES sgx_uae_service PATHS ${SGX_LIBRARY_PATH})
find_library(SGX_KE_LIB NAMES sgx_ukey_exchange PATHS ${SGX_LIBRARY_PATH})
find_library(PTHREADS_LIB pthread)
find_library(LOG4CPP_LIB log4cpp)
find_library(SSL_LIB ssl)
find_library(CRYPTO_LIB crypto)
find_library(PROTOBUF_LIB protobuf)
find_library(JSONCPP_LIB jsoncpp)
find_library(CURL_LIB curl)
find_library(SAMPLE_CRYPTO_LIB NAMES sample_libcrypto PATHS ${CMAKE_CURRENT_SOURCE_DIR}/ServiceProvider/sample_libcrypto)

target_link_libraries (isv_app
        ${SGX_URTS_LIB}
        ${SGX_UAE_LIB}
        ${SGX_KE_LIB}
        ${PTHREADS_LIB}
        ${SAMPLE_CRYPTO_LIB}
        ${LOG4CPP_LIB} ${SSL_LIB}
        ${Boost_SYSTEM_LIBRARY}
        ${Boost_THREAD_LIBRARY}
        ${CRYPTO_LIB}
        ${PROTOBUF_LIB}
        ${JSONCPP_LIB}
        ${CURL_LIB}
)

set (APP_SOURCE_FILES
        ${CMAKE_CURRENT_BINARY_DIR}/isv_enclave_u.c
        ${CMAKE_CURRENT_SOURCE_DIR}/Application/isv_app/isv_app.cpp
        Util/LogBase.cpp
        MessageHandler/MessageHandler.cpp
        Networking/NetworkManager.cpp
        Networking/Session.cpp
        Networking/Server.cpp
        Networking/Client.cpp
        Networking/NetworkManagerServer.cpp
        GoogleMessages/Messages.pb.cc
        Networking/AbstractNetworkOps.cpp
        Util/UtilityFunctions.cpp
        Enclave/Enclave.cpp
        Util/Base64.cpp
)

add_executable(app ${APP_SOURCE_FILES})

target_include_directories (app
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Util
        ${CMAKE_CURRENT_SOURCE_DIR}/MessageHandler
        ${CMAKE_CURRENT_SOURCE_DIR}/Enclave
        ${SGX_DIR}/include
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/Networking
        ${CMAKE_CURRENT_SOURCE_DIR}/GoogleMessages
)

target_compile_options(app PRIVATE ${SGX_COMMON_CFLAGS} -DEnableServer)
target_compile_options(app PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-std=c++11>)

target_link_libraries (app
        ${SGX_URTS_LIB}
        ${SGX_KE_LIB}
        ${SGX_UAE_LIB}
        ${PTHREADS_LIB}
        ${LOG4CPP_LIB}
        ${Boost_SYSTEM_LIBRARY}
        ${SSL_LIB}
        ${CRYPTO_LIB}
        ${Boost_THREAD_LIBRARY}
        ${PROTOBUF_LIB}
        ${JSONCPP_LIB}
)

# How to generate isv_enclave_{u,t}.{c,h}

set (EDGER8R_PATH --search-path ${SGX_DIR}/include --search-path ${CMAKE_CURRENT_SOURCE_DIR}/Application/isv_enclave)

add_custom_command(
        OUTPUT isv_enclave_u.c isv_enclave_u.h
        COMMAND ${SGX_EDGER8R} --untrusted ${EDGER8R_PATH} ${CMAKE_SOURCE_DIR}/Application/isv_enclave/isv_enclave.edl
        DEPENDS ${CMAKE_SOURCE_DIR}/Application/isv_enclave/isv_enclave.edl
)

add_custom_command(
        OUTPUT isv_enclave_t.c isv_enclave_t.h
        COMMAND ${SGX_EDGER8R} --trusted ${EDGER8R_PATH} ${CMAKE_SOURCE_DIR}/Application/isv_enclave/isv_enclave.edl
        DEPENDS ${CMAKE_SOURCE_DIR}/Application/isv_enclave/isv_enclave.edl
)

# How to generate the enclave .so file

find_library(SGX_TSERVICE_LIB sgx_tservice PATHS ${SGX_LIBRARY_DIR})
find_library(SGX_TCRYPTO_LIB sgx_tcrypto PATHS ${SGX_LIBRARY_DIR})

set (ENCLAVE_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/Application/isv_enclave/isv_enclave.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/isv_enclave_t.c
)

add_library(isv_enclave_objects OBJECT ${ENCLAVE_SOURCES})

target_include_directories (isv_enclave_objects
        PRIVATE
        ${SGX_DIR}/include
        ${SGX_DIR}/include/tlibc
        ${SGX_DIR}/include/stlport
        ${SGX_DIR}/include/crypto_px/include
        ${CMAKE_CURRENT_SOURCE_DIR}/Enclave
        ${CMAKE_CURRENT_BINARY_DIR}
)

target_compile_options(isv_enclave_objects PRIVATE ${SGX_COMMON_CFLAGS} -nostdinc -fvisibility=hidden -fpie -fstack-protector)
target_compile_options(isv_enclave_objects PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-std=c++11 -nostdinc++>)

find_library(SGX_TSTDC_LIB NAMES sgx_tstdc PATHS ${SGX_LIBRARY_PATH})
find_library(SGX_TSTDCXX_LIB NAMES sgx_tstdcxx PATHS ${SGX_LIBRARY_PATH})
find_library(SGX_SERVICE_LIB NAMES sgx_tservice PATHS ${SGX_LIBRARY_PATH})
find_library(SGX_TRTS_LIB NAMES sgx_trts PATHS ${SGX_LIBRARY_PATH})
find_library(SGX_TCRYPTO_LIB NAMES sgx_tcrypto PATHS ${SGX_LIBRARY_PATH})
find_library(SGX_TKE_LIB NAMES sgx_tkey_exchange PATHS ${SGX_LIBRARY_PATH})
find_library(SGX_TAE_LIB NAMES sgx_tae_service PATHS ${SGX_LIBRARY_PATH})

set (SGX_ENCLAVE_LFLAGS ${SGX_COMMON_CFLAGS} -Wl,--no-undefined -nostdlib -nodefaultlibs -nostartfiles
        -Wl,--whole-archive ${SGX_TRTS_LIB}
        -Wl,--no-whole-archive -Wl,--start-group ${SGX_TSTDC_LIB}
        ${SGX_TSTDCXX_LIB} ${SGX_TKE_LIB}
        ${SGX_TCRYPTO_LIB} ${SGX_SERVICE_LIB}
        -Wl,--end-group -Wl,-Bstatic -Wl,-Bsymbolic -Wl,--no-undefined
        -Wl,-pie,-eenclave_entry -Wl,--export-dynamic -Wl,--defsym,__ImageBase=0
        -Wl,--version-script=${CMAKE_CURRENT_SOURCE_DIR}/Application/isv_enclave/isv_enclave.lds)

add_custom_command(
        OUTPUT isv_enclave.so
        COMMAND g++ -o isv_enclave.so
        ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/isv_enclave_objects.dir/Application/isv_enclave/*.o
        ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/isv_enclave_objects.dir/*.o
        ${SGX_ENCLAVE_LFLAGS}
        DEPENDS isv_enclave_objects
)

# How to sign the enclave .so file

add_custom_target(
        isv_enclave.signed ALL
        DEPENDS isv_enclave.so ${CMAKE_CURRENT_SOURCE_DIR}/Application/isv_enclave/isv_enclave.config.xml
)

add_custom_command(
        TARGET isv_enclave.signed
        POST_BUILD
        COMMAND ${SGX_ENCLAVE_SIGNER} sign -key ${CMAKE_CURRENT_SOURCE_DIR}/Application/isv_enclave/isv_enclave_private.pem
        -enclave ${CMAKE_CURRENT_BINARY_DIR}/isv_enclave.so
        -out isv_enclave.signed.so
        -config ${CMAKE_CURRENT_SOURCE_DIR}/Application/isv_enclave/isv_enclave.config.xml
        COMMAND cp ${CMAKE_CURRENT_BINARY_DIR}/isv_enclave.signed.so ${CMAKE_CURRENT_SOURCE_DIR}/Application/
)
