import struct
from multiprocessing import Process
import multiprocessing as mp
import time
import threading
from socket import *
from Crypto.Cipher import AES
from Crypto.Util.Padding import pad
from Crypto.Util.Padding import unpad
import os
import json
from os.path import join, isfile, getmtime, getsize, exists
import argparse





######################## Nomal #######################
def traverse1(dir_path): # check the current directory and ignore the .lefting files
    file_list = []
    file_folder_list = os.listdir(dir_path)
    for file_folder_name in file_folder_list:
        if isfile(join(dir_path, file_folder_name)):
            if len(file_folder_name)>8:
                if file_folder_name[-8:] == '.lefting':
                    pass
                else:
                    file_list.append(join(dir_path, file_folder_name))
            else:
                file_list.append(join(dir_path, file_folder_name))
        else:
            file_list.extend(traverse1(join(dir_path, file_folder_name)))
    return file_list # return the file list

def traverse2(dir_path): # check if the current directory has .lefting files
    file_list= []
    file_folder_list= os.listdir(dir_path)
    for file_folder_name in file_folder_list:
        if isfile(join(dir_path, file_folder_name)):
            if len(file_folder_name)>8:
                if file_folder_name[-8:] == '.lefting':
                    file_list.append(join(dir_path, file_folder_name))
        else:
            file_list.extend(traverse2(join(dir_path, file_folder_name)))
    return file_list # return the file list

def makeFileDic(file): # make a dictionary for a single file
    dict = {'filename': file,
            'mtime': getmtime(file),
            'filesize': getsize(file)}
    return dict

def makeDictionary(file_list): # make a dictionary list for a file list
    file_dict = []
    for f in file_list:
        dict = {'filename': f,
                  'mtime': getmtime(f),
                  'filesize': getsize(f)}
        file_dict.append(dict)
    return file_dict

def send_dic(socket, sign, dict): # send a file dictionary list to the peer
    dict_json = json.dumps(dict) # change the dictionsry list to json tpye
    dict_bytes = dict_json.encode() # change into binary
    header = {'sign': sign,
              'size': len(dict_bytes)}
    header_json = json.dumps(header) # change the header to json tpye
    header_bytes = header_json.encode()
    headerlen = struct.pack('i', len(header_bytes))
    # send header length
    socket.send(headerlen)
    # send header
    socket.send(header_bytes)
    # send dictionary list
    socket.send(dict_bytes)

def _argparse():
    parser = argparse.ArgumentParser(description="This is description")
    parser.add_argument('--ip', action = 'store', required = True, dest = 'ip', help = 'ip address of peers')
    parser.add_argument('--encryption', action='store', required=False, dest='e', help='encryption')
    return parser.parse_args()

def savedict(dict, filename):
    d = []
    for k in dict:
        d.append(k)
    dict_json = json.dumps(d)
    dict_bytes = dict_json.encode()
    with open(filename,'wb') as f:
        f.write(dict_bytes)

def encryptfile(filename):
    nf = open(filename, 'rb')
    data = nf.read()
    key = b'feimax_zuishai!!'
    iv = b'qwertyuiopasdfgh'
    cipher = AES.new(key, AES.MODE_CBC, iv=iv)
    encrypted_data = cipher.encrypt(pad(data, AES.block_size))
    ef = open(filename + '.lefting', 'wb')
    ef.write(encrypted_data)
    nf.close()
    ef.close()


def decryptfile(filename):
    key = b'feimax zuishai!!'
    iv = b'qwertyuiopasdfgh'
    with open(filename, 'rb') as f:
        encrypted_data = f.read()
    cipher = AES.new(key, AES.MODE_CBC, iv=iv)
    decrypted_data = unpad(cipher.decrypt(encrypted_data), AES.block_size)
    with open(filename, 'wb') as f:
        f.write(decrypted_data)

######################## Scanner #######################

def scanner(local_dict, open_dict, server_port, peer):
    print('I an scanning...')
    while True:
        news = []
        file_list1 = traverse1('share')
        new_dict1 = makeDictionary(file_list1).copy()
        time.sleep(0.1)
        file_list2 = traverse1('share')
        new_dict2 = makeDictionary(file_list2).copy()

        # compare with local_dict, if news, add the new files
        if new_dict1 == new_dict2:
            for f in new_dict2:
                if f not in local_dict:
                    news.append(f)
        if len(news) == 0: # if the length of news is 0, there are no new files
            time.sleep(0.1)
        else:
            print('have new files!!', news)
            socket1 = socket(AF_INET, SOCK_STREAM)
            socket2 = socket(AF_INET, SOCK_STREAM)
            try: # try to connect with peer1, and send news
                socket1.connect((peer[0], server_port))
                send_dic(socket1, 'news', news)
                socket1.close()
            except ConnectionRefusedError:
                print('connection1 fail')
            try: # try to connect with peer2, and send news
                socket2.connect((peer[1], server_port))
                send_dic(socket2, 'news', news)
                socket2.close()
            except ConnectionRefusedError:
                print('connection2 fail')
            for d1 in new_dict2: # update the local_dict
                local_dict.append(d1)
            savedict(local_dict, 'local_dict.log')
            for d2 in news: # update the open_dict
                open_dict.append(d2)
            savedict(open_dict, 'open_dict.log')
        time.sleep(0.5)



######################## Listener #######################
def listener(open_dict, ticket, server_port, encryption):
    server_socket = socket(AF_INET, SOCK_STREAM)
    server_socket.setsockopt(SOL_SOCKET, SO_REUSEADDR, 1)
    server_socket.bind(('', server_port))
    server_socket.listen(30)
    print('I am listening...')
    while True: # listen new requested connections, if there is connection, put it to a new thread
        connection_socket, client_address = server_socket.accept()
        print('Connect with ', client_address)
        thd = threading.Thread(target=subfunc, args=(connection_socket, open_dict, ticket, encryption))
        thd.daemon = True
        thd.start()

def subfunc(connection_socket, open_dict, ticket, encryption):
    # reveice and check the sign
    obj = connection_socket.recv(4)
    header_size = struct.unpack('i', obj)[0]
    header_bytes = connection_socket.recv(header_size)
    header_json = header_bytes.decode()
    header = json.loads(header_json)
    sign = header['sign']
    data_size = header['size']
    dict_bytes = connection_socket.recv(data_size)
    dict_json = dict_bytes.decode()
    dict = json.loads(dict_json)

    if sign == 'hello':
        print('received Hello !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!',dict)
        recv_hello(connection_socket, dict, open_dict, ticket)
    elif sign == 'news':
        print('received news !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!',dict)
        recv_news(dict, open_dict, ticket)
    elif sign == 'get_file':
        print('received get_file !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!',dict)
        get_file(connection_socket, dict, open_dict, encryption)
    connection_socket.close()


def recv_hello(connection_socket, dict, open_dict, ticket):
    print('recv_hello open_dict: ',open_dict)
    d = []
    for k in open_dict:
        d.append(k)
    send_dic(connection_socket, 'news', d)
    for d in dict:
        if d not in open_dict:
            ticket.append(d)

def recv_news(dict, open_dict, ticket):
    for d in dict:
        if d not in open_dict:
            ticket.append(d)

def get_file(connection_socket, dict, open_dict, encryption):
    if dict not in open_dict:
        header = struct.pack('i', 1)
        connection_socket.send(header)
        connection_socket.close()
    else:
        header = struct.pack('i', 0)
        connection_socket.send(header)
        b = 0
        BUFFER_SIZE = 20480
        restsize = dict['filesize']
        f = open(dict['filename'], 'rb')
        while restsize >= BUFFER_SIZE:
            data = f.read(BUFFER_SIZE)
            connection_socket.sendall(data)
            restsize = restsize - BUFFER_SIZE
            b += 1
            print('send:    ',dict['filename'],'     block: ', b, '       rest_size: ', restsize)
        data = f.read(restsize)
        connection_socket.sendall(data)
        f.close()
    connection_socket.close()

######################## Downloader #######################

def downloader(local_dict, open_dict, ticket, server_port, peer, encryption):
    print('Downloader is ready...')
    while True:
        if len(ticket) != 0: # check if there is a ticket
            dict = ticket[0]
            print('downloader, the first ticket: ', dict)
            if dict in open_dict:
                ticket.remove(ticket[0])
            else:
                # make a new file to store the received data
                socket1 = socket(AF_INET, SOCK_STREAM)
                socket2 = socket(AF_INET, SOCK_STREAM)
                sign = [1]
                sign[0] = 0
                BUFFER_SIZE = 20480
                file = dict['filename']
                a = file.split('/')
                path = os.path.abspath('share')+'/'+a[1]############!!!!!!!!!
                if len(a) == 2:
                    f = open(path+'.lefting', 'wb')
                else:
                    folder = exists(path)
                    if not folder:
                        os.makedirs(path)########################!!!!!!!!!!!!
                    f = open(path+'/'+a[2]+'.lefting', 'wb')
                if sign[0] == 0: # connect with peer1 to get the file
                    try:  ###############!!!!!!!!!!!
                        socket1.connect((peer[0], server_port))
                        send_dic(socket1, 'get_file', dict)
                        obj = socket1.recv(4)
                        sign[0] = struct.unpack('i', obj)[0]
                        if sign[0] == 0:
                            recv_size = 0
                            b = 0
                            filesize = dict['filesize']
                            print('file size: ', filesize)  #
                            while True:
                                data = socket1.recv(BUFFER_SIZE)
                                recv_size = recv_size + len(data)
                                f.write(data)
                                if recv_size == filesize:
                                    break
                                b = b + 1
                                print('filename: ', dict['filename'],'   block: ', b, '      recv_size: ', recv_size)
                            print('rsize:', recv_size,' tsize:', dict['filesize'])
                            socket1.close()
                    except:
                        print('connection1 fail')
                        sign[0] = 1

                if sign[0] == 1: # if peer1 does not have the file, connect with peer2
                    try:  ###############!!!!!!!!!!!
                        socket2.connect((peer[1], server_port))
                        send_dic(socket2, 'get_file', dict)
                        obj = socket2.recv(4)
                        sign[0] = struct.unpack('i', obj)[0]
                        if sign[0] == 0:
                            recv_size = 0
                            b = 0
                            filesize = dict['filesize']
                            print('file size: ', filesize)  #
                            while True:
                                data = socket2.recv(BUFFER_SIZE)
                                recv_size = recv_size + len(data)
                                f.write(data)
                                if recv_size == filesize:
                                    break
                                b = b + 1
                                print('filename: ', dict['filename'],'   block: ', b, '      recv_size: ', recv_size)
                            print('rsize:', recv_size,' tsize:', dict['filesize'])
                            socket2.close()
                    except:
                        print('connection2 fail')

                if sign[0] == 0:
                    f.close()
                    open_dict.append(ticket[0])
                    dic = makeFileDic(dict['filename'] + '.lefting')
                    dic['filename'] =  dict['filename']##################改顺序
                    local_dict.append(dic)

                    print('start rename ' + dict['filename'] + '.lefting ---> ' + dict['filename'])  #
                    os.renames(dict['filename'] + '.lefting', dict['filename'])
                    print('rename finished')  #

                    savedict(open_dict, 'open_dict.log')
                    savedict(local_dict, 'local_dict.log')
                    ticket.remove(ticket[0])
        else:
            time.sleep(0.1)

######################## Say_Hello #######################
def Say_Hello():
    socket1 = socket(AF_INET, SOCK_STREAM)
    socket2 = socket(AF_INET, SOCK_STREAM)
    try: # try to connect with peer1
        socket1.connect((peer[0], server_port))
        print('successfully connect with ', peer[0])
        d = []
        for k in open_dict:
            d.append(k)
        send_dic(socket1, 'hello', d)
        obj = socket1.recv(4)
        header_size = struct.unpack('i', obj)[0]
        header_bytes = socket1.recv(header_size)
        header_json = header_bytes.decode()
        header = json.loads(header_json)
        data_size = header['size']
        dict_bytes = socket1.recv(data_size)
        dict_json = dict_bytes.decode()
        dict = json.loads(dict_json)
        recv_news(dict, open_dict, ticket)
        socket1.close()
    except:
        print('connection1 fail')
    try: # try to connect with peer2
        socket2.connect((peer[1], server_port))
        print('successfully connect with ', peer[1])
        d = []
        for k in open_dict:
            d.append(k)
        send_dic(socket2, 'hello', d)
        obj = socket2.recv(4)
        header_size = struct.unpack('i', obj)[0]
        header_bytes = socket2.recv(header_size)
        header_json = header_bytes.decode()
        header = json.loads(header_json)
        data_size = header['size']
        dict_bytes = socket2.recv(data_size)
        dict_json = dict_bytes.decode()
        dict = json.loads(dict_json)
        recv_news(dict, open_dict, ticket)
        socket2.close()
    except:
        print('connection2 fail')


######################## Main ########################
if __name__ == '__main__':
    parse = _argparse()
    encryption = parse.e
    peer = parse.ip.split(',')
    server_port = 22122

    # if there is no 'share' folder, make one
    if not exists('share'):
        os.makedirs('share')

    # if there are no local_dict.log and open_dict.log, make them
    if exists('local_dict.log') and exists('open_dict.log'):
        with open('local_dict.log','rb') as f1:
            dict_bytes = f1.read()
            dict_json = dict_bytes.decode()
            local_dict = json.loads(dict_json)
        with open('open_dict.log','rb') as f2:
            dict_bytes = f2.read()
            dict_json = dict_bytes.decode()
            open_dict = json.loads(dict_json)
    # if they exist, scan them and store the data in local_dict and open_dict
    else:
        initial_file_list = traverse1('share')
        local_dict = makeDictionary(initial_file_list).copy()
        open_dict = local_dict.copy()
        savedict(local_dict,'local_dict.log')
        savedict(open_dict, 'open_dict.log')

    # if there are lefting files, remove them
    leftingfiles = traverse2('share')
    for leftfile in leftingfiles:
        os.remove(leftfile)

    local_dict = mp.Manager().list(local_dict)
    open_dict = mp.Manager().list(open_dict)
    ticket = mp.Manager().list()

    Say_Hello()
    p1 = Process(target=listener, args=(open_dict, ticket, server_port, encryption))
    p2 = Process(target=scanner, args=(local_dict, open_dict, server_port, peer))
    p3 = Process(target=downloader, args=(local_dict, open_dict, ticket, server_port, peer, encryption))
    p1.start()
    p2.start()
    p3.start()


    p3.join()
    p2.join()
    p1.join()