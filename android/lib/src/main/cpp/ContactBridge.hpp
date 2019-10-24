/**
 * @file	Contact.hpp
 * @brief	Contact
 * @details	
 *
 * @author	xxx
 * @author	<xxx@xxx.com>
 * @copyright	(c) 2012 xxx All rights reserved.
 **/

#ifndef _ELASTOS_SDK_JNI_CONTACT_BRIDGE_HPP_
#define _ELASTOS_SDK_JNI_CONTACT_BRIDGE_HPP_

#include <sstream>

#include <ContactListener.hpp>
#include <CrossBase.hpp>
#include <Elastos.SDK.Contact.hpp>

class ContactBridge {
public:
    /*** type define ***/

    /*** static function and variable ***/

    /*** class function and variable ***/
    explicit ContactBridge();
    virtual ~ContactBridge();

    void setListener(CrossBase* listener);
    int start();
    int stop();

    int setUserInfo(int item, const char* value);
    int setIdentifyCode(int type, const char* value);

    int getHumanInfo(const char* humanCode, std::stringstream* info);
    int getHumanStatus(const char* humanCode);

    int addFriend(const char* friendCode, const char* summary);
    int removeFriend(const char* friendCode);
    int acceptFriend(const char* friendCode);
    int getFriendList(std::stringstream* info);

    int sendMessage(const char* friendCode, int chType, CrossBase* message);
    int pullData(const char* friendCode, int chType, const char* devId, const char* fileInfo);

    int syncInfoDownloadFromDidChain();
    int syncInfoUploadToDidChain();

    int setWalletAddress(const char* name, const char* value);

private:
    /*** type define ***/

    /*** static function and variable ***/

    /*** class function and variable ***/
    std::shared_ptr<elastos::Contact> mContactImpl;
    ContactListener* mListener;

}; // class Contact

#endif /* _ELASTOS_SDK_JNI_CONTACT_BRIDGE_HPP_ */
