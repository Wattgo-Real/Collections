import random

class Chicken(): #雞
    def __init__(self,Name,State,State_,Level,EXP,EXP_,HP,HP_,ATK,ATK_,DF,DF_,
                 MP,MP_,Hunger,Emotion,Luck,Map,Point,Resistance,Day,Time):#Time一秒為2分鐘
        self.State = State
        self.State_ = State_
        self.Name = Name
        self.EXP = EXP
        self.EXP_ = EXP_
        self.Level = Level
        self.HP = HP
        self.HP_ = HP_ 
        self.ATK = ATK
        self.ATK_ = ATK_
        self.DF = DF
        self.DF_ = DF_
        self.Hunger = Hunger
        self.Emotion = Emotion
        self.MP = MP
        self.MP_ = MP_
        self.Luck = Luck
        self.Map = Map
        self.Point = Point
        self.Resistance = Resistance
        self.Day = Day
        self.Time = Time
    def _State(self,i=0):
        if i==0:
            if self.Hunger <= 100: #怕程式碼太亂打的
                if 10 < self.Hunger <= 33:
                    print('狀態: 有點餓了')
                if 0 < self.Hunger <= 10:
                    self.State_[0] = 1
                    print('狀態: 現在非常餓')
                if self.Hunger <= 0:
                    print('狀態: 鬧飢荒咯! 生命值大幅降低')
                    if self.State_[0] != 2:
                        self.HP_ = int(self.HP_/2)
                    self.State_[0] = 2
                if self.State_[0] == 1: #debug用
                    if self.Hunger >= 33:
                        self.State_[0] = 0
                if self.State_[0] == 2:
                    if self.Hunger >= 33:
                        print('狀態: 飢荒問題解決了...暫時...(生命上限回升)')
                        oo = input("")
                        self.State_[0] = 0
        elif i==1:
            #體力值
            if self.MP_ <= self.MP:
                if self.MP/10 < self.MP_ <= self.MP/3:
                    print('狀態: 有點累了')
                if 0 < self.MP_ <= self.MP/10:
                    self.State_[1] = 1
                    print('狀態: 真D累了')
                if self.MP_ <= 0:
                    print('狀態: 你的雞累到沒力了 攻擊力大幅降低')
                    if self.State_[1] != 2:
                        self.ATK_ = int(self.ATK_/2)
                    self.State_[1] = 2
                if self.State_[1] == 1: #debug用
                    if self.MP_ >= self.MP/3:
                        self.State_[1] = 0
                if self.State_[1] == 2:
                    if self.MP_ >= self.MP/3:
                        print('狀態: 休息夠了 你的雞願意繼續奮鬥了(攻擊力回升)')
                        self.ATK_ = self.ATK
                        self.State_[1] = 0
        elif i==2:
            #心情值
            if self.Emotion <= 100:
                if 10 < self.Emotion <= 33:
                    print('狀態: 心情不太好')
                if 0 < self.Emotion <= 10:
                    self.State_[2] = 1
                    print('狀態: 心情頗遭(聽說是計概要被當了)')
                if self.Emotion <= 0:
                    print('狀態: 人格分裂 出現疑似抖M傾向 防禦力大幅降低')
                    if self.State_[2] != 2:
                        self.DF_ = int(self.DF_/2)
                    self.State_[2] = 2
                if self.State_[2] == 1: #debug用
                    if self.Emotion >= 33:
                        self.State_[2] = 0
                if self.State_[2] == 2:
                    if self.Emotion >= 33:
                        print('狀態: 人格分裂還沒有很嚴重 硬是救回來了(防禦力回升)')
                        oo = input("")
                        self.DF_ = self.DF
                        self.State_[2] = 0
        elif i==3:
             #生命值
            if self.HP_ <= self.HP:
                if self.HP/10 < self.HP_ <= self.HP/3 and self.State_[3] != 1:
                    self.State_[3] = 1
                    print(self.Name,'狀態: 的狀況不太優 請盡快進行治療')
                if 0 < self.HP_ <= self.HP/10 and self.State_[3] != 2:
                    self.State_[3] = 2
                    print('狀態: 當前血量十分危急 請盡速治療')
                if self.HP_ <= 0:
                    self.HP_ = 0
                    print(self.Name,' 與你的冒險就此終結 關於你傳奇的一生 沒人想知道','\n'
                          ,'      R.I.P 願你的計概分數不會如此\n')
                    print("提示: 重新開啟後會讀入當天早上的存檔")
                    while 1:
                        c = 8763
                if self.State_[3] == 1: #debug用
                    if self.HP_ >= self.HP/3:
                        self.State_[2] = 0
                if self.State_[3] == 2:
                    if self.HP_ >= self.HP/3:
                        print('狀態: 血量提升 感覺好多了')
                        oo = input("")
                        self.State_[3] = 0
        elif i==4:
            #狀態語音
            print("")
            if self.State_[0]+self.State_[1]+self.State_[2]+self.State_[3] == 0:
                self.State = '優良'
            elif 3 <= self.State_[0]+self.State_[1]+self.State_[2]+self.State_[3]  <= 4:
                self.State = '不太優'
                print('你家的雞狀況不太優 自己注意嘿',end="")
                oo = input("")
            elif 3 < self.State_[0]+self.State_[1]+self.State_[2]+self.State_[3]  <= 5:
                self.State = '很不優'
                print('你家的雞狀況真的很不優 有沒有在照顧阿?',end="")
                oo = input("")
            elif self.State_[0]+self.State_[1]+self.State_[2]+self.State_[3]  == 6:
                self.State = '糟糕'
                print('你不怕可愛動物保護協會來找你?',end="")
                oo = input("")
            elif self.State_[0]+self.State_[1]+self.State_[2]+self.State_[3]  >= 7:
                self.State = '難以理解'
                print('請停止虐雞 你的行為害我們得多打這串程式碼來提醒你 請別增加工作負擔 謝謝!',end="")
                oo = input("")
        if i==5:
            print("")
            print(self.Name,'   ','Level ',self.Level,'(',self.EXP_,'/',self.EXP,')'
                  ,'\n','HP: ','(',self.HP_,'/',self.HP,')','   ','ATK: ',self.ATK_,'   ','DF: ',self.DF_,'   '
                  ,'\n','飽食度: ','(',self.Hunger,'/100',')','   ','心情值: ','(',self.Emotion,'/100',')'
                  ,'   ','體力值: ','(',self.MP_,'/',self.MP,')','\n','當前狀態: '
                  ,self.State,'   ','幸運值: ',self.Luck,'   ','點數:',self.Point)
            print("")    
    def _Name(self,Name):
        self.Name = Name
    def _EXP(self,EXP):
        self.EXP_ += EXP
        if self.EXP_ >= self.EXP and self.Level < 50:
            self.EXP_ -= self.EXP
            self.EXP = int(self.EXP*1.09)
            if self.Level <= 10:
                self.Level += 1
                self.Point += 2
                self.MP += 2
                self.MP_ += 2
            elif 10 < self.Level <= 20:
                self.Level += 1
                self.Point += 2
                self.MP += 2
                self.MP_ += 2
            elif 20 < self.Level <= 30:
                self.Level += 1
                self.Point += 3
                self.MP += 2
                self.MP_ += 2
            elif 30 < self.Level <= 40:
                self.Level += 1
                self.Point += 3
                self.MP += 2
                self.MP_ += 2
            elif 40 < self.Level < 50:
                self.Level += 1
                self.Point += 4
            print(self.Name,"升級了",end  = "")
            oo = input("")
            if self.Level == 10:
                self.ATK += 5
                self.ATK_ += 5
                self.HP += 50
                self.HP_ += 50
                self.DF += 5
                self.DF_ += 5
                self.Luck += 2
            elif self.Level == 20:
                self.ATK += 7
                self.ATK_ += 7
                self.HP += 75
                self.HP_ += 75
                self.DF += 7
                self.DF_ += 7
                self.Luck += 2
            elif self.Level == 30:
                self.ATK += 9
                self.ATK_ += 9
                self.HP += 100
                self.HP_ += 100
                self.DF += 9
                self.DF_ += 9
                self.Luck += 2
            elif self.Level == 40:
                self.ATK += 12
                self.ATK_ += 12
                self.HP += 150
                self.HP_ += 150
                self.DF += 12
                self.DF_ += 12
                self.Luck += 2
            elif self.Level == 50:
                self.ATK += 15
                self.ATK_ += 15
                self.HP += 200
                self.HP_ += 200
                self.DF += 15
                self.DF_ += 15
                self.Luck += 2

            print("")
    def _HP(self,HP):
        self.HP_ += HP
        if self.HP_ > self.HP and self.State_[0] == 0:
            self.HP_ = self.HP
        if self.HP_ > self.HP/2 and self.State_[0] == 2:
            self.HP_ = int(self.HP/2)
    def _ATK(self,ATK):
        self.ATK_ += ATK
        if self.ATK_ > self.ATK and self.State_[1] == 0:
            self.ATK_ = self.ATK
        if self.ATK_ > self.ATK/2 and self.State_[1] == 2:
            self.ATK_ = int(self.ATK/2)
    def _DF(self,DF):
        self.DF_ += DF
        if self.DF_ > self.DF and self.State_[2] == 0:
            self.DF_ = self.DF
        if self.DF_ > self.DF/2 and self.State_[2] == 2:
            self.DF_ = int(self.DF/2)
    def _Hunger(self,Hunger):
        self.Hunger += Hunger
        if self.Hunger > 100:
            self.Hunger = 100
        if self.Hunger < 0:
            self.Hunger = 0
    def _MP(self,MP):
        self.MP_ += MP
        if self.MP_ > self.MP:
            self.MP_ = self.MP
        if self.MP_ <= 0:
            self.MP_ = 0
    def _Emotion(self,Emotion):
        self.Emotion += Emotion
        if self.Emotion > 100:
            self.Emotion = 100
        if self.Emotion < 0:
            self.Emotion = 0
    def Add_Time(self,Time):
        self.Time += Time
        if self.Time >= 720:
            self.Day += 1
            self.Time -= 720
        if self.Time-Time < 690 <= self.Time and self.Day>=2:
            print("時間太晚了",self.Name,"心情值降低")
            self._Emotion(-15)
            print("心情值減少15",end="")
            oo = input("")
        if self.Time-Time < 630 <= self.Time:
            print("時間有點晚搂 要回去睡覺了")
            oo = input("")
        if (self.Time-Time < 660 <= self.Time or
            self.Time-Time < 600 <= self.Time or
            self.Time-Time < 530 <= self.Time or
            self.Time-Time < 470 <= self.Time or
            self.Time-Time < 410 <= self.Time or
            self.Time-Time < 350 <= self.Time or
            self.Time-Time < 290 <= self.Time or
            self.Time-Time < 230 <= self.Time) and self.Day>=2:
            p = self.Hunger - 2
            if p < 0:
                self.Hunger = 0
            else:
                self.Hunger = p
    def Now_Time(self):
        print("現在的時間是: ")
        print("第",self.Day,"天   ",int(self.Time/30),"點",self.Time%30,"分",sep = "")
