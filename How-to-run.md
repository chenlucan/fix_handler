
## dependences: 
	boost
	sbe
    zeromq
    quickfix
    mongodb
    gtest/gmock
    protobuf

<br>   

## 行情数据的接受，保存，解析

### 周末启动的场合（pre-opening startup）

* 启动行情数据接受模块(从端口 30001，30002 接受行情数据；发送原始数据到 5557 端口，发送 book 情报到 5558 端口)：
	> ./udp_receiver_test -s

* 启动 TCP 服务器模拟模块(接受到 tcp recovery 请求后，发送行情数据到 30007 端口)
	> ./tcp_sender_test 30007

* 启动策略模拟模块(从端口 5558 接受 book 情报)
	> ./strategy_test 6001 6002 5558 1000

* 启动原始行情数据接受模块(从端口 5557 接受数据)
	> ./zmq_receiver_test

* 启发送动行情数据模拟模块(从 30001，30002 端口发送行情数据，每 500ms 发送一条，在 10 的倍数左右会丢失几条不发送)
	> ./udp_sender_test -a ***192.168.1.185*** 30001 ***500*** ***10***  
	> ./udp_sender_test -a ***192.168.1.185*** 30002 ***500*** ***10***


### 周中启动的场合（later joiner startup）

* 启动行情数据接受模块(从端口 30001-30006 接受行情数据；发送原始数据到 5557 端口，发送 book 情报到 5558 端口)：
	> ./udp_receiver_test -j

* 启动 TCP 服务器模拟模块(接受到 tcp recovery 请求后，发送行情数据到 30007 端口)
	> ./tcp_sender_test 30007

* 启动策略模拟模块(从端口 5558 接受 book 情报)
	> ./strategy_test 6001 6002 5558 1000

* 启动原始行情数据接受模块(从端口 5557 接受数据)
	> ./zmq_receiver_test

* 启发送动定义数据模拟模块(从 30003，30004 端口发送行情定义数据，每 500ms 发送一条，在 3 的倍数会不发送)
	> ./udp_sender_test -d ***192.168.1.185*** 30003 ***500*** ***3***  
	> ./udp_sender_test -d ***192.168.1.185*** 30004 ***500*** ***3***
	
* 启发送动恢复数据模拟模块(从 30005，30006 端口发送行情恢复数据，每 500ms 发送一条，在 4 的倍数会不发送)
	> ./udp_sender_test -r ***192.168.1.185*** 30005 ***500*** ***4***  
	> ./udp_sender_test -r ***192.168.1.185*** 30006 ***500*** ***4***

* 启发送动行情数据模拟模块(从 30001，30002 端口发送行情数据，每 500ms 发送一条，在 10 的倍数左右会丢失几条不发送)
	> ./udp_sender_test -i ***192.168.1.185*** 30001 ***500*** ***3***  
	> ./udp_sender_test -i ***192.168.1.185*** 30002 ***500*** ***3***

<br>

## 交易指令处理

* 启动 CME 服务器模拟模块(从端口 5001 接受交易消息)
	> ./exchange_server_test	

* 启动交易模块(从端口 6001 接受策略模块的交易指令，往端口 5001 发送交易消息到 CME 服务器；发送交易结果到端口 6002)
	> ./exchange_client_test -s|-j    (s: week begin startup; j: middle week startup)


* 启动策略模拟模块(从端口 5558 接受 book 情报；从端口 6001 每隔 1000ms 发送一条交易指令，从端口 6002 接受交易结果)
	> ./strategy_test 6001 6002 5558 1000

<br>

## TODOs:
	virtual destructor is almost unnecessary
	zmqreceiver inherit is unnecessary
	file name of cme/market/*
	struct to class
	cpplint
	gtest
