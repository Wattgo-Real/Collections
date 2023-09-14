using UnityEngine;
using Unity.Mathematics;
using System.Collections.Generic;


public class SplitMeshData
{
    //原始數據
    public Vector3[] ObjOPP;  // ObjPointPositio
    public Vector3[] ObjN;  // ObjNormal
    public int[] ObjT;  //ObjTriangle

    //輸出
    public Vector3[] ToOPP;  // ObjPointPositio
    public Vector3[] ToN;  // ObjNormal
    public int[] ToT;  //ObjTriangle
    int OPPC; //Count

    //真實位置
    Matrix4x4 ObjectMat;
    Matrix4x4 ObjectMatInverse;

    //原始 + 世界座標數據
    Vector3[] GlobalRPL;  //從RepeatPointList

    //重複的點集合
    int[] RepeatPointTo;
    List<Vector3> RepeatPointList = new List<Vector3>();
    float[] RepeatPointValue;  //點分數
    int[] RepeatPointInOrNot;  //PC 組合的點位置

    //域值
    public float threshold = 0.0005f;

    public void InputMesh(Vector3[] P, Vector3[] N, int[] T, Matrix4x4 ObjMat){
        ObjOPP = P;
        ObjT = T;
        ObjN = N;

        RepeatPointTo = new int[P.Length];

        ConbineRepeatPoint();

        GlobalRPL = new Vector3[RepeatPointList.Count];

        ToWorldPosition(ObjMat);
        RepeatPointValue = new float[GlobalRPL.Length];
        RepeatPointInOrNot = new int[GlobalRPL.Length];
    }

    void ConbineRepeatPoint(){
        for (int i = 0;i < ObjOPP.Length; i++){
            bool noRepeat = true;
            for (int j = 0;j < RepeatPointList.Count; j++){
                if (ObjOPP[i] == RepeatPointList[j]){
                    RepeatPointTo[i] = j;
                    noRepeat = false;
                    break;
                }
            }

            if (noRepeat){
                RepeatPointList.Add(ObjOPP[i]);
                RepeatPointTo[i] = RepeatPointList.Count-1;
            }
        }
    }

    public void ToWorldPosition(Matrix4x4 ObjMat){
        ObjectMat = ObjMat;
        ObjectMatInverse = ObjMat.inverse;
        for (int i = 0;i < GlobalRPL.Length; i++){
            GlobalRPL[i] = ObjectMat.MultiplyPoint(RepeatPointList[i]);
        }
    }

    public void CutByFace(Vector3 planeFunction,float planeValue){
        for(int i = 0; i < GlobalRPL.Length;i++){
            RepeatPointValue[i] = Vector3.Dot(GlobalRPL[i], planeFunction) - planeValue;
            RepeatPointInOrNot[i] = (RepeatPointValue[i] >= 0) ? 1:0;
        }
        int totalPoint = 0; 
        
        for(int i = 0; i < ObjT.Length/3; i++){
            int score = RepeatPointInOrNot[RepeatPointTo[ObjT[i*3]]] + 
                        RepeatPointInOrNot[RepeatPointTo[ObjT[i*3+1]]] + 
                        RepeatPointInOrNot[RepeatPointTo[ObjT[i*3+2]]];
            
            if(score == 3){
                totalPoint += 3;
            }else if(score == 2){
                totalPoint += 6;
            }else if(score == 1){
                totalPoint += 3;
            }
        }

        ToOPP = new Vector3[totalPoint];
        ToN = new Vector3[totalPoint];
        ToT = new int[totalPoint];
        OPPC = 0;

        //兩個數值為一個邊線，並且紀錄點位置
        List<Vector3> SideRepeatPointList = new List<Vector3>(0);
        List<int2> SidePointT = new List<int2>(0);  
        for(int i = 0; i < ObjT.Length/3; i++){
            int s = i*3;
            float a1 = RepeatPointValue[RepeatPointTo[ObjT[s]]];
            float a2 = RepeatPointValue[RepeatPointTo[ObjT[s+1]]];
            float a3 = RepeatPointValue[RepeatPointTo[ObjT[s+2]]];
            Vector3 v1 = ObjOPP[ObjT[s]];
            Vector3 v2 = ObjOPP[ObjT[s+1]];
            Vector3 v3 = ObjOPP[ObjT[s+2]];

            if (a1 >= 0 && a2 >= 0 && a3 >= 0){
                MeshEditZeroS(ObjN[ObjT[s]], v1, v2, v3, a1, a2, a3);
            }else if(a1 < 0 && a2 >= 0 && a3 >= 0){
                MeshEditOneS(ObjN[ObjT[s]], v1, v2, v3, a1, a2, a3, 
                ref SideRepeatPointList, ref SidePointT);
            }else if(a2 < 0 && a3 >= 0 && a1 >= 0){
                MeshEditOneS(ObjN[ObjT[s]], v2, v3, v1, a2, a3, a1,  
                ref SideRepeatPointList, ref SidePointT);
            }else if(a3 < 0 && a1 >= 0 && a2 >= 0){
                MeshEditOneS(ObjN[ObjT[s]], v3, v1, v2, a3, a1, a2,  
                ref SideRepeatPointList, ref SidePointT);
            }else if(a1 >= 0 && a2 < 0 && a3 < 0){
                MeshEditTwoS(ObjN[ObjT[s]], v1, v2, v3, a1, a2, a3,  
                ref SideRepeatPointList, ref SidePointT);
            }else if(a2 >= 0 && a3 < 0 && a1 < 0){
                MeshEditTwoS(ObjN[ObjT[s]], v2, v3, v1, a2, a3, a1,  
                ref SideRepeatPointList, ref SidePointT);
            }else if(a3 >= 0 && a1 < 0 && a2 < 0){
                MeshEditTwoS(ObjN[ObjT[s]], v3, v1, v2, a3, a1, a2,  
                ref SideRepeatPointList, ref SidePointT);
            }
        }

        //for (int i = 0; i < SideRepeatPointList.Count; i++){
        //    Debug.Log(i.ToString() + " , " + SideRepeatPointList[i].ToString());
        //}
        //for (int i = 0; i < SidePointT.Count; i++){
        //    Debug.Log(i.ToString()+ " , " +SidePointT[i].ToString() + " , " + SideRepeatPointList[SidePointT[i][0]].ToString() + " , " + SideRepeatPointList[SidePointT[i][1]].ToString() );
        //}

        for (int i = SidePointT.Count-1; i >= 0 ;i--){
            if (SidePointT[i][0] == SidePointT[i][1]){
                SidePointT.RemoveAt(i);
            }
        }

        //邊線連線
        List<List<int>> SideLine = new List<List<int>>(); 
        if(SideRepeatPointList.Count != 0){
            //因為使用的是原始點數據，所以平面要從世界空間轉到物體空間
            Vector3 planeFunctionR = ObjectMatInverse.MultiplyVector(planeFunction).normalized;
            LineInOrder(ref SideLine, SidePointT, SideRepeatPointList, planeFunctionR);
        }

        int totalAddLen = 0;
        for (int i = SideLine.Count-1; i >= 0; i--){
            if (SideLine[i].Count == 0){
                SideLine.RemoveAt(i);
            }
        }  
        for (int i = 0; i < SideLine.Count;i++){
            totalAddLen += (SideLine[i].Count-2) * 3; 
        }  

        if (totalAddLen != 0){
            ArrayAddL(ref ToOPP, totalAddLen);
            ArrayAddL(ref ToN, totalAddLen);
            ArrayAddL(ref ToT, totalAddLen);

            for (int i = 0; i < SideLine.Count;i++){
                for (int j = 0; j < SideLine[i].Count-3;j++){
                    ToOPP[OPPC] = SideRepeatPointList[SideLine[i][0]];
                    ToN[OPPC] = -planeFunction;
                    ToT[OPPC] = OPPC;
                    OPPC += 1;
                    for (int k = j+2; k > j;k--){
                        ToOPP[OPPC] = SideRepeatPointList[SideLine[i][k]];
                        ToN[OPPC] = -planeFunction;
                        ToT[OPPC] = OPPC;
                        OPPC += 1;
                    }
                }
            }
        }
    }

    void LineInOrder(ref List<List<int>> SideLine, List<int2> SidePointT, List<Vector3> SideRepeatPointList, Vector3 planeFunction){

        SideLine = new List<List<int>>();
        //假如 (1,2) | (2,4) | (4,1) 找到重複的連起來 -> 1,2,4,1 
        //並且 (1,2) cross (2,4) = 0 -> 1,2,1 -> 不合4個，不會輸出面
        FindTheFace(ref SideLine, SidePointT, SideRepeatPointList, planeFunction);

        //尋找有沒有面中面的情況，如果有他媽的，先找出可能的情況。
        //在這些面中面情況中，將兩個面連在一起變成一個大型凹面<-天才
        FindFaceInside(ref SideLine, SideRepeatPointList, planeFunction);

        //如果凹面，則新增數個SideLine把凹面分割成一凸一凹或是二凸，不斷迴圈直到所有都是凸面
        SplitToConvex(ref SideLine, SideRepeatPointList, planeFunction);
    }

    void FindTheFace(ref List<List<int>> SideLine, List<int2> SidePointT, List<Vector3> SideRepeatPointList, Vector3 planeFunction){
        int breakTimes = 0;
        bool finish = true;
        int i = 0;
        int pCount = 0;
        int nowSide = 0;
        while (true){
            breakTimes += 1;
            if (finish == true){
                finish = false;
                SideLine.Add(new List<int>());
            }

            List<int> samePoint = new List<int>();
            for (i = 0;i < SidePointT.Count;i++){
                if(SidePointT[nowSide][1] == SidePointT[i][0] && i != nowSide){
                    breakTimes = 0;
                    samePoint.Add(i);
                }
            }

            int SPC = samePoint.Count;
            if (SPC >= 1){   
                int point = 0;
                //找到角度最小的點
                if (SPC >= 2){
                    float angleVS = -1;
                    float SangleVS = 0;
                    Vector3 a2_a1 = (SideRepeatPointList[SidePointT[nowSide][1]] - SideRepeatPointList[SidePointT[nowSide][0]]).normalized;
                    for (i = 0;i < samePoint.Count;i++){
                        Vector3 a3_a2 = (SideRepeatPointList[SidePointT[samePoint[i]][1]] - SideRepeatPointList[SidePointT[nowSide][1]]).normalized;
                        float isIn = Vector3.Dot(Vector3.Cross(a2_a1, a3_a2), planeFunction);
                        float angleB = Vector3.Dot(a2_a1, a3_a2) * 0.5f + 0.5f ;
                        angleVS = (isIn >= -threshold) ? ((angleB > angleVS) ? angleB:angleVS) : ((-angleB > angleVS) ? -angleB:angleVS);
                        if (SangleVS != angleVS){
                            point = i;
                            SangleVS = angleVS;
                        }
                    }
                }else{
                    point = 0;
                }

                int SSPT = SidePointT[samePoint[point]][1];  //因為會Remove所以要儲存SaveSidePointT
                SideLine[pCount].Add(SidePointT[nowSide][0]);  //To環會不斷減少，直到沒有
                SidePointT.RemoveAt(nowSide);
                nowSide = (samePoint[point] > nowSide) ? samePoint[point]-1:samePoint[point];   //因為有Remove所以要排

                //看環是否有連接，如果只有兩個?? (1,0) | (0,1) 先不冒風險
                if(SSPT == SideLine[pCount][0]){
                    SideLine[pCount].Add(SidePointT[nowSide][0]);
                    SideLine[pCount].Add(SideLine[pCount][0]);  //為了之後探測凹凸方便
                    SidePointT.RemoveAt(nowSide);
                    pCount += 1;

                    //如果沒環，結束，如果還是有環，再進行其他環的探測
                    if (SidePointT.Count == 0){
                        break;
                    }else{
                        finish = true;
                        nowSide = 0;
                    }
                }
            }else{ 
                //如果找不到，刪除這個找到的
                SidePointT.RemoveAt(nowSide--);
            }

            if(breakTimes > 200){
                Debug.LogError("Error Edit Side Mesh. ErrorCode: CESM001");
                break;
            }
        }
    }

    void SplitToConvex(ref List<List<int>> SideLine, List<Vector3> SideRepeatPointList, Vector3 planeFunction){
        int i=0;int j;
        //int breakTimes = 0;
        Vector3 a1;
        Vector3 a2;
        Vector3 a3;
        while (SideLine.Count > i){
            j = 1;
            while (j < SideLine[i].Count-1){
                a1 = SideRepeatPointList[SideLine[i][j-1]];
                a2 = SideRepeatPointList[SideLine[i][j]];
                a3 = SideRepeatPointList[SideLine[i][j+1]];
                Vector3 normal = Vector3.Cross(a2-a1, planeFunction).normalized;
                float value = Vector3.Dot(normal, a2);
                int SLC = SideLine[i].Count;
                if(Vector3.Dot(normal, a3) - value > threshold*10 && SLC >= 5){
                    int fondCR = -1; //分割終點
                    int startCR = -1;  //分割起始點
                    Vector3 normal2 = Vector3.Cross(a3 - a2, planeFunction).normalized;
                    float value2 = Vector3.Dot(normal2, a2);

                    //Debug.Log(i+" , "+j);
                    //for (int k  = 0;k<SLC;k++){
                    //    Debug.Log(k+" , "+SideRepeatPointList[SideLine[i][k]]);
                    //}
                    Vector3 p1,p2;
                    if (fondCR == -1){
                        //往前探測凹面範圍
                        List<int> saveK = new List<int>();
                        for (int k = j-3; k >= -1; k--){
                            p1 = (k==-1)? SideRepeatPointList[SideLine[i][SLC-2]]:SideRepeatPointList[SideLine[i][k]];
                            p2 = SideRepeatPointList[SideLine[i][k+1]];
                            //Debug.Log(SideRepeatPointList[SideLine[i][k+1]]+" , "+(Vector3.Dot(normal2,p1)-value2).ToString() +" , "+(Vector3.Dot(normal2,p2)-value2).ToString() );
                            if (((Vector3.Dot(normal,p1) - value > threshold && Vector3.Dot(normal,p2) - value <= threshold) ||
                                (Vector3.Dot(normal2,p1) - value2 > threshold && Vector3.Dot(normal2,p2) - value2 <= threshold)) &&
                                SideLine[i][k+1] != SideLine[i][j] && SideLine[i][k+1] != SideLine[i][j+1]){  // <=因為域值大保險，並且不能是特殊情形，以及面中面不重複
                                saveK.Add(k);
                            }
                        }
                        //繼續往前探測凹面範圍
                        for (int k = SLC-3; k >= j+2; k--){
                            p1 = SideRepeatPointList[SideLine[i][k]];
                            p2 = SideRepeatPointList[SideLine[i][k+1]];
                            if (((Vector3.Dot(normal,p1) - value > threshold && Vector3.Dot(normal,p2) - value <= threshold) ||
                                (Vector3.Dot(normal2,p1) - value2 > threshold && Vector3.Dot(normal2,p2) - value2 <= threshold)) &&
                                SideLine[i][k+1] != SideLine[i][j] && SideLine[i][k+1] != SideLine[i][j+1]){  // <=經過value=的點，並且不能是特殊情形，以及面中面不重複
                                saveK.Add(k);
                            }
                        }
                        for (int k = 0; k < saveK.Count;k++){
                            //Debug.Log("k: "+SideRepeatPointList[SideLine[i][saveK[k]+1]]);
                        }
                        
                        int SK = 0;
                        if (saveK.Count == 1){
                            SK =  saveK[0];
                            if (j > SK){
                                startCR = SK+1; fondCR = j; j = SK+2;
                            }else{
                                startCR = j; fondCR = SK+1; j++;
                            }
                            //Debug.Log(i+" , "+SideLine[i][startCR]+" , "+SideLine[i][fondCR]+" , "+SideRepeatPointList[SideLine[i][startCR]] + " , " + SideRepeatPointList[SideLine[i][fondCR]]);
                        }else if (saveK.Count > 1){
                            float minDK = -1000000;  //篩選，距離要最小，並且不能是反的面
                            for (int l = 0; l < saveK.Count; l++){    
                                p1 = (saveK[l] == -1)? SideRepeatPointList[SideLine[i][SLC-2]]:SideRepeatPointList[SideLine[i][saveK[l]]];
                                p2 = SideRepeatPointList[SideLine[i][saveK[l]+1]];
                                Vector3 normal3 = Vector3.Cross(p2 - p1, planeFunction).normalized;
                                //計算分數，分數越大越好，但是不能大於零
                                float Cdis = Vector3.Dot(normal3,SideRepeatPointList[SideLine[i][j]]) - Vector3.Dot(normal3, p1);
                                //Debug.Log("fa"+" , "+Cdis);
                                if (Cdis <= threshold){
                                    if(Cdis >= minDK){
                                        SK = saveK[l];
                                        minDK = Cdis;
                                    }
                                }
                            }
                            if (j > SK){
                                startCR = SK+1; fondCR = j; j = SK+2;
                            }else{
                                startCR = j; fondCR = SK+1; j++;
                            }
                            //Debug.Log(i+" , "+startCR+" , "+fondCR+" , "+SideRepeatPointList[SideLine[i][startCR]] + " , " + SideRepeatPointList[SideLine[i][fondCR]]);
                        }
                    }

                    if (fondCR == -1){
                        if(j >= 2){
                            startCR = j-2; fondCR = j; j--;
                        }else{
                            startCR = j; fondCR = SLC-3+j; j++;
                        }
                        Debug.LogError("Error Edit Side Mesh. ErrorCode: CESM004");
                    }

                    if (fondCR != -1){
                        if(fondCR-startCR <= 1 || ((SLC-1==fondCR) && (startCR <= 1))){
                            Debug.Log("CESM005: " +SideRepeatPointList[SideLine[i][startCR]] + " , " + SideRepeatPointList[SideLine[i][fondCR]]+ " , " + SLC);
                            Debug.Log("CESM005: " +startCR + " , " + fondCR+ " , " + SLC);
                            SideLine[i].RemoveAt(startCR);
                            Debug.LogError("Error Edit Side Mesh. ErrorCode: CESM005");
                            break;
                        }

                        int pCount = SideLine.Count;
                        SideLine.Add(new List<int>());
                        SideLine[pCount].Add(SideLine[i][startCR]);
                        for(int k = 0; k < fondCR-startCR - 1; k++){   // -1，因為fondCR自己不算
                            SideLine[pCount].Add(SideLine[i][startCR+1]);
                            SideLine[i].RemoveAt(startCR+1);
                        }
                        SideLine[pCount].Add(SideLine[i][startCR+1]);
                        SideLine[pCount].Add(SideLine[i][startCR]);   //為了方便
                        continue;
                    }
                }
                j++;
            }
            i++;
        }
    }

    void FindFaceInside(ref List<List<int>> SideLine, List<Vector3> SideRepeatPointList, Vector3 planeFunction){
        int i,j;
        Vector3 a1;
        Vector3 a2;
        Vector3 a3;
        for (i = 0; i < SideLine.Count;i++){
            for (int k = 1; k < SideLine[i].Count-1; k++){
                a1 = SideRepeatPointList[SideLine[i][k-1]];
                a2 = SideRepeatPointList[SideLine[i][k]];
                a3 = SideRepeatPointList[SideLine[i][k+1]];
                float oValue = Vector3.Dot(Vector3.Cross(a2-a1, a3-a2),planeFunction);
                //如果角度太接近180，直接連在一起就好，減少計算量
                if (oValue < threshold*10 && oValue > -threshold*10){
                    SideLine[i].RemoveAt(k);
                } 
            }
        }

        int SCount = SideLine.Count;
        int[] SDIDByPoint = new int[SCount];              //離圓心最遠的點，一定是凸點
        Vector3[] CrossSDIDByPoint = new Vector3[SCount];    //面是正轉還是反轉
        bool[] OuterSDIDByPoint = new bool[SCount];    //面是正轉還是反轉
        Vector3[] CenterOfTheFace = new Vector3[SCount];  //圓心簡略估計
        float[] maxDistanceByPoint = new float[SCount];   //離圓心最遠的距離
        for (i = 0; i < SCount; i++){
            int SideLineC = SideLine[i].Count;
            //圓心簡略估計
            a1 = SideRepeatPointList[SideLine[i][0]];
            a2 = SideRepeatPointList[SideLine[i][((SideLineC-1)/3)]];
            a3 = SideRepeatPointList[SideLine[i][(2*(SideLineC-1)/3)]];
            CenterOfTheFace[i] = a1+a2+a3;

            maxDistanceByPoint[i] = 0;
            float SaveDis = 0;
            Vector3 SaveA4 = new Vector3();
            for (j = 0; j < SideLineC-1; j++){
                float distancet = Vector3.Distance(SideRepeatPointList[SideLine[i][j]],CenterOfTheFace[i]);
                SaveDis += distancet;
                if (maxDistanceByPoint[i] < distancet){
                    SaveA4 = SideRepeatPointList[SideLine[i][j]];
                    maxDistanceByPoint[i] = distancet;
                    SDIDByPoint[i] = j;
                }
            }
            if (SaveA4 != a1 || SaveA4 != a2 || SaveA4 != a3){
                CenterOfTheFace[i] = CenterOfTheFace[i]*3/4 + SaveA4*1/4;
            }
            maxDistanceByPoint[i] = (maxDistanceByPoint[i] + SaveDis/(SideLineC-1))/2;
        }


        for (i = 0; i < SCount; i++){
            a1 = (SDIDByPoint[i] == 0)? SideRepeatPointList[SideLine[i][SideLine[i].Count-2]]:
                                        SideRepeatPointList[SideLine[i][SDIDByPoint[i]-1]];
            a2 = SideRepeatPointList[SideLine[i][SDIDByPoint[i]]];               
            a3 = SideRepeatPointList[SideLine[i][SDIDByPoint[i]+1]]; 
            CrossSDIDByPoint[i] = Vector3.Cross(a2 - a1, a3 - a2);
            OuterSDIDByPoint[i] = (Vector3.Dot(CrossSDIDByPoint[i], planeFunction) > 0);
            
            //Debug.Log(OuterSDIDByPoint[i] + " , " + CenterOfTheFace[i]  + " , " + maxDistanceByPoint[i]);
        }

        List<int2> FaceInside = new List<int2>();   //面中面紀錄
        List<float> FaceInsideScore = new List<float>();   //面中面圓心距離
        //先找出有可能的選項，這樣可以必免計算輛太大int2(外面,裡面)

        for (i = 0; i < SCount; i++){
            for (j = 0; j < SCount; j++){
                if (OuterSDIDByPoint[i] == true && OuterSDIDByPoint[j] == false){
                    float maxDis = Vector3.Distance(CenterOfTheFace[i], CenterOfTheFace[j]);
                    if (maxDistanceByPoint[i]*2 - (maxDistanceByPoint[j]+maxDis) > 0){
                        float maxDis2 = Vector3.Distance(SideRepeatPointList[SideLine[i][SDIDByPoint[i]]], CenterOfTheFace[j]);
                        float Score = (maxDis2 > maxDis) ? -maxDis2:-maxDis;
                        //如果出現複數個外部，選則外面半徑最小的，以及離圓心最近的
                        bool NoOuter = true;
                        for (int k = 0; k < FaceInside.Count; k++){
                            if (FaceInside[k][1] == j){
                                NoOuter = false;
                                Debug.Log(i + " , " + j);
                                Debug.Log(Score + " , " + FaceInsideScore[k]);
                                if(Score > FaceInsideScore[k]){
                                    FaceInside[k] = new int2(i,j);
                                    FaceInsideScore[k] = Score;
                                    break;
                                }
                            }
                        }
                        if (NoOuter){
                            FaceInside.Add(new int2(i,j));
                            FaceInsideScore.Add(Score);
                        }
                    }
                }
            }
        }


        int2[] SMDID = new int2[FaceInside.Count];
        //以裡面的一個非凹點去探測與外面所有點最近的點(沒做好)
        for (i = 0; i < FaceInside.Count; i++){
            IsInsideTheFace(ref SMDID, i, FaceInside, SDIDByPoint, CrossSDIDByPoint, SideLine, SideRepeatPointList, planeFunction);
            //如果ok, 組和面
            if (SMDID[i][0] != -1){
                //Debug.Log(i+" , "+SideRepeatPointList[SideLine[FaceInside[i][0]][SMDID[i][0]]]+" , "+SideRepeatPointList[SideLine[FaceInside[i][1]][SMDID[i][1]]]);

                int back = SMDID[i][0];
                int SB = SideLine[FaceInside[i][0]][SMDID[i][0]];
                for (j = SMDID[i][1]; j < SideLine[FaceInside[i][1]].Count-1;j++){
                    back++;
                    SideLine[FaceInside[i][0]].Insert(back, SideLine[FaceInside[i][1]][j]);
                }
                for (j = 0; j <= SMDID[i][1];j++){
                    back++;
                    SideLine[FaceInside[i][0]].Insert(back, SideLine[FaceInside[i][1]][j]);
                }
                back++;
                SideLine[FaceInside[i][0]].Insert(back, SB);
            }
        }

        //刪除
        for (i = 0; i < SMDID.Length; i++){
            if (SMDID[i][0] != -1){    
                SideLine[FaceInside[i][1]] = new List<int>();
            }
        }  
    }

    void IsInsideTheFace(ref int2[] SMDID, int i, List<int2> FaceInside,int[] SDIDByPoint, Vector3[] CrossSDIDByPoint, 
            List<List<int>> SideLine, List<Vector3> SideRepeatPointList, Vector3 planeFunction){
        Vector3 a1;
        Vector3 a2;
        Vector3 a3;
        Vector3 Ftp;

        Debug.Log(FaceInside[i]);
        SMDID[i][0] = -1;
        int StargetP = SDIDByPoint[FaceInside[i][1]];  //小圓距離圓心最遠的一定是凸點
        Vector3 targetP = SideRepeatPointList[SideLine[FaceInside[i][1]][StargetP]];
        a1 = (StargetP == 0) ? SideRepeatPointList[SideLine[FaceInside[i][1]][SideLine[FaceInside[i][1]].Count-2]]:
                                        SideRepeatPointList[SideLine[FaceInside[i][1]][StargetP-1]];
        a2 = SideRepeatPointList[SideLine[FaceInside[i][1]][StargetP]];               
        a3 = SideRepeatPointList[SideLine[FaceInside[i][1]][StargetP+1]];

        //先計算凸點兩個邊的分數
        Vector3 normal1 = Vector3.Cross(a2 - a1, -CrossSDIDByPoint[FaceInside[i][1]]);
        float value1 = Vector3.Dot(normal1, a2);
        Vector3 normal2 = Vector3.Cross(a3 - a2, -CrossSDIDByPoint[FaceInside[i][1]]);
        float value2 = Vector3.Dot(normal2, a2);

        int minDistantID = -1;
        float minDistant = 1000000;
        for (int j = 0; j < SideLine[FaceInside[i][0]].Count; j++){
            Ftp = SideRepeatPointList[SideLine[FaceInside[i][0]][j]];
            if ((Vector3.Dot(normal1, Ftp) - value1 < threshold) || (Vector3.Dot(normal2, Ftp) - value2 < threshold)){
                Vector3 distancet = targetP - Ftp;
                float distancetValue = distancet.magnitude;
                //必須要是那個點的上方才行
                if (distancetValue < minDistant){
                    minDistant = distancetValue;
                    minDistantID = j;
                }
            }
        }

        SMDID[i][0] = minDistantID;
        SMDID[i][1] = StargetP;
        /*
        if (minDistantID != -1){
            //兩個都要是凸角才可以
            a1 = (minDistantID == 0) ? SideRepeatPointList[SideLine[FaceInside[i][0]][SideLine[FaceInside[i][0]].Count-2]]:
                                            SideRepeatPointList[SideLine[FaceInside[i][0]][minDistantID-1]];
            a2 = SideRepeatPointList[SideLine[FaceInside[i][0]][minDistantID]];               
            a3 = SideRepeatPointList[SideLine[FaceInside[i][0]][minDistantID+1]]; 
            bool b1 = FindIsConcave(a1, a2, targetP, planeFunction);
            bool b2 = FindIsConcave(a2, a3, targetP, planeFunction);

            Debug.Log("eer"+a2+" , "+targetP);
            //如果ok, 組和面
            if (b1 == false && b2 == false){
                SMDID[i][0] = minDistantID;
                SMDID[i][1] = StargetP;
            }
        }
        */
    }

    bool FindIsConcave(Vector3 a1,Vector3 a2, Vector3 a3, Vector3 planeFunction){
        Vector3 normal = Vector3.Cross(a2-a1, planeFunction).normalized;
        float p1 = Vector3.Dot(normal, a3) - Vector3.Dot(normal, a2);
        if (p1 > -threshold){
            return true;  //凹面
        }else{
            return false;
        }
    }
    
    void MeshEditZeroS(Vector3 Normal, Vector3 p1, Vector3 p2, Vector3 p3, float a1,float a2,float a3){
        ToOPP[OPPC] = p1;
        ToN[OPPC] = Normal;
        ToT[OPPC] = OPPC;
        OPPC += 1;
        ToOPP[OPPC] = p2;
        ToN[OPPC] = Normal;
        ToT[OPPC] = OPPC;
        OPPC += 1;
        ToOPP[OPPC] = p3;
        ToN[OPPC] = Normal;
        ToT[OPPC] = OPPC;
        OPPC += 1;
    }

    //只有 a1<0 的情況
    void MeshEditOneS(Vector3 Normal,Vector3 p1, Vector3 p2, Vector3 p3, float a1, float a2, float a3, 
                      ref List<Vector3> SideRepeatPointList, ref List<int2> SidePointT){
        Vector3 p12 = Vector3.Lerp(p1,p2,-a1/(-a1+a2)); //因為a1是負的
        Vector3 p13 = Vector3.Lerp(p1,p3,-a1/(-a1+a3));

        SidePointT.Add(new int2(SideRPL(ref SideRepeatPointList, p13),SideRPL(ref SideRepeatPointList, p12)));

        ToOPP[OPPC] = p12;
        ToN[OPPC] = Normal;
        ToT[OPPC] = OPPC;
        OPPC += 1;
        ToOPP[OPPC] = p2;
        ToN[OPPC] = Normal;
        ToT[OPPC] = OPPC;
        OPPC += 1;
        ToOPP[OPPC] = p3;
        ToN[OPPC] = Normal;
        ToT[OPPC] = OPPC;
        OPPC += 1;

        ToOPP[OPPC] = p13;
        ToN[OPPC] = Normal;
        ToT[OPPC] = OPPC;
        OPPC += 1;
        ToOPP[OPPC] = p12;
        ToN[OPPC] = Normal;
        ToT[OPPC] = OPPC;
        OPPC += 1;
        ToOPP[OPPC] = p3;
        ToN[OPPC] = Normal;
        ToT[OPPC] = OPPC;
        OPPC += 1;
    }

   //只有 a1>0 的情況
    void MeshEditTwoS(Vector3 Normal, Vector3 p1, Vector3 p2, Vector3 p3, float a1,float a2,float a3,
                      ref List<Vector3> SideRepeatPointList, ref List<int2> SidePointT){
        Vector3 p12 = Vector3.Lerp(p1,p2,a1/(a1-a2));//因為a2、a3是負的
        Vector3 p13 = Vector3.Lerp(p1,p3,a1/(a1-a3));

        SidePointT.Add(new int2(SideRPL(ref SideRepeatPointList, p12),SideRPL(ref SideRepeatPointList, p13)));

        ToOPP[OPPC] = p1;
        ToN[OPPC] = Normal;
        ToT[OPPC] = OPPC;
        OPPC += 1;
        ToOPP[OPPC] = p12;
        ToN[OPPC] = Normal;
        ToT[OPPC] = OPPC;
        OPPC += 1;
        ToOPP[OPPC] = p13;
        ToN[OPPC] = Normal;
        ToT[OPPC] = OPPC;
        OPPC += 1;
    }

    int SideRPL(ref List<Vector3> SideRepeatPointList, Vector3 add){
        for (int i = 0 ; i < SideRepeatPointList.Count; i++){
            if(SideRepeatPointList[i] == add){
                return i;
            } 
        }
        SideRepeatPointList.Add(add);
        return SideRepeatPointList.Count-1;
    } 

    void ArrayAddL(ref int[] obj,int Num)
    {
        int nowHave = obj.Length;
        int[] SAVE = new int[nowHave];
        SAVE = obj;
        obj = new int[nowHave + Num];
        SAVE.CopyTo(obj,0);
    }

    void ArrayAddL(ref Vector3[] obj,int Num)
    {
        int nowHave = obj.Length;
        Vector3[] SAVE = new Vector3[nowHave];
        SAVE = obj;
        obj = new Vector3[nowHave + Num];
        SAVE.CopyTo(obj,0);
    }
}