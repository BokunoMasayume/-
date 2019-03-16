
# coding: utf-8

# python c:\users\10953\desktop\conn.py  -p 1-65535 -t 200 127.0.0.1

# In[12]:


import subprocess
import threading


# In[13]:


import tkinter as tk


# In[14]:


scan_process=None
#child = subprocess.Popen('python c:\\users\\10953\\desktop\\conn.py  -p 1-65535 -t 500 127.0.0.1',stdin=subprocess.PIPE,stdout=subprocess.PIPE,shell=True)
#for l in iter(child.stdout.readline,''):
 #   if l!=b'':
 #       print(l.rstrip())


# In[36]:


root = tk.Tk()
root.title("TCP端口扫描")
root.geometry('500x300')


# In[37]:


host_var = tk.StringVar()
host_var.set("127.0.0.1")

port_start_var = tk.StringVar()
port_start_var.set(8000)

port_end_var = tk.StringVar()
port_end_var.set(9000)

thread_count_var = tk.StringVar()
thread_count_var.set(50)

host_label = tk.Label(root,text="主机ip:")
host_input = tk.Entry(root,show=None,textvariable=host_var)

port_label = tk.Label(root , text="端口范围:")
port_start_input = tk.Entry(root,show=None,textvariable=port_start_var,width=7)

port_end_input = tk.Entry(root,show=None,textvariable=port_end_var,width=7)

thread_count_label = tk.Label(root,text="使用线程数目：",width=10)
thread_count_input = tk.Entry(root,show=None,textvariable=thread_count_var)
def start_scan():
    global scan_process
    cmd_handle = "python c:\\users\\10953\\desktop\\conn.py  "
#    if scan_process != None:
#        scan_process.kill()
    
    host_str = host_var.get()
    port_start_str = port_start_var.get()
    port_end_str = port_end_var.get()
    
    cmd_str = cmd_handle+" "+host_str+" -t "+thread_count_var.get()+" -p "
    if port_start_str=='' or port_end_str=='':
        cmd_str += port_start_str if port_start_str!='' else port_end_str
    else:
        cmd_str += (port_start_str+"-"+port_end_str)
    
    scan_text.insert(1.0,cmd_str+'\n')
    
    if scan_process !=None and scan_process.poll()==None:
        scan_process.kill()
    scan_process = subprocess.Popen(cmd_str,stdin=subprocess.PIPE,stdout=subprocess.PIPE,shell=True)
    while scan_process.poll() is None :
        sss=scan_process.stdout.readline().strip().decode()
        if sss:
            scan_text.insert(1.0,sss+'\n')
        #print(scan_process.stdout.readline().strip().decode()+'\n')

    return

def scan():
    threading.Thread(target = start_scan).start()
    

    

scan_button = tk.Button(root,text="扫描",command=scan)

scan_text = tk.Text(root)

host_label.place(x=20,y=20)
host_input.place(x=120,y=20)

port_label.place(x=20,y=50)
port_start_input.place(x=120,y=50)
port_end_input.place(x=200,y=50)

scan_button.place(x=350,y=20)

scan_text.place(x=20,y=120,width=460,height=170)

dash_label = tk.Label(root,text="-")
dash_label.place(x=180,y=50)

scroll = tk.Scrollbar(root)
scroll.place(x=480,y=120,width=10,height=170)

scroll.config(command=scan_text.yview)
scan_text.config(yscrollcommand=scroll.set)

thread_count_label.place(x=20,y=80)
thread_count_input.place(x=120,y=80)
root.mainloop()

