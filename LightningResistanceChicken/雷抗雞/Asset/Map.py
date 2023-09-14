import os,random,math,time 

def Map_Load(_id):
    Map = ""
    p = []
    q = []
    if _id == 0: #野外
        with open(os.getcwd()+os.sep+"Map"+os.sep+"0.txt","rt",encoding="utf-8") as Map1:
            Map = Map1.readlines()
    if _id == 1: #野外
        with open(os.getcwd()+os.sep+"Map"+os.sep+"1.txt","rt",encoding="utf-8") as Map1:
            Map = Map1.readlines()
    if _id == 2: #野外
        with open(os.getcwd()+os.sep+"Map"+os.sep+"2.txt","rt",encoding="utf-8") as Map1:
            Map = Map1.readlines()
    if _id == 3: #野外
        with open(os.getcwd()+os.sep+"Map"+os.sep+"3.txt","rt",encoding="utf-8") as Map1:
            Map = Map1.readlines()
    if _id == 4: #野外
        with open(os.getcwd()+os.sep+"Map"+os.sep+"4.txt","rt",encoding="utf-8") as Map1:
            Map = Map1.readlines()
    if _id == 5: #野外
        with open(os.getcwd()+os.sep+"Map"+os.sep+"5.txt","rt",encoding="utf-8") as Map1:
            Map = Map1.readlines()
    if _id == 6: #野外
        with open(os.getcwd()+os.sep+"Map"+os.sep+"6.txt","rt",encoding="utf-8") as Map1:
            Map = Map1.readlines()
    if _id == 7: #野外
        with open(os.getcwd()+os.sep+"Map"+os.sep+"7.txt","rt",encoding="utf-8") as Map1:
            Map = Map1.readlines()
    if _id == 8: #野外
        with open(os.getcwd()+os.sep+"Map"+os.sep+"8.txt","rt",encoding="utf-8") as Map1:
            Map = Map1.readlines()
    if _id == 9: #主頁面
        with open(os.getcwd()+os.sep+"Map"+os.sep+"主頁面地圖.txt","rt",encoding="utf-8") as Map1:
            Map = Map1.readlines()
    if _id == 10: #主頁面
        with open(os.getcwd()+os.sep+"Map"+os.sep+"房間.txt","rt",encoding="utf-8") as Map1:
            Map = Map1.readlines()
    Map_H = len(Map)
    for i in range(Map_H):
        o = Map[i].split("\n")
        Map[i] = o[0]
    Map_W = len(Map[0])
    for i in range(Map_H):
        for j in range(Map_W):
            p += Map[i][j]
    return (p,Map_H,Map_W)

def Map_Print(q,Map_H,Map_W,p,Boss,Chicken,Mon=0):
    k = q[p]
    if Mon != 0:
        for i in range(len(Mon)-1):
            if Mon[i].HP > 0:
                q[Mon[i].Map[1]] = "怪"
        if Chicken.Map[0] == 0:
            if Mon[len(Mon)-1].HP > 0 and Boss[0] == 0:
                q[Mon[len(Mon)-1].Map[1]] = "王"
            else:
                q[179] = "\u3000"
                q[199] = "\u3000"
                q[219] = "\u3000"
                q[239] = "\u3000"
        elif Chicken.Map[0] == 1:
            if Mon[len(Mon)-1].HP > 0 and Boss[1] == 0:
                q[Mon[len(Mon)-1].Map[1]] = "王"
            else:
                q[179] = "\u3000"
                q[199] = "\u3000"
                q[219] = "\u3000"
                q[239] = "\u3000"
        elif Chicken.Map[0] == 2:
            if Mon[len(Mon)-1].HP > 0 and Boss[2] == 0:
                q[Mon[len(Mon)-1].Map[1]] = "王"
            else:
                q[388] = "\u3000"
                q[389] = "\u3000"
                q[390] = "\u3000"
                q[391] = "\u3000"
        elif Chicken.Map[0] == 3:
            if Mon[len(Mon)-1].HP > 0 and Boss[3] == 0:
                q[Mon[len(Mon)-1].Map[1]] = "王"
            else:
                q[388] = "\u3000"
                q[389] = "\u3000"
                q[390] = "\u3000"
                q[391] = "\u3000"
        elif Chicken.Map[0] == 4:
            if Mon[len(Mon)-1].HP > 0 and Boss[4] == 0:
                q[Mon[len(Mon)-1].Map[1]] = "王"
            else:
                q[160] = "\u3000"
                q[180] = "\u3000"
                q[200] = "\u3000"
                q[220] = "\u3000"
        elif Chicken.Map[0] == 5:
            if Mon[len(Mon)-1].HP > 0 and Boss[5] == 0:
                q[Mon[len(Mon)-1].Map[1]] = "王"
            else:
                q[160] = "\u3000"
                q[180] = "\u3000"
                q[200] = "\u3000"
                q[220] = "\u3000"
        elif Chicken.Map[0] == 6:
            if Mon[len(Mon)-1].HP > 0 and Boss[6] == 0:
                q[Mon[len(Mon)-1].Map[1]] = "王"
            else:
                q[179] = "\u3000"
                q[199] = "\u3000"
                q[219] = "\u3000"
                q[239] = "\u3000"
        elif Chicken.Map[0] == 7:
            if Mon[len(Mon)-1].HP > 0 and Boss[7] == 0:
                q[Mon[len(Mon)-1].Map[1]] = "王"
            else:
                q[179] = "\u3000"
                q[199] = "\u3000"
                q[219] = "\u3000"
                q[239] = "\u3000"
        elif Chicken.Map[0] == 8:
            if Mon[len(Mon)-1].HP > 0 and Boss[8] == 0:
                q[Mon[len(Mon)-1].Map[1]] = "王"
            else:
                q[179] = "\u3000"
                q[199] = "\u3000"
                q[219] = "\u3000"
                q[239] = "\u3000"
    q[p] = "雞"
    H  = int(p/Map_W)+1
    W = p%Map_W+1
    ppo = ""
    print("")
    for i in range(Map_H):
        ppo = abs(i+1-H)
        if ppo == 1:
            s = "一"
        elif ppo == 2:
            s = "二"
        elif ppo == 3:
            s = "三"
        elif ppo == 4:
            s = "四"
        elif ppo == 5:
            s = "五"
        elif ppo == 6:
            s = "六"
        elif ppo == 7:
            s = "七"
        elif ppo == 8:
            s = "八"
        elif ppo == 9:
            s = "九"
        else:
            s = ""
        o = q[i*Map_W:i*Map_W+Map_W]
        print("".join(o),s,end="",sep="\u3000")
        print("")
    print("")
    for i in range(Map_W):
        ppo = abs(i+1-W)
        if ppo == 1:
            print("一",end = "")
        elif ppo == 2:
            print("二",end = "")
        elif ppo == 3:
            print("三",end = "")
        elif ppo == 4:
            print("四",end = "")
        elif ppo == 5:
            print("五",end = "")
        elif ppo == 6:
            print("六",end = "")
        elif ppo == 7:
            print("七",end = "")
        elif ppo == 8:
            print("八",end = "")
        elif ppo == 9:
            print("九",end = "")
        else:
            print("\u3000",end = "")
    print("")
    q[p] = k
    
def Monster_Place(q,Mon):
    while 1:
        p = random.randint(0,len(q)-1)
        if q[p] == "\u3000":
            Mon.Map = [Mon.Map[0],p]
            break
            
def Player_Move(q,Map_H,Map_W,Chicken,d,t,Bag,Mons = 0): 
    o = Chicken.Map[1]
    for i in range(t):
        if d == "w":
            o -= Map_W
            if q[o] == "\u3000":
                Chicken.Map[1] = o
        elif d == "s":
            o += Map_W
            if q[o] == "\u3000":
                Chicken.Map[1] = o
        elif d == "d":
            o += 1
            if q[o] == "\u3000":
                Chicken.Map[1] = o
        elif d == "a":
            o -= 1
            if q[o] == "\u3000":
                Chicken.Map[1] = o
        a = 2
        if Mons != 0:
            for j in range(len(Mons)):
                if Mons[j].Map[1] == o and Mons[j].HP > 0:
                    Mons[j]._State()
                    print('\n是否要對',Mons[j].Name,'進行攻擊 y or n: ',end='')
                    op = input('')
                    if op == "y":
                        a = Attack(Chicken,Mons[j])
                        Chicken._MP(-4)
                        print("在戰鬥的過程中 時間過了 30 分鐘")
                        Chicken.Add_Time(15)
                    else:
                        a = 3
                if a == 0:
                    Chicken._State(3)
                    break
                if a == 1:
                    Chicken._State(0)
                    Chicken._State(1)
                    Chicken._State(2)
                    Chicken._State(3)
                    Chicken._State(4)
                    Bag.Money += Mons[j].Coin
                    q[o] = "\u3000"
                    break
        if a == 3 or a == 1:
            break
        if Chicken.Map[1]%Map_W == 19:
            Chicken.Map[0] += 1
            Chicken.Map[1] -= Map_W-2
            break
        if Chicken.Map[1]%Map_W == 0:
            Chicken.Map[0] -= 1
            Chicken.Map[1] += Map_W-2
            break
        if Chicken.Map[1]/Map_W < 1:
            Chicken.Map[0] -= 3
            Chicken.Map[1] += (Map_H-1)*(Map_W)-Map_W
            break
        if (Chicken.Map[1])/Map_W >= Map_H-1:
            Chicken.Map[0] += 3
            Chicken.Map[1] -= (Map_H-1)*(Map_W)-Map_W
            break
        if Chicken.Day >= 5:
            if (Chicken.Map[0] == 9 and (81 <= o and o <=84)):
                Shower(Chicken)
                break
            if (0 <= Chicken.Map[0] <= 8 and q[o] == "家") or (Chicken.Map[0] == 9 and (86 <= o and o <=89)):
                Chicken.Map[0] = 10
                Chicken.Map[1] = 257
                break
            if (0 <= Chicken.Map[0] <= 8 and q[o] == "洞") or (Chicken.Map[0] == 9 and (86 <= o and o <=89)):
                print("你與",Chicken.Name,"掉到了洞中")
                Chicken._HP(-10000)
                Chicken._State(3)
                break
            if (Chicken.Map[0] == 9 and (91 <= o and o <=94)):
                Buy(Bag)
                break
            if (Chicken.Map[0] == 9 and (161 <= o and o <=164)):
                print("你帶著 ",Chicken.Name," 來到了訓練所",end="")
                oo = input("")
                School(Chicken)
                break
            if (Chicken.Map[0] == 9 and (166 <= o and o <=169)):
                Chicken.Map[0] = 0
                Chicken.Map[1] = 190
                break
            if (Chicken.Map[0] == 9 and (171 <= o and o <=174)):
                print("你望著以前工作的地方,心想絕對不要再回到這裡")
                oo = input("")
                break
        else:
            if (Chicken.Map[0] == 9 and (81 <= o and o <=84)):
                Shower(Chicken)
                break
            if (Chicken.Map[0] == 9 and (86 <= o and o <=89)):
                Chicken.Map[0] = 10
                Chicken.Map[1] = 257
                break
            if (Chicken.Map[0] == 9 and (91 <= o and o <=94)):
                Buy(Bag)
                break
            if (Chicken.Map[0] == 9 and (161 <= o and o <=164)):
                print("我不需要",end='')
                oo = input("")
                break
            if (Chicken.Map[0] == 9 and (166 <= o and o <=169)):
                print("我不需要去探索",end='')
                oo = input("")
                break
            if (Chicken.Map[0] == 9 and (171 <= o and o <=174)):
                Work(Chicken,Bag)
                break
        if Chicken.Map[0] == 10 and q[o] == "教":
            Teach(Chicken)
            break
        if Chicken.Map[0] == 10 and q[o] == "窩":
            Interactive(Chicken,Bag)
            break
        if Chicken.Map[0] == 10 and q[o] == "吃":
            Bag.Eat(Chicken)
            break
        if Chicken.Map[0] == 10 and q[o] == "門":
            Chicken.Map[0] = 9
            Chicken.Map[1] = 103
            break
        if Chicken.Map[0] == 10 and q[o] == "床":
            if Chicken.Time < 120 or 420<=Chicken.Time<=720:
                Chicken.Map[0] = -1
                Sleep(Chicken)
            elif Chicken.Day >= 2:
                print(Chicken.Name,"還不太想睡",end = "")
                oo = input("")
            else:
                print("你還不太想睡",end = "")
                oo = input("")
            break
        o = Chicken.Map[1]
#----------------------------------隨機事件------------------------------------------
        j = random.randint(1,2001)
        if 0<=Chicken.Map[0]<=8:
            if j in range(1,4) and 0<=Chicken.Map[0]<=8:#------------被雷劈
                print('蒼穹之上落下一道雷! 準確無誤劈在',Chicken.Name,'的身上!',end = "")
                oo = input("")
                chance = random.randint(1,11)
                if Chicken.Resistance == 0:
                    print('雖然',Chicken.Name,'已經被雷劈過了 但是你能感覺到這與之前的不同!')
                    time.sleep(1)
                    print('受到了巨額傷害! 當前生命值剩1 !',end = "")
                    Chicken.HP_ = 1
                    oo = input("")
                elif Chicken.Resistance == 1:
                    print('又一次的雷擊使',Chicken.Name,'回憶起牠滄桑的過去!')
                    time.sleep(1)
                    print('受到了大量的傷害! 當前生命值減二分之一!',end = "")
                    Chicken._HP(-int(Chicken.HP_/2))
                    oo = input("")
                    Chicken._State(3)
                elif 2 <= Chicken.Resistance <= 5:
                    print('一陣暈眩後',Chicken.Name,'又站了起來')
                    time.sleep(1)
                    print('剛才的雷擊並沒有造成多大的傷害 而且你感覺到',Chicken.Name,'體內的能量增加了!',end = "")
                    oy = int(Chicken.HP_/Chicken.Resistance+1)
                    Chicken._HP(-oy)
                    print("受到了",oy,"點傷害")
                    oo = input("")
                else:
                    print('但',Chicken.Name,'已經成為了雷抗雞 所以這次的雷擊根本不痛不癢!',end = "")
                    oo = input("")
                Chicken.Resistance += 1
                break
            elif j in range(4,10):#------------採到陷阱
                damage = random.randint(0,int(Chicken.HP_/3))
                Chicken._HP(-damage) 
                print('採到陷阱! 受到了',damage,'點傷害!',end = "")
                oo = input("")
                Chicken._State(3)
                break
            elif j in range(10,24):#------------撿到寶箱
                print('撿到了寶箱!',end = "")
                oo = input("")
                chance = random.randint(1,101)
                if chance in range(1,3):
                    Bag.Diamond += 1
                    print('從寶箱翻出了一顆鑽石!!!',end = "")
                    oo = input("")
                elif chance in range(3,33):
                    Add = random.randint(10,51)
                    Bag.Money += Add
                    print('從寶箱翻出了',Add,'枚金幣!',end = "")
                    oo = input("")
                elif chance in range(34,54):
                    print('從寶箱翻出了神祕藥水!',end = "")
                    oo = input("")
                    Unknown = random.randint(1,11)
                    Heal = random.randint(10,Chicken.HP_)
                    if Unknown == 4:
                        Chicken._HP(-Heal) 
                        print('藥水過期了! 中毒受到',Heal,'點傷害!',end = "")
                        oo = input("")
                        Chicken._State(3)
                    else:
                        Chicken._HP(+Heal)
                        print('藥效顯著! 回復',Heal,'點生命值!',end = "")
                        oo = input("")
                else:
                    print('結果寶箱是空的 可憐哪!',end = "")
                    oo = input("")
                break
            elif j in range(24,27):#------------金幣被偷
                print('一個黑影快速閃過 回過神來時你發現你的學分...喔不 是金幣被偷了!',end="")
                oo = input("")
                money = random.randint(0,int(Bag.Money/6))
                money += 20
                if Bag.Money <= money:
                    Bag.Money = 0
                    print('一毛都不剩 可憐哪(我說的是金幣 你的學分還好好的...應該啦...)',end = "")
                    oo = input("")
                else:
                    Bag.add_money(-money)
                    print('金幣減少了',money,'枚!',end = "")
                    oo = input("")
                break
#-----------------------------------戰鬥系統--------------------------------------------
def Attack(Chicken,Mons):
    MyAttack = Chicken.ATK_ - Mons.DF #雞的攻擊
    MonAttack = Mons.ATK - Chicken.DF_ #怪物的攻擊
    if MyAttack <= 0:
        MyAttack = 1
    if MonAttack <= 0:
        MonAttack = 1
    while 1:
        time.sleep(0.5)
        p = random.randint(1,101)
        if p in range(1,10):
            MyFinalAttack = 2*MyAttack
            print('%s 暴擊! 對 %s 造成'%(Chicken.Name,Mons.Name), MyFinalAttack, '點傷害',end="")
        elif p in range(96,100):
            MyFinalAttack = 0
            print('%s 攻擊失誤， %s 成功迴避攻擊'%(Chicken.Name,Mons.Name),end="")
        else:
            MyFinalAttack = MyAttack
            print('%s 對 %s 造成'%(Chicken.Name,Mons.Name), MyFinalAttack, '點傷害',end="")
        print("    ",Chicken.Name,"飽食減少 1")
        Chicken._Hunger(-1)
        Mons._HP(-MyFinalAttack)
        print(Mons.Name,"血量剩下",Mons.HP)
        print("")
        if Mons.HP <= 0:
            time.sleep(0.5)
            print('%s 被 %s 擊殺了'%(Mons.Name,Chicken.Name))
            time.sleep(1)
            print("")
            Chicken._EXP(Mons.EXP)
            return 1
        
        time.sleep(0.5)
        q = random.randint(1,101)
        if q in range(1,10):
            MonFinalAttack = 2*MonAttack
            print('%s 暴擊! 對 %s 造成'%(Mons.Name,Chicken.Name), MonFinalAttack, '點傷害',end="")
        elif q in range(96,100):
            MonFinalAttack = 0
            print('%s 攻擊失誤， %s 成功迴避攻擊'%(Mons.Name,Chicken.Name),end="")
        else:
            MonFinalAttack = MonAttack
            print('%s 對 %s 造成'%(Mons.Name,Chicken.Name), MonFinalAttack, '點傷害',end="")
        print("    ",Chicken.Name,"心情減少 1")
        Chicken._Emotion(-1)
        Chicken._HP(-MonFinalAttack)
        print(Chicken.Name,"血量剩下",Chicken.HP_)
        print("")
        if Chicken.HP_ <= 0:
            time.sleep(0.5)
            print('%s 被 %s 擊殺了'%(Chicken.Name,Mons.Name))
            time.sleep(1)
            print("")
            print('你死了!!')
            return 0

#------------------------------------商店--------------------------------------------
def Buy(Bag):
    Bag.show()
    while True:
        print('--------------------------------------------------------------------------------')
        print('你想買些什麼東西呢 ?')
        print(' 0. 離開商店')
        print(' 1. 有機雞飼料 : 5金幣/袋')
        print('    效果: 體力值增加 1 ,飽食度增加 8')
        print(' 2. 生乳酪蛋糕 : 40金幣/塊')
        print('    效果: 體力值增加 3 ,飽食度增加 20')
        print(' 3. 鴨子造型餅乾 : 80金幣/包')
        print('    效果: 體力值增加 8 ,飽食度增加 30 ,心情增加 5')
        print(' 4. 特大麥香雞 : 100金幣/個')
        print('    效果: 體力值增加 20 ,飽食度增加 40')
        print(' 5. 飛盤 : 40金幣/個')
        print('    效果: 心情值增加 15')
        print(' 6. 全球最便宜的PS5 : 100金幣/個')
        print('    效果: 心情值增加 30')
        print(' 7. PS5 : 15980金幣/臺')
        print('    效果: 血量增加 40')
        print(' 8. 小-回復藥水 : 30金幣/個')
        print('    效果: 血量增加 50')
        print(' 9. 大-回復藥水 : 150金幣/個')
        print('    效果: 血量增加 200')
        print(' 10. 極-回復藥水 : 800金幣/個')
        print('    效果: 血量增加 1000')
        print('')
        shop_id = input('請輸入想買的物品編號: ')
        print('')
        if shop_id == "0":
            break
        while 1:
            try:
                num_id = int(input('請輸入購買的個數: '))
                break
            except:
                print("是數字歐~Difficult?")
        print('')
        a=0
        if shop_id == "1" and Bag.Money >= num_id*5:
            Bag.add_money(-1*num_id*5)
            Bag.add_food( (num_id) * 1)
            print('獲得了 ',num_id,' 個有機雞飼料 ! ! ')                
            time.sleep(0.5)
            a = 1
            print('--------------------------------------------------------------------------------')
            Bag.show()            
        elif shop_id == "2" and Bag.Money >= num_id*40:
            Bag.add_money(-1*num_id*40)
            Bag.add_cake(num_id)
            print('獲得了 ',num_id,' 個生乳酪蛋糕 ! ! ')
            time.sleep(0.5)
            a = 1
            print('--------------------------------------------------------------------------------')
            Bag.show()
        elif shop_id == "3" and Bag.Money >= num_id*80:
            Bag.add_money(-1*num_id*80)
            Bag.add_cookies(num_id)
            print('獲得了 ',num_id,' 個鴨子造型餅乾 ! ! ')
            time.sleep(0.5)
            a = 1
            print('--------------------------------------------------------------------------------')
            Bag.show()
        elif shop_id == "4" and Bag.Money >= num_id*100:
            Bag.add_money(-1*num_id*100)
            Bag.add_hamburger(num_id)
            print('獲得了 ',num_id,' 個特大麥香雞 ! ! ')
            time.sleep(0.5)
            a = 1
            print('--------------------------------------------------------------------------------')
            Bag.show()
        elif shop_id == "5" and Bag.Money >= num_id*40:
            Bag.add_money(-1*num_id*40)
            Bag.add_plate(num_id)
            print('獲得了',num_id,'個飛盤 ! ! ')
            time.sleep(0.5)
            a = 1
            print('--------------------------------------------------------------------------------')
            Bag.show()
        elif shop_id == "6" and Bag.Money >= num_id*100:
            Bag.add_money(-1*num_id*100)
            Bag.add_PS5(num_id)
            print('獲得了',num_id,'全球最便宜的一臺PS5 ! ! ')
            time.sleep(0.5)
            a = 1
            print('--------------------------------------------------------------------------------')
            Bag.show()
        elif shop_id == "7" and Bag.Money >= num_id*15980:
            print("你忽然發現你根本買不到PS5(沒貨)",end="")
            oo = input("")
            time.sleep(0.5)
            a = 1
            print('--------------------------------------------------------------------------------')
            Bag.show()
        elif shop_id == "8" and Bag.Money >= num_id*30:
            Bag.add_money(-1*num_id*30)
            Bag.add_Potion(num_id)
            print('獲得了',num_id,'個小-回復藥水 ! ! ')   
            time.sleep(0.5)
            a = 1
            print('--------------------------------------------------------------------------------')
            Bag.show()
        elif shop_id == "9" and Bag.Money >= num_id*150:
            Bag.add_money(-1*num_id*150)
            Bag.add_Potion2(num_id)
            print('獲得了',num_id,'個大-回復藥水 ! ! ')   
            time.sleep(0.5)
            a = 1
            print('--------------------------------------------------------------------------------')
            Bag.show()
        elif shop_id == "10" and Bag.Money >= num_id*800:
            Bag.add_money(-1*num_id*800)
            Bag.add_Potion3(num_id)                                    #增加商品的通式
            print('獲得了',num_id,'個極-回復藥水 ! ! ')                            #--更改的內容--#   
            time.sleep(0.5)
            a = 1
            print('--------------------------------------------------------------------------------')
            Bag.show()
        #增加商品(從這裡開始)    
        if a!=1:
            print("口袋空空,錢不夠握~~~",end="")
            oo = input("")
            print('--------------------------------------------------------------------------------')
            Bag.show()
'''                                                                                                                                     
        elif shop_id == "#--id_shop--#" and Bag.Money >= num_id*#--價格--#:
            Bag.add_money(-1*num_id*#--價格--#)
            Bag.add_#--商品變數(小寫)--#(num_id)                                    #增加商品的通式
            print('獲得了',num_id,'個#--商品名--# ! ! ')                            #--更改的內容--#   
            time.sleep(0.5)
            a = 1
            print('--------------------------------------------------------------------------------')
            Bag.show()
'''                         
#------------------------------------工作--------------------------------------------
def Work(Chicken,Bag):
    if  180 < Chicken.Time < 270:
        print("度過漫長的工作....")
        time.sleep(1)
        print("你賺了200元")
        time.sleep(1)
        Chicken.Time = 510
        Bag.add_money(200)
    else:
        print("你遲到後乾脆不去了")
        oo = input("")
    
#------------------------------------澡堂--------------------------------------------
def Shower(Chicken):
    if Chicken.Day < 2:
        print("你開心的來泡溫泉")
        print("兩小時候.....",end="")
        oo = input("")
        Chicken.Add_Time(60)
    else:
        print("你決定帶著",Chicken.Name,"來泡溫泉")
        time.sleep(1)
        print("三小時候.....")
        time.sleep(1)
        Chicken.Add_Time(90)
        o = int((Chicken.HP-Chicken.HP_)/5)
        Chicken._HP(o)
        Chicken._Hunger(-15)
        Chicken._MP(20)
        Chicken._Emotion(+5)
        print("血量增加",o,",體力值增加 20 ,飽食度減少 15 ,心情增加 5")
        Chicken._State(0)
        Chicken._State(1)
        Chicken._State(2)
        Chicken._State(3)
        Chicken._State(4)
        time.sleep(1)
#------------------------------------點數--------------------------------------------
def School(Chicken):
    while 1:
        Chicken._State(5)
        Chicken._State(0)
        Chicken._State(1)
        Chicken._State(2)
        Chicken._State(3)
        Chicken._State(4)
        if Chicken.Point > 0:
            print("目前的點數為: ",Chicken.Point)
            print("")
            print("----------------------")
            print(" 0.退出")
            print(" 1.血量最大值加15")
            print(" 2.攻擊加3")
            print(" 3.防禦加3")
            print("----------------------")
            print("")
            c = input("請選擇你要的訓練:")
            if c == "0":
                break
            if c == "1":
                Chicken.HP += 15
                Chicken._HP(15)
                Chicken.Point -= 1
                print("訓練成功",end = "")
                oo = input("")
            elif c == "2":
                Chicken.ATK += 3
                Chicken._ATK(3)
                Chicken.Point -= 1
                print("訓練成功",end = "")
                oo = input("")
            elif c == "3":
                Chicken.DF += 3
                Chicken._DF(3)
                Chicken.Point -= 1
                print("訓練成功",end = "")
                oo = input("")
            else:
                break
        else:
            print("\n點數不足\m")
            break
#------------------------------------睡覺--------------------------------------------
def Sleep(Chicken):
    print('今天的冒險就此結束 並且睡個了好覺!',end = "")
    oo = input("")
    Chicken.Time = 180
    Chicken.Day += 1
    if Chicken.Day >= 2:
        Chicken._Hunger(-15)
        Chicken._Emotion(+10)
        p = int((Chicken.HP-Chicken.HP_)/10) + 30
        Chicken._HP(p)
        Chicken._MP(Chicken.MP)
        print("心情增加 10, 飽食度減少 15, 血量增加",p)
    time.sleep(0.5)
    Chicken._State(5)
    Chicken._State(0)
    Chicken._State(1)
    Chicken._State(2)
    Chicken._State(3)
    Chicken._State(4)
    print('也許接下來會更有趣喔?',end = "")
    oo = input("")
#------------------------------------互動--------------------------------------------
def Interactive(Chicken,Bag):
    while 1:
        Chicken._State(5)
        Chicken._State(0)
        Chicken._State(1)
        Chicken._State(2)
        Chicken._State(3)
        Chicken._State(4)
        print("要對",Chicken.Name,"做甚麼呢?")
        print("")
        print("----------------------")
        print(" 0.退出")
        print(" 1.觀察牠!")
        print(" 2.摸牠")
        print(" 3.餵牠喝水")
        print(" 4.和牠玩飛盤")
        print(" 5.和牠玩全球最便宜的PS5")
        print("----------------------")
        print("")
        c = input("請選擇:")
        print("")
        if c == "0":
            break
        if c == "1" and Chicken.Day >= 2:
            o = random.randint(1,3)
            if o == 1:
                print("牠感受到了壓力")
                print("心情值減少 5",end = "")
                oo = input("")
                Chicken._Emotion(-5)
            if o == 2:
                print("牠體會到了自己與牠雞的不同")
                print("心情值增加 5",end = "")
                oo = input("")
                Chicken._Emotion(5)
        elif c == "1" and Chicken.Day < 2:
            print("你被牠深深的吸引",end ="")
            oo = input("")
        if c == "2" and Chicken.Day >= 2:
            emotion = random.randint(0,int(100-Chicken.Emotion/10))
            Chicken._Emotion(emotion)
            print("你摸了摸",Chicken.Name,end = "")
            oo = input("")
            if Chicken.Day >= 5:
                print("你被電了!")
            if Chicken.Emotion<=50:
                print('與',Chicken.Name,'進行互動 心情值增加',emotion,end="")
            oo = input("")
        elif c == "2" and Chicken.Day < 2:
            print("很硬!",end ="")
            oo = input("")
        if Chicken.Day < 2:
            print("牠還是雞蛋歐")
        if c == "3":
            print('你餵雞喝了水',end = "")
            oo = input("")
            print('但是甚麼都沒發生',end = "")
            oo = input("")
        if c == "4" and Bag.Plate >= 1:
            Bag.add_plate(-1)
            print('與',Chicken.Name,'進行互動 心情值增加 15')
            Chicken._Emotion(15)
            oo = input("")
        elif c == "4":
            print("你翻了翻倉庫")
            time.sleep(1)
            print("你突然發現你根本沒有買飛盤",end="")
            oo = input("") 
        if c == "5" and Bag.PS5 >= 1:
            Bag.add_PS5(-1)
            print('與',Chicken.Name,'進行互動 心情值增加 30')
            Chicken._Emotion(30)
            oo = input("")
        elif c == "5":
            print("你翻了翻倉庫")
            time.sleep(1)
            print("你突然發現你根本沒有買全球最便宜的PS5",end="")
            oo = input("")
        Chicken.Add_Time(15)
        print("時間過了 30 分鐘......",end = "")
        oo = input("")
#------------------------------------教學--------------------------------------------
def Teach(Chicken):
    while 1:
        print("這裡是教學頁面")
        print("")
        print("----------------------")
        print(" 0.退出")
        print(" 1.特殊字元")
        print(" 2.關於移動")
        print(" 3.關於存檔")
        if Chicken.Day >= 5:
            print(" 4.戰鬥說明")
            print(" 5.技能點數的使用")
            print(" 6.快捷鍵的使用")
            print(" 7.負面狀態")
        print("----------------------")
        print("")
        c = input("請選擇:")
        print("")
        if c == "0":
            break
        elif c == "1":
            print("你能夠觸碰一些特定的文字:",end = "")
            print("\n[教]字能夠打開教學")
            print("[窩]字能讓你與小雞互動")
            print("[吃]字能讓小雞吃東西")
            print("[床]字能夠睡覺 注意要讓牠太晚睡")
            print("[門]字能走出方房門")
            print("[家]字能夠回家")
            print("[○]字比較特殊  這是作者們對玩家的方便")
            print("[洞]則是惡趣味")
            print("其餘的字則會阻擋你的路線",end ="")
            oo = input("")
        elif c == "2":
            print("wasd 分別對應 上下左右")
            print("輸入 w10 為往上十格  輸入 d3 則往右三格")
            print("好好利用字母後面的數字")
            print("除非你時間很多",end ="")
            oo = input("")
        elif c == "3":
            print("睡覺後會自動存檔",end ="")
            oo = input("")
        elif c == "4" and Chicken.Day >= 5:
            print("每張地圖會有6個小怪1個王")
            print("探索時小怪不會重生 出非回家後再回來探索")
            print("每個王打死就不會再重生了")
            print("")
            print("攻擊與防禦為加減關係")
            print("要是敵人防禦超過你的攻擊")
            print("則敵人只會扣一滴血\n")
            print("請不要以為 (打了就對了) ")
            print("血量必須拿捏好")
            print("不然等著你的就是 死",end ="")
            oo = input("")
        elif c == "5" and Chicken.Day >= 5:
            print("只要小雞一升級,你就會獲取三點點數")
            print("你可以拿點數來去訓練你的小雞")
            print("門外右下角就是訓練的場所",end ="")
            oo = input("")
        elif c == "6" and Chicken.Day >= 5:
            print("輸入 time 可以查詢現在時間")
            print("輸入 eat  可以吃東西")
            print("輸入 home  可以快速回家",end ="")
            oo = input("")
        elif c == "7" and Chicken.Day >= 5:
            print("體力值 飢餓值 行動值 分別對應到 HP ATK DF")
            print("只要這三著的數值過低")
            print("你將會有負面狀態",end ="")
            oo = input("")
        else:
            break
        print("")
