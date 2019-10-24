/**
 * @file	MessageChannelStrategy.hpp
 * @brief	MessageChannelStrategy
 * @details	
 *
 * @author	xxx
 * @author	<xxx@xxx.com>
 * @copyright	(c) 2012 xxx All rights reserved.
 **/

#ifndef _ELASTOS_MESSAGE_CHANNEL_STRATEGY_HPP_
#define _ELASTOS_MESSAGE_CHANNEL_STRATEGY_HPP_

#include <cstdint>
#include <HumanInfo.hpp>
#include <FriendInfo.hpp>

namespace elastos {

class MessageChannelStrategy {
public:
    /*** type define ***/
    class ChannelListener {
    public:
        enum class ChannelStatus : uint32_t {
            Online        = 0x00000001,
            Offline       = 0x00000002,
            Pending       = 0x00000004,
        };

        explicit ChannelListener() = default;
        virtual ~ChannelListener() = default;

        virtual void onStatusChanged(const std::string& userCode,
                                     uint32_t channelType,
                                     ChannelStatus status) = 0;

        virtual void onReceivedMessage(const std::string& friendCode,
                                       uint32_t channelType,
                                       const std::vector<uint8_t>& msgContent) = 0;
        virtual void onSentMessage(int msgIndex, int errCode) = 0;

        virtual void onFriendRequest(const std::string& friendCode,
                                     uint32_t channelType,
                                     const std::string& summary) = 0;

        virtual void onFriendStatusChanged(const std::string& friendCode,
                                           uint32_t channelType,
                                           ChannelStatus status) = 0;

        virtual int onReadData(const std::string& friendCode,
                               uint32_t channelType,
                               const std::string& dataId,
                               uint64_t offset,
                               std::vector<uint8_t>& data) = 0;

//        virtual int onWriteData(const std::string& friendCode,
//                                   uint32_t channelType,
//                                   const std::string& dataId,
//                                   const std::vector<uint8_t>& data) = 0;
    };

//    struct FileInfo {
//    public:
//        std::string mName;
//        uint64_t mSize;
//        std::string mMd5;
//    protected:
//        explicit FileInfo(const std::string& name,
//                          uint64_t size,
//                          const std::string& md5);
//        explicit FileInfo() = default;
//        virtual ~FileInfo() = default;
//    };

    /*** static function and variable ***/

    /*** class function and variable ***/
    virtual int preset(const std::string& profile) = 0;
    virtual int open() = 0;
    virtual int close() = 0;

    virtual int getAddress(std::string& address) = 0;

    virtual bool isReady() = 0;

    virtual int requestFriend(const std::string& friendAddr,
                              const std::string& summary,
                              bool remoteRequest = true,
                              bool forceRequest = false) = 0;

    virtual int removeFriend(const std::string& friendAddr) = 0;

    virtual int sendMessage(const std::string& friendCode,
                            std::vector<uint8_t> msgContent) = 0;

//    virtual int sendFile(const std::string& friendCode,
//                         const std::shared_ptr<FileInfo> fileInfo) {
//        return ErrCode::UnimplementedError;
//    }
    virtual int sendData(const std::string& friendCode,
                         const std::string& dataId) {
        return ErrCode::UnimplementedError;
    }

protected:
    /*** type define ***/

    /*** static function and variable ***/

    /*** class function and variable ***/
    explicit MessageChannelStrategy(uint32_t chType,
                                    std::shared_ptr<ChannelListener> listener);
    virtual ~MessageChannelStrategy();

    uint32_t mChannelType;
    std::shared_ptr<ChannelListener> mChannelListener;

}; // class MessageChannelStrategy

} // namespace elastos

#endif /* _ELASTOS_MESSAGE_CHANNEL_STRATEGY_HPP_ */
