find_library(SSL_LIB NAMES libssl.a)
find_library(CRYPTO_LIB NAMES libcrypto.a)
find_library(SASL2_LIB NAMES libsasl2.a)

set(ENCRYPT_LIBRARIES
 ${SSL_LIB}
 ${CRYPTO_LIB}
 ${SASL2_LIB}
)

message(STATUS "encrypt libraries:" ${ENCRYPT_LIBRARIES})
