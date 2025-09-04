sudo sysctl -w kernel.msgmax=16384   # 将单条消息最大改为 16KB
sudo sysctl -w kernel.msgmnb=65536   # 将单个队列容量改为 64KB
