/* This holds all constants used in the QNX examples */

#ifndef WH_QNX_CFG_H
#define WH_QNX_CFG_H

/* ===========================================
 * TRANSPORT AND COMMUNICATION CONSTANTS
 * =========================================== */

/* Client and Server IDs */
#define WH_QNX_CLIENT_ID 12
#define WH_QNX_SERVER_ID 57
#define WH_QNX_MAX_CLIENT_ID 255

/* TCP Communication */
#define WH_QNX_SERVER_TCP_PORT 23456
#define WH_QNX_SERVER_TCP_IPSTRING "127.0.0.1"

/* Shared Memory Configuration */
#define WH_QNX_SHARED_MEMORY_NAME "wh_example_shm"

/* ===========================================
 * DMA AND BUFFER SIZES
 * =========================================== */

/* Request and Response Buffer Sizes */
#define WH_QNX_REQ_SIZE 1024
#define WH_QNX_RESP_SIZE 1024
#define WH_QNX_DMA_SIZE 8000

/* Data Buffer Sizes */
#define WH_QNX_DATA_BUFFER_SIZE 0x400 /* 1024 bytes */
#define WH_QNX_KEY_BUFFER_SIZE 4096
#define WH_QNX_MAX_LINE_LENGTH 4608 /* 512 + PATH_MAX */

/* ===========================================
 * FILE SYSTEM CONSTANTS
 * =========================================== */

/* File Path Limits */
#define WH_QNX_PATH_MAX 4096
#define WH_QNX_LABEL_SIZE 256

/* ===========================================
 * CRYPTO CONSTANTS
 * =========================================== */

/* RSA Key Sizes */
#define WH_QNX_RSA_MIN_SIZE 1024
#define WH_QNX_RSA_2048_SIZE 2048
#define WH_QNX_RSA_4096_SIZE 4096

/* ECC Key Sizes */
#define WH_QNX_ECC_KEYSIZE 32
#define WH_QNX_FP_MAX_BITS 8192

/* AES Constants */
#define WH_QNX_AES_KEYSIZE 16
#define WH_QNX_AES_TEXTSIZE 16
#define WH_QNX_AES_AUTHSIZE 16
#define WH_QNX_AES_TAGSIZE 16

/* CMAC Constants */
#define WH_QNX_CMAC_TEXTSIZE 1000

/* Key Cache Constants */
#define WH_QNX_KEYCACHE_KEYSIZE 16

/* ===========================================
 * MEMORY AND STORAGE CONSTANTS
 * =========================================== */

/* Flash and RAM Sizes */
#define WH_QNX_FLASH_RAM_SIZE (1024 * 1024) /* 1MB */
#define WH_QNX_STATIC_MEMORY_TEST_SZ 120000

/* NVM Object Count */
#define WH_QNX_NVM_OBJECT_COUNT 30


/* Macros for maximum client ID and key ID */
#define MAX_CLIENT_ID 255
#define MAX_KEY_ID UINT16_MAX

/* Macros for maximum file path length (Linux PATH_MAX is a good reference) */
#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

/* Parameterize MAX_LINE_LENGTH by 512 bytes + MAX_FILE_PATH_LENGTH */
#define MAX_LINE_LENGTH (512 + PATH_MAX)


/* ===========================================
 * STATIC MEMORY ALLOCATION SIZES
 * =========================================== */

/* Static Memory Size List */
#define WH_QNX_STATIC_MEM_LIST_SIZE 9
#define WH_QNX_STATIC_MEM_SIZE_1 176
#define WH_QNX_STATIC_MEM_SIZE_2 1024
#define WH_QNX_STATIC_MEM_SIZE_3 1056
#define WH_QNX_STATIC_MEM_SIZE_4 2048
#define WH_QNX_STATIC_MEM_SIZE_5 4096
#define WH_QNX_STATIC_MEM_SIZE_6 4128
#define WH_QNX_STATIC_MEM_SIZE_7 8192
#define WH_QNX_STATIC_MEM_SIZE_8 8224
#define WH_QNX_STATIC_MEM_SIZE_9 32768

/* Static Memory Distribution List */
#define WH_QNX_STATIC_MEM_DIST_1 10
#define WH_QNX_STATIC_MEM_DIST_2 5
#define WH_QNX_STATIC_MEM_DIST_3 5
#define WH_QNX_STATIC_MEM_DIST_4 5
#define WH_QNX_STATIC_MEM_DIST_5 5
#define WH_QNX_STATIC_MEM_DIST_6 2
#define WH_QNX_STATIC_MEM_DIST_7 2
#define WH_QNX_STATIC_MEM_DIST_8 1
#define WH_QNX_STATIC_MEM_DIST_9 1

#endif /* WH_QNX_CFG_H */
