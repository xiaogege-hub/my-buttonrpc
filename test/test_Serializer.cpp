#include <iostream>
#include "../Serializer.hpp"
using namespace std;


template<typename T>
T read_val(Serializer& io_device, int offset) {
    int len = sizeof(T);
    char* d = new char[len];
    memcpy(d, io_device.current_data() + offset, len);
    T ans = *reinterpret_cast<T*>(&d[0]);
    delete []d;
    return ans;
}

int read_len(const char* src) {
    int marklen = sizeof(uint16_t);
    char* d = new char[marklen];
    memcpy(d, src, marklen);
    int len = *reinterpret_cast<uint16_t*>(&d[0]);
    delete []d;
    return len;
}

class A {
public:
    int m;
    char n;
};

int main() {
    //-------------------StreamBuffer API test-------------------
    cout << "-----------------StreamBuffer API test-----------------" << endl;
    StreamBuffer stream_1;
    cout << "[stream_1 before insert : ]" << endl;
    cout << stream_1.size() << endl;

    cout << "[stream_1 after insert : ]" << endl;
    string data_1 = "xiaopeng";
    stream_1.insert_data(data_1.c_str(), data_1.size());
    cout << stream_1.size() << " " << stream_1.all_data() << " " << stream_1.current_data() << endl;

    cout << "[stream_1 after offset : ]" << endl;
    stream_1.offset_pos(2);
    cout << stream_1.size() << " " << stream_1.all_data() << " " << stream_1.current_data() << endl;

    cout << "[stream_1 after reset : ]" << endl;
    stream_1.reset_pos();
    cout << stream_1.size() << " " << stream_1.all_data() << " " << stream_1.current_data() << endl;

    cout << "[stream_1 find char : ]" << endl;
    cout << stream_1.find_char('p') << " " << stream_1.find_char('X') << endl;

    //-------------------Serializer API test-------------------
    cout << "-----------------Serializer API test-----------------" << endl;
    Serializer serial_1(stream_1);
    cout << "[serial_1 before operation : ]" << endl;
    cout << serial_1.size() << " " << serial_1.all_data() << " " << serial_1.current_data() << endl;

    cout << "[serial_1 after write_raw_data : ]" << endl;
    serial_1.write_raw_data(",lixiang1", 9);
    cout << serial_1.size() << " " << serial_1.all_data() << " " << serial_1.current_data() << endl;

    cout << "[serial_1 after skip_raw_data : ]" << endl;
    serial_1.skip_raw_data(3);
    cout << serial_1.size() << " " << serial_1.all_data() << " " << serial_1.current_data() << endl;

    cout << "[serial_1 after reset : ]" << endl;
    serial_1.reset();
    cout << serial_1.size() << " " << serial_1.all_data() << " " << serial_1.current_data() << endl;

    cout << "[serial_1 after clear : ]" << endl;
    serial_1.clear();//clear后,size=0了，但是通过地址访问内存的话，那些字符还在的
    cout << serial_1.size() << " " << serial_1.all_data() << " " << serial_1.current_data() << endl;

    cout << "[serial_1 before operator<< : ]" << endl;
    serial_1.clear();//clear后,size=0了，但是通过地址访问内存的话，那些字符还在的
    serial_1.write_raw_data("Weilai,Tesla", 12);
    serial_1.reset();
    cout << serial_1.size() << " " << serial_1.all_data() << " " << serial_1.current_data() << endl;
    cout << "[serial_1 after operator<< string type : ]" << endl;
    data_1 = ",BYD";
    serial_1 << data_1;
    cout << serial_1.size() << " " << serial_1.all_data() << " " << serial_1.current_data() << endl;
    int len_1 = read_len(serial_1.current_data() + 12);
    string all_data_1 = string(serial_1.all_data() + 14, serial_1.all_data() + 18);
    string cur_data_1 = string(serial_1.current_data() + 14, serial_1.current_data() + 18);
    cout << len_1 << " " <<  all_data_1 << " " << cur_data_1 << endl;

    cout << "[serial_1 after operator<< const char* type : ]" << endl;
    serial_1 << ",Horizon";
    cout << serial_1.size() << " " << serial_1.all_data() << " " << serial_1.current_data() << endl;
    len_1 = read_len(serial_1.current_data() + 18);
    all_data_1 = string(serial_1.all_data() + 20, serial_1.all_data() + 28);
    cur_data_1 = string(serial_1.current_data() + 20, serial_1.current_data() + 28);
    cout << len_1 << " " <<  all_data_1 << " " << cur_data_1 << endl;

    cout << "[serial_1 after operator<< int type : ]" << endl;
    int num_1 = 12345;
    serial_1 << num_1;
    cout << serial_1.size() << " " << serial_1.all_data() << " " << serial_1.current_data() << endl;
    int val_1 = read_val<int>(serial_1, 28);
    cout << val_1 << endl;

    cout << "[serial_1 after operator<< bool type : ]" << endl;
    bool b_1 = false;
    serial_1 << b_1;
    cout << serial_1.size() << " " << serial_1.all_data() << " " << serial_1.current_data() << endl;
    cout << read_val<bool>(serial_1, 32) << endl;

    cout << "[serial_1 after operator<< float type : ]" << endl;
    float f_1 = 3.14159;
    serial_1 << f_1;
    cout << serial_1.size() << " " << serial_1.all_data() << " " << serial_1.current_data() << endl;
    cout << read_val<float>(serial_1, 33) << endl;

    cout << "[serial_1 after operator<< char type : ]" << endl;
    char c_1 = 'A';
    serial_1 << c_1;
    cout << serial_1.size() << " " << serial_1.all_data() << " " << serial_1.current_data() << endl;
    cout << read_val<char>(serial_1, 37) << endl;

    cout << "[serial_1 after operator<< class type : ]" << endl;
    A a;
    a.m = 123;
    a.n = 'N';
    cout << a[0] << endl;





    

    return 0;
}