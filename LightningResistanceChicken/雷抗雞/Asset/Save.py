import os

def save(Chicken,Bag,Boss,First_Open):
    with open(os.getcwd()+os.sep+"save.txt","wt",encoding="utf-8") as save:
        save.write("Chicken = Chicken_System.Chicken"+ "(" +"'"+
                   str(Chicken.Name) +"'"+","+ "'"+
                   str(Chicken.State) +"'"+","+
                   str(Chicken.State_)+","+
                   str(Chicken.Level) +","+
                   str(Chicken.EXP) +","+
                   str(Chicken.EXP_) +","+
                   str(Chicken.HP) +","+
                   str(Chicken.HP_) +","+
                   str(Chicken.ATK) +","+
                   str(Chicken.ATK_) +","+
                   str(Chicken.DF) +","+
                   str(Chicken.DF_) +","+
                   str(Chicken.MP) +","+
                   str(Chicken.MP_) +","+
                   str(Chicken.Hunger) +","+
                   str(Chicken.Emotion) +","+
                   str(Chicken.Luck) +","+
                   str(Chicken.Map) +","+
                   str(Chicken.Point) +","+
                   str(Chicken.Resistance) +","+
                   str(Chicken.Day) +","+
                   str(Chicken.Time) + ")"+"\n")
        save.write("Bag = Bag_System.Bag"+"("+
                   str(Bag.Money) +","+
                   str(Bag.Diamond) +","+
                   str(Bag.Food) +","+
                   str(Bag.Cake) +","+
                   str(Bag.Cookies) +","+
                   str(Bag.Hamburger) +","+
                   str(Bag.Plate) +","+
                   str(Bag.PS5) +","+
                   str(Bag.PS5r) +","+
                   str(Bag.Potion) +","+
                   str(Bag.Potion2) +","+
                   str(Bag.Potion3) +")"+"\n")
        save.write("Boss = "+str(Boss))
'''
        save.write("First_Open = "+ First_Open)
'''
