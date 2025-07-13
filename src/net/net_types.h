#ifndef NET_TYPES_H
#define NET_TYPES_H

#include <array>
#include <cstdint>

namespace NetTypes {

    const uint32_t INPUT_SIZE = 64;
    const uint32_t GAME_SIZE = 512;
    typedef std::array<uint8_t,INPUT_SIZE> Input;
    typedef std::array<uint8_t,GAME_SIZE> Game;

    struct RecentInputMessage {
        uint32_t frame;
        Input in;
    };

    struct MissedFrameRequest {
        uint32_t frame;
    };

    struct MissedFrameResponse {
        uint32_t frame;
        Input in;
        Game game;
    };

    typedef std::array<uint8_t,sizeof(RecentInputMessage)> RecentInputMessageSerial;
    typedef std::array<uint8_t,sizeof(MissedFrameRequest)> MissedFrameRequestSerial;
    typedef std::array<uint8_t,sizeof(MissedFrameResponse)> MissedFrameResponseSerial;

    static void RecentInputMessageSerialize(const RecentInputMessage& rim, RecentInputMessageSerial& rims);
    static void MissedFrameRequestSerialize(const MissedFrameRequest& rim, MissedFrameRequestSerial& rims);
    static void MissedFrameResponseSerialize(const MissedFrameResponse& rim, MissedFrameResponseSerial& rims);
    static void RecentInputMessageDeserialize(RecentInputMessage& rim, const RecentInputMessageSerial& rims);
    static void MissedFrameRequestDeserialize(MissedFrameRequest& rim, const MissedFrameRequestSerial& rims);
    static void MissedFrameResponseDeserialize(MissedFrameResponse& rim, const MissedFrameResponseSerial& rims);

    // returns size of x, writes into addr
    static uint32_t writeNetworkSafeSize(uint8_t* addr, const uint32_t& x);

    // returns size of x, writes into x
    static uint32_t readNetworkSafeSize(const uint8_t* addr, uint32_t& x);

}


#endif //NET_TYPES_H