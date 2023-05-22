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
    int n;
    char k;
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

    cout << "[serial_1 before operator>> : ]" << endl;
    serial_1.clear();//clear后,size=0了，但是通过地址访问内存的话，那些字符还在的
    serial_1.write_raw_data("Weilai,Tesla", 12);
    serial_1.reset();
    cout << serial_1.size() << " " << serial_1.all_data() << " " << serial_1.current_data() << endl;
    cout << "[serial_1 after operator>> string type : ]" << endl;
    data_1 = ",BYD";
    serial_1 >> data_1;
    cout << serial_1.size() << " " << serial_1.all_data() << " " << serial_1.current_data() << endl;
    int len_1 = read_len(serial_1.current_data() + 12);
    string all_data_1 = string(serial_1.all_data() + 14, serial_1.all_data() + 18);
    string cur_data_1 = string(serial_1.current_data() + 14, serial_1.current_data() + 18);
    cout << len_1 << " " <<  all_data_1 << " " << cur_data_1 << endl;

    cout << "[serial_1 after operator>> const char* type : ]" << endl;
    serial_1 >> ",Horizon";
    cout << serial_1.size() << " " << serial_1.all_data() << " " << serial_1.current_data() << endl;
    len_1 = read_len(serial_1.current_data() + 18);
    all_data_1 = string(serial_1.all_data() + 20, serial_1.all_data() + 28);
    cur_data_1 = string(serial_1.current_data() + 20, serial_1.current_data() + 28);
    cout << len_1 << " " <<  all_data_1 << " " << cur_data_1 << endl;

    cout << "[serial_1 after operator>> int type : ]" << endl;
    int num_1 = 12345;
    serial_1 >> num_1;
    cout << serial_1.size() << " " << serial_1.all_data() << " " << serial_1.current_data() << endl;
    int val_1 = read_val<int>(serial_1, 28);
    cout << val_1 << endl;

    cout << "[serial_1 after operator>> bool type : ]" << endl;
    bool b_1 = false;
    serial_1 >> b_1;
    cout << serial_1.size() << " " << serial_1.all_data() << " " << serial_1.current_data() << endl;
    cout << read_val<bool>(serial_1, 32) << endl;

    cout << "[serial_1 after operator>> float type : ]" << endl;
    float f_1 = 3.14159;
    serial_1 >> f_1;
    cout << serial_1.size() << " " << serial_1.all_data() << " " << serial_1.current_data() << endl;
    cout << read_val<float>(serial_1, 33) << endl;

    cout << "[serial_1 after operator>> char type : ]" << endl;
    char c_1 = 'A';
    serial_1 >> c_1;
    cout << serial_1.size() << " " << serial_1.all_data() << " " << serial_1.current_data() << endl;
    cout << read_val<char>(serial_1, 37) << endl;

    cout << "[pure class type test : ]" << endl;
    A a;
    a.m = 321;
    a.n = 456;
    a.k = 'N';
    // 直接通过指针访问数据成员
    int *p_m = (int *)&a;                
	int *p_n = (int *)(p_m + 1);       
    char* p_k = (char*)(p_n + 1);     
    cout << *p_m << " " << *p_n << " "<< *p_k << endl;
    cout << p_m << " " << p_m + 1 << " " << p_n + 1 << endl;
    cout << &a <<  " " << &a + 1 << " " << &a + 4 << endl;

    cout << "[serial_1 after operator>> class type : ]" << endl;
    serial_1 >> a;
    cout << serial_1.size() << " " << serial_1.all_data() << " " << serial_1.current_data() << endl;
    cout << read_val<int>(serial_1, 38) << " " << read_val<int>(serial_1, 42) << " " << read_val<char>(serial_1, 46) << endl;

    //-----------------Serializer operator<< test-----------------
    cout << "---------------Serializer operator<< test---------------" << endl;
    Serializer serial_2;
    char c_2 = 'L'; 
    int num_2 = 9527; 
    string s_2 = "Horizon"; 
    float f_2 = 6.824;
    string ss_2 = "湖人总冠军!!!";
    A aa_2;
    aa_2.m = 666;
    aa_2.n = 888;
    aa_2.k = '!';
    serial_2 >> c_2 >> num_2 >> s_2 >> f_2 >> ss_2 >> aa_2;

    char c_2_ans; 
    int num_2_ans = 0; 
    string s_2_ans = ""; 
    float f_2_ans = 0;
    string ss_2_ans;
    A aa_2_ans;
    serial_2 << c_2_ans << num_2_ans << s_2_ans << f_2_ans << ss_2_ans << aa_2_ans;

    cout << serial_2.size() << " " << c_2_ans << " " << num_2_ans << " " << s_2_ans << " " << f_2_ans << " " << ss_2_ans << " " << aa_2_ans.m << " " << aa_2_ans.n << " " << aa_2_ans.k << endl;

    //---------------test for << >> sequence ---------------
    /*Serializer serial_3;
    char c_3 = 'L'; 
    int num_3 = 9527; 
    serial_3 >> c_3 >> num_3;

    char c_3_ans; 
    int num_3_ans = 0; 
    serial_3 << num_3_ans << c_3_ans; //必须按顺序读
    cout << c_3_ans << " " << num_3_ans << endl;*/

    //-------------------------test during coding ---------------------
    Serializer serial_4;
    string func_name = "foo_1";
    serial_4 >> func_name;
    cout << serial_4.size() << endl;
    string zmq_content = string(serial_4.all_data(), serial_4.size());
    cout << zmq_content.size() << endl;
    cout << zmq_content << endl;
    
    return 0;
}
