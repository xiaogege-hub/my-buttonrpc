#pragma once
#include <vector>
#include <algorithm>
#include <string>
#include <string.h>
using namespace std;

class StreamBuffer : public vector<char> {
public:
    StreamBuffer();
    StreamBuffer(const char* in, size_t len);

    const char* all_data() const;
    const char* current_data() const;
    void insert_data(const char* in, size_t len) ;
    void reset_pos();
    void offset_pos(int k);//only this function will move m_curpos
    bool is_eof() const;
    int find_char(char c);

private:
    unsigned int m_curpos; // 当前字节流位置
};

StreamBuffer::StreamBuffer() {
    m_curpos = 0;
}

StreamBuffer::StreamBuffer(const char* in, size_t len) {
    m_curpos = 0;
    //use function of vector<char>
    insert(begin(), in, in + len);
}

const char* StreamBuffer::all_data() const{
    return &(*this)[0]; 
}

const char* StreamBuffer::current_data() const{ 
    return &(*this)[m_curpos]; 
}

void StreamBuffer::insert_data(const char* in, size_t len) { 
    insert(end(), in, in + len); 
}

void StreamBuffer::reset_pos() { 
    m_curpos = 0; 
}

void StreamBuffer::offset_pos(int k) { 
    m_curpos += k;
}

bool StreamBuffer::is_eof() const { 
    return (m_curpos >= size()); 
}

int StreamBuffer::find_char(char c) {
    iterator it = find(begin() + m_curpos, end(), c);
    if (it != end()) {
        return it - (begin() + m_curpos);
    }
    return -1;
}

//-------------------------Serializer-------------------------
class Serializer {
public:
    enum ByteOrder { BigEndian = 0, LittleEndian };
    Serializer();
    Serializer(const StreamBuffer& dev, int byteOrder = LITTLE_ENDIAN);

    const char* all_data() const;
    const char* current_data() const;
    void skip_raw_data(int k);
    void write_raw_data(const char* in, int len);//会移动指针
    void reset();
    void clear();
    int size();

    template<typename T>
    Serializer& operator<< (T& t);//输出操作符

    template<typename T>
    Serializer& operator>> (T t);//输入操作符

private:
    template<typename T>
    void read_data(T& out);//读数据，移动指针

    template<typename T>
    void write_data(T in);//写数据，不移动指针

    // 直接给一个长度，返回当前位置以后x个字节数据
    void get_length_mem(char* p, int len);

    //确保输入的字节序变为小端字节序
    void reverse_byte_seq(char* in, int len);

private:
    int  m_byteorder;
    StreamBuffer m_io_device;
};

Serializer::Serializer() {
    m_byteorder = LittleEndian;
}

Serializer::Serializer(const StreamBuffer& dev, int byteOrder){
	m_byteorder = byteOrder;
	m_io_device = dev;
}

const char* Serializer::all_data() const{
    return m_io_device.all_data();
}

const char* Serializer::current_data() const{
    return m_io_device.current_data();
}

void Serializer::skip_raw_data(int k) {
    m_io_device.offset_pos(k);
}

void Serializer::write_raw_data(const char* in, int len) {
    m_io_device.insert_data(in, len);
    m_io_device.offset_pos(len);
}

void Serializer::reset() {
    m_io_device.reset_pos();
}

void Serializer::clear() {
    //clear后,size=0了，但是通过地址访问内存的话，那些字符还在的
    m_io_device.clear();
    m_io_device.reset_pos();
}

int Serializer::size() {
    return m_io_device.size();
}

//--------------------------操作符重载--------------------------
//返回Serializer& 是为了能够连续调用 << 或者 >> 
template<typename T>
Serializer& Serializer::operator<< (T& t) {
    read_data(t);
    return* this;
}

template<typename T>
Serializer& Serializer::operator>> (T t) {
    write_data(t);
    return *this;
}

//-------------------------read_data-------------------------
template<typename T>
inline void Serializer::read_data(T& out) {//out传入传出参数
    int len = sizeof(T);
    char* d = new char[len];
    if (!m_io_device.is_eof()) {
        memcpy(d, m_io_device.current_data(), len);
        m_io_device.offset_pos(len);
        reverse_byte_seq(d, len);
        out = *reinterpret_cast<T*>(&d[0]);
    }
    delete []d;
}

template<>
inline void Serializer::read_data(string& out) {
    // 先读出字符串长度
    int marklen = sizeof(uint16_t);
    char* d = new char[marklen];
    memcpy(d, m_io_device.current_data(), marklen);
    reverse_byte_seq(d, marklen);
    int len = *reinterpret_cast<uint16_t*>(&d[0]);
    delete []d;

    // 再读出字符串实际内容
    m_io_device.offset_pos(marklen);
    if (len == 0) return;
    out.insert(out.begin(), m_io_device.current_data(), m_io_device.current_data() + len);
    m_io_device.offset_pos(len);
}

//-------------------------write_data-------------------------
template<typename T>
inline void Serializer::write_data(T in) {
    int len = sizeof(T);
    char* d = new char[len];
    const char* p = reinterpret_cast<const char*>(&in);
    memcpy(d, p, len);
    reverse_byte_seq(d, len);
    m_io_device.insert_data(d, len);//根据input，写到自己的m_io_device中;
    delete []d;
}

template<>
inline void Serializer::write_data(string in) {
    // 先存入字符串长度
    uint16_t len = in.size();
    char* p = reinterpret_cast<char*>(&len);
    reverse_byte_seq(p, sizeof(uint16_t));
    m_io_device.insert_data(p, sizeof(uint16_t));

    // 再存入字符串
    if (len == 0) return;
    char* d = new char[len];
    memcpy(d, in.c_str(), len);
    m_io_device.insert_data(d, len);
    delete []d;
}

template<>
inline void Serializer::write_data(const char* in) {
    write_data(string(in));
}

void Serializer::get_length_mem(char* p, int len) {
    memcpy(p, m_io_device.current_data(), len);
    m_io_device.offset_pos(len);
}

//确保输入的字节序变为小端字节序
void Serializer::reverse_byte_seq(char* in, int len) {
    if (m_byteorder == BIG_ENDIAN) {
        reverse(in, in + len);
    }
}