1. s->c:	质询
2. c->s:       hash2(hash1(username,passwd) , 质询)
3. s:            hash3(hash2 , 质询)



口令更新

- hash1(username , old-passwd)
- hash2(username , new-passwd)
- 用hash1加密hash2,send to server
- server decode hash2 with known hash1







电子信封：用接收方的公钥加密的会话密钥

数字签名：用发送方私钥加密的信息的散列值