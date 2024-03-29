//
//  Elastos.SDK.Contact.cpp
//
//  Created by mengxk on 19/03/16.
//  Copyright © 2016 mengxk. All rights reserved.
//

#include <ContactMessage.hpp>

#include <Elastos.SDK.Contact.hpp>
#include <Platform.hpp>
#include "Log.hpp"
#include "ContactMessage.hpp"

#ifdef WITH_CROSSPL
namespace crosspl {
namespace native {
#endif // WITH_CROSSPL

/***********************************************/
/***** static variables initialize *************/
/***********************************************/

/***********************************************/
/***** static function implement ***************/
/***********************************************/

/***********************************************/
/***** class public function implement  ********/
/***********************************************/
ContactMessage::ContactMessage()
    : mMessageInfo()
{
    Log::I(Log::TAG, "%s", __PRETTY_FUNCTION__);
}
ContactMessage::~ContactMessage()
{
    Log::I(Log::TAG, "%s", __PRETTY_FUNCTION__);
}

int ContactMessage::syncMessageToNative(int type,
                                        const std::span<uint8_t>* data,
                                        const char* cryptoAlgorithm,
                                        int64_t timestamp)
{
    auto msg = std::vector<uint8_t>(data->data(), data->data() + data->size());
    auto algorithm = (cryptoAlgorithm != nullptr ? cryptoAlgorithm : "");
    mMessageInfo = elastos::MessageManager::MakeMessage(static_cast<elastos::MessageManager::MessageType>(type),
                                                        msg,
                                                        algorithm);
    mMessageInfo->mTimeStamp = timestamp;
    return 0;
}

/***********************************************/
/***** class protected function implement  *****/
/***********************************************/


/***********************************************/
/***** class private function implement  *******/
/***********************************************/

#ifdef WITH_CROSSPL
} //namespace native
} //namespace crosspl
#endif // WITH_CROSSPL
