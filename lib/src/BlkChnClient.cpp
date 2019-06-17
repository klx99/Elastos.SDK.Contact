#include <BlkChnClient.hpp>

#include <iomanip>
#include <HttpClient.hpp>
#include <Log.hpp>
#include <MD5.hpp>
#include <SafePtr.hpp>


namespace elastos {

/* =========================================== */
/* === static variables initialize =========== */
/* =========================================== */
std::shared_ptr<BlkChnClient> BlkChnClient::gBlkChnClient {};

/* =========================================== */
/* === static function implement ============= */
/* =========================================== */
int BlkChnClient::InitInstance(std::weak_ptr<Config> config, std::weak_ptr<SecurityManager> sectyMgr)
{
    if(gBlkChnClient.get() != nullptr) {
        return 0;
    }

    struct Impl: BlkChnClient {
        Impl(std::weak_ptr<Config> config, std::weak_ptr<SecurityManager> sectyMgr) : BlkChnClient(config, sectyMgr) {
        }
    };

    HttpClient::InitGlobal();
    gBlkChnClient = std::make_shared<Impl>(config, sectyMgr);

    return 0;
}

std::shared_ptr<BlkChnClient> BlkChnClient::GetInstance()
{
    assert(gBlkChnClient.get() != nullptr);
    return gBlkChnClient;
}

/* =========================================== */
/* === class public function implement  ====== */
/* =========================================== */
int BlkChnClient::setConnectTimeout(uint32_t milliSecond)
{
    mConnectTimeoutMS = milliSecond;
    return 0;
}

int BlkChnClient::getDidProps(const std::string& did, std::map<std::string, std::string>& propMap)
{
    auto sectyMgr = SAFE_GET_PTR(mSecurityManager);
    auto config = SAFE_GET_PTR(mConfig);

    std::string didPath;
    int ret = getDidPropPath(didPath);
    if(ret < 0) {
        return ret;
    }

    auto didConfigUrl = config->mDidChainConfig->mUrl;
    auto agentGetProps = config->mDidChainConfig->mAgentApi.mGetDidProps;
    std::string agentGetPropsUrl = didConfigUrl + agentGetProps + did;

    HttpClient httpClient;
    httpClient.url(agentGetPropsUrl);
    ret = httpClient.syncGet();
    if(ret < 0) {
        return ErrCode::HttpClientError + ret;
    }

    std::string respBody;
    ret = httpClient.getResponseBody(respBody);
    if(ret < 0) {
        return ErrCode::HttpClientError + ret;
    }
    Log::I(Log::TAG, "respBody=%s", respBody.c_str());

    Json jsonResp = Json::parse(respBody);
    if(jsonResp["status"] != 200) {
        return ErrCode::BlkChnGetPropError;
    }
    std::string result = jsonResp["result"];
    if(result.empty() == true) {
        return ErrCode::BlkChnEmptyPropError;
    }

    Json jsonPropArray = Json::parse(result);
    for(const auto& it: jsonPropArray){
        std::string propKey = it["key"];
        std::string propValue = it["value"];

        size_t pos = propKey.find(didPath);
        if (pos != std::string::npos) {
            propKey.erase(pos, didPath.length());
        }

        propMap[propKey] = propValue;
    }

    return 0;
}

int BlkChnClient::uploadDidProps(const std::map<std::string, std::string>& propMap)
{
    auto sectyMgr = SAFE_GET_PTR(mSecurityManager);
    auto config = SAFE_GET_PTR(mConfig);

    std::string didPath;
    int ret = getDidPropPath(didPath);
    if(ret < 0) {
        return ret;
    }

    Json jsonPropProt = Json::object();
    Json jsonPropArray = Json::array();
    for(const auto& prop: propMap) {
        std::string propKey = didPath + prop.first;
        std::string propValue = prop.second;
        Json jsonProp = Json::object();
        jsonProp[DidProtocol::Name::Key] = propKey;
        jsonProp[DidProtocol::Name::Value] = propValue;
        if(prop.second.empty() == true) {
            jsonProp[DidProtocol::Name::Status] = DidProtocol::Value::Status::Deprecated;
        } else {
            jsonProp[DidProtocol::Name::Status] = DidProtocol::Value::Status::Normal;
        }

        jsonPropArray.push_back(jsonProp);
    }

    jsonPropProt[DidProtocol::Name::Tag] = DidProtocol::Value::Tag;
    jsonPropProt[DidProtocol::Name::Ver] = DidProtocol::Value::Ver;
    jsonPropProt[DidProtocol::Name::Status] = DidProtocol::Value::Status::Normal;
    jsonPropProt[DidProtocol::Name::Properties] = jsonPropArray;

    auto propProtStr = jsonPropProt.dump();
    Log::I(Log::TAG, "BlkChnClient::uploadDidProps() propProt: %s", propProtStr.c_str());
    std::vector<uint8_t> originBytes(propProtStr.begin(), propProtStr.end());
    std::vector<uint8_t> signedBytes(propProtStr.begin(), propProtStr.end());
    ret = sectyMgr->signData(originBytes, signedBytes);
    if(ret < 0) {
        return ret;
    }

    std::string pubKey;
    ret = sectyMgr->getPublicKey(pubKey);
    if(ret < 0) {
        return ret;
    }

    std::string msgStr = MD5::MakeHexString(originBytes);
    std::string sigStr = MD5::MakeHexString(signedBytes);

    // did prop key, sign, make {msg, sig, pub}
    std::string reqBody = std::string("{")
        + "\"pub\":\"" + pubKey + "\","
        + "\"msg\":\"" + msgStr + "\","
        + "\"sig\":\"" + sigStr + "\""
        + "}";

    auto didConfigUrl = config->mDidChainConfig->mUrl;
    auto agentUploadPath = config->mDidChainConfig->mAgentApi.mUploadDidProps;
    std::string agentUploadUrl = didConfigUrl + agentUploadPath;
    std::string authHeader;
    ret = sectyMgr->getDidAgentAuthHeader(authHeader);
    if(ret < 0) {
        return ret;
    }
    Log::I(Log::TAG, "reqBody=%s", reqBody.c_str());

    HttpClient httpClient;
    httpClient.url(agentUploadUrl);
    httpClient.setHeader("Content-Type", "application/json");
    httpClient.setHeader("X-Elastos-Agent-Auth", authHeader);
    ret = httpClient.syncPost(reqBody);
    if(ret < 0) {
        return ErrCode::HttpClientError + ret;
    }

    std::string respBody;
    ret = httpClient.getResponseBody(respBody);
    if(ret < 0) {
        return ErrCode::HttpClientError + ret;
    }
    Log::I(Log::TAG, "respBody=%s", respBody.c_str());

    Json jsonResp = Json::parse(respBody);
    if(jsonResp["status"] != 200) {
        return ErrCode::BlkChnSetPropError;
    }

    return 0;
}

/* =========================================== */
/* === class protected function implement  === */
/* =========================================== */


/* =========================================== */
/* === class private function implement  ===== */
/* =========================================== */
BlkChnClient::BlkChnClient(std::weak_ptr<Config> config, std::weak_ptr<SecurityManager> sectyMgr)
    : mConfig(config)
    , mSecurityManager(sectyMgr)
    , mTaskThread()
    , mConnectTimeoutMS(10000)
{
}

BlkChnClient::~BlkChnClient()
{
}

int BlkChnClient::getDidPropPath(std::string& didPath)
{
    auto sectyMgr = SAFE_GET_PTR(mSecurityManager);

    std::string appId;
    int ret = sectyMgr->getDidPropAppId(appId);
    if(ret < 0) {
        return ret;
    }

    didPath = "Apps/" + appId + "/";
    return 0;
}

} // namespace elastos