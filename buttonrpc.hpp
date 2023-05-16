#pragma once
#include <cstring>
#include <iostream>
#include <string>
#include <map>
#include <functional>
#include "zmq.h"  //zmq.hpp与zmq.h内容不一样

class buttonrpc {
public:
    enum rpc_role { RPC_CLIENT = 0, RPC_SERVER };
    buttonrpc();
    //~buttonrpc();

    // network
    void as_client(std::string ip, int port);
    void as_server(int port);

    // 我自己测试的，外部调用的接口
    void send(const std::string &content);
    std::string recv();
    // void send(zmq_msg_t& data);
    // void recv(zmq_msg_t &data);

    void set_timeout(uint32_t ms);
    void run();

public:
    // server
    template <typename F>
    void bind(std::string name, F func);

    // client

private:
    std::map<std::string, std::function<>> m_handlers;
    void *m_context = nullptr;
    void *m_socket = nullptr;
    int m_role;
};

buttonrpc::buttonrpc() { m_context = zmq_ctx_new(); }

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

void buttonrpc::send(const std::string &content) {
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
    printf("send message success : %s\n", content.c_str());
    zmq_msg_close(&msg);
    return;
}

std::string buttonrpc::recv() {
    zmq_msg_t msg;
    zmq_msg_init(&msg);
    if (zmq_msg_recv(&msg, m_socket, 0) < 0) {
        printf("zmq message receive fail, reason: %s\n", zmq_strerror(errno));
        zmq_msg_close(&msg);
        return "";
    }
    std::string data(static_cast<char *>(zmq_msg_data(&msg)), zmq_msg_size(&msg));
    printf("recv message : %s\n", data.c_str());
    zmq_msg_close(&msg);
    return data;
}

/*void buttonrpc::send(zmq_msg_t& data) {
    zmq_msg_send(&data, m_socket, 0);
}


void buttonrpc::recv(zmq_msg_t &data) {
    zmq_msg_recv()
}*/

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

void buttonrpc::run() {
    // only server can call
    if (m_role != RPC_SERVER) return;
    while (1) {



    }
}

template <typename F>
void buttonrpc::bind(std::string name, F func) {

}