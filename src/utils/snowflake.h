/*************************************************************************
 * SnowFlake算法，生成的共64位，用uint64即可表示，各位情况说明：
 * 第1位（不提供调整）：
 *		二进制中最高位为1的都是负数，但是我们生成的id一般都使用整数，所以这个最高位固定是0
 * 2-42位（41位，本库可调整）：
 *		用来记录时间戳（毫秒）。
 *		41位可以表示2^41−1个数字，如果只用来表示正整数（计算机中正数包含0）
 *		可以表示的数值范围是：0 至 2^41−1，减1是因为可表示的数值范围是从0开始算的，而不是1。
 *		也就是说41位可以表示2^41−1个毫秒的值，转化成单位年则是(2^41−1)/(1000∗60∗60∗24∗365)=69年
 * 43-52位（10位，本库可调整）：
 *		用来记录工作机器id，可以部署在210=1024个节点，包括5位datacenterId和5位workerId
 *		5位（bit）可以表示的最大正整数是25−1=31，即可以用0、1、2、3、....31这32个数字，
 *		来表示不同的datecenterId或workerId
 * 53-64位（12位，本库可调整）：
 *		序列号，用来记录同毫秒内产生的不同id。12位（bit）可以表示的最大正整数是212−1=4096
 *		即可以用0、1、2、3、....4095这4096个数字，来表示同一机器同一时间截（毫秒)内产生的4096个ID序号
 *
 * File:	snowflake.h
 * Author:	liuyongshuai<liuyongshuai@hotmail.com>
 * Time:	2019年03月23日 星期六 18时13分24秒
 ************************************************************************/

#ifndef _UTILS_SNOWFLAKE_H_
#define _UTILS_SNOWFLAKE_H_

#include <stdint.h>
#include <vector>
#include <pthread.h>
#include <string>

using namespace std;

namespace cpputils{
    class SnowFlake{
    public:
        void setWorkerId(int64_t workerId);

        void setTimeBitSize(uint8_t sz);

        void setWorkerIdBitSize(uint8_t sz);

        void setSequenceBitSize(uint8_t sz);

        //生成下一个ID
        int64_t genId();

        SnowFlake();

        SnowFlake(int64_t workerId, uint8_t timeBit, uint8_t workerIdBit, uint8_t seqBit);

    private:
        pthread_mutex_t *mutex;
        int64_t workerId;//当前的workerId
        int64_t workerIdAfterShift;//移位后的workerId，可直接跟时间戳、序号取位或操作
        int64_t lastMsTimestamp;//上一次用的时间戳
        int64_t curSequence;//当前的序号

        uint8_t timeBitSize;//时间戳占的位数，默认为41位，最大不超过60位
        uint8_t workerIdBitSize;//workerId占的位数，默认10，最大不超过60位
        uint8_t sequenceBitSize;//序号占的位数，默认12，最大不超过60位

        bool isHaveInit;//是否已经初始化了

        int64_t maxWorkerId;//workerId的最大值，初始化时计算出来的
        int64_t maxSequence;//最后序列号最大值，初始化时计算出来的
        int64_t workerIdLeftShift;//生成的workerId只取最低的几位，这里要左移，给序列号腾位，初始化时计算出来的
        int64_t timestampLeftShift;//生成的时间戳左移几位，给workId、序列号腾位，初始化时计算出来的

        //初始化函数
        void init();

        //生成时间戳，根据bit size设置取几位
        int64_t genTs();

        //生成下一个时间戳，如果时间戳的位数较小，且序号用完时此处等待的时间会较长
        int64_t genNextTs(int64_t last);
    };
}


#endif // _UTILS_SNOWFLAKE_H_
