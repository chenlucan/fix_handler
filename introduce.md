## 1. 行情数据的接收

参数：    
	> channel ids（例如：360,361）（默认为配置文件中所有的 channel）  
	> cme channel 设定文件路径（默认为当前目录的 market_config.xml）  
	> 行情配置文件路径（默认为当前目录的 market_settings.ini）  

### 1.1. 读取配置文件      
	1.1.1. 读取参数指定的 cme channel 设定文件，将参数指定的所有 channel id 的监听网络端口配置信息读入内存
			(如果没有指定 channel，则读入所有的 channel 信息)  
	1.1.2. 读取参数指定的行情配置文件,将其中的所有设定读入内存  

### 1.2. 初始化以下模块  
	1.2.1. 行情状态数据监听模块  
	         负责将整理好的行情状态数据（book state）发送到行情配置文件中指定的 zeromq 端口（zeromq.book_url）  
	1.2.2. 行情数据整理模块（每个 channel 对应一个独立模块）  
             负责将从网络端口接收到的行情数据（mdp message）整理成行情状态数据，通过行情状态数据监听模块发送出去；
             同时将接收到的原始数据发送到行情配置文件中指定的 zeromq 端口（zeromq.org_url）  

### 1.3. 启动所有 channel 的行情数据整理模块  
	1.3.1. 对当前 channel 定义中每一个需要监听的 increment feed，启动一个线程监听指定的端口（行情数据接收者线程）  
	1.3.2. 对当前 channel 定义中每一个需要监听的 definition feed，启动一个线程监听指定的端口（产品定义接收者线程）  
	1.3.3. 启动一个线程，按顺序读取接收到的行情数据进行处理（处理者线程）

### 1.4. 行情数据接收者线程的处理
	1.4.1. 网络端口监听模块（UDPReceiver）接收到数据，启动数据预处理模块（DatProcessor）  
	1.4.2. 数据预处理模块调用数据裁决模块（DatArbitrator），判断本次接收到的数据是否是需要的数据
			 （由于有多个接收者线程，所以处理时需要加锁）
			 1）如果是接收到的第一条数据，继续处理该数据
			 2）如果该数据的 sequence_number < 已接受到的最大 sequence_number + 1
				A. 如果该 sequence_number 的数据已经接收过了，丢弃
				B. 继续处理该数据
			 3）如果该数据的 sequence_number == 已接受到的最大 sequence_number + 1，继续处理该数据
			 4）继续处理该数据，同时启动缺失数据请求模块（DatReplayer）去 cme 请求缺失的数据
	1.4.3. 如果需要继续处理接收到的数据
			 1）解析该数据，提取出 mdp message（多条），按 sequence_number 顺序设置给处理者线程，让其保存到内存
			 2）如果出现 sequence number 的 GAP，将缺失的 sequence number 列表也保存到内存
	1.4.4. 如果启动了缺失数据请求模块（DatReplayer）
			 1）根据行情配置文件中的配置，建立与 cme 的 tcp 连接，发送 logon 请求
			 2）接收到 cme 的 logon 成功应答后，发送缺失数据请求
		     3）对接收到的数据逐条处理
				A. 如果该条数据的 sequence_number 已经接收过了，丢弃
				B. 解析该条数据，提取出 mdp message（多条），按 sequence_number 顺序设置给处理者线程，让其保存到内存
				C. 同时需要更新内存中缺失的 sequence number 列表

### 1.5. 产品定义接收者线程的处理
	1.5.1. 网络端口监听模块（UDPReceiver）接收到数据，启动产品定义数据接收模块（RecoverySaver）处理该数据
	1.5.2. 如果该数据的 sequence_number 不满足以下所有条件，则丢弃该数据
			 1）sequence_number == 1
			 2）sequence_number == 当前已接收到的产品定义数据的最大 sequence_number + 1 
	1.5.3. 如果该数据的 sequence_number == 1，那么从中提取产品定义数据的总件数，同时清空内存中保存的已收到的产品定义数据
	1.5.4. 从该数据中提取产品定义情报，保存到内存
	1.5.5. 如果该数据的 sequence_number == 产品定义数据的总件数
			 1）停止所有 increment feed 的监听（意味着停止产品定义数据接收者线程）
			 2）对当前 channel 定义中每一个需要监听的 recovery feed，启动一个线程监听指定的端口（恢复数据接收者线程）

### 1.6. 恢复数据接收者线程的处理
	1.6.1. 网络端口监听模块（UDPReceiver）接收到数据，启动恢复数据接收模块（RecoverySaver）处理该数据
	1.6.2. 如果该数据的 sequence_number 不满足以下所有条件，则丢弃该数据
			 1）sequence_number == 1
			 2）sequence_number == 当前已接收到的恢复数据的最大 sequence_number + 1 
	1.6.3. 如果该数据的 sequence_number == 1，那么从中提取恢复数据的总件数，同时清空内存中保存的已收到的恢复数据
	1.6.4. 从该数据中提取恢复数据，保存到内存
	1.6.5. 如果该数据的 sequence_number == 恢复数据的总件数
			 1）停止所有 recovery feed 的监听（意味着停止恢复数据接收者线程）
			 2) 将接受到的恢复数据和产品定义接收者线程接受到的产品定义数据设置给处理者线程，让其保存到内存

#### 1.7  处理者线程的处理
	1.7.1. 如果内存中没有接受到的行情数据，休眠固定时间，直到接受到第一条行情数据
	1.7.2. 如果第一条行情数据的 sequence number ！= 1， 说明需要处理恢复数据
		     1) 如果内存中还没有恢复数据和产品定义数据，休眠固定时间，直到恢复数据接受完毕
		     2) 将内存中的产品定义数据交给产品定义数据管理模块（DefinitionManager）逐条处理
				    A. 解析出产品定义信息（各种属性：动作（创建，更新，删除等），id，名称，最大深度等等）
				    B. 根据其动作，将该产品定义信息更新到内存
				    C. 通过行情状态数据监听模块将产品定义信息发送到指定端口
				    D. 如果产品深度发生变化，要更新内存中该产品的行情状态数据（book state），将超过该深度的数据丢弃
		     3) 对内存中的恢复数据逐条处理：提取 book 信息，保存到内存，并整理成初期行情状态数据，最后发送到策略端 
		     4) 将内存中的产品定义数据发送到行情配置文件中指定的 zeromq 端口（有另外的进程监听该端口，将数据保存到数据库） 
		     5) 将内存中的恢复数据发送到行情配置文件中指定的 zeromq 端口（有另外的进程监听该端口，将数据保存到数据库）
	1.7.3. 从内存中按顺序提取一条 mdp message 
			 1）如果其 sequence_number > 缺失的 sequence number 列表中最小的，休眠固定时间
			 2）如果（内存中有备份数据情况下）其 sequence_number <= 内存中备份数据的第一条中的 LastMsgSeqNumProcessed 字段，丢弃该数据
	1.7.4. 调用 book 管理模块（BookManager）解析该 mdp message
			 1）解析 mdp message，提取出 book 信息
				A. message type = f 的无需处理
				B. message type = R 的无需处理
				C. message type = d 的是产品定义数据，交给产品定义数据管理模块（DefinitionManager）处理
				   a. 解析出产品定义信息（各种属性：动作（创建，更新，删除等），id，名称，最大深度等等）
				   b. 根据其动作，将该产品定义信息更新到内存
				   c. 通过行情状态数据监听模块将产品定义信息发送到指定端口
				   d. 如果产品深度发生变化，要更新内存中该产品的行情状态数据（book state），将超过该深度的数据丢弃
				D. message type = X 的是行情信息数据，提取 book 信息（多条）
				E. 其他消息类型忽略
			 2）如果有恢复数据需要处理，则需要将上述  book 列表中那些已经在恢复数据中存在的产品的 book 删除
			 4）对整理后的  book 信息列表逐条处理
				A. 如果 MDEntryType == BookReset(J)，则清空内存中所有产品的状态数据
				B. 如果该 book 对应的产品还没有状态数据，将该 book 丢弃
				C. 如果 MDEntryType == Bid 或者 Offer
				   a. 根据 book 信息的 UpdateAction 相应处理对应产品内存中行情状态数据
				   b. 如果行情状态数据发生了变更，则通过行情状态数据监听模块将该产品最新的行情状态信息发送到指定端口
					  数据包括：二级行情；最优价位
				D. 如果 MDEntryType == Trade，则通过行情状态数据监听模块将 trade 信息发送到指定端口
				E. 其他 MDEntryType 类型，直接丢弃
	1.7.5. 将该 mdp message 原始数据发送到行情配置文件中指定的 zeromq 端口（有另外的进程监听该端口，将数据保存到数据库）
	1.7.6. 从内存中删除该条 mdp message
	1.7.7. 如果当前内存中没有剩下的 mdp message 可以处理，休眠固定时间，直到有新的 message 出现，重复 1.7.3 处理

<br>

## 2. 交易指令发送

### 2.1. 

