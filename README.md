# 作品區

### 目錄 ###
1. 跟 Unity 有關
   * 利用 Unity 框架的 ComputerShader 來做出有利用 GPU 實體渲染具有重力與碰撞的物體
   * 利用 Unity 框架做出 Real-Time 的以一個二維平面將任何形狀的三維物體切成兩半 (完全是自己想的方法)
2. 跟 DeepLearning 有關
   * 利用 Reinforcement 原理用 pytorch 打造一個自動尋找出路的 Agent
   * 利用 ViT 原理用 pytorch 打造一個基於 Transformer 的 Text Recognition，可見TrOCR論文
   * 其他小實作 (U-Net 實作、GaN 笑臉與哭臉 等等)
3. 與無人機相關的
   * 無人機控制端，利用 ESP32 寫出一個四軸無人機的系統。
   * 用C++做的OpenCV的SLAM部分系統 (找出相機相對角度與位置)
4. 其他的小作品
   * Python版的檔案總管。
   * Python版的文字電子雞RPG遊戲。
   * C++圍棋遊戲。

### 介紹 ###
### 1. 跟 Unity 有關 ###
   * #### 利用 Unity 框架的 ComputerShader 來做出有利用 GPU 實體渲染具有重力與碰撞的物體 ####
     檔案在 MyUnityAssets/Assets/Project/GPUPhysicalPartical 中。

     當時我的巴哈介紹文章 : [https://forum.gamer.com.tw/C.php?bsn=60602&snA=3923](https://forum.gamer.com.tw/C.php?bsn=60602&snA=3923)
     
     2021 年的作品，因為我當時是因為我看了 Ming-Lun Chou(周明倫) 的 [GPU Particles with Primitive Collider Physics](https://www.youtube.com/watch?v=t2yPfenzkII&ab_channel=Ming-LunChou) 影片，我覺得很酷所以也想自己來做一個，了解到這是利用 ComputerShader 做出的後，我就開始著手製作這個做品，雖然我是想要達到粒子與粒子之間也能夠進行碰撞效果，但是因為這具有很多的限制，所以到最後還是打算不繼續做下去，其實其中的一個原因也是因為我當時的程式設計能力還不太行。

     ![ComputerShader](https://github.com/Wattgo-Real/Collections/blob/main/Data/Unity%20ComputerShader.gif)

   * #### 利用 Unity 框架做出 Real-Time 的以一個二維平面將任何形狀的三維物體切成一半 (完全是自己想的方法) ####
     檔案在 MyUnityAssets/Assets/Project/2Dsplit 中。

     作法原理 :
     1. 將物體與平面轉換為世界座標系，並且將二維平面法線方向的物體面都去除。
     2. 一個面只能有 3 個點，如果其中有 1~2 個點在二維平面法線方向以外，重新編輯面的資訊，使截面環出現 (此時截面是中空的所以要想方法組合截面)。
     3. 判斷邊線的資訊，並且記錄順序形成一個環，或多個環 (將面所包覆住的環一定為固定方向、這跟面的資訊-法向量-有關，除非有甜甜圈截面)。
     4. 判斷環的拓譜關係並做一些處理，接下來利用環的順序將凹環全部分解為多個凸環，使其能夠組合成面 (要處裡甜甜圈截面、特殊拓譜截面、螺旋截面等特殊情況)。
     5. 面組合後，將物體轉換為原本的坐標系，並且只要平面一有動靜，重新執行以上步驟 (實時)。
    
     ![2DSplit](https://github.com/Wattgo-Real/Collections/blob/main/Data/HI.png)
     
     會做這個是因為我以前看了 mtbdesignworks 所做的 4D 遊戲之後，我覺得非常的酷，所以我當時是想要做一個 3D 截面透視的遊戲，就是 3D 的世界中只能看到 2D 的截面，但是經過了不斷的嘗試之後，都沒有一個好方式去實現 3D 透視的遊戲，所以我就想自己做了一個可以實時的將一個物體切成一半的程式，來形成 3D 截面透視的效果。

     ![2DSplit](https://github.com/Wattgo-Real/Collections/blob/main/Data/2DSplit.gif)

     我有做過兩個版本，第一個是 MeshSplit，第二個是 MeshSplit2，第一個版本是我在 2021 年做的，當時的編程能力還沒有很好，所以寫出來的程式碼非常亂，而且當時因為 Debug 太難就半途而廢了，當時是做到可以將一個簡單的物體分成兩半，簡單的意思就是不是球殼、詭異拓鋪形狀等等特殊的圖形，而第二個版本是我在2023年的暑假花了我三個禮拜做出來的，可以解決剛剛提到的那些問題，就如演示的一樣，只要是複雜沒有面中面的情形是一定能截的完美，但是有時候這些特殊情形會出問題，其實這些問題是可以繼續去解決的，但是其實這個版本已經達成的我當時的要求了，而且我太追求完美，一直去想有沒有更好的算法讓人疲累，最後還是將計就計。

     ![2DSplit](https://github.com/Wattgo-Real/Collections/blob/main/Data/2DSplit.png)

     而經過我這次的練習後，我逐漸意識到寫解釋的用意，因為我寫程式很少會去寫解釋，所以當我花了 3 天去看我以前寫的 MeshSplit 時，發現我完全看不懂我自己寫的，所以才打算重寫。
### 2. 跟DeepLearning有關 ###
   * #### 利用 Reinforcement 原理用 pytorch 打造一個自動尋找出路的Agent ####
     檔案在 AutomaticallySolveTheMaze 中。

     原本是想要在 Unreal Engine 5 裡面訓練 Reinforcement 的 model，但是因為我還不太會用UE5，而且這製作難度也蠻高的，所以我就先直接全部都在Python實現，我使用的方法是結合 DuelingDQN、Distributional DQN這兩種技巧來實現的。

     圖中黑方塊為障礙物，白色是道路，黃色是玩家位置，淺綠色的是出口。

     ![DQN](https://github.com/Wattgo-Real/Collections/blob/main/Data/DQN%20and%20TriORC.gif)

     這其實在 2023 年 4 月我就做出來了，但是訓練了整整一天，成功率還是不高，但是 8 月後有修改了一下，結果出乎我的意料，成功率變超高，而且也只訓練一個半小時。

     這迷宮有一個很重要的規則是走過的地方就只能走一次，走到黑色的格子就要重來，所以跟一般求解迷宮的算法差異很大，讓我蠻驚訝的是他不是走最短路徑，而是有那種摸牆求解迷宮的感覺，甚麼時候該不該摸牆也知道，真酷~

     由於迷宮的生成方式是隨機生成的，這樣做迷宮沒辦法做到人性化，而且沒辦法生成特殊的迷宮，也許以後可以利用Transformer來生成迷宮，這感覺也蠻有趣的。

   * #### 利用 ViT 原理來用 pytorch 打造一個 Transformer-based 的 Text Recognition，可見 TrOCR 論文 ####
     檔案在 TextRecognition 中，參考了 [TrOCR 這篇論文](https://arxiv.org/pdf/2109.10282.pdf)。

     會做這個起因是因為學校的量物課本真的實在太難讀懂了，所以我想要打造一個能夠自動整理重點的AI，有了這個AI也許就能統一重點，讀起來更順暢。但是要能夠分析文本，就要從PDF中取得文本，我是可以利用 Python 的其他庫來直接提取文本，但是如果是圖片檔的話怎麼辦，所以我就順便做了這個出來。

     這個是利用 Transformer 做出來的，這也是我最早利用 Transformer 的作品，2020 年推出的 Vision Transformer 真的是很厲害，讓 Transformer 不再只是只能處裡文字，也可以用於圖片上。

     為了訓練，這裡我使用的是 WIKI 問答的 train 檔案，然後再一個一個字分開後轉成圖片，並加上一些圖像的縮放與位移後丟到 Transformer 模型裡面去，訓練完的成果還不錯，也許是因為參數量我調太大了，效能有點不行，還有這影片的前置作業沒有做，所以也就造成測試時沒有很準確的原因。

   * #### 其他小實作(U-Net實作、GaN笑臉與哭臉 等等) ####
     我沒有放檔案在這裡，因為這些東西太過零散，專業度也不高，我放些照片與介紹較好。

     a. U-Net 實作:
     
     ![U-net](https://github.com/Wattgo-Real/Collections/blob/main/Data/U-net.png)
 
     2022 年做的，我使用的數據集是自己使用 blender 製作的，總共有 2000 張，下面張就是已經訓練完 15 輪的模型與真實做對比的圖片，從中間切一半，右邊是訓練最好的五張，左邊是訓練最爛的五張，看這最爛的五張的特徵，就知道問題出在哪了哈，當時這成果也嚇到我了，因為訓練的時間不到 10 分鐘就有這樣的效果，甚至只要走過 2、3 輪，形就已經出來了。

     b. GAN 笑臉與哭臉 實作:

     ![U-net](https://github.com/Wattgo-Real/Collections/blob/main/Data/SBC.png)

     2021 年做的，是我學完 tensotflow 後的第一個作品，簡單來說就是將笑臉數據利用訓練完的 GAN 轉換成哭臉，我說的這個臉其實是我自己畫的火柴人的臉。
### 3. 與無人機相關的 有關 ###
   * #### 無人機與電腦視覺系統 ####
     檔案在 Drone/MainControl 中，並且 無人機與電腦視覺ToTal.pdf 是整個無人機的報告。
     
     這是大學電子學實驗的專題，題目是用 Arduino 編程式做出一個小裝置，而我們這組就直接挑戰了無人機來當作當時專題內容，而後來就變成了我一個人的專題，做到現在也一年多的，也努力了 500~600 小時，目前能做到的程度是能夠穩定飛行，也能夠用 GPS 來修正無人機的飛行，而這無人機上面也有一個攝像機，是為了用來排除特殊狀況，包括收不到 GPS、磁力受干擾等致命問題，所以才增加了電腦視覺系統的部分，這部分的做法是將 OpenCV 的 SLAM 的部分系統實現在 ESP32CAM 上，只可惜因為效能不佳，應用上去後並沒有達到預期，所以 ESP32CAM 目前只能夠用來傳輸影像。
     
     ![drone_test](https://github.com/Wattgo-Real/Collections/blob/main/Data/drone%20test.gif)

     再來這個專題也有一個很特殊的地方，那就是我使用了高斯濾波的方法來濾波一維實時數據，而這個方法是因為熱物的課程給我的靈感，我認為這是一個比加權平均還要好的濾波手法，教授也是第一次聽過這種方法，只是我都沒看到有人使用這個方法，是因為有弊端嗎?還是其實有更好的方法，蠻好奇的。

   * #### 用 C++ 做的 OpenCV 的 SLAM 部分系統 (找出相機相對角度與位置) ####
     檔案在 Drone/MainControl 中。
     
     這一樣是屬於無人機的部分，
### 4. 其他的小作品 有關 ###
   * #### Python 版的檔案總管 ####
     檔案在 FileManager 中。
     
     ![FileManerger](https://github.com/Wattgo-Real/Collections/blob/main/Data/FileManerger.png)
     
     這個作品是大一計算機概論的報告，主要是利用 python 的 os 庫來製作一個檔案總管的分組作業，功能越多越好，雖然我們是分組來進行的，但是其實幾乎都是我完成的，而我塞了很多功能進去，除了一些最基本的以外，還包括分類排序、還有顯示詳細資料、尋找檔案、而且也可以儲存用戶上次的設定，算是我最早的 python 作品，檔案在 FileManager 中。
   * #### Python 版的文字電子雞RPG遊戲 ####
     檔案在 LightningResistanceChicken 中。
     我與我組員在大一計算機概論的報告，原本其實要求只要做普通的電子雞遊戲而已，但是我們這組越做越多直接把這做成了 RPG 遊戲，其實我們這組的靈感是從 steam 上的文字遊戲而來的，這個遊戲都是文字真的很酷，超有創意的。
   * #### C++ 圍棋遊戲 ####

     ![GO](https://github.com/Wattgo-Real/Collections/blob/main/Data/GO.png)
     
     這也是我 2021 年的作品，這是當時學完網路上 C# 的教學後，最後有實作一個五子棋的遊戲，但我想和他不一樣，所以我就做了圍棋出來。

