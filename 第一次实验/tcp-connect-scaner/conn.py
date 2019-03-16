
# coding: utf-8

# In[1]:



import time
import threading
import socket
import sys


# In[2]:


i_host = '127.0.0.1'
i_port = 8887

host_l=[]
port_l=[]
next_host=[172,20,0,248]
host_p = 0
next_port=80
port_p=0

thread_count=1


# In[3]:


threadLock = threading.Lock()


# In[ ]:


class conn_thread(threading.Thread):
    def __init__(self,name):
        threading.Thread.__init__(self)
        self.name=name
        
    def run(self):
        global next_host
        global next_port
        global host_p
        global port_p
        global host_l
        global port_l
        host=[127,0,0,1]
        port=80
        def get_target():
            global next_host
            global next_port
            global host_p
            global port_p
            global host_l
            global port_l
            nonlocal host
            nonlocal port
            host = next_host
            port = next_port
            def set_next_host():
                global next_host
                global next_port
                global host_p
                global port_p
                global host_l
                global port_l
                nonlocal host
                nonlocal port
                for i in range(4):
                    if isinstance(host_l[host_p][i],list):
                        if next_host[i] < host_l[host_p][i][1]:
                            next_host[i]+=1
                            return
                
                host_p+=1
                if host_p < len(host_l):
                    for i in range(4):
                        if isinstance(host_l[host_p][i],list):
                            next_host[i]=host_l[host_p][i][0]
                        else:
                            next_host[i]=host_l[host_p][i]
                return
            
            
            
            if isinstance(port_l[port_p],list) and next_port<port_l[port_p][1]:
                next_port+=1
            elif port_p < (len(port_l)-1):
                port_p+=1
                next_port=port_l[port_p][0] if isinstance(port_l[port_p],list) else port_l[port_p]
            else:
                set_next_host()
                port_p=0
                next_port=port_l[port_p][0] if isinstance(port_l[port_p],list) else port_l[port_p]
            return
        
        
        while True:
            if host_p >= len(host_l):
                return
            threadLock.acquire()
            get_target()
            threadLock.release()
            if host_p >= len(host_l):
                return
            
            host_str = ".".join(map(str,host))
            try:
                sock = socket.socket(socket.AF_INET ,socket.SOCK_STREAM)
                sock.settimeout(1)
                sock.connect((host_str,port))
                threadLock.acquire()
                #print(self.name," --success",host_str,":",port)
                sys.stdout.write(str(self.name)+" --success"+host_str+":"+str(port)+"\n")
                sys.stdout.flush()
                threadLock.release()
            except socket.error as e:
                #print(self.name ,e,host_str,":",port)
                pass
            finally:
                sock.close()
                
                    
            
                
            
            


# In[ ]:


def cook_command(cmd_l):
    #cmd_l = str.split()
    #print(cmd_l)
    sys.stdout.write(",".join(cmd_l)+"\n")
    sys.stdout.flush()
    global thread_count
    global host_l
    global port_l
    r_host_l=[]
    r_port_l=[]
    
    i=0
    while i <len(cmd_l):
        if cmd_l[i] == '-p':
            r_port_l.append(cmd_l[i+1].split('-'))
            i+=2
        elif cmd_l[i] =='-t':
            thread_count = int(cmd_l[i+1])
            i+=2
        else:
            r_host_l.append(cmd_l[i].split('.'))
            i+=1

   
    i=0
    while i < len(r_port_l):
        if len(r_port_l[i])==1:
            port_l.append(int(r_port_l[i][0]))
            i+=1
        elif len(r_port_l[i]) ==2:
            port_l.append(list(map(int,r_port_l[i])))
            i+=1
    i=0
    while i < len(r_host_l):
        ip_l=[]
        r_ip=r_host_l[i]
        for j in r_ip:
            tmp = j.split('-')
            if len(tmp)==1:
                ip_l.append(int(tmp[0]))
            elif len(tmp)==2:
                ip_l.append(list(map(int,tmp)))
        i+=1
        host_l.append(ip_l)
    return
            
            
        
        
            
            
            
    
    
    


# In[ ]:


def init_host_port():
    global port_p
    global host_p
    global post_l
    global host_l
    global next_port
    global next_host
    
    port_p=0
    host_p=0
    if isinstance(port_l[port_p],int):
        next_port=port_l[port_p]
    else:
        next_port=port_l[port_p][0]
    
   
    for i in range(4):
        if isinstance(host_l[host_p][i],int):
            next_host[i]=host_l[host_p][i]
        else:
            next_host[i]=host_l[host_p][i][0]
    return


# In[ ]:


def insert_cmd():
    str = input("input :")
    return str


# In[ ]:


#cook_command(insert_cmd())
cook_command(sys.argv[1:])
init_host_port()
thread_pool=[]
for i in range(thread_count):
    t = conn_thread(i)
    t.start()
    thread_pool.append(t)
for i in thread_pool:
    i.join()
#print("完成")
sys.stdout.write("done")
sys.stdout.flush()

