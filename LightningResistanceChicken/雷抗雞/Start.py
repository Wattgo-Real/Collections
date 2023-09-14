from Asset import Save,Map,Plot,Chicken_System,Monster_System,Bag_System
import threading,os,time,random

plott = 0
qqqq = 1
#-----------------------------Load()
if os.path.exists(os.getcwd()+os.sep+"save.txt") == True:
    with open(os.getcwd()+os.sep+"save.txt","rt",encoding="utf-8") as save:
        data = save.readlines()
        for i in data:
            a = i.rsplit("\n")
            a = a[0]
            a = exec(a)

#-----------------------------測試修改區!!!!!!!!!!!!!!!!!!!!
else:
    Chicken = Chicken_System.Chicken('閃電雞蛋',"優良",[0,0,0,0],1,50,0,
                                     100,100,10,10,10,10,20,20,
                                     100,100,0,[10,48],0,0,0,180)
    Bag = Bag_System.Bag(0,5,3,0,3,0,0,0,0,0,0,0)
    Boss = [0,0,0,0,0,0,0,0,0]
#-----------------------------測試修改區!!!!!!!!!!!!!!!!!!!!

#-----------------------------Updata()
def Updata():
    while(1):
        time.sleep(1)
        if plott == 1:
            Chicken.Add_Time(1)


updata = threading.Thread(target = Updata, name = 'Updata')
updata.start()

#-----------------------------Save()
def Save_Date(Chicken,Bag,Boss,p=0):
    Save.save(Chicken,Bag,Boss,p)

#----------------------------------------------------------Day()
  
while 1:
    re_Monster = 0
    Now_Map = 10
    Mon = []
    Day = Chicken.Day
    if Chicken.Map[0] == -1:
        Chicken.Map[0] = 10
        Save_Date(Chicken,Bag,Boss)
        print("儲存成功!",end="")
        oo = print("")
    plott = 1
#-----------------------------Plot()
    if Chicken.Day == 0:
        qqqq = 0
        plott = 0
        Plot.Main_Plot(0)
        time.sleep(1)
        Chicken.Time = 210
        Chicken.Now_Time()
        time.sleep(1)
        Plot.Main_Plot(1)
        time.sleep(1)
        Plot.Main_Plot(2)
        Chicken.Map = [10,48]
        plott = 1
    elif Chicken.Day == 1:
        plott = 0
        Chicken.Now_Time()
        time.sleep(1)
        Plot.Main_Plot(3)
        Chicken.Name = "雞蛋"
        Chicken.Map = [10,48]
        plott = 1
    elif Chicken.Day == 2:
        plott = 0
        Chicken.Now_Time()
        time.sleep(1)
        Plot.Main_Plot(4)
        Chicken.Name = "(黃色)小雞"
        Chicken.Map = [10,48]
        Chicken.Level = 1
        plott = 1
    elif Chicken.Day == 3:
        plott = 0
        Chicken.Now_Time()
        time.sleep(1)
        Plot.Main_Plot(5)
        Chicken.Map = [10,48]
        plott = 1
    elif Chicken.Day == 4:
        plott = 0
        Chicken.Now_Time()
        time.sleep(1)
        Plot.Main_Plot(6)
        Plot.Main_Plot(7)
        Chicken.Map = [10,48]
        plott = 1
        Chicken.Day = 5
    elif Chicken.Day == 5:
        plott = 0
        Chicken.Now_Time()
        time.sleep(1)
        Plot.Main_Plot(8)
        Plot.Main_Plot(9)
        Chicken.Name = input("(為你的黃色小雞取名吧): ")
        print("你與",Chicken.Name,"的冒險才剛剛開始")
        print("等等請觸碰上頭的[教]字獲取更多訊息",end = "")
        o = input("")
        Chicken.Map = [10,48]
        plott = 1
    else:
        wer = "w"

#-----------------------------Map()
    for i in range(9):
        o = Map.Map_Load(i)
        Mon1 = Monster_System.Mon(i*2)
        Map.Monster_Place(o[0],Mon1)
        Mon2 = Monster_System.Mon(i*2+1)
        Map.Monster_Place(o[0],Mon2)
        Mon1_ = Monster_System.Mon(i*2)
        Map.Monster_Place(o[0],Mon1_)
        Mon2_ = Monster_System.Mon(i*2+1)
        Map.Monster_Place(o[0],Mon2_)
        Mon11 = Monster_System.Mon(i*2)
        Map.Monster_Place(o[0],Mon11)
        Mon21 = Monster_System.Mon(i*2+1)
        Map.Monster_Place(o[0],Mon21)
        Mon3 = Monster_System.Mon(i+18)
        Mon.append([Mon1,Mon1_,Mon2,Mon2_,Mon3])
    while 1:
        if Day != Chicken.Day:
            break
        if Now_Map == 9 and Chicken.Map[0] == 0:
            for i in range(9):
                o = Map.Map_Load(i)
                Mon1 = Monster_System.Mon(i*2)
                Map.Monster_Place(o[0],Mon1)
                Mon2 = Monster_System.Mon(i*2+1)
                Map.Monster_Place(o[0],Mon2)
                Mon1_ = Monster_System.Mon(i*2)
                Map.Monster_Place(o[0],Mon1_)
                Mon2_ = Monster_System.Mon(i*2+1)
                Map.Monster_Place(o[0],Mon2_)
                Mon11 = Monster_System.Mon(i*2)
                Map.Monster_Place(o[0],Mon11)
                Mon21 = Monster_System.Mon(i*2+1)
                Map.Monster_Place(o[0],Mon21)
                Mon3 = Monster_System.Mon(i+18)
                Mon.append([Mon1,Mon1_,Mon2,Mon2_,Mon11,Mon21,Mon3])
        if Chicken.Map[0] >= 9:
            Mon = []
        o = Map.Map_Load(Chicken.Map[0])
        Now_Map = Chicken.Map[0]
        while 1:
            Chicken._State(5)
            Bag.show()
            Chicken.Now_Time()
            if 0 <= Chicken.Map[0] <= 8:
                Map.Map_Print(o[0],o[1],o[2],Chicken.Map[1],Boss,Chicken,Mon[Chicken.Map[0]])
            else:
                Map.Map_Print(o[0],o[1],o[2],Chicken.Map[1],Boss,Chicken)

            if Chicken.Day == 0 and qqqq == 0:
                qqqq = 1
                print("\n你是地圖中的[雞]字")
                print("wasd 分別對應 上下左右")
                print("輸入 w10 為往上十格  輸入 d3 則往右三格")
                print("好好利用字母後面的數字")
                print("除非你時間很多",end = "")
                oo = input("")
                print("請觸碰上頭的[教]字獲取更多訊息")
                print("然後快一去上班吧 九點後就不能去了")
            Move = 0
            while 1:
                Move = input("")
                if len(Move) == 1:
                    Move = Move + "1"
                    break
                elif Move == "time":
                    Chicken.Now_Time()
                    Move = "w0"
                elif Move == "eat" and Chicken.Day >= 5:
                    Bag.Eat(Chicken)
                    Move = "w0"
                elif Move == "home" and Chicken.Day >= 5:
                    Chicken.Map = [10,257]
                    Move = "w0"
                elif Move[1:].isdigit() == False:
                    Move = "w0"
                    break
                else:
                    break
            if Now_Map != Chicken.Map[0]:
                break
            if 0 <= Chicken.Map[0] <= 8:
                Map.Player_Move(o[0],o[1],o[2],Chicken,Move[:1],int(Move[1:]),Bag,Mon[Chicken.Map[0]])
            else:
                Map.Player_Move(o[0],o[1],o[2],Chicken,Move[:1],int(Move[1:]),Bag)
            if Now_Map != Chicken.Map[0]:
                break
        Chicken._State(0)
        Chicken._State(1)
        Chicken._State(2)
        Chicken._State(3)
