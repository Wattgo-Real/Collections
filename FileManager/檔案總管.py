import os,shutil,time,stat

is_fileMode = True
is_wordMode = False
ppppp = 1
okkkk = 1

#-----------------------------------------------------------讀入存檔
print("此檔案總管會新增一個c:\\檔案總管--\\save.txt 文件去紀錄你的使用設定")
print("剛開始先設定你要使用的硬盤空間(必須，打入數字選擇)")
if os.path.exists("c:\\檔案總管--\\save.txt") == False:
    now = []
    now_Remember = ["" for x in range(50)]
    now_Remember_Now = 0
    sort = "off"
    show = {"date":False,"size":False,"extension":True}
    sort_ud = "u"
    first_Open = True
    use_ = {"copy": "" , "cut": "" } 
else:
    with open("c:\\檔案總管--\\save.txt","rt",encoding="utf-8") as save:
        pnp = save.readlines()
        for i in pnp:
            a = i.rsplit("\n",1)
            a = a[0]
            a = exec(a)
    ao = now[0].rsplit("\\",1)
    os.chdir(ao[0])
    okkkk = 0

#-----------------------------------------------------------幫助

help_ = ["/add <dir,text> <命名>    -新增"  ,
         "/rename <數字(選擇)> <命名>    -重新命名檔案",
         "/remove <數字(選擇)>    -移除檔案",
         "/copy <數字(選擇)>    -複製檔案",
         "/cut <數字(選擇)>    -剪下檔案",
         "/paste <co(貼上複製的檔案),cu(貼上剪下的檔案)> [數字(選擇) 默認為-目前的資料夾]    -貼上檔案",
         "/sort <方法(off(關閉功能),d(日期),s(大小,不用於資料夾),e(副檔名),n(名稱),)> [u(遞增),d(遞減)默認為-u]    -分類檔案",
         "/show <方法(d(日期),s(大小,不用於資料夾),e(副檔名),)> [off(關閉),on(開啟)默認為-on]    -詳細顯示" ,
         "/find <名稱>    -尋找檔案(僅限此資料夾內)",
         "/re    -重新整理",
         "/drive    -回到最初的位置(選擇槽位)",
         "/b [數字(次數)默認為-列出前十個瀏覽紀錄]    -回上一個搜尋的資料夾",
         "/f [數字(次數)默認為-列出後十個瀏覽紀錄]    -回下一個搜尋的資料夾",
         "/save    -紀錄目前設定",
         "/close    -關閉檔案總管",
         "/renamee <數字(選擇)> <命名>    -修改副黨名",
         "/findt <名稱>    -尋找檔案(資料夾以及所有的子資料夾)",
         "// <檔案位置>    -直接搜尋",
         "輸入數字來進入 or 開啟檔案",
         "<>符號一定要有值,但[]則不用\n"]

#-----------------------------------------------------------控制方法

def start_(now):
    drive_0 = ["c:\\" , "d:\\", "e:\\", "f:\\"]
    drive_exist = [os.path.isdir("c:\\"),
                   os.path.isdir("d:\\"),
                   os.path.isdir("e:\\"),
                   os.path.isdir("f:\\")]
    now = []
    for i in range(len(drive_0)):
        if drive_exist[i] == True:
            print(i,".",drive_0[i])
            now.append(drive_0[i])
    return(now)#啟動 **
    
def path_in(now,i):   #輸入你想要進入的資料夾，加負號倒退
    if i >= 0:                   
        if os.path.isfile(now[i]) == True:
            try:
                os.startfile(now[i])
            except:
                print("錯誤: 無格式可開啟這檔案")
            return(now)#開啟檔案
    if i >= 0:
        try:                     #檢查權限
            if os.listdir(path=now[i]):
                file = os.listdir(path=now[i])
                p = now[i]
            else:
                file = []
            os.chdir(now[int(point)])
        except:
            print("\n錯誤: 無權限")
            return(now)
    else:                         #倒退
        m = os.getcwd().rsplit("\\",int(str(i).lstrip('-')))
        m[0] = m[0]+"\\"
        os.chdir(m[0])
        file = os.listdir(path=m[0])
        p = m[0]

    remember_Now()            #紀錄now

    now = []
    if len(file) == 0:            #檢測資料夾中是否有檔案
        
        print("\n沒東西 ¦3[▓▓]")
    else:  
        for i in range(len(file)):
            now.append(os.path.join(p,file[i]))    #輸出
        now = print_now(now)
    return(now)#讀入資料夾 ****

def remember_Now():
    global now_Remember,now_Remember_Now
    if now_Remember_Now == 0:
        for s in range(len(now_Remember)-1,0,-1):
            now_Remember[s] = now_Remember[s-1]
        now_Remember[0] = os.getcwd()
    elif now_Remember_Now > 0:
        for s in range(now_Remember_Now,len(now_Remember),1):
            now_Remember[s-now_Remember_Now+1] = now_Remember[s]
            now_Remember[s] = "" 
        now_Remember[0] = os.getcwd()
    now_Remember_Now = 0# /b and /f 指令-紀錄的排序 ****

def path_re_now(now):
    global now_Remember,now_Remember_Now
    os.chdir(now_Remember[now_Remember_Now])
    p = now_Remember[now_Remember_Now]
    file = os.listdir(p)
    now = []
    for i in range(len(file)):
        now.append(os.path.join(p,file[i]))
    now = print_now(now)
    return(now)#重新讀入 now **

def print_now(now):
    global sort,sort_ud,show
    pp = []
    setnow = now
    if sort == "s":
        for p in range(len(now)):
            if os.path.isdir(now[p]) == False:
                pp.append([setnow[p],os.path.getsize(now[p])])
                setnow[p] = ""
        setnow = [item for item in setnow if item != ""]
        pp = sortt(pp,sort_ud)
        for p in pp:
            setnow.append(p[0])

    elif sort == "d":
        for p in range(len(now)):
            date  = os.stat(now[p])
            pp.append([setnow[p],date.st_mtime])
            setnow[p] = ""
        setnow = [item for item in setnow if item != ""]
        pp = sortt(pp,sort_ud)              
        for p in pp:
            setnow.append(p[0])
        
    elif sort == "e":
        for p in range(len(now)):
            if os.path.isfile(now[p]):
                n = now[p].rsplit(".",1)
                pp.append([setnow[p],n[1]])
                setnow[p] = ""
        setnow = [item for item in setnow if item != ""]
        pp = sortt(pp,sort_ud)              
        for p in pp:
            setnow.append(p[0])

    elif sort == "n":
        for p in range(len(now)):
            pp.append([setnow[p],setnow[p]])
            setnow[p] = ""
        setnow = [item for item in setnow if item != ""]
        pp = sortt(pp,sort_ud)              
        for p in pp:
            setnow.append(p[0])

    now = setnow

    for i in range(len(now)):
        showw = ""
        p = now[i].rsplit("\\",1)
        p = p[1]
        if show["extension"] == False and os.path.isfile(now[i]):
            p = p.rsplit(".",1)
            p = p[0]
        if show["date"]:
            date  = os.stat(now[i])
            showw =  showw+ "  -  " +date_(time.localtime(date.st_mtime))
        if show["size"] and os.path.isfile(now[i]):

            showw = showw +"  -  "+ str(size_(os.path.getsize(now[i])))

        print(i,".",p,showw)
            
    return(now)#排序檔案 ****

def date_(m):
    y = str(m.tm_year) + "年" 
    mo = str(m.tm_mon) + "月" 
    d = str(m.tm_mday) + "日" 
    h = str(m.tm_hour) + "時" 
    mi = str(m.tm_min) + "分" 
    s = str(m.tm_sec) + "秒" 
    return y+mo+d+h+mi+s#日期格式 *

def sortt(pp,sort_ud):
    if sort_ud =="u":
        pp.sort(key=lambda x: x[1])
    if sort_ud =="d":
        pp.sort(key=lambda x: x[1],reverse=True) 
    return(pp)# /sort 分類方向 *

def size_(s):
    if 500 >= s:
        return str(s) + " bite"
    if 500000 >= s:
        return str(round(s/1024,3))+ "KB"
    if 500000000 >= s:
        return str(round(s/(1024**2),3))+ "MB"
    if 500000000000 >= s:
        return str(round(s/(1024**3),3))+ "GB"
    if 500000000000000 >= s:
        return str(round(s/(1024**4),3))+ "TB"#大小判定 *

def np_(now):
    np = []
    for i in range(len(now)):
        a = now[i].rsplit("\\",1)
        a = a[1]
        np.append(a)
    return(np)

def authority_checking(s):

    os.chmod(s, stat.S_IWRITE)

def canName(name):
    wrong = ["\\","/",":",r'"',"?","*","<",">","|"]
    for i in wrong:
        if name.find(i) != -1:
            print(r"錯誤: 不可有\/:^?*<>|符號")
            return False
    return True#偵測名稱是否正確 *

#-----------------------------------------------------------方法類別

def add_(now,n,p):
    a = 0
    np = np_(now)
    if n == "dir" and canName(p) == True:
        while p in np:  #**************important****************
            p = p + "-複製"
        else:
            os.mkdir(os.path.join(os.getcwd(),p))
            a = 1
    if n == "text" and canName(p) == True:
        p = p + ".txt"
        while p in np:  #**************important****************
            p = p.rstrip(".txt")
            p = p + "-複製.txt"
        else:
            with open(os.path.join(os.getcwd(),p),"wt",encoding="utf-8") as ppp:
                ppp = ppp
            a = 1
    if a == 1:
        print("系統: 建立成功\n")
        now = path_re_now(now)

    return(now)          #新增的指令團  dir *

def rename_(now,i,m):
    p = np_(now)
    if os.path.isdir(now[int(i)]) == False:
        a = now[int(i)].rsplit(".",1)
        m = m + "." +a[1]
    if canName(m) == True:
        if m in p:
            print("錯誤: 名稱相同")
        else:
            os.rename(now[int(i)],os.path.join(os.getcwd(),m))
            now = path_re_now(now)
            print("更改完成\n")
    return(now)          #重新命名  finish *

def remove_(now,m):
    print("是否刪除",now[int(m)],"  檔案，輸入YES")
    a = input(": ")
    if a=="YES":
        if os.path.isdir(now[int(m)]):
            shutil.rmtree(now[int(m)], onerror=readonly_handler)
        else:
            os.chmod(now[int(m)], stat.S_IWRITE)
            os.remove(now[int(m)])
        print(now[int(m)]," 已成功移除\n")
    else:
        print(now[int(m)]," 未成功移除\n")
    now = path_re_now(now)
    return now          #移除檔案  finish *

def readonly_handler(func, path, execinfo):
    os.chmod(path, stat.S_IWRITE)
    func(path)          #修改移除檔案的權限(危險!)  finish

def copy_(now,i):       
    global use_
    use_["copy"] = now[int(i)]
    print("複製了",use_["copy"],"檔案")
    return(now)          #複製檔案  finish *

def cut_(now,i):        
    global use_
    use_["cut"] = now[int(i)]
    print("剪下了",use_["cut"],"檔案")
    return(now)          #剪下檔案  finish *

def paste_(now,m,k=-1):       
    global use_
    pppp = now
    if k == -1:
        qp = os.getcwd()
    else:
        qp = now[int(k)]
        try:
            if os.path.isdir(now[int(k)]):
                pp = os.listdir(path=now[int(k)])
                pppp = []
                for i in range(len(pp)):
                    pppp.append(os.path.join(qp,pp[i]))
            else:
                print("只能選資料夾文件內剪下貼上")
                return(now)
        except:
            print("\n錯誤: 無權限")
            return(now)
    np = np_(pppp)
    if use_["copy"] != "" and m == "co":
        if os.path.exists(use_["copy"]) == False:
            use_["copy"] = ""
            print("錯誤: 複製的檔案消失\n")
            return(now)
        p = use_["copy"].rsplit("\\",1)
        if p[1] in np:
            if os.path.isdir(use_["copy"]):
                shutil.copytree(use_["copy"],qp+"\\"+"1q2w3e4q5w6e7q8w9e0q1w2e3q4w5e6")
                while p[1] in np:
                    p[1] = p[1] + "-複製"
                os.rename(qp + "\\" + "1q2w3e4q5w6e7q8w9e0q1w2e3q4w5e6",qp + "\\" + p[1])
            else:
                os.mkdir(qp+"\\"+"1q2w3e4q5w6e7q8w9e0q1w2e3q4w5e6")
                shutil.copy(use_["copy"],qp+"\\"+"1q2w3e4q5w6e7q8w9e0q1w2e3q4w5e6")
                ok = p[1]
                while p[1] in np:
                    p2 = p[1].rsplit(".")
                    p2[0] = p2[0] + "-複製"
                    p[1] = p2[0] +"."+ p2[1]
                os.rename(qp + "\\" + "1q2w3e4q5w6e7q8w9e0q1w2e3q4w5e6" +"\\"+ ok,qp + "\\" +"1q2w3e4q5w6e7q8w9e0q1w2e3q4w5e6"+"\\"+ p[1])
                shutil.move(qp+"\\"+"1q2w3e4q5w6e7q8w9e0q1w2e3q4w5e6"+"\\"+p[1],qp)
                os.rmdir(qp+"\\"+"1q2w3e4q5w6e7q8w9e0q1w2e3q4w5e6")
        else:
            if os.path.isdir(use_["copy"]):
                shutil.copytree(use_["copy"],qp+ "\\" + p[1])
            else:
                shutil.copy(use_["copy"],qp)
        now = path_re_now(now)
        print("貼上成功\n")
    elif m == "co":
        print("錯誤: 無複製文件")
    if use_["cut"] != "" and m == "cu" and qp.find(use_["cut"]) == -1:
        if os.path.exists(use_["cut"]) == False:
            use_["cut"] = ""
            print("錯誤: 剪下的檔案消失")
            return(now)
        p = use_["cut"].rsplit("\\",1)
        if len(p[0]) == 2:
            p[0] = p[0] + "\\"

        if p[1] in np:
            if qp != p[0]:
                os.mkdir(qp+"\\"+"1q2w3e4q5w6e7q8w9e0q1w2e3q4w5e6")
                shutil.move(use_["cut"],qp+"\\"+"1q2w3e4q5w6e7q8w9e0q1w2e3q4w5e6")
                aa=1
                while aa==1:
                    name = input("錯誤: 檔名相同，請重新命名: ")
                    print("\n")
                    if canName(name):
                        if (name in np) == False:
                            aa=2
                            os.rename(qp+"\\"+"1q2w3e4q5w6e7q8w9e0q1w2e3q4w5e6"+"\\"+p[1],qp+"\\"+"1q2w3e4q5w6e7q8w9e0q1w2e3q4w5e6"+"\\"+name)
                            p[1] = name
                    else:
                        print("名稱錯誤請重新輸入\n")
                shutil.move(qp+"\\"+"1q2w3e4q5w6e7q8w9e0q1w2e3q4w5e6"+"\\"+name,qp)
                os.rmdir(qp+"\\"+"1q2w3e4q5w6e7q8w9e0q1w2e3q4w5e6")
        else:
            shutil.move(use_["cut"],qp)
        use_["cut"] = qp+"\\"+p[1]
        now = path_re_now(now)
        print("剪下貼上成功\n")
    elif m == "cu" and qp.find(use_["cut"]) == -1:
        print("錯誤: 無複製文件")
    elif m == "cu" and qp.find(use_["cut"]) == 0:
        print("錯誤: 無法剪下檔案貼在你選擇或是你目前的資料夾因為那個在那個檔案的裏面")
    return(now)          #貼上檔案  finish ******

def sort_(now,i,m = "u"):
    global sort,sort_ud
    if i == "off":
        sort = "off" 
    elif i == "s":
        sort = "s" 
    elif i == "d":
        sort = "d"
    elif i == "e":
        sort = "e"
    elif i == "n":
        sort = "n"
    else:
        print("錯誤: 無此功能")
    if m == "u":
        sort_ud = "u"
    elif m == "d":
        sort_ud = "d"

    now = print_now(now)
    return(now)          #排序檔案  finish ***

def show_(now,i,m = "on"):
    global show
    p = 0
    if i == "s" and m == "on":
        show["size"] = True
        p += 1
    elif i == "s" and m == "off":
        show["size"] = False
        p += 1
    if i == "d" and m == "on":
        show["date"] = True
        p += 1
    elif i == "d" and m == "off":
        show["date"] = False
        p += 1
    if i == "e" and m == "on":
        show["extension"] = True
        p += 1
    elif i == "e" and m == "off":
        show["extension"] = False
        p += 1
    if p == 0:
        print("錯誤: 無此顯示方法\n")
    now = print_now(now)
    return(now)          #詳細顯示 finish ***

def find_(now,i):
    p = []
    for o in now:
        n = o.rsplit("\\",1)
        n = n[1]
        if n.find(i) != -1:
            p.append(o)
    if p == []:
        print("\n沒東西 ¦3[▓▓]")
    now = p
    now = print_now(now)
    return(now)          #小尋找 finish **

def findt_(now,i):
    p = []
    for root, dirs, files in os.walk(os.getcwd(), topdown=True):
        for name in files:
            if name.find(i) != -1:
                p.append(os.path.join(root, name))
        for name in dirs:
            if name.find(i) != -1:
                p.append(os.path.join(root, name))
    if p == []:
        print("\n沒東西 ¦3[▓▓]")
    now = p
    now = print_now(now)
    return(now)          #大尋找 finish **

def re_(now):
    now = path_re_now(now)
    return(now)          #重新整理 finish *

def drive_(now):    
    now = start_(now)
    return(now)          #回到最初的位置(選擇槽位) *

def back_(now,i = -1):
    global now_Remember,now_Remember_Now
    if i != -1:
        now_Remember_Now = now_Remember_Now + i
        if now_Remember_Now < 50 and now_Remember[now_Remember_Now] != "":
            now = path_re_now(now)
        else:
            now_Remember_Now = now_Remember_Now - i
            print("錯誤: 超出記憶範圍")
    elif i == -1:
        m = 10
        if now_Remember_Now > 39:
            m = 49 - now_Remember_Now
        for p in range(m):
            print(now_Remember[now_Remember_Now+p+1],end="->\n")
        if m == 0:
            print("無")
    return(now)          #回剛剛上一個搜尋的資料夾 **

def forward_(now,i = -1):
    global now_Remember,now_Remember_Now
    if i != -1:
        now_Remember_Now = now_Remember_Now - i
        if now_Remember_Now >= 0:
            now = path_re_now(now)
        else:
            now_Remember_Now = now_Remember_Now + i
            print("錯誤: 不在記憶範圍內")
    elif i == -1:
        m = 10
        if now_Remember_Now < 10:
            m = now_Remember_Now
        for p in range(m):
            print(now_Remember[now_Remember_Now-p-1],end="->\n")
        if m == 0:
            print("無")
    return(now)          #回剛剛下一個搜尋的資料夾 **

def save_():
    global now_Remember,now_Remember_Now,sort,show,sort_ud,first_Open,use_,now
    if os.path.exists("c:\\檔案總管--") == False:
        os.mkdir("c:\\檔案總管--")
    with open("c:\\檔案總管--\\save.txt","wt",encoding="utf-8") as save:
        save.write("first_Open="+str(first_Open)+"\n")
        save.write("now="+str(now)+"\n")
        save.write("now_Remember="+str(now_Remember)+"\n")
        save.write("now_Remember_Now="+str(now_Remember_Now)+"\n")
        save.write("sort="+'"'+str(sort)+'"'+"\n")
        save.write("sort_ud="+'"'+str(sort_ud)+'"'+"\n")
        save.write("show="+str(show)+"\n")
        save.write("use_="+str(use_)+"\n")
    print("儲存成功!")
    return(now)          #紀錄你目前所有的調整 ****

def renamee_(now,i,m):
    if os.path.isdir(now[int(i)]) == True:
        print("錯誤: 無意義")
    else:
        p = now[int(i)].rsplit(".",1)
        p = p[0]
        if canName(m) == True:
            if m.rfind(".") == 0:
                p = p+m
                os.rename(now[int(i)],p)
                now = path_re_now(now)
                print("更改完成\n")
            else:
                print("錯誤: 格式錯誤")
    return(now)          #修改副黨名  finish *

def path_(now,i):
    if os.path.isdir(i):
        os.chdir(i)
        remember_Now()
        now = path_re_now(now)
    else:
        os.open(i)
    return(now)
#-----------------------------------------------------------
        
while 1:
    if now_Remember[0] == "":
        now_Remember[0] = "c\\"
    if first_Open == True:
        now = start_(now)
        first_Open = False
    elif ppppp == 1 and first_Open == False and okkkk == 0:
        now = path_re_now(now)
        print("\n目前位置: ",os.getcwd(),"\n")
    ppppp = 0
    point = (input(": "))
    print("\n")
    if False == True:
        p = 0
    elif is_fileMode == True and point.lstrip('-').isdigit():
        if int(point) < len(now):
            okkkk = 0
            now = path_in(now,int(point))
    elif point.find("/") == 0 and okkkk == 0:
        if point.find("//") == 0:
            if point.find("// ") == 0:
                p = point.split(" ",1)
                if os.path.exists(p[1]):
                    now = path_(now,p[1])
            point = ""
        elif point.find("/add") == 0:
            if point.find("/add dir ") == 0:
                p = point.split(" ",2)
                now = add_(now,p[1],p[2])
            if point.find("/add text ") == 0:
                p = point.split(" ",2)
                now = add_(now,p[1],p[2])
            else:
                print(help_[0])
            point = ""
        elif point.find("/b") == 0:
            p = point.rsplit(" ")
            if point == "/b":
                now = back_(now)
            elif len(p) == 2:
                now = back_(now,int(p[1]))
            else:
                print(help_[11])
            point = ""
        elif point.find("/close") == 0:
            if point == "/close":
                save_()
                break
            else:
                print(help_[14])
            point = ""
        elif point.find("/copy") == 0:
            p = point.rsplit(" ")
            if len(p) == 2 and p[1].isdigit():
                if int(p[1]) <= len(now)-1:
                    now = copy_(now,p[1])
                else:
                    print("錯誤: 無選取目標")
            else:
                print(help_[3])
            point = ""
        elif point.find("/cut") == 0:
            p = point.rsplit(" ")
            if len(p) == 2 and p[1].isdigit():
                if int(p[1]) <= len(now)-1:
                    now = cut_(now,p[1])
                else:
                    print("錯誤: 無選取目標")
            else:
                print(help_[4])
            point = ""
        elif point.find("/drive") == 0:
            if point == "/drive":
                now = drive_(now)
                okkkk == 1
            else:
                print(help_[10])
            point = ""
        elif point.find("/findt") == 0:
            if point.find("/findt ") == 0:
                p = point.lstrip("/findt ")
                if canName(p) == True and canName(p):
                    now = findt_(now,p)
            else:
                print(help_[16])
            point = ""
        elif point.find("/find") == 0:
            if point.find("/find ") == 0:
                p = point.lstrip("/find ")
                if canName(p) == True and canName(p):
                    now = find_(now,p)
            else:
                print(help_[8])
            point = ""
        elif point.find("/f") == 0:
            p = point.rsplit(" ")
            if point == "/f":
                now = forward_(now)
            elif len(p) == 2:
                now = forward_(now,int(p[1]))
            else:
                print(help_[11])
            point = ""
        elif point.find("/paste") == 0:
            p = point.rsplit(" ")
            if len(p) == 2:
                now = paste_(now,p[1])
            elif len(p) == 3 and p[2].isdigit():
                if int(p[2]) <= len(now)-1:
                    now = paste_(now,p[1],p[2])
            elif len(p) == 3 and p[1].isdigit() == False:
                print("錯誤: 無選取目標")
            else:
                print(help_[5])
            point = ""
        elif point.find("/remove") == 0:
            p = point.rsplit(" ")
            if len(p) == 2 and p[1].isdigit():
                if int(p[1]) <= len(now)-1:
                    now = remove_(now,p[1])
                else:
                    print("錯誤: 無選取目標")
            elif len(p) == 2 and p[1].isdigit() == False:
                print("錯誤: 無選取目標")
            else:
                print(help_[2])
            point = ""
        elif point.find("/renamee") == 0:
            p = point.rsplit(" ")
            if len(p) >= 3 and p[2] != "" and p[1].isdigit():
                if int(p[1]) <= len(now)-1:
                    now = renamee_(now,p[1],p[2])
                else:
                    print("錯誤: 無選取目標")
            elif len(p) == 3 and p[1].isdigit() == False:
                print("錯誤: 無選取目標")
            elif len(p) == 3 and p[2] == "":
                print("錯誤: 名稱不可為空白")
            else:
                print(help_[1])
            point = ""
        elif point.find("/rename") == 0:
            p = point.rsplit(" ")
            if len(p) >= 3 and p[2] != "" and p[1].isdigit():
                if int(p[1]) <= len(now)-1:
                    now = rename_(now,p[1],p[2])
                else:
                    print("錯誤: 無選取目標")
            elif len(p) >= 3 and p[1].isdigit() == False:
                print("錯誤: 無選取目標")
            elif len(p) >= 3 and p[2] == "":
                print("錯誤: 名稱不可為空白")
            else:
                print(help_[1])
            point = ""
        elif point.find("/re") == 0:
            if point == "/re":
                now = re_(now)
            else:
                print(help_[9])
            point = ""
        elif point.find("/save") == 0:
            if point == "/save":
                save_()
            else:
                print(help_[13])
            point = ""
        elif point.find("/show") == 0:
            p = point.rsplit(" ")
            if len(p) == 2 and len(p) >= 2:
                now = show_(now,p[1])
            elif len(p) == 3:
                now = show_(now,p[1],p[2])
            else:
                print(help_[7])
            point = ""
        elif point.find("/sort") == 0:
            p = point.rsplit(" ")
            if len(p) == 2:
                now = sort_(now,p[1])
            elif len(p) == 3:
                now = sort_(now,p[1],p[2])
            else:
                print(help_[6])
            point = ""
        elif point.find("/help") == 0:
            for i in help_:
                print(i)
        else:
            print("\nError","Type:/help")
    elif okkkk == 0:
        print("\nError","Type:/help")
    print("\n目前位置: ",os.getcwd(),"\n")
