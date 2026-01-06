#include "lib.h"

int lib_init_all(void)
{
    if (lib_core_init() != 0)
        return -1;
    if (libapparmor_init() != 0)
        return -1;
    if (libasan_init() != 0)
        return -1;
    if (libboost_init() != 0)
        return -1;
    if (libbz2_init() != 0)
        return -1;
    if (libc_init() != 0)
        return -1;
    if (libcap_init() != 0)
        return -1;
    if (libcrypt_init() != 0)
        return -1;
    if (libcrypto_init() != 0)
        return -1;
    if (libcurl_init() != 0)
        return -1;
    if (libdbus_init() != 0)
        return -1;
    if (libdl_init() != 0)
        return -1;
    if (libexpat_init() != 0)
        return -1;
    if (libffi_init() != 0)
        return -1;
    if (libgc_init() != 0)
        return -1;
    if (libgccrt_init() != 0)
        return -1;
    if (libglib_init() != 0)
        return -1;
    if (libjson_c_init() != 0)
        return -1;
    if (libltdl_init() != 0)
        return -1;
    if (liblzma_init() != 0)
        return -1;
    if (libm_init() != 0)
        return -1;
    if (libmagic_init() != 0)
        return -1;
    if (libmvec_init() != 0)
        return -1;
    if (libncurses_init() != 0)
        return -1;
    if (libnsl_init() != 0)
        return -1;
    if (libpac_init() != 0)
        return -1;
    if (libpam_init() != 0)
        return -1;
    if (libpthread_init() != 0)
        return -1;
    if (libreadline_init() != 0)
        return -1;
    if (librt_init() != 0)
        return -1;
    if (libselinux_init() != 0)
        return -1;
    if (libssl_init() != 0)
        return -1;
    if (libstdcxx_init() != 0)
        return -1;
    if (libsystemd_init() != 0)
        return -1;
    if (libudev_init() != 0)
        return -1;
    if (libutil_init() != 0)
        return -1;
    if (libxml2_init() != 0)
        return -1;
    if (libxslt_init() != 0)
        return -1;
    if (libz_init() != 0)
        return -1;
    return 0;
}

int lib_shutdown_all(void)
{
    return lib_core_shutdown();
}
