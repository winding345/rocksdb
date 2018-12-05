//
// Created by 张艺文 on 2018/11/29.
//
#pragma once

namespace rocksdb{
#define NVM_DEBUG

#ifdef NVM_DEBUG

#define DBG_PRINT(format, a...) \
    printf("DEBUG: %s %4d %-40s : " format "\n", __FILE__,__LINE__, __FUNCTION__,  ##a)

#define DBG_TRACE() \
    printf("TRACE: %-40s %4d: \n" format, __FUNCTION__, __LINE__)

#else

#define DBG_PRINT(format, a...)
#define DBG_TRACE()

#endif

}//end rocksdb
