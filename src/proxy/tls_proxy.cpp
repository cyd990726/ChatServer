#include "tls_crypto.h"
#include <iostream>
#include <cstring>

TLSProxy::TLSProxy(Role role, const std::string& certFile, const std::string& keyFile)
    : role_(role), certFile_(certFile), keyFile_(keyFile),
      ctx_(nullptr), ssl_(nullptr), inBio_(nullptr), outBio_(nullptr),
      isInitialized_(false), isHandshakeComplete_(false) {}

TLSProxy::~TLSProxy() {
    shutdown();
    
    if (ssl_) {
        SSL_free(ssl_);
        ssl_ = nullptr;
    }
    
    if (ctx_) {
        SSL_CTX_free(ctx_);
        ctx_ = nullptr;
    }
    
    // 清理OpenSSL库
    EVP_cleanup();
    ERR_free_strings();
}

bool TLSProxy::init() {
    // 初始化OpenSSL库
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();

    // 创建SSL上下文
    const SSL_METHOD* method = nullptr;
    if (role_ == Role::CLIENT) {
        method = TLS_client_method();
    } else {
        method = TLS_server_method();
    }

    if (!method) {
        setError("无法创建SSL方法");
        return false;
    }

    ctx_ = SSL_CTX_new(method);
    if (!ctx_) {
        recordOpenSSLErrors();
        return false;
    }

    // 服务器需要加载证书和私钥
    if (role_ == Role::SERVER) {
        // 加载证书
        if (SSL_CTX_use_certificate_file(ctx_, certFile_.c_str(), SSL_FILETYPE_PEM) <= 0) {
            recordOpenSSLErrors();
            return false;
        }

        // 加载私钥
        if (SSL_CTX_use_PrivateKey_file(ctx_, keyFile_.c_str(), SSL_FILETYPE_PEM) <= 0) {
            recordOpenSSLErrors();
            return false;
        }

        // 验证私钥
        if (SSL_CTX_check_private_key(ctx_) <= 0) {
            recordOpenSSLErrors();
            return false;
        }
    }

    // 创建SSL对象
    ssl_ = SSL_new(ctx_);
    if (!ssl_) {
        recordOpenSSLErrors();
        return false;
    }

    // 创建BIO用于数据交互（不直接使用网络套接字）
    inBio_ = BIO_new(BIO_s_mem());
    outBio_ = BIO_new(BIO_s_mem());
    
    if (!inBio_ || !outBio_) {
        setError("无法创建BIO对象");
        return false;
    }

    // 将BIO与SSL对象关联
    SSL_set_bio(ssl_, inBio_, outBio_);

    // 如果是服务器，设置为接受模式
    if (role_ == Role::SERVER) {
        SSL_set_accept_state(ssl_);
    } else {
        SSL_set_connect_state(ssl_);
    }

    isInitialized_ = true;
    return true;
}

std::string TLSProxy::getHandshakeData() {
    if (!isInitialized_) {
        setError("尚未初始化");
        return "";
    }

    // 检查握手是否已完成
    if (isHandshakeComplete_) {
        return "";
    }

    // 执行握手操作
    int ret = (role_ == Role::SERVER) ? SSL_accept(ssl_) : SSL_connect(ssl_);
    
    // 检查握手结果
    if (ret == 1) {
        isHandshakeComplete_ = true;
    } else if (ret <= 0) {
        int error = SSL_get_error(ssl_, ret);
        // 如果是需要更多数据的情况，不是真正的错误
        if (error != SSL_ERROR_WANT_READ && error != SSL_ERROR_WANT_WRITE) {
            recordOpenSSLErrors();
            return "";
        }
    }

    // 从输出BIO中获取需要发送的数据
    char buffer[4096];
    int bytesRead = BIO_read(outBio_, buffer, sizeof(buffer));
    
    if (bytesRead > 0) {
        return std::string(buffer, bytesRead);
    }
    
    return "";
}

bool TLSProxy::processHandshakeData(const std::string& data) {
    if (!isInitialized_) {
        setError("尚未初始化");
        return false;
    }

    // 将接收到的数据写入输入BIO
    if (BIO_write(inBio_, data.data(), data.size()) != (int)data.size()) {
        setError("写入数据到BIO失败");
        return false;
    }

    return true;
}

bool TLSProxy::isHandshakeComplete() const {
    return isHandshakeComplete_;
}

std::string TLSProxy::encrypt(const std::string& plaintext) {
    if (!isHandshakeComplete_) {
        setError("握手尚未完成");
        return "";
    }

    // 分配缓冲区，大小为最大可能的加密长度
    int maxSize = SSL_get_max_write_length(ssl_);
    unsigned char* buffer = new unsigned char[maxSize];
    
    // 加密数据
    int bytesWritten = SSL_write(ssl_, plaintext.data(), plaintext.size());
    
    if (bytesWritten <= 0) {
        recordOpenSSLErrors();
        delete[] buffer;
        return "";
    }

    // 从输出BIO获取加密后的数据
    std::string ciphertext;
    char bioBuffer[4096];
    int bioBytes;
    
    while ((bioBytes = BIO_read(outBio_, bioBuffer, sizeof(bioBuffer))) > 0) {
        ciphertext.append(bioBuffer, bioBytes);
    }

    delete[] buffer;
    return ciphertext;
}

std::string TLSProxy::decrypt(const std::string& ciphertext) {
    if (!isHandshakeComplete_) {
        setError("握手尚未完成");
        return "";
    }

    // 将加密数据写入输入BIO
    if (BIO_write(inBio_, ciphertext.data(), ciphertext.size()) != (int)ciphertext.size()) {
        setError("写入加密数据到BIO失败");
        return "";
    }

    // 解密数据
    char buffer[4096];
    int bytesRead = SSL_read(ssl_, buffer, sizeof(buffer) - 1);
    
    if (bytesRead <= 0) {
        int error = SSL_get_error(ssl_, bytesRead);
        // 如果是需要更多数据，不算错误
        if (error != SSL_ERROR_WANT_READ) {
            recordOpenSSLErrors();
            return "";
        }
        return "";
    }

    return std::string(buffer, bytesRead);
}

std::string TLSProxy::getLastError() const {
    return lastError_;
}

void TLSProxy::shutdown() {
    if (ssl_) {
        SSL_shutdown(ssl_);
    }
    
    if (inBio_) {
        BIO_free(inBio_);
        inBio_ = nullptr;
    }
    
    if (outBio_) {
        BIO_free(outBio_);
        outBio_ = nullptr;
    }
    
    isHandshakeComplete_ = false;
    isInitialized_ = false;
}

void TLSProxy::setError(const std::string& message) {
    lastError_ = message;
}

void TLSProxy::recordOpenSSLErrors() {
    char errMsg[256];
    ERR_error_string_n(ERR_get_error(), errMsg, sizeof(errMsg));
    lastError_ = std::string("OpenSSL错误: ") + errMsg;
}
