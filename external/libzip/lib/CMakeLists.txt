include(CheckFunctionExists)
include(CheckIncludeFiles)
include(CheckLibraryExists)
include(CheckSymbolExists)
include(CheckTypeSize)
include(CheckCSourceRuns)
include(CheckCSourceCompiles)
include(CheckStructHasMember)
include(TestBigEndian)

# Checks

check_function_exists(_close HAVE__CLOSE)
check_function_exists(_dup HAVE__DUP)
check_function_exists(_fdopen HAVE__FDOPEN)
check_function_exists(_fileno HAVE__FILENO)
check_function_exists(_setmode HAVE__SETMODE)
check_symbol_exists(_snprintf stdio.h HAVE__SNPRINTF)
check_function_exists(_strdup HAVE__STRDUP)
check_symbol_exists(_stricmp string.h HAVE__STRICMP)
check_function_exists(_strtoi64 HAVE__STRTOI64)
check_function_exists(_strtoui64 HAVE__STRTOUI64)
check_function_exists(_unlink HAVE__UNLINK)
check_function_exists(arc4random HAVE_ARC4RANDOM)
check_function_exists(clonefile HAVE_CLONEFILE)
check_function_exists(explicit_bzero HAVE_EXPLICIT_BZERO)
check_function_exists(explicit_memset HAVE_EXPLICIT_MEMSET)
check_function_exists(fileno HAVE_FILENO)
check_function_exists(fseeko HAVE_FSEEKO)
check_function_exists(ftello HAVE_FTELLO)
check_function_exists(getprogname HAVE_GETPROGNAME)
check_function_exists(localtime_r HAVE_LOCALTIME_R)
check_function_exists(setmode HAVE_SETMODE)
check_symbol_exists(snprintf stdio.h HAVE_SNPRINTF)
check_symbol_exists(strcasecmp strings.h HAVE_STRCASECMP)
check_function_exists(strdup HAVE_STRDUP)
check_function_exists(stricmp HAVE_STRICMP)
check_function_exists(strtoll HAVE_STRTOLL)
check_function_exists(strtoull HAVE_STRTOULL)

check_include_files("sys/types.h;sys/stat.h;fts.h" HAVE_FTS_H)
# fts functions may be in external library
if(HAVE_FTS_H)
  check_function_exists(fts_open HAVE_FTS_OPEN)
  if(NOT HAVE_FTS_OPEN)
    check_library_exists(fts fts_open "" HAVE_LIB_FTS)
  else(NOT HAVE_FTS_OPEN)
    set(HAVE_LIB_FTS "" CACHE INTERNAL "")
  endif(NOT HAVE_FTS_OPEN)
else(HAVE_FTS_H)
  set(HAVE_LIB_FTS "" CACHE INTERNAL "")
endif(HAVE_FTS_H)

if(HAVE_LIB_FTS)
  set(FTS_LIB fts CACHE INTERNAL "")
else()
  set(FTS_LIB "" CACHE INTERNAL "")
endif()

check_include_files(stdbool.h HAVE_STDBOOL_H)
check_include_files(strings.h HAVE_STRINGS_H)
check_include_files(unistd.h HAVE_UNISTD_H)

check_include_files(inttypes.h HAVE_INTTYPES_H_LIBZIP)
check_include_files(stdint.h HAVE_STDINT_H_LIBZIP)
check_include_files(sys/types.h HAVE_SYS_TYPES_H_LIBZIP)

# TODO: fix test
# this test does not find __progname even when it exists
#check_symbol_exists(__progname stdlib.h HAVE___PROGNAME)

check_type_size(__int8 __INT8_LIBZIP)
check_type_size(int8_t INT8_T_LIBZIP)
check_type_size(uint8_t UINT8_T_LIBZIP)
check_type_size(__int16 __INT16_LIBZIP)
check_type_size(int16_t INT16_T_LIBZIP)
check_type_size(uint16_t UINT16_T_LIBZIP)
check_type_size(__int32 __INT32_LIBZIP)
check_type_size(int32_t INT32_T_LIBZIP)
check_type_size(uint32_t UINT32_T_LIBZIP)
check_type_size(__int64 __INT64_LIBZIP)
check_type_size(int64_t INT64_T_LIBZIP)
check_type_size(uint64_t UINT64_T_LIBZIP)
check_type_size("short" SHORT_LIBZIP)
check_type_size("int" INT_LIBZIP)
check_type_size("long" LONG_LIBZIP)
check_type_size("long long" LONG_LONG_LIBZIP)
check_type_size("off_t" SIZEOF_OFF_T)
check_type_size("size_t" SIZEOF_SIZE_T)

check_c_source_compiles("#include <sys/ioctl.h>
#include <linux/fs.h>
int main(int argc, char *argv[]) { unsigned long x = FICLONERANGE; }" HAVE_FICLONERANGE)

check_c_source_compiles("
int foo(char * _Nullable bar);
int main(int argc, char *argv[]) { }" HAVE_NULLABLE)

test_big_endian(WORDS_BIGENDIAN)

if(HAVE_INTTYPES_H_LIBZIP)
  set(LIBZIP_TYPES_INCLUDE "#if !defined(__STDC_FORMAT_MACROS)
#define __STDC_FORMAT_MACROS 1
#endif
#include <inttypes.h>")
elseif(HAVE_STDINT_H_LIBZIP)
  set(LIBZIP_TYPES_INCLUDE "#include <stdint.h>")
elseif(HAVE_SYS_TYPES_H_LIBZIP)
  set(LIBZIP_TYPES_INCLUDE "#include <sys/types.h>")
endif()

if(HAVE_INT8_T_LIBZIP)
  set(ZIP_INT8_T int8_t)
elseif(HAVE___INT8_LIBZIP)
  set(ZIP_INT8_T __int8)
else()
  set(ZIP_INT8_T "signed char")
endif()

if(HAVE_UINT8_T_LIBZIP)
  set(ZIP_UINT8_T uint8_t)
elseif(HAVE___INT8_LIBZIP)
  set(ZIP_UINT8_T "unsigned __int8")
else()
  set(ZIP_UINT8_T "unsigned char")
endif()

if(HAVE_INT16_T_LIBZIP)
  set(ZIP_INT16_T int16_t)
elseif(HAVE___INT16_LIBZIP)
  set(INT16_T_LIBZIP __int16)
elseif(SHORT_LIBZIP EQUAL 2)
  set(INT16_T_LIBZIP short)
endif()

if(HAVE_UINT16_T_LIBZIP)
  set(ZIP_UINT16_T uint16_t)
elseif(HAVE___INT16_LIBZIP)
  set(UINT16_T_LIBZIP "unsigned __int16")
elseif(SHORT_LIBZIP EQUAL 2)
  set(UINT16_T_LIBZIP "unsigned short")
endif()

if(HAVE_INT32_T_LIBZIP)
  set(ZIP_INT32_T int32_t)
elseif(HAVE___INT32_LIBZIP)
  set(ZIP_INT32_T __int32)
elseif(INT_LIBZIP EQUAL 4)
  set(ZIP_INT32_T int)
elseif(LONG_LIBZIP EQUAL 4)
  set(ZIP_INT32_T long)
endif()

if(HAVE_UINT32_T_LIBZIP)
  set(ZIP_UINT32_T uint32_t)
elseif(HAVE___INT32_LIBZIP)
  set(ZIP_UINT32_T "unsigned __int32")
elseif(INT_LIBZIP EQUAL 4)
  set(ZIP_UINT32_T "unsigned int")
elseif(LONG_LIBZIP EQUAL 4)
  set(ZIP_UINT32_T "unsigned long")
endif()

if(HAVE_INT64_T_LIBZIP)
  set(ZIP_INT64_T int64_t)
elseif(HAVE___INT64_LIBZIP)
  set(ZIP_INT64_T __int64)
elseif(LONG_LIBZIP EQUAL 8)
  set(ZIP_INT64_T long)
elseif(LONG_LONG_LIBZIP EQUAL 8)
  set(ZIP_INT64_T "long long")
endif()

if(HAVE_UINT64_T_LIBZIP)
  set(ZIP_UINT64_T uint64_t)
elseif(HAVE___INT64_LIBZIP)
  set(ZIP_UINT64_T "unsigned __int64")
elseif(LONG_LIBZIP EQUAL 8)
  set(ZIP_UINT64_T "unsigned long")
elseif(LONG_LONG_LIBZIP EQUAL 8)
  set(ZIP_UINT64_T "unsigned long long")
endif()

if(HAVE_NULLABLE)
  set(ZIP_NULLABLE_DEFINES)
else()
  set(ZIP_NULLABLE_DEFINES "#define _Nullable
#define _Nonnull")
endif()


set(CMAKE_C_VISIBILITY_PRESET hidden)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake-config.h.in ${PROJECT_BINARY_DIR}/config.h)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake-zipconf.h.in ${PROJECT_BINARY_DIR}/zipconf.h)
include_directories(
  ${CMAKE_SOURCE_DIR}/external/zlib
  ${CMAKE_BINARY_DIR}/external/zlib
  ${CMAKE_CURRENT_SOURCE_DIR}
)

add_library(zip STATIC
  zip_add.c
  zip_add_dir.c
  zip_add_entry.c
  zip_algorithm_deflate.c
  zip_buffer.c
  zip_close.c
  zip_delete.c
  zip_dir_add.c
  zip_dirent.c
  zip_discard.c
  zip_entry.c
  zip_error.c
  zip_error_clear.c
  zip_error_get.c
  zip_error_get_sys_type.c
  zip_error_strerror.c
  zip_error_to_str.c
  zip_extra_field.c
  zip_extra_field_api.c
  zip_fclose.c
  zip_fdopen.c
  zip_file_add.c
  zip_file_error_clear.c
  zip_file_error_get.c
  zip_file_get_comment.c
  zip_file_get_external_attributes.c
  zip_file_get_offset.c
  zip_file_rename.c
  zip_file_replace.c
  zip_file_set_comment.c
  zip_file_set_encryption.c
  zip_file_set_external_attributes.c
  zip_file_set_mtime.c
  zip_file_strerror.c
  zip_fopen.c
  zip_fopen_encrypted.c
  zip_fopen_index.c
  zip_fopen_index_encrypted.c
  zip_fread.c
  zip_fseek.c
  zip_ftell.c
  zip_get_archive_comment.c
  zip_get_archive_flag.c
  zip_get_encryption_implementation.c
  zip_get_file_comment.c
  zip_get_name.c
  zip_get_num_entries.c
  zip_get_num_files.c
  zip_hash.c
  zip_io_util.c
  zip_libzip_version.c
  zip_memdup.c
  zip_name_locate.c
  zip_new.c
  zip_open.c
  zip_pkware.c
  zip_progress.c
  zip_rename.c
  zip_replace.c
  zip_set_archive_comment.c
  zip_set_archive_flag.c
  zip_set_default_password.c
  zip_set_file_comment.c
  zip_set_file_compression.c
  zip_set_name.c
  zip_source_accept_empty.c
  zip_source_begin_write.c
  zip_source_begin_write_cloning.c
  zip_source_buffer.c
  zip_source_call.c
  zip_source_close.c
  zip_source_commit_write.c
  zip_source_compress.c
  zip_source_crc.c
  zip_source_error.c
  zip_source_file_common.c
  zip_source_file_stdio.c
  zip_source_free.c
  zip_source_function.c
  zip_source_get_file_attributes.c
  zip_source_is_deleted.c
  zip_source_layered.c
  zip_source_open.c
  zip_source_pkware_decode.c
  zip_source_pkware_encode.c
  zip_source_read.c
  zip_source_remove.c
  zip_source_rollback_write.c
  zip_source_seek.c
  zip_source_seek_write.c
  zip_source_stat.c
  zip_source_supports.c
  zip_source_tell.c
  zip_source_tell_write.c
  zip_source_window.c
  zip_source_write.c
  zip_source_zip.c
  zip_source_zip_new.c
  zip_stat.c
  zip_stat_index.c
  zip_stat_init.c
  zip_strerror.c
  zip_string.c
  zip_unchange.c
  zip_unchange_all.c
  zip_unchange_archive.c
  zip_unchange_data.c
  zip_utf-8.c
  ${CMAKE_CURRENT_BINARY_DIR}/zip_err_str.c
  )
add_library(libzip::zip ALIAS zip)

if(WIN32)
  target_sources(zip PRIVATE
    zip_source_file_win32.c
    zip_source_file_win32_named.c
    zip_source_file_win32_utf16.c
    zip_source_file_win32_utf8.c
    )
  if(CMAKE_SYSTEM_NAME MATCHES WindowsPhone OR CMAKE_SYSTEM_NAME MATCHES WindowsStore)
    target_sources(zip PRIVATE zip_random_uwp.c)
  else()
    target_sources(zip PRIVATE zip_source_file_win32_ansi.c zip_random_win32.c)
    target_link_libraries(zip PRIVATE advapi32)
  endif()
else(WIN32)
  target_sources(zip PRIVATE
    zip_source_file_stdio_named.c
    zip_random_unix.c
    )
endif(WIN32)

if(HAVE_LIBBZ2)
  target_sources(zip PRIVATE zip_algorithm_bzip2.c)
  target_link_libraries(zip PRIVATE BZip2::BZip2)
endif()

if(HAVE_LIBLZMA)
  target_sources(zip PRIVATE zip_algorithm_xz.c)
  target_link_libraries(zip PRIVATE LibLZMA::LibLZMA)
endif()

if(HAVE_LIBZSTD)
  target_sources(zip PRIVATE zip_algorithm_zstd.c)
  target_link_libraries(zip PRIVATE Zstd::Zstd)
endif()

if(HAVE_COMMONCRYPTO)
  target_sources(zip PRIVATE zip_crypto_commoncrypto.c)
elseif(HAVE_WINDOWS_CRYPTO)
  target_sources(zip PRIVATE zip_crypto_win.c)
  target_link_libraries(zip PRIVATE bcrypt)
elseif(HAVE_GNUTLS)
  target_sources(zip PRIVATE zip_crypto_gnutls.c)
  target_link_libraries(zip PRIVATE GnuTLS::GnuTLS Nettle::Nettle)
elseif(HAVE_OPENSSL)
  target_sources(zip PRIVATE zip_crypto_openssl.c)
  target_link_libraries(zip PRIVATE OpenSSL::Crypto)
elseif(HAVE_MBEDTLS)
  target_sources(zip PRIVATE zip_crypto_mbedtls.c)
  target_link_libraries(zip PRIVATE MbedTLS::MbedTLS)
endif()

if(HAVE_CRYPTO)
  target_sources(zip PRIVATE zip_winzip_aes.c zip_source_winzip_aes_decode.c zip_source_winzip_aes_encode.c)
endif()

if(SHARED_LIB_VERSIONNING)
  set_target_properties(zip PROPERTIES VERSION 5.5 SOVERSION 5)
endif()

target_link_libraries(zip PRIVATE zlibstatic)
target_include_directories(zip
  PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/lib>
  $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  )

# create zip_err_str.h from zip.h
file(READ ${CMAKE_CURRENT_SOURCE_DIR}/zip.h zip_h)
string(REGEX MATCHALL "#define ZIP_ER_([A-Z0-9_]+) ([0-9]+)[ \t]+/([-*0-9a-zA-Z, ']*)/" zip_h_err ${zip_h})
file(READ ${CMAKE_CURRENT_SOURCE_DIR}/zipint.h zipint_h)
string(REGEX MATCHALL "#define ZIP_ER_DETAIL_([A-Z0-9_]+) ([0-9]+)[ \t]+/([-*0-9a-zA-Z, ']*)/" zipint_h_err ${zipint_h})
set(zip_err_str [=[
/*
  This file was generated automatically by CMake
  from zip.h and zipint.h\; make changes there.
*/

#include "zipint.h"

#define L ZIP_ET_LIBZIP
#define N ZIP_ET_NONE
#define S ZIP_ET_SYS
#define Z ZIP_ET_ZLIB

#define E ZIP_DETAIL_ET_ENTRY
#define G ZIP_DETAIL_ET_GLOBAL

const struct _zip_err_info _zip_err_str[] = {
]=])
set(zip_err_type)
foreach(errln ${zip_h_err})
  string(REGEX MATCH "#define ZIP_ER_([A-Z0-9_]+) ([0-9]+)[ \t]+/([-*0-9a-zA-Z, ']*)/" err_t_tt ${errln})
  string(REGEX MATCH "([L|N|S|Z]+) ([-0-9a-zA-Z,, ']*)" err_t_tt "${CMAKE_MATCH_3}")
  string(STRIP "${CMAKE_MATCH_2}" err_t_tt)
  string(APPEND zip_err_str "    { ${CMAKE_MATCH_1}, \"${err_t_tt}\" },\n")
endforeach()
string(APPEND zip_err_str [=[}\;

const int _zip_err_str_count = sizeof(_zip_err_str)/sizeof(_zip_err_str[0])\;

const struct _zip_err_info _zip_err_details[] = {
]=])
foreach(errln ${zipint_h_err})
  string(REGEX MATCH "#define ZIP_ER_DETAIL_([A-Z0-9_]+) ([0-9]+)[ \t]+/([-*0-9a-zA-Z, ']*)/" err_t_tt ${errln})
  string(REGEX MATCH "([E|G]+) ([-0-9a-zA-Z, ']*)" err_t_tt "${CMAKE_MATCH_3}")
  string(STRIP "${CMAKE_MATCH_2}" err_t_tt)
  string(APPEND zip_err_str "    { ${CMAKE_MATCH_1}, \"${err_t_tt}\" },\n")
endforeach()
string(APPEND zip_err_str [=[}\;

const int _zip_err_details_count = sizeof(_zip_err_details)/sizeof(_zip_err_details[0])\;
]=])
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/zip_err_str.c ${zip_err_str})
