using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Unity.Mathematics;

public class SplicMesh : MonoBehaviour
{
    public GameObject[] obj;
    Mesh[] objectMesh;
    int[][] triangles;
    Vector3[][] objNormal;
    Vector3[][] objPointsR; //原始世界座標數據，目標是紀錄所有的初始點位置，並且不要有重複的點
    Vector3[][] objPointsO; //原始世界座標數據，目標是紀錄所有的初始點位置
    int[][][] objPointsRB; //恢復成原始數據的指標，也就第幾個點
    Vector3[][] objPointsOB; //目標是紀錄所有的初始點位置
    Vector3[] objCenter;
    float[] objFarthestPoint;

    public GameObject plane;
    Mesh planeMesh;
    Vector3[] planePoints;
    float objValue,dValue;

    struct PointsData
    {
        public int3 triangle;
        public int triangleW;
        public int triangleW2;  //沒用不知道當時在想甚麼
        public float Value;
        public Vector3 normal;
        public bool inOrNot;
    }
    PointsData[][] PD;

    int publicInt, publicInt2;
    float publicFloat, publicFloat2;
    bool publicBool;
    Vector3 publicVector3;
    
    // Start is called before the first frame update
    void Start()
    {
        firstSplicMesh = true;
        planeMesh = plane.GetComponent<MeshFilter>().mesh;
        planePoints = planeMesh.vertices;

        PD = new PointsData[obj.Length][];

        objectMesh = new Mesh[obj.Length];
        triangles = new int[obj.Length][];
        objNormal = new Vector3[obj.Length][];
        objPointsR = new Vector3[obj.Length][];
        objPointsRB = new int[obj.Length][][];
        objPointsO = new Vector3[obj.Length][];
        objPointsOB = new Vector3[obj.Length][];
        objCenter = new Vector3[obj.Length];
        objFarthestPoint = new float[obj.Length];

        for (int i = 0; i < obj.Length; i++)
        {
            objectMesh[i] = obj[i].GetComponent<MeshFilter>().mesh;
            MeshKill(ref objectMesh[i]);
            triangles[i] = objectMesh[i].triangles;
            objNormal[i] = objectMesh[i].normals; 
            objPointsR[i] = objectMesh[i].vertices;
            objPointsO[i] = objectMesh[i].vertices;
            objPointsOB[i] = objectMesh[i].vertices;
            PD[i] = new PointsData[objectMesh[i].vertices.Length];

            for (int j = 0; j < triangles[i].Length/3; j++)
            {
                int3 tri= new int3(triangles[i][j*3],triangles[i][j*3+1],triangles[i][j*3+2]);
                for (int k = 0; k < 3; k++)
                {
                    PD[i][tri[k]].triangle = tri; //得到物體triangle的點索引
                    PD[i][tri[k]].triangleW = j;  //第幾個
                    PD[i][tri[k]].normal = objNormal[i][tri[k]];
                }
            }
            //objPointsR合併重複的點並且objPointsRB給與原先點的位置objPointsO則轉換成世界座標
            CombineRepeatPointAndReOnMesh(ref objPointsR[i],ref objPointsRB[i],ref objPointsO[i],obj[i].transform.localToWorldMatrix);

            //objCenter全部點加起來的中心
            MyLibrary.FindCenter(out objCenter[i], objPointsR[i]);

            //objFarthestPoint與中心相距最遠的點
            MyLibrary.CalculationPointMixDistanceFromCenter(ref objFarthestPoint[i], objPointsR[i], objCenter[i]);
        }  
        sw = new System.Diagnostics.Stopwatch();   
    }

    public static void CombineRepeatPointAndReOnMesh(ref Vector3[] point,ref int[][] pointTo,ref Vector3[] pointT,Matrix4x4 mat)
    {
        Vector3[] pointO = new Vector3[point.Length];
        point.CopyTo(pointO,0);
        int[] thf = new int[point.Length];
        int t = 0;
        for(int j=0;j<point.Length;j++)
        {
            pointT[j] = mat.MultiplyPoint(new Vector4(pointO[j].x,pointO[j].y,pointO[j].z,1f));
            for(int i=0;i<point.Length;i++)
            {
                if (point[j] == point[i] && j != i)
                    point[i] = Vector3.zero;
            }
            if (point[j] != Vector3.zero)
            {
                thf[t] = j;
                t++;
            }
        }

        point = new Vector3[t];
        pointTo = new int[t][];
        for (int i = 0; i < t; i++)
        {
            Vector3 pointIn = pointO[thf[i]];
            point[i] = mat.MultiplyPoint(new Vector4(pointIn.x,pointIn.y,pointIn.z,1f));
            int ss = 0;
            for (int j = 0; j < pointO.Length; j++)
            {
                if (pointIn == pointO[j])
                {
                    ss++;
                }
            }
            pointTo[i] = new int[ss];
            int s = 0;
            for (int j = 0; j < pointO.Length; j++)
            {
                if (pointIn == pointO[j])
                {
                    pointTo[i][s] = j;
                    s++;
                }
            }
        }
    }

    void MeshKill(ref Mesh meshR)
    {
        int[] s = new int[meshR.vertices.Length];
        Vector3[] poi = meshR.vertices;
        Vector3[] nor = meshR.normals;
        int[] tri = meshR.triangles;
        publicInt = meshR.vertexCount;

        for (int i = 0; i < meshR.triangles.Length/3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                if (s[tri[i*3+j]] == 0)
                {
                    s[tri[i*3+j]] += 1;
                }else
                {
                    MyLibrary.ArrayAddL(ref poi, poi[tri[i*3+j]]);
                    MyLibrary.ArrayAddL(ref nor, nor[tri[i*3+j]]);
                    tri[i*3+j] = publicInt;
                    publicInt++;
                }
            }
        }

        meshR.vertices = poi;
        meshR.normals = nor;
        meshR.triangles = tri;
    }

    int[] trianglesIn;
    Vector3[] objNormalIn;
    Vector3[] objpointIn;
    int tL,nL,pL;
    bool stopEditMesh, firstSplicMesh;

    Vector3[][] sideNormalS;
    Vector3[] sidePointS;
    Vector3 planeNormal;
    Matrix4x4 planeMat;
    Matrix4x4 SAVEplaneMat;
    System.Diagnostics.Stopwatch sw;

    void Update()
    {
        planeMat = plane.transform.localToWorldMatrix;
        sw = new System.Diagnostics.Stopwatch();
        
        if (SAVEplaneMat != planeMat)
        {
            Splic();
        }
        SAVEplaneMat = planeMat;
        if(sw.ElapsedMilliseconds != 0)
        {
            Debug.Log(sw.ElapsedMilliseconds);
        }
    }

    void Splic()
    {
        planeNormal = planeMat.MultiplyVector(planeMesh.normals[0]).normalized;
        Vector4 pointNow = new Vector4(planePoints[0].x,planePoints[0].y,planePoints[0].z,1);
        pointNow = planeMat.MultiplyPoint(pointNow);
        float planeValue = Vector3.Dot(pointNow, planeNormal);

        for (int i = 0; i < obj.Length; i++)
        {
            objValue = Vector3.Dot(objCenter[i],planeNormal) - planeValue;

            //當面遠離物體時不運作
            if ((objValue <= (objFarthestPoint[i]*1.1) &&
                objValue >= -(objFarthestPoint[i]*1.1)) || firstSplicMesh == true)
            {
                firstSplicMesh = false;
                stopEditMesh = false;
                sideNormalS = new Vector3[0][];
                sidePointS = new Vector3[0];
                sw.Start(); 
                int[] splicPoint =  new int[0];
                int splicL = 0;
                for (int j = 0; j < objPointsR[i].Length; j++)
                {
                    objValue = Vector3.Dot(objPointsR[i][j],planeNormal) - planeValue;
                    if (objValue < 0) //Here
                    {
                        //增加list個數
                        MyLibrary.ArrayAddL(ref splicPoint,j);
                        for (int k = 0; k < objPointsRB[i][j].Length; k++)
                        {
                            PD[i][objPointsRB[i][j][k]].inOrNot = false;
                            PD[i][objPointsRB[i][j][k]].Value = objValue;
                            splicL++;
                        }
                    }else
                    {
                        for (int k = 0; k < objPointsRB[i][j].Length; k++)
                        {
                            PD[i][objPointsRB[i][j][k]].inOrNot = true;
                            PD[i][objPointsRB[i][j][k]].Value = objValue;
                        }
                    }
                }
                tL = triangles[i].Length;
                nL = objNormal[i].Length;
                pL = objPointsOB[i].Length;
                trianglesIn = new int[tL+(splicL*2+splicL)*3];
                objNormalIn = new Vector3[nL+splicL*2+splicL];
                objpointIn = new Vector3[pL+splicL*2+splicL];
                triangles[i].CopyTo(trianglesIn,0);
                objNormal[i].CopyTo(objNormalIn,0);
                objPointsOB[i].CopyTo(objpointIn,0);
                sw.Stop();
                for (int j = 0; j < splicPoint.Length; j++)
                {
                    for (int k = 0; k < objPointsRB[i][splicPoint[j]].Length; k++)
                    {
                        MeshEdit(i ,objPointsRB[i][splicPoint[j]][k], planeValue, planeNormal);                   
                    }
                }

                PointSort(i);

                if(stopEditMesh == false)
                {
                    objectMesh[i].Clear();
                    objectMesh[i].vertices = objpointIn;
                    objectMesh[i].normals = objNormalIn;
                    objectMesh[i].triangles = trianglesIn;
                }
            }
        }
    }
    
    Vector3[] SAVE;
    Vector3 save;
    int Re = 0;

    //如果沒有重複就加上
    public void ArrayAddLNC(ref Vector3[] obj,ref int Re,Vector3 Num)
    {
        publicBool = true;
        Re = 0;
        for (int i = 0; i < obj.Length; i++)
        {
            //obj[i].x + 0.001 >= Num.x && obj[i].x - 0.001 <= Num.x && obj[i].y + 0.001 >= Num.y && obj[i].y - 0.001 <= Num.y && obj[i].z + 0.001 >= Num.z && obj[i].z - 0.001 <= Num.z
            if (obj[i] == Num)
            {
                publicBool = false;
                break;
            }
            Re++;
        }

        if (publicBool == true)
        {
            int nowHave = obj.Length;
            Vector3[] SAVE = new Vector3[nowHave];
            SAVE = obj;
            obj = new Vector3[nowHave+1];
            SAVE.CopyTo(obj,0);
            obj[nowHave] = Num;        
        }
    }

    //如果沒有重複就加上
    public void ArrayAddLNC(ref Vector3[] obj ,Vector3 Num)
    {
        publicBool = true;
        for (int i = 0; i < obj.Length; i++)
        {
            if (obj[i] == Num)
            {
                publicBool = false;
                break;
            }
        }

        if (publicBool == true)
        {
            int nowHave = obj.Length;
            SAVE = new Vector3[nowHave];
            SAVE = obj;
            obj = new Vector3[nowHave+1];
            SAVE.CopyTo(obj,0);
            obj[nowHave] = Num;
        }


    }

    void AddSide(Vector3 p,Vector3 normal)
    {
        int u = sidePointS.Length;
        Re = 0;
        ArrayAddLNC(ref sidePointS,ref Re, p);
        if (u == sidePointS.Length)
        {
            ArrayAddLNC(ref sideNormalS[Re],normal);
        }else
        {
            MyLibrary.ArrayAddL(ref sideNormalS);
            sideNormalS[Re] = new Vector3[0];
            ArrayAddLNC(ref sideNormalS[Re],normal);
        }
    }

    void PointSort(int o)
    {
        Re = 1;
        List<int> ct = new List<int>();
        for (int i = 0; i < sidePointS.Length; i++)
        {
            if (1 == sideNormalS[i].Length)
            {
                sideNormalS[i] = new Vector3[0];
            }else
            {
                //Debug.Log(sideNormalS[i].Length);
                ct.Add(i);
            }
        }

        int[] LPS = new int[ct.Count*2];
        int ii = 0;
        bool noSame = false;
        bool isConcavePolygon = false;
        int2 loopIF = new int2(0,0);
        Vector3 crossIs = new Vector3(0f,1f,0f);
        List<int> isCoxPoint = new List<int>();
        List<int2> isSplitMesh = new List<int2>();

        while (ii < ct.Count-1)
        {
            List<int> yy = new List<int>();
            List<int> oo = new List<int>();

            for (int j = ii+1; j < ct.Count; j++)
            {
                //for (int k = 0; k < sideNormalS[ct[j]].Length; k++)
                //{
                //    Debug.Log(ct[ii]+" : "+ct[j]+" : "+sideNormalS[ct[j]][k]+" : "+sideNormalS[ct[ii]][Re]);
                //}
                if(sideNormalS[ct[ii]].Length==Re)
                {
                    //stopEditMesh = true;
                    //Debug.LogError("Mesh Edit ERROR 1");
                    //return;
                }
                for (int k = 0; k < sideNormalS[ct[j]].Length; k++)
                {
                    //找到相似的點
                    if (sideNormalS[ct[j]][k].x + 0.001f >= sideNormalS[ct[ii]][Re].x && sideNormalS[ct[j]][k].x - 0.001f <= sideNormalS[ct[ii]][Re].x)
                    {
                        oo.Add(ct[j]);
                        yy.Add(k);  
                        break;
                    }
                }
            }

            publicBool = false;
            if (oo.Count > 1)
            {
                Debug.Log("Noooo");
                objValue =  Vector3.Dot(sideNormalS[ct[ii]][Re],sidePointS[ct[ii]]);
                float MD = 100000;
                for (int j = 0; j < oo.Count; j++)
                {
                    publicFloat = Vector3.Distance(sidePointS[oo[j]], sidePointS[ct[ii]]);
                    dValue = Vector3.Dot(sidePointS[oo[j]],sideNormalS[ct[ii]][Re]);
                    if (dValue + 0.0001 >= objValue && dValue - 0.0001 <= objValue)
                    {
                        if (publicFloat < MD)
                        {
                            publicBool = true;
                            MD = publicFloat;
                            oo[0] = oo[j];
                            yy[0] = yy[j];
                        }
                    }
                }
            }
            if ((oo.Count == 0 && Re-1 == sideNormalS[ct[ii]].Length)|| (oo.Count > 1 && publicBool == false))
            {
                //感覺有問題?? 因為sideNormalS應該要動全員去找
                objValue =  Vector3.Dot(sideNormalS[ct[ii]][Re],sidePointS[ct[ii]]);
                for (int i = 0; i < sideNormalS[ct[loopIF[1]]].Length; i++)
                {
                    if (sideNormalS[ct[ii]][Re] == sideNormalS[ct[loopIF[1]]][i])
                    {
                        dValue = Vector3.Dot(sidePointS[ct[loopIF[1]]],sideNormalS[ct[ii]][Re]);
                        if (dValue + 0.0001 >= objValue && dValue - 0.0001 <= objValue)
                        {
                            noSame = true;
                            loopIF[0] = loopIF[1];
                            loopIF[1] = ii+1;
                        }
                    }
                }
            }
            else if(oo.Count != 0)
            {
                LPS[ii*2+1] = Re;
                LPS[ii*2+2] = yy[0];

                Re = (yy[0] == 0) ? 1:0;

                save = sidePointS[ct[ii+1]];
                sidePointS[ct[ii+1]] = sidePointS[oo[0]];
                sidePointS[oo[0]] = save;

                SAVE = sideNormalS[ct[ii+1]];
                sideNormalS[ct[ii+1]] = sideNormalS[oo[0]];
                sideNormalS[oo[0]] = SAVE;
                ii++;
                publicBool = true;
            }

            if (publicBool == false)
            {
                Re += 1;
            }
            if (Re > 5)
            {
                Debug.LogError("Mesh Edit ERROR 2");
                return;
            }

            //ii在倒數第二行的地方會停下，或是遇到了特殊截面
            if (ii == ct.Count-1 || noSame == true)
            {
                if (ii == ct.Count-1)
                {
                    loopIF[0] = loopIF[1];
                    loopIF[1] = ct.Count;
                    isSplitMesh.Add(loopIF);
                }

                for (int i = 0; i < sideNormalS[ct[loopIF[0]]].Length; i++)
                {
                    for (int j = 0; j < sideNormalS[ct[loopIF[1]-1]].Length; j++)
                    {
                        if (sideNormalS[ct[loopIF[0]]][i] == sideNormalS[ct[loopIF[1]-1]][j])
                        {
                            LPS[(loopIF[0])*2] = i;
                            LPS[2*loopIF[1]-1] = j;
                        }
                    }
                }

                for (int i = loopIF[0]; i < loopIF[1]; i++)
                {
                    save = sideNormalS[ct[i]][LPS[i*2+1]];
                    sideNormalS[ct[i]][0] = sideNormalS[ct[i]][LPS[i*2]];
                    sideNormalS[ct[i]][1] = save;
                }            

                if(noSame == true)
                {
                    isSplitMesh.Add(loopIF);
                    Re = 1;
                    noSame = false;
                    ii++;
                }

                FideSideMeshDirection(ref ct,ref isConcavePolygon,ref isCoxPoint,ref crossIs, loopIF, o);
            }
            
        }

        if (isConcavePolygon == false && isCoxPoint.Count == 0 && isSplitMesh.Count == 0)
        {
            SideMeshEdit(ct, o, crossIs);
        }else
        {
            List<List<int>> ctIn = new List<List<int>>();
            if (isSplitMesh.Count!=0)
            {
                for (int i = 0; i < isSplitMesh.Count; i++)
                {
                    ctIn.Add(new List<int>());
                    ctIn[i] = ct.GetRange(isSplitMesh[i][0],isSplitMesh[i][1]-isSplitMesh[i][0]);
                }
            }
            
            //for (int i = 0; i < ctIn.Count; i++)
            //{
            //   for (int j = 0; j < ctIn[i].Count; j++)
            //    {
            //        Debug.Log(sidePointS[ctIn[i][j]]);
            //    }
            //    Debug.Log("----------");
            //}

            int4 sore = new int4();
            int2 wtf = new int2();
            Re = 0;
            while(ctIn.Count != 0) 
            {
                List<int> isClear = new List<int>();
                for (int i = 0; i < ctIn.Count; i++)
                {
                    publicInt2 = 0;
                    if (ctIn[i].Count == 3)
                    {
                        SideMeshEdit(ctIn[i], o, crossIs);
                        isClear.Add(i);
                    }else
                    {
                        for (int j = 0; j < ctIn[i].Count; j++)
                        {
                            //isConcavePolygon
                            int isCPP = 0;
                            for (int k = 0; k < isCoxPoint.Count; k++)
                            {
                                if (isCoxPoint[k] == ctIn[i][j])
                                {
                                    isCPP = k;
                                    break;
                                }
                            }
                            if (isCoxPoint.Count != 0 && isCoxPoint[isCPP] == ctIn[i][j])
                            {
                                publicInt2++;
                                if(j == 0)
                                {
                                    publicVector3 = Vector3.Cross(planeNormal,sidePointS[ctIn[i][j]]-sidePointS[ctIn[i][ctIn[i].Count-1]]);
                                }else
                                {
                                    publicVector3 = Vector3.Cross(planeNormal,sidePointS[ctIn[i][j]]-sidePointS[ctIn[i][j-1]]);
                                }

                                objValue = Vector3.Dot(sidePointS[ctIn[i][j]],publicVector3); 
                                for (int k = 1; k < ctIn[i].Count; k++)
                                {
                                    publicInt = j+k;
                                    if ( publicInt >= ctIn[i].Count)
                                    {
                                         publicInt -= ctIn[i].Count;
                                    }
                                    publicFloat = Vector3.Dot(sidePointS[ctIn[i][publicInt]],publicVector3);
                                    if (publicFloat - objValue < 0)
                                    {
                                        sore[0] = j;
                                        sore[1] =  (publicInt == 0)? ctIn[i].Count-1: publicInt-1;
                                        sore[2] =  publicInt;
                                        if (sore[0]<sore[2])
                                        {
                                            wtf[0] = sore[1] - sore[0];
                                            wtf[1] =ctIn[i].Count + sore[0] - sore[2];
                                        }else if (sore[0]>sore[2] && sore[0]>sore[1])
                                        {
                                            wtf[0] = ctIn[i].Count + sore[1] - sore[0];
                                            wtf[1] = sore[0]-sore[2];
                                        }else
                                        {
                                            wtf[0] = sore[1] - sore[0];
                                            wtf[1] = sore[0];
                                        }

                                        ctIn.Add(new List<int>());
                                        ctIn[ctIn.Count-1].Add(ctIn[i][sore[0]]);
                                        ctIn[ctIn.Count-1].Add(ctIn[i][sore[1]]);
                                        ctIn[ctIn.Count-1].Add(ctIn[i][sore[2]]);

                                        ctIn.Add(new List<int>());
                                        for (int m = 0; m < wtf[0]+1; m++)
                                        {
                                            publicInt = j+m;
                                            if ( publicInt >= ctIn[i].Count)
                                            {
                                                publicInt -= ctIn[i].Count;
                                            }
                                            ctIn[ctIn.Count-1].Add(ctIn[i][ publicInt]);
                                        }
                                        
                                        ctIn.Add(new List<int>());
                                        for (int m = 0; m < wtf[1]+1; m++)
                                        {
                                            publicInt = sore[2]+m;
                                            if ( publicInt >= ctIn[i].Count)
                                            {
                                                publicInt -= ctIn[i].Count;
                                            }
                                            ctIn[ctIn.Count-1].Add(ctIn[i][ publicInt]);
                                        }
                                        isCoxPoint.RemoveAt(isCPP);
                                        isClear.Add(i);
                                        break;
                                    }
                                    else if(publicFloat - objValue - 0.01 < 0)
                                    {
                                        sore[0] = j;
                                        sore[1] =  publicInt;
                                        if (sore[0] < sore[1])
                                        {
                                            wtf[0] = sore[1] - sore[0];
                                            wtf[1] = ctIn[i].Count + sore[0] - sore[1];
                                        }else if(sore[1] < sore[0])
                                        {
                                            wtf[1] = sore[0] - sore[1];
                                            wtf[0] = ctIn[i].Count + sore[1] - sore[0];
                                        }

                                        ctIn.Add(new List<int>());
                                        for (int m = 0; m < wtf[0]+1; m++)
                                        {
                                            publicInt = sore[0]+m;
                                            if (publicInt >= ctIn[i].Count)
                                            {
                                                publicInt -= ctIn[i].Count;
                                            }
                                            ctIn[ctIn.Count-1].Add(ctIn[i][publicInt]);
                                        }

                                        ctIn.Add(new List<int>());
                                        for (int m = 0; m < wtf[1]+1; m++)
                                        {
                                            publicInt = sore[1]+m;
                                            if ( publicInt >= ctIn[i].Count)
                                            {
                                                publicInt -= ctIn[i].Count;
                                            }
                                            ctIn[ctIn.Count-1].Add(ctIn[i][publicInt]);
                                        }

                                        isCoxPoint.RemoveAt(isCPP);
                                        isClear.Add(i);
                                        break;
                                    }
                                }
                            }
                            if ( publicInt2 != 0)
                            {
                                break;
                            }
                        }
                        if ( publicInt2 == 0)
                        {
                            SideMeshEdit(ctIn[i], o, crossIs);
                            isClear.Add(i);
                        }
                    }
                    if ( publicInt2 != 0)
                    {
                        break;
                    }
                }
                for (int i = isClear.Count-1; i >= 0; i--)
                {
                    ctIn.RemoveAt(isClear[i]);
                }

                Re++;
                if(Re>100)
                {
                    Debug.LogError("Error on whlie");
                    return;
                }
            }
        }
    }

    bool Roeww;
    void FideSideMeshDirection(ref List<int> ct,ref bool isConcavePolygon,ref List<int> isCoxPoint,ref Vector3 crossIs, int2 loopIF, int o)
    {
        int3 intP = new int3(0,0,0);
        List<int> isCoxPointC = new List<int>();
        List<int> isCoxPointC2 = new List<int>();
        Vector3 SAVEpublicVector3 = new Vector3(0f,0f,0f);
        publicFloat = 0;
        publicBool = false;
        for (int i = loopIF[0]; i < loopIF[1]; i++)
        {
            intP = new int3(i,i+1,i-1);
            if (i == loopIF[0])
                intP[2] += loopIF[1] - loopIF[0];
            if (i == loopIF[1]-1)
                intP[1] -= loopIF[1] - loopIF[0];

            publicVector3 = obj[o].transform.localToWorldMatrix.MultiplyVector(
                    Vector3.Cross(sidePointS[ct[intP[1]]]-sidePointS[ct[intP[0]]],sidePointS[ct[intP[2]]]-sidePointS[ct[intP[0]]]).normalized);
            publicFloat2 = sidePointS[ct[i]].magnitude;

            if (Vector3.Dot(publicVector3,planeNormal) > 0.01)
            {
                if (publicFloat2 > publicFloat)
                {
                    publicFloat = publicFloat2;
                    publicBool = true;
                    SAVEpublicVector3 = publicVector3;
                }
                isCoxPointC2.Add(ct[i]);
            }else if (Vector3.Dot(publicVector3,planeNormal) < -0.01)
            {
                if (publicFloat2 > publicFloat)
                {
                    publicFloat = publicFloat2;
                    publicBool = false;
                    SAVEpublicVector3 = publicVector3;
                }
                isCoxPointC.Add(ct[i]);
            }
        }
        //Debug.Log(SAVEpublicVector3);
        //所有凹點與凸點位置加起來的值比較，如果凸點加起來小於凹點，則表示凸點的位置其實是凹點
        if (publicBool == true && loopIF[0] == 0)
        {
            Roeww = false;
            if(isCoxPointC.Count == 0 && loopIF[0] == 0)
            {
                isConcavePolygon = false;
            }else if(loopIF[0] == 0)
            {
                isConcavePolygon = true;
            }
            crossIs = SAVEpublicVector3;
        }else if (publicBool == false && loopIF[0] == 0)
        {
            Roeww = true;
            if(isCoxPointC2.Count == 0 && loopIF[0] == 0)
            {
                isConcavePolygon = false;
            }else if (loopIF[0] == 0)
            {
                isConcavePolygon = true;
            }
            crossIs = SAVEpublicVector3;
        }

        if (Roeww == true && publicBool == true && loopIF[0] != 0)
        {
            ct.Reverse(loopIF[0],loopIF[1]-loopIF[0]);
        }else if (Roeww == false && publicBool == false && loopIF[0] != 0)
        {
            ct.Reverse(loopIF[0],loopIF[1]-loopIF[0]);
        }

        if (publicBool == true)
        {
            for (int i = 0; i < isCoxPointC.Count; i++)
            {
                isCoxPoint.Add(isCoxPointC[i]);
            }
        }else if (publicBool == false)
        {
            for (int i = 0; i < isCoxPointC2.Count; i++)
            {
                isCoxPoint.Add(isCoxPointC2[i]);
            }  
        }
    }

    void SideMeshEdit(List<int> ct,int o,Vector3 crossIs)
    {
        if (ct.Count >= 3)
        {
            Vector3 nTo = obj[o].transform.worldToLocalMatrix.MultiplyVector(-planeNormal);
            publicBool = crossIs.x + 0.01 > planeNormal.x && crossIs.x - 0.01 < planeNormal.x;

            for (int i = 0; i < ct.Count; i++)
            {
                objpointIn[pL++] = sidePointS[ct[i]];
                objNormalIn[nL++] = nTo;
            }
            if (publicBool == true)
            {
                for (int i = 0; i < ct.Count-2; i++)
                {
                   trianglesIn[tL++] = pL-1;
                   trianglesIn[tL++] = pL-i-2;
                   trianglesIn[tL++] = pL-i-3;
                }
            }else
            {
                for (int i = 0; i < ct.Count-2; i++)
                {
                   trianglesIn[tL++] = pL-i-3;
                   trianglesIn[tL++] = pL-i-2;
                   trianglesIn[tL++] = pL-1;
                }
            }
        }
    }
    
    Vector3 p0,p1,p2,p3,p4;
    float a0,a1,a2;
    int3 NP;

    void MeshEdit(int i ,int pointIn ,float planeValue,Vector3 normal)
    {
        NP = PD[i][pointIn].triangle;

        if(PD[i][NP[0]].inOrNot == false && PD[i][NP[1]].inOrNot == false && PD[i][NP[2]].inOrNot == false)
        {
            trianglesIn[PD[i][pointIn].triangleW*3] = 0;
            trianglesIn[PD[i][pointIn].triangleW*3+1] = 0;
            trianglesIn[PD[i][pointIn].triangleW*3+2] = 0;
            return;
        }

        a0 = PD[i][NP[0]].Value;
        a1 = PD[i][NP[1]].Value;
        a2 = PD[i][NP[2]].Value;
        
        if (a0 < 0 && a1 > 0 && a2 > 0) //(p3,p1,p2) add(p2,p4,p3)
        {
            p1 = objPointsOB[i][NP[1]];
            p2 = objPointsOB[i][NP[2]];
            p3 = Vector3.Lerp(objPointsOB[i][NP[0]],objPointsOB[i][NP[1]],(-a0/(-a0+a1)));
            p4 = Vector3.Lerp(objPointsOB[i][NP[0]],objPointsOB[i][NP[2]],(-a0/(-a0+a2)));

            AddSide(p3,PD[i][pointIn].normal);
            AddSide(p4,PD[i][pointIn].normal);

            objpointIn[NP[0]] = p3;
            objpointIn[pL++] = p4;
            objNormalIn[nL++] = objNormal[i][NP[0]];
            trianglesIn[tL++] = NP[2];
            trianglesIn[tL++] = pL-1;
            trianglesIn[tL++] = NP[0];

            PD[i][NP[0]].triangleW2 = tL/3-1;
        }else if(a0 > 0 && a1 < 0 && a2 > 0) //(p0,p3,p2) add(p2,p3,p4)
        {
            p0 = objPointsOB[i][NP[0]];
            p2 = objPointsOB[i][NP[2]];
            p3 = Vector3.Lerp(objPointsOB[i][NP[1]],objPointsOB[i][NP[0]],(-a1/(-a1+a0)));
            p4 = Vector3.Lerp(objPointsOB[i][NP[1]],objPointsOB[i][NP[2]],(-a1/(-a1+a2)));

            AddSide(p3,PD[i][pointIn].normal);
            AddSide(p4,PD[i][pointIn].normal);

            objpointIn[NP[1]] = p3;
            objpointIn[pL++] = p4;
            objNormalIn[nL++] = objNormal[i][NP[0]];
            trianglesIn[tL++] = NP[2];
            trianglesIn[tL++] = NP[1];
            trianglesIn[tL++] = pL-1;

            PD[i][NP[0]].triangleW2 = tL/3-1;
        }else if(a0 > 0 && a1 > 0 && a2 < 0) //(p0,p1,p3) add(p1,p4,p3)
        {
            p0 = objPointsOB[i][NP[0]];
            p1 = objPointsOB[i][NP[1]];
            p3 = Vector3.Lerp(objPointsOB[i][NP[2]],objPointsOB[i][NP[0]],(-a2/(-a2+a0)));
            p4 = Vector3.Lerp(objPointsOB[i][NP[2]],objPointsOB[i][NP[1]],(-a2/(-a2+a1)));

            AddSide(p3,PD[i][pointIn].normal);
            AddSide(p4,PD[i][pointIn].normal);

            objpointIn[NP[2]] = p3;
            objpointIn[pL++] = p4;
            objNormalIn[nL++] = objNormal[i][NP[0]];
            trianglesIn[tL++] = NP[1];
            trianglesIn[tL++] = pL-1;
            trianglesIn[tL++] = NP[2];

            PD[i][NP[0]].triangleW2 = tL/3-1;
        }else if(a0 > 0 && a1 < 0 && a2 < 0)
        {
            p0 = objPointsOB[i][NP[0]];
            p1 = Vector3.Lerp(objPointsOB[i][NP[0]],objPointsOB[i][NP[1]],(a0/(a0-a1)));
            p2 = Vector3.Lerp(objPointsOB[i][NP[0]],objPointsOB[i][NP[2]],(a0/(a0-a2)));

            AddSide(p1,PD[i][pointIn].normal);
            AddSide(p2,PD[i][pointIn].normal);

            objNormalIn[nL++] = objNormal[i][NP[0]];
            objpointIn[pL++] = p0;
            objpointIn[NP[1]] = p1;
            objpointIn[NP[2]] = p2;
            trianglesIn[PD[i][pointIn].triangleW*3] = pL-1;
        }else if(a0 < 0 && a1 > 0 && a2 < 0)
        {
            p0 = Vector3.Lerp(objPointsOB[i][NP[1]],objPointsOB[i][NP[0]],(a1/(a1-a0)));
            p1 = objPointsOB[i][NP[1]];
            p2 = Vector3.Lerp(objPointsOB[i][NP[1]],objPointsOB[i][NP[2]],(a1/(a1-a2)));

            AddSide(p0,PD[i][pointIn].normal);
            AddSide(p2,PD[i][pointIn].normal);

            objNormalIn[nL++] = objNormal[i][NP[0]];
            objpointIn[NP[0]] = p0;
            objpointIn[pL++] = p1;
            objpointIn[NP[2]] = p2;
            trianglesIn[PD[i][pointIn].triangleW*3+1] = pL-1;
        }else if(a0 < 0 && a1 < 0 && a2 > 0)
        {
            p0 = Vector3.Lerp(objPointsOB[i][NP[2]],objPointsOB[i][NP[0]],(a2/(a2-a0)));
            p1 = Vector3.Lerp(objPointsOB[i][NP[2]],objPointsOB[i][NP[1]],(a2/(a2-a1)));
            p2 = objPointsOB[i][NP[2]];

            AddSide(p0,PD[i][pointIn].normal);
            AddSide(p1,PD[i][pointIn].normal);

            objNormalIn[nL++] = objNormal[i][NP[0]];
            objpointIn[NP[0]] = p0;
            objpointIn[NP[1]] = p1;
            objpointIn[pL++] = p2;
            trianglesIn[PD[i][pointIn].triangleW*3+2] = pL-1;
        }
    }
}
