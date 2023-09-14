class Mons():
    def __init__(self,Name,HP,ATK,DF,EXP,Coin,Map):
        self.Name = Name
        self.HP = HP
        self.ATK = ATK
        self.DF = DF
        self.EXP = EXP
        self.Coin = Coin
        self.Map = Map
    def _HP(self,HP):
        self.HP += HP
    def _State(self):
        print(self.Name,'\n生命值:',self.HP,' ',
               '\n攻擊:',self.ATK ,' ','防禦:',self.DF,' ','擊殺經驗:',self.EXP,
               ' ','金幣:',self.Coin)
        
#-------------新增野怪區--------------------------------------------------------
def Mon(_id):
    if _id == 0:
        return Mons('史萊姆 Lv1',25,22,0,8,10,[0,0])
    elif _id == 1:
        return Mons('雞骨頭 Lv1',45,18,5,10,12,[0,0])
    elif _id == 2:
        return Mons('期中霉運吉娃娃 Lv2',60,25,16,12,15,[1,0])
    elif _id == 3:
        return Mons('重修怨魂 Lv2',80,32,10,12,15,[1,0])
    elif _id == 4:
        return Mons('不重要的路人甲 Lv3',60,40,30,15,20,[2,0])
    elif _id == 5:
        return Mons('不重要的路人乙 Lv3',60,30,40,15,20,[2,0])
    elif _id == 6:
        return Mons('鳥人幫成員 Lv4',65,20,25,22,25,[3,0])
    elif _id == 7:
        return Mons('學分守門員 Lv4',20,40,65,30,25,[3,0])
    elif _id == 8:
        return Mons('嬰靈雞 Lv5',200,60,40,34,55,[4,0])
    elif _id == 9:
        return Mons('星爆連流雞 Lv5',140,75,15,43,38,[4,0])
    elif _id == 10:
        return Mons('渾元形意太雞門徒 Lv6',150,90,64,55,60,[5,0])
    elif _id == 11:
        return Mons('渾元形意太雞團員 Lv6',250,86,24,45,90,[5,0])
    elif _id == 12:
        return Mons('長太矮的逃兵 Lv7',90,350,0,40,120,[6,0])
    elif _id == 13:
        return Mons('拿著退伍令基本無敵的士兵 Lv7',250,118,80,60,140,[6,0])
    elif _id == 14:
        return Mons('職業賭徒.阿爾伯特 Lv8',1500,145,75,60,180,[7,0])
    elif _id == 15:
        return Mons('職業賭博團成員 Lv8',230,130,140,75,160,[7,0])
    elif _id == 16:
        return Mons('迷因之王 Lv9',400,164,162,100,250,[8,0])
    elif _id == 17:
        return Mons('迷因之王的投影 Lv9',200,158,180,120,280,[8,0])
#-------------新增boss區--------------------------------------------------------
    elif _id == 18:
        return Mons('阿雞米德 Lv10',100,25,10,30,70,[0,357])
    elif _id == 19:
        return Mons('青眼白斬雞 Lv20',150,55,0,80,130,[1,197])
    elif _id == 20:
        return Mons('拿著野太刀的雞頭人 Lv30',100,48,33,140,200,[2,329])
    elif _id == 21:
        return Mons('究極青眼白斬雞 Lv40',300,65,35,200,300,[3,342])
    elif _id == 22:
        return Mons('青眼九頭古巨雞 Lv50',120,100,60,250,400,[4,42])
    elif _id == 23:
        return Mons('重甲雞神.雞動號 Lv60',288,125,66,333,600,[5,342])
    elif _id == 24:
        return  Mons('鳥人幫幫主.助教 Lv70',650,100,120,420,1000,[6,229])
    elif _id == 25:
        return Mons('拿著期中預警通知對你微笑.啟彬.吳. Lv80',900,150,140,560,3000,[7,315])
    elif _id == 26:
        return Mons('真.學分掌控.啟彬.吳.The Boss Lv90',500,200,150,600,5800,[8,196])
