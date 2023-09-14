import time
#-----------------------------------倉庫----------------------------------------

class Bag():        #####倉庫類別#####
    def __init__(self,Money,Diamond,Food,Cake,Cookies,Hamburger,Plate,PS5,PS5r,Potion,Potion2,Potion3):
        self.Money = Money           #金幣
        self.Diamond = Diamond       #鑽石
        self.Food = Food             #雞飼料
        self.Cake = Cake             #蛋糕
        self.Cookies = Cookies       #餅乾
        self.Hamburger = Hamburger   #特大麥香雞
        self.Plate = Plate           #飛盤
        self.PS5 = PS5               #全球最便宜的PS5
        self.PS5r = PS5r             #PS5
        self.Potion = Potion
        self.Potion2 = Potion2
        self.Potion3 = Potion3
    def show(self):     #列出資源
        print("目前身上的物品: ")
        print('金幣:    ',self.Money,'  鑽石:    ',self.Diamond)
        print("")

    def show2(self):
        print("目前身上的物品: ")
        print('金幣:    ',self.Money,'  鑽石:    ',self.Diamond)
        print('飛盤:    ',self.Plate,'  全球最便宜的PS5:    ')
#-------------------------------##增減##函式--------------------------------------
    def add_money(self,add):
        self.Money += add
    
    def add_diamond(self,add):
        self.Diamond += add

    def add_food(self,add):
        self.Food += add

    def add_cake(self,add):
        self.Cake += add

    def add_cookies(self,add):
        self.Cookies += add

    def add_hamburger(self,add):
        self.Hamburger += add
        
    def add_plate(self,add):
        self.Plate += add

    def add_PS5(self,add):
        self.PS5 += add

    def add_PS5r(self,add):
        self.PS5r += add

    def add_Potion(self,add):
        self.Potion += add
        
    def add_Potion2(self,add):
        self.Potion2 += add
        
    def add_Potion3(self,add):
        self.Potion3 += add
#--------------------------------##進食##函式---------------------------------------
    def Eat(self,Chicken):
        while 1:
            if Chicken.Day < 2:
                print('此功能尚未開放',end="")
                oo = input("")
                break
            Chicken._State(5)
            Chicken._State(0)
            Chicken._State(1)
            Chicken._State(2)
            Chicken._State(3)
            Chicken._State(4)
            print("----------------------")
            print('0.不吃了',
                  '\n1.有機雞飼料:    剩餘',self.Food,"袋",
                  '\n  效果: 體力值增加 1 ,飽食度增加 8',
                  '\n2.生乳酪蛋糕:    剩餘',self.Cake,"塊",
                  '\n  效果: 體力值增加 3 ,飽食度增加 20',
                  '\n3.餅乾:    剩餘',self.Cookies,"包",
                  '\n  效果: 體力值增加 8 ,飽食度增加 30 ,心情增加 5',
                  '\n4.特大麥香雞:    剩餘',self.Hamburger,"個",
                  '\n  效果: 體力值增加 20 ,飽食度增加 40',end="")
            print('\n5.小-回復藥水:    剩餘',self.Potion,"個",
                  '\n  效果: 血量增加 50',
                  '\n6.大-回復藥水:    剩餘',self.Potion2,"個",
                  '\n  效果: 血量增加 200',
                  '\n7.極-回復藥水:    剩餘',self.Potion3,"個",
                  '\n  效果: 血量增加 1000')
            print("----------------------")
            print("")
            eat = input('請輸入想吃的物品編號: ')
            if eat == "0":
                break
            if eat == "1" and self.Food > 0:
                Chicken._MP(1)      #能力值增加
                Chicken._Hunger(8)
                self.add_food(-1)    #食物減少
                print(Chicken.Name,'吃了有機雞飼料 ! ! ')
                time.sleep(0.5)
                print(Chicken.Name,'的體力值增加 1 ,飽食度增加 8 ')
                time.sleep(0.5)
            elif eat == "1":
                print("食物不足")
            if eat == "2" and self.Cake > 0:
                Chicken._MP(3)
                Chicken._Hunger(20)
                self.add_cake(-1)
                print(Chicken.Name,'一臉滿足的吃下了生乳酪蛋糕 ')
                time.sleep(0.5)
                print(Chicken.Name,'的體力值增加 3 ,飽食度增加 20 ')
                time.sleep(0.5)
            elif eat == "2":
                print("食物不足")
            if eat == "3" and self.Cookies > 0:
                Chicken._MP(8)
                Chicken._Hunger(30)
                Chicken._Emotion(5)
                self.add_cookies(-1)
                print('喀滋喀滋... ',Chicken.Name,'優雅的吃完餅乾了 ')
                time.sleep(0.5)
                print(Chicken.Name,'的體力值增加 8 ,飽食度增加 30 ,心情增加 5 ')
                time.sleep(0.5)
            elif eat == "3":
                print("食物不足")
            if eat == "4" and self.Hamburger > 0:
                Chicken._MP(20)
                Chicken._Hunger(40)
                Chicken._Emotion(30)
                self.add_hamburger(-1)
                print('吧唧吧唧... ',Chicken.Name,'含著眼淚吃完了特大麥香雞... ')
                time.sleep(0.5)
                print(Chicken.Name,'的體力值增加 20 ,飽食度增加 40 \n但是心情減少 30 ......')
                time.sleep(0.5)
            elif eat == "4":
                print("食物不足")
            if eat == "5" and self.Potion > 0:
                Chicken._HP(50)
                self.add_Potion(-1)
                print('逼',Chicken.Name,' 喝下了回復藥水')
                time.sleep(0.5)
                print(Chicken.Name,'的血量增加 50')
                time.sleep(0.5)
            elif eat == "5":
                print("藥水不足")
            if eat == "6" and self.Potion2 > 0:
                Chicken._HP(200)
                self.add_Potion2(-1)
                print('逼',Chicken.Name,' 喝下了回復藥水')
                time.sleep(0.5)
                print(Chicken.Name,'的血量增加 200')
                time.sleep(0.5)
            elif eat == "6":
                print("藥水不足")
            if eat == "7" and self.Potion3 > 0:
                Chicken._HP(1000)
                self.add_Potion3(-1)
                print('逼',Chicken.Name,' 喝下了回復藥水')
                time.sleep(0.5)
                print(Chicken.Name,'的血量增加 1000')
                time.sleep(0.5)
            elif eat == "7":
                print("藥水不足")
