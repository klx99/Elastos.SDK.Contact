//
//  Elastos.SDK.Contact.cpp
//
//  Created by mengxk on 19/03/16.
//  Copyright © 2016 mengxk. All rights reserved.
//

#include <ContactListener.hpp>

#include <ContactListener.proxy.hpp>
#include "Log.hpp"

namespace crosspl {
namespace native {
    
/***********************************************/
/***** static variables initialize *************/
/***********************************************/
ContactListener* ContactListener::sContactListenerInstance = nullptr;

/***********************************************/
/***** static function implement ***************/
/***********************************************/

/***********************************************/
/***** class public function implement  ********/
/***********************************************/
ContactListener::ContactListener()
        : mSecurityListener()
        , mMessageListener()
{
    Log::I(Log::TAG, "%s", __PRETTY_FUNCTION__);

    sContactListenerInstance = this;

    mSecurityListener = makeSecurityListener();
    mMessageListener = makeMessageListener();
}
ContactListener::~ContactListener()
{
    Log::I(Log::TAG, "%s", __PRETTY_FUNCTION__);
    if(sContactListenerInstance == this) {
        sContactListenerInstance = nullptr;
    }
}

void ContactListener::onError(int errCode, const std::string& errStr, const std::string& ext)
{
    int64_t platformHandle = getPlatformHandle();
    crosspl::proxy::ContactListener::onError(platformHandle,
                                             errCode, errStr.c_str(), ext.c_str());
    return;
}

std::shared_ptr<elastos::SecurityManager::SecurityListener> ContactListener::getSecurityListener()
{
    return mSecurityListener;
}
std::shared_ptr<elastos::MessageManager::MessageListener> ContactListener::getMessageListener()
{
    return mMessageListener;
}

/***********************************************/
/***** class protected function implement  *****/
/***********************************************/


/***********************************************/
/***** class private function implement  *******/
/***********************************************/
std::shared_ptr<elastos::SecurityManager::SecurityListener> ContactListener::makeSecurityListener()
{
    class SecurityListener final : public elastos::SecurityManager::SecurityListener {
    public:
        explicit SecurityListener() = default;
        virtual ~SecurityListener() = default;

        std::string onAcquirePublicKey() override {
            Log::I(Log::TAG, "%s", __PRETTY_FUNCTION__);
            if(mCachedPublicKey.empty() == false) {
                return mCachedPublicKey;
            }

            auto ret = sContactListenerInstance->onAcquire(AcquireType::PublicKey, nullptr, nullptr);
            if(ret.get() == nullptr) {
                return "";
            }

            mCachedPublicKey = std::string(reinterpret_cast<char*>(ret->data()), ret->size());
            return mCachedPublicKey;
        }

        std::vector<uint8_t> onEncryptData(const std::string& pubKey, const std::vector<uint8_t>& src) override {
            Log::I(Log::TAG, "%s", __PRETTY_FUNCTION__);
            const std::span<uint8_t> data(const_cast<uint8_t*>(src.data()), src.size());
            auto ret = sContactListenerInstance->onAcquire(AcquireType::EncryptData, pubKey.c_str(), &data);
            if(ret.get() == nullptr) {
                return std::vector<uint8_t>();
            }

            std::vector<uint8_t> cryptoData(ret->data(), ret->data() + ret->size());
            return cryptoData;
        }
        std::vector<uint8_t> onDecryptData(const std::vector<uint8_t>& src) override {
            Log::I(Log::TAG, "%s", __PRETTY_FUNCTION__);
            const std::span<uint8_t> data(const_cast<uint8_t*>(src.data()), src.size());
            auto ret = sContactListenerInstance->onAcquire(AcquireType::DecryptData, nullptr, &data);
            if(ret.get() == nullptr) {
                return std::vector<uint8_t>();
            }

            std::vector<uint8_t> cryptoData(ret->data(), ret->data() + ret->size());
            return cryptoData;
        }

        std::string onAcquireDidPropAppId() override {
            Log::I(Log::TAG, "%s", __PRETTY_FUNCTION__);
            if(mCachedDidPropAppId.empty() == false) {
                return mCachedDidPropAppId;
            }

            auto ret = sContactListenerInstance->onAcquire(AcquireType::DidPropAppId, nullptr, nullptr);
            if(ret.get() == nullptr) {
                return "";
            }

            mCachedDidPropAppId = std::string(reinterpret_cast<char*>(ret->data()), ret->size());
            return mCachedDidPropAppId;
        }

        std::string onAcquireDidAgentAuthHeader() override {
            Log::I(Log::TAG, "%s", __PRETTY_FUNCTION__);
            auto ret = sContactListenerInstance->onAcquire(AcquireType::DidAgentAuthHeader, nullptr, nullptr);
            if(ret.get() == nullptr) {
                return "";
            }

            std::string authHeader(reinterpret_cast<char*>(ret->data()), ret->size());
            return authHeader;
        }

        std::vector<uint8_t> onSignData(const std::vector<uint8_t>& originData) override {
            Log::I(Log::TAG, "%s", __PRETTY_FUNCTION__);
            const std::span<uint8_t> data(const_cast<uint8_t*>(originData.data()), originData.size());
            auto ret = sContactListenerInstance->onAcquire(AcquireType::SignData, nullptr, &data);
            if(ret.get() == nullptr) {
                return std::vector<uint8_t>();
            }

            std::vector<uint8_t> signedData(ret->data(), ret->data() + ret->size());
            return signedData;
        }

    private:
        std::string mCachedPublicKey;
        std::string mCachedDidPropAppId;
    };

    return std::make_shared<SecurityListener>();
}

std::shared_ptr<elastos::MessageManager::MessageListener> ContactListener::makeMessageListener()
{
    class MessageListener final : public elastos::MessageManager::MessageListener {
    public:
        explicit MessageListener() = default;
        virtual ~MessageListener() = default;

        virtual void onStatusChanged(std::shared_ptr<elastos::UserInfo> userInfo,
                                     elastos::MessageManager::ChannelType channelType,
                                     elastos::UserInfo::Status status) override {
            Log::I(Log::TAG, "%s", __PRETTY_FUNCTION__);
            std::string humanCode;
            int ret = userInfo->getHumanCode(humanCode);
            CHECK_AND_NOTIFY_RETVAL(ret);

            std::span<uint8_t> data {reinterpret_cast<uint8_t*>(&status), 1 };
            sContactListenerInstance->onEvent(EventType::StatusChanged, humanCode,
                                              static_cast<ContactChannel>(channelType), &data);
        }

        virtual void onReceivedMessage(std::shared_ptr<elastos::HumanInfo> humanInfo,
                                       elastos::MessageManager::ChannelType channelType,
                                       const std::shared_ptr<elastos::MessageManager::MessageInfo> msgInfo) override {
            Log::I(Log::TAG, "%s", __PRETTY_FUNCTION__);
            std::string humanCode;
            int ret = humanInfo->getHumanCode(humanCode);
            CHECK_AND_NOTIFY_RETVAL(ret);

            sContactListenerInstance->onReceivedMessage(humanCode, static_cast<ContactChannel>(channelType), msgInfo);
        }

        virtual void onSentMessage(int msgIndex, int errCode) override {
            Log::I(Log::TAG, "%s", __PRETTY_FUNCTION__);
        }

        virtual void onFriendRequest(std::shared_ptr<elastos::FriendInfo> friendInfo,
                                     elastos::MessageManager::ChannelType channelType,
                                     const std::string& summary) override {
            Log::I(Log::TAG, "%s", __PRETTY_FUNCTION__);
            std::string humanCode;
            int ret = friendInfo->getHumanCode(humanCode);
            CHECK_AND_NOTIFY_RETVAL(ret);

            std::span<uint8_t> data {reinterpret_cast<uint8_t*>(const_cast<char*>(summary.c_str())),
                                     summary.length() };
            sContactListenerInstance->onEvent(EventType::FriendReuqest, humanCode,
                                              static_cast<ContactChannel>(channelType), &data);
        }

        virtual void onFriendStatusChanged(std::shared_ptr<elastos::FriendInfo> friendInfo,
                                           elastos::MessageManager::ChannelType channelType,
                                           elastos::FriendInfo::Status status) override {
            Log::I(Log::TAG, "%s", __PRETTY_FUNCTION__);
            std::string humanCode;
            int ret = friendInfo->getHumanCode(humanCode);
            CHECK_AND_NOTIFY_RETVAL(ret);

            std::span<uint8_t> data {reinterpret_cast<uint8_t*>(&status), 1 };
            sContactListenerInstance->onEvent(EventType::StatusChanged, humanCode,
                                              static_cast<ContactChannel>(channelType), &data);
        }

        virtual void onHumanInfoChanged(std::shared_ptr<elastos::HumanInfo> humanInfo,
                                        elastos::MessageManager::ChannelType channelType) override {
            Log::I(Log::TAG, "%s", __PRETTY_FUNCTION__);
            std::string humanCode;
            int ret = humanInfo->getHumanCode(humanCode);
            CHECK_AND_NOTIFY_RETVAL(ret);

            auto jsonInfo = std::make_shared<elastos::Json>();
            ret = humanInfo->toJson(jsonInfo);
            CHECK_AND_NOTIFY_RETVAL(ret);
            std::string info = jsonInfo->dump();

            std::span<uint8_t> data(reinterpret_cast<uint8_t*>(info.data()), info.size());
            sContactListenerInstance->onEvent(EventType::HumanInfoChanged, humanCode,
                                              static_cast<ContactChannel>(channelType), &data);
        }
    };

    return std::make_shared<MessageListener>();
}

std::shared_ptr<std::span<uint8_t>> ContactListener::onAcquire(AcquireType type,
                                                              const char* pubKey,
                                                              const std::span<uint8_t>* data)
{
    int64_t platformHandle = getPlatformHandle();
    auto ret = crosspl::proxy::ContactListener::onAcquire(platformHandle,
                                                          static_cast<int>(type), pubKey, data);

    return ret;
}

void ContactListener::onEvent(EventType type,
                              const std::string& humanCode,
                              ContactChannel channelType,
                              const std::span<uint8_t>* data)
{
    int64_t platformHandle = getPlatformHandle();
    crosspl::proxy::ContactListener::onEvent(platformHandle,
                                             static_cast<int>(type), humanCode.c_str(), static_cast<int>(channelType), data);
    return;
}

void ContactListener::onReceivedMessage(const std::string& humanCode, ContactChannel channelType,
                                        std::shared_ptr<elastos::MessageManager::MessageInfo> msgInfo)
{
    std::span<uint8_t> data(msgInfo->mPlainContent.data(), msgInfo->mPlainContent.size());

    int64_t platformHandle = getPlatformHandle();
    crosspl::proxy::ContactListener::onReceivedMessage(platformHandle,
                                                       humanCode.c_str(), static_cast<int>(channelType),
                                                       static_cast<int>(msgInfo->mType),
                                                       &data,
                                                       msgInfo->mCryptoAlgorithm.c_str(),
                                                       msgInfo->mTimeStamp);
    return;
}

} //namespace native
} //namespace crosspl
