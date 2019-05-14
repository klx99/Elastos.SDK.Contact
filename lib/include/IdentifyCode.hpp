#ifndef _ELASTOS_IDENTIFY_CODE_HPP_
#define _ELASTOS_IDENTIFY_CODE_HPP_

#include <string>

#include "ErrCode.hpp"

namespace elastos {

class IdentifyCode {
public:
    /*** type define ***/
    enum Type {
        Did = 1,
        PhoneNumber,
        EmailAddress,
        WechatId,
    };

    /*** static function and variable ***/

    /*** class function and variable ***/
    explicit IdentifyCode(const std::string& phoneNumber,
                          const std::string& emailAddress,
                          const std::string& wechatId);
    explicit IdentifyCode();
    virtual ~IdentifyCode();

    int set(Type type, const std::string& value);
    std::string get(Type type) const;

private:
    /*** type define ***/

    /*** static function and variable ***/

    /*** class function and variable ***/
    std::string mPhoneNumber;
    std::string mEmailAddress;
    std::string mWechatId;
}; // class IdentifyCode

} // namespace elastos

#endif /* _ELASTOS_IDENTIFY_CODE_HPP_ */