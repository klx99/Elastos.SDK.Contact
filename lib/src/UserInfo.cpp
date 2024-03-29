//
//  UserInfo.cpp
//
//  Created by mengxk on 19/03/16.
//  Copyright © 2016 mengxk. All rights reserved.
//

#include <UserInfo.hpp>
#include <UserManager.hpp>

#include <Log.hpp>
#include <JsonDefine.hpp>
#include <Platform.hpp>
#include <SafePtr.hpp>
#include <DateTime.hpp>

namespace elastos {

/***********************************************/
/***** static variables initialize *************/
/***********************************************/

/***********************************************/
/***** static function implement ***************/
/***********************************************/

/***********************************************/
/***** class public function implement  ********/
/***********************************************/
UserInfo::UserInfo(std::weak_ptr<UserManager> userMgr)
    : HumanInfo()
    , IdentifyCode()
    , mUserManager(userMgr)
{
}

UserInfo::~UserInfo()
{
}

// int UserInfo::serialize(const CarrierInfo& info, std::string& value) const
// {
//     int ret = HumanInfo::serialize(info, value);
//     return ret;
// }

// int UserInfo::deserialize(const std::string& value, CarrierInfo& info) const
// {
//     int ret = HumanInfo::deserialize(value, info);
//     return ret;
// }

int UserInfo::serialize(std::string& value, bool summaryOnly) const
{
    Json jsonInfo = Json::object();

    std::string humanInfo;
    int ret = HumanInfo::serialize(humanInfo, summaryOnly);
    CHECK_ERROR(ret)
    jsonInfo[JsonKey::HumanInfo] = humanInfo;

    if(summaryOnly == false) {
        std::string identCode;
        ret = IdentifyCode::serialize(identCode);
        CHECK_ERROR(ret)
        jsonInfo[JsonKey::IdentifyCode] = identCode;
    }

    value = jsonInfo.dump();

    return 0;
}

int UserInfo::deserialize(const std::string& value, bool summaryOnly)
{
    Json jsonInfo= Json::parse(value);

    std::string humanInfo = jsonInfo[JsonKey::HumanInfo];
    int ret = HumanInfo::deserialize(humanInfo, summaryOnly);
    CHECK_ERROR(ret)

    if(summaryOnly == false) {
        std::string identCode = jsonInfo[JsonKey::IdentifyCode];
        ret = IdentifyCode::deserialize(identCode);
        CHECK_ERROR(ret)
    }

    return 0;

}

int UserInfo::toJson(std::shared_ptr<Json>& value) const
{
    if(value.get() == nullptr) {
        return ErrCode::InvalidArgument;
    }

    int ret = HumanInfo::toJson(value);
    CHECK_ERROR(ret)

    ret = IdentifyCode::toJson(value);
    CHECK_ERROR(ret)

    (*value)[JsonKey::IsMyself] = true;

    return 0;
}

int UserInfo::addCarrierInfo(const CarrierInfo& info, const Status status)
{
    int ret = HumanInfo::addCarrierInfo(info, status);
    if(ret < 0) { // error or not changed
        return ret;
    }

    auto userMgr = SAFE_GET_PTR(mUserManager);
    ret = userMgr->saveLocalData();
    CHECK_ERROR(ret)

    return 0;
}

int UserInfo::setHumanInfo(Item item, const std::string& value)
{
    int ret = HumanInfo::setHumanInfo(item, value);
    Log::D(Log::TAG, "%s ret=%d", __PRETTY_FUNCTION__, ret);
    if(ret <= 0) { // error or not changed
        return ret;
    }

    auto userMgr = SAFE_GET_PTR(mUserManager);
    ret = userMgr->saveLocalData();
    CHECK_ERROR(ret)

    return 0;
}

int UserInfo::mergeHumanInfo(const HumanInfo& value, const Status status)
{
    int ret = HumanInfo::mergeHumanInfo(value, status);
    Log::D(Log::TAG, "%s ret=%d", __PRETTY_FUNCTION__, ret);
    if(ret < 0) { // error or not changed
        return ret;
    }

    auto userMgr = SAFE_GET_PTR(mUserManager);
    ret = userMgr->saveLocalData();
    CHECK_ERROR(ret)

    return 0;
}

int UserInfo::setWalletAddress(const std::string& name, const std::string& value)
{
    if(name.empty() == true) {
        return ErrCode::InvalidArgument;
    }

    mWalletAddressMap[name] = value;
    HumanInfo::mUpdateTime = DateTime::CurrentMS();

    auto userMgr = SAFE_GET_PTR(mUserManager);
    int ret = userMgr->saveLocalData();
    CHECK_ERROR(ret)

    return 0;
}

int UserInfo::setIdentifyCode(Type type, const std::string& value)
{
    int ret = IdentifyCode::setIdentifyCode(type, value);
    if(ret == 0) { // not changed
        return ret;
    }
    CHECK_ERROR(ret);

    auto userMgr = SAFE_GET_PTR(mUserManager);
    ret = userMgr->saveLocalData();
    CHECK_ERROR(ret)

    return 0;
}

/***********************************************/
/***** class protected function implement  *****/
/***********************************************/


/***********************************************/
/***** class private function implement  *******/
/***********************************************/

} // namespace elastos
