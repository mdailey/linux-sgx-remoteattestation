cmake_minimum_required(VERSION 3.6)
project(Linux)

#set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_LINKER g++)
set(CMAKE_BUILD_TYPE "Release")

set (SGX_DIR /opt/intel/sgx-sdk/sgxsdk)
set (SGX_LIBRARY_PATH ${SGX_DIR}/lib64)
set (SGX_COMMON_CFLAGS -m64 -O2 -fPIC -Wno-attributes -UEDEBUG -UDEBUG -std=c++11 -DEnableClient)
set (SGX_EDGER8R ${SGX_DIR}/bin/x64/sgx_edger8r)
set (SGX_ENCLAVE_SIGNER $(SGX_DIR)/bin/x64/sgx_sign)

set(SOURCE_FILES
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

#g++ -m64 -O2 -fPIC -Wno-attributes -Iservice_provider -I/opt/intel/sgx-sdk/sgxsdk/include -Iheaders -I../Util -I../Networking -Iisv_app -I../GoogleMessages -I/usr/local/include -I../WebService -DNDEBUG -UEDEBUG -UDEBUG -std=c++11 -DEnableClient -c isv_app/isv_app.cpp -o isv_app/isv_app.o

add_executable(isv_app ${SOURCE_FILES})

find_package (Boost REQUIRED COMPONENTS thread system date_time chrono)

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

target_compile_options(isv_app PRIVATE ${SGX_COMMON_CFLAGS})

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

