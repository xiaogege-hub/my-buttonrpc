#pragma once
#include <cstring>
#include <iostream>
#include <string>
#include <map>
#include <functional>
#include "zmq.h"  //zmq.hpp与zmq.h内容不一样
#include "Serializer.hpp"

template<typename T>
struct type_xx {typedef T type; };

template<>
struct type_xx<void> { typedef int8_t type; };

// return value
template<typename T>
class value_t {
public:
    typedef typename type_xx<T>::type value_type;
    value_t() { err_code_ = 0; err_msg_.clear(); }
    int get_err_code() { return err_code_; }
    std::string get_err_msg() { return err_msg_; }
    T val() { return val_;}
    bool valid() { return (err_code_ == 0 ? true : false); }
    void set_err_code(int err_code) { err_code_ = err_code; }
    void set_err_msg(const std::string& err_msg) { err_msg_ = err_msg; }
    void set_val(const value_type& val) { val_ = val;}

    friend Serializer& operator<< (Serializer& out, value_t<T>& d) {
		out << d.err_code_ << d.err_msg_;
		if (d.err_code_ == 0) {
			out << d.val_;
		}
		return out;
	}
	friend Serializer& operator>> (Serializer& in, value_t<T> d) {
		in >> d.err_code_ >> d.err_msg_ >> d.val_;
		return in;
	}

private:
    int err_code_;
    std::string err_msg_;
    value_type val_;
};

class buttonrpc {
public:
    enum rpc_role { RPC_CLIENT = 0, RPC_SERVER };
    enum rpc_err_code {
        RPC_ERR_SUCCESS = 0, 
        RPC_ERR_FUNCTIION_NOT_BIND, 
        RPC_ERR_RECV_TIMEOUT
    };
    buttonrpc();
    //~buttonrpc();

    // network: bind or connect
    void as_client(std::string ip, int port);
    void as_server(int port);
    
    // -----------------------server API-----------------------
    void run();

    template <typename F>
    void bind(std::string name, F func);

    template<typename F, typename S>
    void bind(std::string name, F func, S* s);

    // -----------------------client API-----------------------
    void set_timeout(uint32_t ms);

    template<typename R>
    value_t<R> call(std::string name);

    template<typename R, typename P1>
    value_t<R> call(std::string name, P1);

    template<typename R, typename P1, typename P2>
    value_t<R> call(std::string name, P1, P2);

    template<typename R, typename P1, typename P2, typename P3>
    value_t<R> call(std::string name, P1, P2, P3);

    template<typename R, typename P1, typename P2, typename P3, typename P4>
    value_t<R> call(std::string name, P1, P2, P3, P4);

    template<typename R, typename P1, typename P2, typename P3, typename P4, typename P5>
    value_t<R> call(std::string name, P1, P2, P3, P4, P5);

private:
    // invoked by call   call -> net_call
    template<typename R>
    value_t<R> net_call(Serializer& ds);

    // in bind
    template<typename F>
    void callproxy(F fun, Serializer* pr, const char* data, int len);

    template<typename F, typename S>
    void callproxy(F fun, S* s, Serializer* pr, const char* data, int len);

    // -----------------------PROXY FUNCTION POINT-----------------------
    // 函数指针全都转为对应的function进行调用
    template<typename R>
    void callproxy_(R(*func)(), Serializer* pr, const char* data, int len) {
        callproxy_(std::function<R()>(func), pr, data, len);
    }

    template<typename R, typename P1>
    void callproxy_(R(*func)(P1), Serializer* pr, const char* data, int len) {
        callproxy_(std::function<R(P1)>(func), pr, data, len);
    }

    template<typename R, typename P1, typename P2>
    void callproxy_(R(*func)(P1, P2), Serializer* pr, const char* data, int len) {
        callproxy_(std::function<R(P1, P2)>(func), pr, data, len);
    }

    template<typename R, typename P1, typename P2, typename P3>
    void callproxy_(R(*func)(P1, P2, P3), Serializer* pr, const char* data, int len) {
        callproxy_(std::function<R(P1, P2, P3)>(func), pr, data, len);
    }

    template<typename R, typename P1, typename P2, typename P3, typename P4>
    void callproxy_(R(*func)(P1, P2, P3, P4), Serializer* pr, const char* data, int len) {
        callproxy_(std::function<R(P1, P2, P3, P4)>(func), pr, data, len);
    }

    template<typename R, typename P1, typename P2, typename P3, typename P4, typename P5>
    void callproxy_(R(*func)(P1, P2, P3, P4, P5), Serializer* pr, const char* data, int len) {
        callproxy_(std::function<R(P1, P2, P3, P4, P5)>(func), pr, data, len);
    }

    // PROXY CLASS MEMBER
    template<typename R, typename C, typename S>
    void callproxy_(R(C::* func)(), S* s, Serializer* pr, const char* data, int len) {
        callproxy_(std::function<R()>(std::bind(func, s)), pr, data, len);
    }

    template<typename R, typename C, typename S, typename P1>
    void callproxy_(R(C::* func)(P1), S* s, Serializer* pr, const char* data, int len) {
        callproxy_(std::function<R(P1)>(std::bind(func, s, std::placeholders::_1)), pr, data, len);
    }

    template<typename R, typename C, typename S, typename P1, typename P2>
	void callproxy_(R(C::* func)(P1, P2), S* s, Serializer* pr, const char* data, int len) {
		callproxy_(std::function<R(P1, P2)>(std::bind(func, s, std::placeholders::_1, std::placeholders::_2)), pr, data, len);
	}

	template<typename R, typename C, typename S, typename P1, typename P2, typename P3>
	void callproxy_(R(C::* func)(P1, P2, P3), S* s, Serializer* pr, const char* data, int len) {
		callproxy_(std::function<R(P1, P2, P3)>(std::bind(func, s, 
			std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)), pr, data, len);
	}

	template<typename R, typename C, typename S, typename P1, typename P2, typename P3, typename P4>
	void callproxy_(R(C::* func)(P1, P2, P3, P4), S* s, Serializer* pr, const char* data, int len) {
		callproxy_(std::function<R(P1, P2, P3, P4)>(std::bind(func, s,
			std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)), pr, data, len);
	}

	template<typename R, typename C, typename S, typename P1, typename P2, typename P3, typename P4, typename P5>
	void callproxy_(R(C::* func)(P1, P2, P3, P4, P5), S* s, Serializer* pr, const char* data, int len) {
		callproxy_(std::function<R(P1, P2, P3, P4, P5)>(std::bind(func, s,
			std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5)), pr, data, len);
	}

    // -----------------------PROXY FUNCTIONAL-----------------------
    template<typename R>
	void callproxy_(std::function<R()>, Serializer* pr, const char* data, int len);

	template<typename R, typename P1>
	void callproxy_(std::function<R(P1)>, Serializer* pr, const char* data, int len);

	template<typename R, typename P1, typename P2>
	void callproxy_(std::function<R(P1, P2)>, Serializer* pr, const char* data, int len);

	template<typename R, typename P1, typename P2, typename P3>
	void callproxy_(std::function<R(P1, P2, P3)>, Serializer* pr, const char* data, int len);

	template<typename R, typename P1, typename P2, typename P3, typename P4>
	void callproxy_(std::function<R(P1, P2, P3, P4)>, Serializer* pr, const char* data, int len);

	template<typename R, typename P1, typename P2, typename P3, typename P4, typename P5>
	void callproxy_(std::function<R(P1, P2, P3, P4, P5)>, Serializer* pr, const char* data, int len);

private:
    // 封装了zmq_msg_send 和 zmq_msg_recv
    void send_msg(const std::string &content);

    std::string recv_msg();

private:
    std::map<std::string, std::function<void(Serializer*, const char*, int)>> m_handlers;

    void *m_context = nullptr;

    void *m_socket = nullptr;

    rpc_err_code m_error_code;

    int m_role;
};

buttonrpc::buttonrpc() { 
    m_context = zmq_ctx_new(); 
    m_error_code = RPC_ERR_SUCCESS;
}

// network
void buttonrpc::as_client(std::string ip, int port) {
    m_role = RPC_CLIENT;
    m_socket = zmq_socket(m_context, ZMQ_REQ);
    std::string path = "tcp://" + ip + ":" + std::to_string(port);
    zmq_connect(m_socket, path.c_str());
}

void buttonrpc::as_server(int port) {
    m_role = RPC_SERVER;
    m_socket = zmq_socket(m_context, ZMQ_REP);
    std::string path = "tcp://*:" + std::to_string(port);
    zmq_bind(m_socket, path.c_str());
}

void buttonrpc::run() {
    // only server can call
    if (m_role != RPC_SERVER) return;
    while (1) {
        // REP-REQ通信模式中 server 和 client 必须交替使用 zmq_recv 和 zmq_send
        std::string data = recv_msg();
        StreamBuffer io_device(data.c_str(), data.size());
        Serializer ds(io_device);

        std::string func_name;
        ds << func_name;
        Serializer* reply = new Serializer();
        if (m_handlers.find(func_name) == m_handlers.end()) {
            (*reply) >> rpc_err_code::RPC_ERR_FUNCTIION_NOT_BIND;
            (*reply) >> std::string("function not bind: " + func_name);
            send_msg(std::string(reply->all_data(), reply->size()));
        }
        else {
            auto func = m_handlers[func_name];
            func(reply, ds.current_data(), ds.size() - func_name.size() - 2);
            reply->reset();
            send_msg(std::string(reply->all_data(), reply->size()));
        }
        delete reply;
    }
}

template <typename F>
void buttonrpc::bind(std::string name, F func) {
    m_handlers[name] = std::bind(&buttonrpc::callproxy<F>, this, func, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
}

template<typename F, typename S>
void buttonrpc::bind(std::string name, F func, S* s) {
    m_handlers[name] = std::bind(&buttonrpc::callproxy<F, S>, this, func, s, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
}

template<typename F>
void buttonrpc::callproxy(F fun, Serializer* pr, const char* data, int len) {
    callproxy_(fun, pr, data, len);
}

template<typename F, typename S>
void buttonrpc::callproxy(F fun, S* s, Serializer* pr, const char* data, int len) {
    callproxy_(fun, s, pr, data, len);
}

// help call return value type is void function
template<typename R, typename F>
typename std::enable_if<std::is_same<R, void>::value, typename type_xx<R>::type>::type call_helper(F f) {
    f();
    return 0;
}

template<typename R, typename F>
typename std::enable_if<!std::is_same<R, void>::value, typename type_xx<R>::type >::type call_helper(F f) {
	return f();
}

// -----------------------PROXY FUNCTIONAL-----------------------
template<typename R>
void buttonrpc::callproxy_(std::function<R()> func, Serializer* pr, const char* data, int len) {
    typename type_xx<R>::type r = call_helper<R>(std::bind(func));
    value_t<R> val;
    val.set_err_code(RPC_ERR_SUCCESS);
    val.set_val(r);
    (*pr) >> val;
}

template<typename R, typename P1>
void buttonrpc::callproxy_(std::function<R(P1)> func, Serializer* pr, const char* data, int len) {
    Serializer ds(StreamBuffer(data, len));
    P1 p1;
    ds << p1;
    typename type_xx<R>::type r = call_helper<R>(std::bind(func, p1));
    value_t<R> val;
    val.set_err_code(RPC_ERR_SUCCESS);
    val.set_val(r);
    (*pr) >> val;
}

template<typename R, typename P1, typename P2>
void buttonrpc::callproxy_(std::function<R(P1, P2)> func, Serializer* pr, const char* data, int len) {
    Serializer ds(StreamBuffer(data, len));
    P1 p1; P2 p2;
    ds << p1 << p2;
    typename type_xx<R>::type r = call_helper<R>(std::bind(func, p1, p2));
    value_t<R> val;
    val.set_err_code(RPC_ERR_SUCCESS);
    val.set_val(r);
    (*pr) >> val;
}

template<typename R, typename P1, typename P2, typename P3>
void buttonrpc::callproxy_(std::function<R(P1, P2, P3)> func, Serializer* pr, const char* data, int len) {
    Serializer ds(StreamBuffer(data, len));
    P1 p1; P2 p2; P3 p3;
    ds << p1 << p2 << p3;
    typename type_xx<R>::type r = call_helper<R>(std::bind(func, p1, p2, p3));
    value_t<R> val;
    val.set_err_code(RPC_ERR_SUCCESS);
    val.set_val(r);
    (*pr) >> val;
}

template<typename R, typename P1, typename P2, typename P3, typename P4>
void buttonrpc::callproxy_(std::function<R(P1, P2, P3, P4)> func, Serializer* pr, const char* data, int len) {
    Serializer ds(StreamBuffer(data, len));
    P1 p1; P2 p2; P3 p3; P4 p4;
    ds << p1 << p2 << p3 << p4;
    typename type_xx<R>::type r = call_helper<R>(std::bind(func, p1, p2, p3,p4));
    value_t<R> val;
    val.set_err_code(RPC_ERR_SUCCESS);
    val.set_val(r);
    (*pr) >> val;
}

template<typename R, typename P1, typename P2, typename P3, typename P4, typename P5>
void buttonrpc::callproxy_(std::function<R(P1, P2, P3, P4, P5)> func, Serializer* pr, const char* data, int len) {
    Serializer ds(StreamBuffer(data, len));
    P1 p1; P2 p2; P3 p3; P4 p4; P5 p5;
    ds << p1 << p2 << p3 << p4 << p5;
    typename type_xx<R>::type r = call_helper<R>(std::bind(func, p1, p2, p3, p4, p5));
    value_t<R> val;
    val.set_err_code(RPC_ERR_SUCCESS);
    val.set_val(r);
    (*pr) >> val;
}

void buttonrpc::set_timeout(uint32_t ms) {
    // only client can set
    if (m_role == RPC_CLIENT) {
        int ret = zmq_setsockopt(m_socket, ZMQ_RCVTIMEO, &ms, sizeof(uint32_t));
        if (ret < 0) {
        printf("set zmq recv time out[%d] failed, reason: %s\n", ms,
                zmq_strerror(errno));
        }
    }
}

template<typename R>
inline value_t<R> buttonrpc::call(std::string name) {
    Serializer request;
    request >> name;
    return net_call<R>(request);
}

template<typename R, typename P1>
inline value_t<R> buttonrpc::call(std::string name, P1 p1) {
    Serializer request;
    request >> name >> p1;
    return net_call<R>(request);
}

template<typename R, typename P1, typename P2>
inline value_t<R> buttonrpc::call(std::string name, P1 p1, P2 p2) {
    Serializer request;
    request >> name >> p1 >> p2;
    return net_call<R>(request);
}

template<typename R, typename P1, typename P2, typename P3>
inline value_t<R> buttonrpc::call(std::string name, P1 p1, P2 p2, P3 p3) {
    Serializer request;
    request >> name >> p1 >> p2 >> p3;
    return net_call<R>(request);
}

template<typename R, typename P1, typename P2, typename P3, typename P4>
inline value_t<R> buttonrpc::call(std::string name, P1 p1, P2 p2, P3 p3, P4 p4) {
    Serializer request;
    request >> name >> p1 >> p2 >> p3 >> p4;
    return net_call<R>(request);
}

template<typename R, typename P1, typename P2, typename P3, typename P4, typename P5>
inline value_t<R> buttonrpc::call(std::string name, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5) {
    Serializer request;
    request >> name >> p1 >> p2 >> p3 >> p4 >> p5;
    return net_call<R>(request);
}

// 还要修改
template<typename R>
inline value_t<R> buttonrpc::net_call(Serializer& ds) {
    if (m_error_code != RPC_ERR_RECV_TIMEOUT) {
        send_msg(std::string(ds.all_data(), ds.size()));
    }
    string reply = recv_msg();
    value_t<R> val;
    if (reply.size() == 0) {
        // timeout
        m_error_code = RPC_ERR_RECV_TIMEOUT;
        val.set_err_code(RPC_ERR_RECV_TIMEOUT);
        val.set_err_msg("recv timeout");
        return val;
    }
    else {
        m_error_code = RPC_ERR_SUCCESS;
        ds.clear();
        ds.write_raw_data(reply.c_str(), reply.size());
        ds.reset();
        ds << val;//???
    }
    return val;
}

void buttonrpc::send_msg(const std::string &content) {
    //cout << content.size() << endl;
    zmq_msg_t msg;
    // do not use init data, use init size and memcpy
    if (zmq_msg_init_size(&msg, content.size()) != 0) {
        printf("zmq_msg_init_size fail\n");
        zmq_msg_close(&msg);
        return;
    }
    memcpy(zmq_msg_data(&msg), content.data(), content.size());
    if (zmq_msg_send(&msg, m_socket, 0) < 0) {
        printf("zmq send message fail, reason: %s\n", zmq_strerror(errno));
        zmq_msg_close(&msg);
        return;
    }
    //printf("send message success : %s\n", content.c_str());
    zmq_msg_close(&msg);
    return;
}

std::string buttonrpc::recv_msg() {
    zmq_msg_t msg;
    zmq_msg_init(&msg);
    if (zmq_msg_recv(&msg, m_socket, 0) < 0) {
        printf("zmq message receive fail, reason: %s\n", zmq_strerror(errno));
        zmq_msg_close(&msg);
        return "";
    }
    std::string data(static_cast<char *>(zmq_msg_data(&msg)), zmq_msg_size(&msg));
    //printf("recv message : %s\n", data.c_str());
    zmq_msg_close(&msg);
    return data;
}
