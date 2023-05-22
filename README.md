# my-buttonrpc
An implementation of RPC with C++

## 编译运行
```bash
mkdir build
cd build/
cmake ..
```

server:
```bash
./main_server
```

client:
```bash
./main_client
```

# example
server:

```cpp
#include "buttonrpc.hpp"

int foo(int age, int mm){
	return age + mm;
}

int main()
{
	buttonrpc server;
	server.as_server(5555);

	server.bind("foo", foo);
	server.run();

	return 0;
}
```

client:

```cpp
#include <iostream>
#include "buttonrpc.hpp"

int main()
{
	buttonrpc client;
	client.as_client("127.0.0.1", 5555);
	int a = client.call<int>("foo", 2, 3).val();
	std::cout << "call foo result: " << a << std::endl;
	system("pause");
	return 0;
}
// output: call foo result: 5
```

# test
测试Serializer.hpp序列化和反序列化结果是否正确

运行：

```bash
cd test/
sh build.sh
```

# 依赖的库
zmq

# Features
1. 轻量级，跨平台，简单易用 
2. 服务端可以绑定自由函数，类成员函数，std::function对象 
3. 服务端可以绑定参数是任意自定义类型的函数 
4. 客户端与服务端自动重连机制 
5. 客户端调用超时选项 
