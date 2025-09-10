#ifndef TLS_CRYPTO_H
#define TLS_CRYPTO_H

#include <string>
#include <openssl/ssl.h>
#include <openssl/err.h>

/**
 * TLS加密解密功能封装类
 * 专注于TLS协议的加密解密功能，不包含网络通信部分
 */
class TLSProxy {
public:
    // 连接类型：客户端或服务器
    enum class Role { CLIENT, SERVER };

    /**
     * 构造函数
     * @param role 角色（客户端或服务器）
     * @param certFile 证书文件路径（服务器必需）
     * @param keyFile 私钥文件路径（服务器必需）
     */
    TLSProxy(Role role, const std::string& certFile = "", const std::string& keyFile = "");
    
    // 析构函数
    ~TLSProxy();

    /**
     * 初始化TLS环境
     * @return 成功返回true，失败返回false
     */
    bool init();

    /**
     * 获取需要发送给对方的握手数据
     * @return 握手数据，如果为空则表示握手失败或已完成
     */
    std::string getHandshakeData();

    /**
     * 处理接收到的对方握手数据
     * @param data 接收到的握手数据
     * @return 成功返回true，失败返回false
     */
    bool processHandshakeData(const std::string& data);

    /**
     * 检查握手是否完成
     * @return 完成返回true，否则返回false
     */
    bool isHandshakeComplete() const;

    /**
     * 加密数据
     * @param plaintext 明文数据
     * @return 加密后的数据，如果为空则表示加密失败
     */
    std::string encrypt(const std::string& plaintext);

    /**
     * 解密数据
     * @param ciphertext 加密数据
     * @return 解密后的明文，如果为空则表示解密失败
     */
    std::string decrypt(const std::string& ciphertext);

    /**
     * 获取最后一次错误信息
     * @return 错误信息字符串
     */
    std::string getLastError() const;

    /**
     * 关闭TLS连接
     */
    void shutdown();

private:
    Role role_;                  // 角色（客户端或服务器）
    std::string certFile_;       // 证书文件路径
    std::string keyFile_;        // 私钥文件路径
    SSL_CTX* ctx_;               // SSL上下文
    SSL* ssl_;                   // SSL对象
    BIO* inBio_;                 // 输入BIO
    BIO* outBio_;                // 输出BIO
    bool isInitialized_;         // 是否初始化完成
    bool isHandshakeComplete_;   // 握手是否完成
    std::string lastError_;      // 最后一次错误信息

    // 记录错误信息
    void setError(const std::string& message);
    
    // 记录OpenSSL错误信息
    void recordOpenSSLErrors();
};

#endif // TLS_CRYPTO_H
