/*
 * Endpoint.h
 *
 * Basic routines for Wanhive protocol implementation
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_UTIL_ENDPOINT_H_
#define WH_UTIL_ENDPOINT_H_
#include "Message.h"
#include "TransactionKey.h"
#include "../base/Network.h"
#include "../base/security/SSLContext.h"

namespace wanhive {
/**
 * Utility class for Wanhive protocol implementation
 * Expects blocking socket connection
 * Thread safe at class level
 */
class Endpoint {
public:
	Endpoint() noexcept;
	~Endpoint();
	//-----------------------------------------------------------------
	/**
	 * SSL and public key management
	 */
	//Set context for SSL connections
	void setSSLContext(SSLContext *ctx) noexcept;
	//Get the current SSL context associated with this object
	SSLContext* getSSLContext() const noexcept;
	//Set the keys used for message signing and verification
	void useKeyPair(const PKI *pki = nullptr) noexcept;
	//Current PKI object (can be nullptr)
	const PKI* getKeyPair() const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Connection management
	 */

	/*
	 * Establish a new connection after terminating the existing connection.
	 * If the new connection is a unix domain socket connection then the SSL
	 * context is cleared. <timeoutMils> specifies the IO timeout value in
	 * milliseconds (0 to block forever, -1 to ignore).
	 */
	void connect(const NameInfo &ni, int timeoutMils = -1);
	//Terminate any existing connection
	void disconnect();

	//Return the socket associated with this object
	int getSocket() const noexcept;
	//Return the SSL connection associated with this object
	SSL* getSecureSocket() const noexcept;
	/*
	 * Close the existing connection and set a new one. Throws an exception on
	 * SSL/TLS connection error.
	 */
	void setSocket(int sfd);
	/*
	 * Close the existing connection and set a new one. Throws an exception
	 * on SSL/TLS connection error.
	 */
	void setSecureSocket(SSL *ssl);
	//Release the underlying connection and return it
	int releaseSocket() noexcept;
	//Release the underlying connection and return it
	SSL* releaseSecureSocket() noexcept;
	/*
	 * Swap the underlying socket with <sfd> and return it. Throws an
	 * exception on SSL/TLS connection error.
	 */
	int swapSocket(int sfd);
	/*
	 * Swap the underlying SSL object with <ssl> and return it. Throws an
	 * exception on SSL/TLS connection error.
	 */
	SSL* swapSecureSocket(SSL *ssl);
	//Set receive and send timeout (milliseconds)
	void setSocketTimeout(int recvTimeout, int sendTimeout) const;
	//-----------------------------------------------------------------
	/**
	 * Header and IO buffer management
	 * NOTE: The format string follows the Serializer class.
	 */
	//Source identifier of all the requests will be set to <source>
	void setSource(uint64_t source) noexcept;
	//Current source identifier for all the requests
	uint64_t getSource() const noexcept;
	//Set the sequence number counter to the given value
	void setSequenceNumber(uint16_t sequenceNumber = 0) noexcept;
	//Current value of the sequence number counter
	uint16_t getSequenceNumber() const noexcept;
	//Return and increment the sequence number
	uint16_t nextSequenceNumber() noexcept;
	//Set the active session identifier
	void setSession(uint8_t session = 0) noexcept;
	//Currently active session identifier
	uint8_t getSession() const noexcept;

	//The message header
	const MessageHeader& getHeader() const noexcept;
	//Pointer to the offset within the IO buffer (nullptr on overflow)
	const unsigned char* getBuffer(unsigned int offset = 0) const noexcept;
	//Pointer to payload's offset within the IO buffer (nullptr on overflow)
	const unsigned char* getPayload(unsigned int offset = 0) const noexcept;

	//Copy and serialize the <header> and the <payload> (can be nullptr)
	bool pack(const MessageHeader &header,
			const unsigned char *payload) noexcept;
	//Copy the full message consisting of serialized header and payload
	bool pack(const unsigned char *message) noexcept;
	//Message length is automatically calculated, <format> can be nullptr
	bool pack(const MessageHeader &header, const char *format, ...) noexcept;
	bool pack(const MessageHeader &header, const char *format,
			va_list ap) noexcept;
	//Append the data at the end of the message and update message's length
	bool append(const char *format, ...) noexcept;
	bool append(const char *format, va_list ap) noexcept;
	//Deserialize the message header from the IO buffer
	void unpackHeader(MessageHeader &header) const noexcept;
	//Deserialize the payload data
	bool unpack(const char *format, ...) const noexcept;
	bool unpack(const char *format, va_list ap) const noexcept;

	//Verify that message's context are correctly set in the header
	bool checkCommand(uint8_t command, uint8_t qualifier) const noexcept;
	//Verify that message's context are correctly set in the header
	bool checkCommand(uint8_t command, uint8_t qualifier,
			uint8_t status) const noexcept;
	//-----------------------------------------------------------------
	/**
	 * Blocking message IO
	 * NOTE: Assign a PKI object for signing and verification.
	 */
	//Send out a message, message length is taken from the deserialized header
	void send(bool sign = false);
	//Receive a message matching the <sequenceNumber> (set to 0 to ignore)
	void receive(unsigned int sequenceNumber = 0, bool verify = false);
	//Execute a request and receive the response, returns true on success
	bool executeRequest(bool sign = false, bool verify = false);
	//Waits for receipt of a ping and responds with a pong (for testing)
	void sendPong();
protected:
	/**
	 * For efficient implementation of additional protocols in the subclasses.
	 * These methods violate encapsulation and must be used with great care
	 * because incorrect usage can introduce hard to debug issues.
	 */
	//Deserialized message header
	MessageHeader& header() noexcept;
	//Pointer to <offset> within the IO buffer (nullptr on overflow)
	unsigned char* buffer(unsigned int offset = 0) noexcept;
	//Pointer to payload's <offset> within the IO buffer (nullptr on overflow)
	unsigned char* payload(unsigned int offset = 0) noexcept;
public:
	/*
	 * Establish a blocking TCP socket connection with the host <ni> and return
	 * the socket file descriptor. <timeoutMils> specifies the IO timeout value
	 * in milliseconds (0 to block forever, -1 to ignore).
	 */
	static int connect(const NameInfo &ni, SocketAddress &sa, int timeoutMils =
			-1);
	/*
	 * If <pki> is provided then the message will be signed with its
	 * private key. <sfd> should be configured for blocking IO.
	 */
	static void send(int sfd, unsigned char *buf, unsigned int length,
			const PKI *pki = nullptr);
	//Same as above however uses SSL/TLS connection
	static void send(SSL *ssl, unsigned char *buf, unsigned int length,
			const PKI *pki = nullptr);
	/*
	 * If <pki> is provided then the message will be verified using it's public
	 * key. If <sequenceNumber> is 0 then received message's sequence number is
	 * not verified. <sfd> should be configured for blocking IO.
	 */
	static void receive(int sfd, unsigned char *buf, MessageHeader &header,
			unsigned int sequenceNumber = 0, const PKI *pki = nullptr);
	//Same as above but uses a secure SSL/TLS connection
	static void receive(SSL *ssl, unsigned char *buf, MessageHeader &header,
			unsigned int sequenceNumber = 0, const PKI *pki = nullptr);
	//-----------------------------------------------------------------
	/**
	 * Following functions pack and unpack a message (header and payload) to and
	 * from the <buffer>. <format> specifies the message payload format. If the
	 * payload is empty then the format must be nullptr. All the functions return
	 * the number of bytes transferred to/from the <buffer>, 0 on error.
	 * NOTE 1: <buffer> should point to valid memory of sufficient size.
	 * NOTE 2: The format string follows the Serializer class.
	 */
	//Message length is always automatically calculated
	static unsigned int pack(const MessageHeader &header, unsigned char *buffer,
			const char *format, ...) noexcept;
	static unsigned int pack(const MessageHeader &header, unsigned char *buffer,
			const char *format, va_list list) noexcept;

	//<header> is populated from the serialized header data in the <buffer>
	static unsigned int unpack(MessageHeader &header,
			const unsigned char *buffer, const char *format, ...) noexcept;
	static unsigned int unpack(MessageHeader &header,
			const unsigned char *buffer, const char *format,
			va_list list) noexcept;
	//-----------------------------------------------------------------
	/**
	 * Message authentication functions which return true on success and false
	 * on error. <pki> can be nullptr in which case the functions are no-op and
	 * always return true.
	 */
	//<length> is a value-result argument, always returns true if <pki> is nullptr
	static bool sign(unsigned char *out, unsigned int &length,
			const PKI *pki) noexcept;
	//Signs the Message <msg>, always returns true if <pki> is nullptr
	static bool sign(Message *msg, const PKI *pki) noexcept;

	//Verifies a signed message, always returns true if <pki> is nullptr
	static bool verify(const unsigned char *in, unsigned int length,
			const PKI *pki) noexcept;
	//Verifies the Message <msg>, always returns true if <pki> is nullptr
	static bool verify(const Message *msg, const PKI *pki) noexcept;
	//-----------------------------------------------------------------
	/*
	 * Execute a request-response sequence over a blocking socket connection.
	 * If <signer> is provided then it's private key will be used for signing
	 * the message. If <verifier> is provided then it's public key will be used
	 * for message verification. Returns true on success.
	 */
	static bool executeRequest(int sfd, MessageHeader &header,
			unsigned char *buf, const PKI *signer = nullptr,
			const PKI *verifier = nullptr);
	//Same as the above but uses a secure SSL/TLS connection
	static bool executeRequest(SSL *ssl, MessageHeader &header,
			unsigned char *buf, const PKI *signer = nullptr,
			const PKI *verifier = nullptr);
private:
	int sockfd; //The underlying socket
	SSL *ssl;  //The underlying SSL/TLS connection
	SSLContext *sslContext;

	const PKI *pki; //PKI for message signing and verification
	uint64_t sourceId; //Default source identifier
	uint16_t sequenceNumber;
	uint8_t session;

	MessageHeader _header; //The deserialized message header
	unsigned char _buffer[Message::MTU]; //The IO buffer
};

} /* namespace wanhive */

#endif /* WH_UTIL_ENDPOINT_H_ */
