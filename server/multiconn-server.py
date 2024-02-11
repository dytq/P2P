import sys
import socket
import selectors
import types

sel = selectors.DefaultSelector()

DB = {}
AUTH_DB = {'test':'test\x00'}
SESSION_DB = ['127.0.0.1']

def API(data, addr):
    data = (data.decode())
    data = data.split(',')
    if data[0] == 'L' and addr in SESSION_DB:
        return Lookup(data[1])
    elif data[0] == 'P' and addr in SESSION_DB:
        return Publish(data[1], addr)
    elif data[0] == 'A':
        return Authentication(data[1:],addr)
    else:
        ## Error handling for unknown call
        return b'F'
    

def Session(addr):
    if addr in SESSION_DB:
        return True
    return False

def Lookup(key):
    res = [val for keys, val in DB.items() if key in keys]
    if len(res) == 0:
        return b"F,0.0.0.0"
    else:
        ret = 'R,'+str(res[0])+','+str(key)
    return ret
"""     if res[0] not in DB:
        print("error, {} not found".format(key))
        return b"F,0.0.0.0"
    else:
        ret = "R,"+str(DB[key])+','+str(key)
        return ret.encode('utf-8') """

def Publish(key, addr):
    if key not in DB:
        DB[key] = str(addr)
    else:
        #data = ','+str(addr)
        #DB[key] += data
        DB[key] = str(addr)
    ret = "R,"+str(addr)
    return ret.encode('utf-8')

def Authentication(data,addr):
    uname, passwrd = data[0], data[1]
    if AUTH_DB[uname] == passwrd:
        SESSION_DB.append(addr)
        return b'A,accept'
    return b'A,fail'

def accept_wrapper(sock):
    conn, addr = sock.accept()  # Should be ready to read
    print(f"Accepted connection from {addr}")
    conn.setblocking(False)
    data = types.SimpleNamespace(addr=addr, inb=b"", outb=b"")
    events = selectors.EVENT_READ | selectors.EVENT_WRITE
    sel.register(conn, events, data=data)

def service_connection(key, mask):
    sock = key.fileobj
    data = key.data
    if mask & selectors.EVENT_READ:
        recv_data = sock.recv(1024)  # Should be ready to read
        if recv_data:
            data.outb += recv_data
        else:
            print(f"Closing connection to {data.addr}")
            sel.unregister(sock)
            sock.close()
    if mask & selectors.EVENT_WRITE:
        if data.outb:
            print(f"Processing {data.outb!r} from {data.addr}")
            #print(data.outb)
            #sent = sock.send(data.outb)  # Should be ready to write
            tbflush = len(data.outb)
            #sent = sock.send(b"Ack")
            response = API(data.outb, data.addr[0])
            print(response)
            print(data.outb)
            sent = sock.send(response)
            data.outb = data.outb[tbflush:]
            


host, port = sys.argv[1], int(sys.argv[2])
#host, port = "127.0.0.1", 11111
lsock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
lsock.bind((host, port))
lsock.listen()
print(f"Listening on {(host, port)}")
lsock.setblocking(False)
sel.register(lsock, selectors.EVENT_READ, data=None)

try:
    while True:
        events = sel.select(timeout=None)
        for key, mask in events:
            if key.data is None:
                print("new connection")
                accept_wrapper(key.fileobj)
            else:
                service_connection(key, mask)
except KeyboardInterrupt:
    print("Caught keyboard interrupt, exiting")
finally:
    sel.close()
