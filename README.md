<center>计算棒数据透传指南</center>


准备

   - **系统环境**
	
		1. 切换到root用户
		2. 拷贝(如果bin是从win拷贝到计算棒需要注意权限)更新程序到/usr/bin/ (后续会以固件或升级包的形式更新)
		
			rm -rf /usr/bin/toybrickd 
			cp usr/bin/* /usr/bin/
		3. 安装rknn-toolkit 1808 python版本
		
			pip3 install rknn_toolkit-0.9.8.1-cp36-cp36m-linux_aarch64.whl
		4. 重启计算棒

python
   
   - **yolov3**
   
		1. device服务启动
		
			python3 yolov3_server.py 

   - **mobilenet-ssd**

		1. device服务启动
		
			python3 ssd_server.py


c++  

   - **当前只实现host端和device端通信**
