#ifndef REST_Version
#define REST_Version
/* Version information automatically generated by installer. *//*
 * These macros can be used in the following way:
 * 
 * #if REST_VERSION_CODE >= REST_VERSION(2,23,4)
 *     #include <newheader.h>
 * #else
 *     #include <oldheader.h>
 * #endif
 *
 */
#define REST_RELEASE "2.1.8"
#define REST_RELEASE_DATE "2018-10-08"
#define REST_RELEASE_TIME "15:18:48 +0200"
#define REST_GIT_COMMIT "f0a1ce93"
#define REST_GIT_BRANCH "V2.2_merging"
#define REST_GIT_TAG "2.1.8"
#define REST_VERSION_CODE 131336
#define REST_VERSION(a,b,c) (((a) << 16) + ((b) << 8) + (c))
#endif
