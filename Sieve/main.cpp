//
//  main.cpp
//  Sieve of Eratosthenes
//
//  Created by BlueCocoa on 16/1/27.
//  Copyright © 2016 BlueCocoa. All rights reserved.
//

#include <string.h>
#include <iostream>
#include <cmath>
#include <functional>
#include <time.h>
#include <sys/time.h>
#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#endif

using namespace std;

#define CHAR_BIT_LOG 3
#define MASK (~(~0 << CHAR_BIT_LOG))
#define setbit(a, x) ((a)[(x) >> CHAR_BIT_LOG] |= 1 << ((x) & MASK))
#define isset(a, x) ((a)[(x) >> CHAR_BIT_LOG] & (1 << ((x) & MASK)))

char * sieve(unsigned long long n) {
    char * result = NULL;
    
    if (n >= 2) {
        // 计算需要多少个字节, 检查(n & 0xF)确保有足够的位
        size_t length = n >> CHAR_BIT_LOG;
        if (n & 0xF) length += 1;
        
        result = (char *)malloc(length);
        memset(result, 0, length);
        
        for (int i = 2; i <= (int)floor(sqrt(n)); ++i) {
            if (!isset(result, i)) {
                int j = i*i;
                while (j <= n) {
                    setbit(result, j);
                    j += i;
                }
            }
        }
    }
    
    return result;
}

char * sieve2(unsigned long long n) {
    char * result = NULL;
    
    if (n >= 2) {
        // 计算需要多少个字节, 检查(n & 0xF)以确保有足够的位
        size_t length = n >> CHAR_BIT_LOG;
        if (n & 0xF) length += 1;
        
        // 只保存奇数
        length = (size_t)ceil(length / 2.0);
        
        result = (char *)malloc(length);
        memset(result, 0, length);
        
        for (int i = 3; i <= (int)floor(sqrt(n)); i+=2) {
            if (!isset(result, (i - 2) >> 1)) {
                for (int j = i * i; j <= n; j += (i << 1)) {
                    // 因为i一定是奇数, 而j从i*i开始, 那么j也一定是奇数
                    // 又因为 奇数+奇数=偶数, 所以我们直接加上2*i来确保得到奇数
                    setbit(result, (j - 2) >> 1);
                }
            }
        }
    }
    
    return result;
}

template <class Return, class Param>
auto runtime(function<Return(Param)> func, Param param) -> Return {
    Return result;
    
    struct timespec start = {0, 0};
    struct timespec end = {0, 0};
    
#ifdef __MACH__
    clock_serv_t cclock;
    mach_timespec_t mts;
    host_get_clock_service(mach_host_self(), REALTIME_CLOCK, &cclock);
    clock_get_time(cclock, &mts);
    mach_port_deallocate(mach_task_self(), cclock);
    start.tv_sec = mts.tv_sec;
    start.tv_nsec = mts.tv_nsec;
#else
    clock_gettime(CLOCK_REALTIME, &start);
#endif
    
    // execute
    result = func(param);
    
#ifdef __MACH__
    host_get_clock_service(mach_host_self(), REALTIME_CLOCK, &cclock);
    clock_get_time(cclock, &mts);
    mach_port_deallocate(mach_task_self(), cclock);
    end.tv_sec = mts.tv_sec;
    end.tv_nsec = mts.tv_nsec;
#else
    clock_gettime(CLOCK_REALTIME, &start);
#endif
    
    printf("%ld ns\n", (end.tv_sec-start.tv_sec)*1000*1000000+(end.tv_nsec-start.tv_nsec));
    printf("%lf ms\n", (end.tv_sec-start.tv_sec)*1000+(end.tv_nsec-start.tv_nsec)/1000000.0);
    return result;
}

int main(int argc, const char * argv[]) {
    unsigned long long upperbound = 4000;
    
    char * result = runtime<char *, unsigned long long>(sieve, upperbound);
    
    for (int i = 3; i < upperbound; i++) {
        if (!isset(result, i)) {
            printf("%d ", i);
        }
    }
    printf("\n");
    free(result);
    
    result = runtime<char *, unsigned long long>(sieve2, upperbound);
    for (int i = 0; i < upperbound / 2; i++) {
        size_t prime = ((i<<1) + 3);
        if (!isset(result, i) && (prime < upperbound)) {
            printf("%zu ", prime);
        }
    }
    printf("\n");
    
    free(result);
    return 0;
}
