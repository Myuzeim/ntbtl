#include "net_types.h"
#include <cstring>

namespace NetTypes {

    void RecentInputMessageSerialize(const RecentInputMessage& rim, RecentInputMessageSerial& rims) {
        size_t index = writeNetworkSafeSize(&rims[0],rim.frame);
        memcpy(&rims[index],&rim.in,sizeof(rim.in));
    };

    void MissedFrameRequestSerialize(const MissedFrameRequest& rim, MissedFrameRequestSerial& rims) {
        writeNetworkSafeSize(&rims[0],rim.frame);
    };
    
    void MissedFrameResponseSerialize(const MissedFrameResponse& rim, MissedFrameResponseSerial& rims) {
        size_t index = writeNetworkSafeSize(&rims[0],rim.frame);
        memcpy(&rims[index],&rim.in,sizeof(rim.in));
        index += sizeof(rim.in);
        memcpy(&rims[index],&rim.game,sizeof(rim.game));
    };
    
    void RecentInputMessageDeserialize(RecentInputMessage& rim, const RecentInputMessageSerial& rims) {
        size_t index = readNetworkSafeSize(&rims[0],rim.frame);
        memcpy(&rim.in,&rims[index],sizeof(rim.in));
    };
    
    void MissedFrameRequestDeserialize(MissedFrameRequest& rim, const MissedFrameRequestSerial& rims) {
        size_t index = readNetworkSafeSize(&rims[0],rim.frame);
    };
    
    void MissedFrameResponseDeserialize(MissedFrameResponse& rim, const MissedFrameResponseSerial& rims) {
        size_t index = readNetworkSafeSize(&rims[0],rim.frame);
        memcpy(&rim.in,&rims[index],sizeof(rim.in));
        index += sizeof(rim.in);
        memcpy(&rim.game,&rims[index],sizeof(rim.game));
    };

    
    uint32_t writeNetworkSafeSize(uint8_t* addr, const uint32_t& x) {
        uint32_t reg = x & 0xFF000000;
        reg >>= 24;
        uint8_t shortReg = reg;
        memcpy(&addr[0],&shortReg,sizeof(shortReg));
        
        reg = x & 0x00FF0000;
        reg >>= 16;
        shortReg = reg;
        memcpy(&addr[1],&shortReg,sizeof(shortReg));

        reg = x & 0x0000FF00;
        reg >>= 8;
        shortReg = reg;
        memcpy(&addr[2],&shortReg,sizeof(shortReg));

        reg = x & 0x000000FF;
        shortReg = reg;
        memcpy(&addr[3],&shortReg,sizeof(shortReg));
        return sizeof(x);
    };
    
    static uint32_t readNetworkSafeSize(const uint8_t* addr, uint32_t& x) {
        x = 0;
        x += addr[0];
        x <<= 8;
        x += addr[1];
        x <<= 8;
        x += addr[2];
        x <<= 8;
        x += addr[3];
        return sizeof(x);
    };
};