using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO;
using Unity.Mathematics;

public class GPUPhysical : MonoBehaviour
{
    //--------------------------------------
    public GameObject[] collisionObj;  //in
    Mesh[] collisionMesh;
    [HideInInspector] public float[][] normalFunctionValue;  //save
    [HideInInspector] public Vector3[][] normalVector;  //save
    
    //--------------------------------------
    [SpaceAttribute] 
    public GameObject particalObj;  //in
    public Vector3 SpawnRangeMix;  //in
    public Vector3 SpawnRangeMin;  //in
    public Vector3 SpawnPos;  //in
    Mesh particalMesh;
    Vector4[] particalVerticesO;
    //bool isCollision=false;
    public int population;  //in
    ParticalGridData[] PGDu;
    GridData[] GDu;
    public Material material;  //in
    private MaterialPropertyBlock materialPropertyBlock;
    private ComputeBuffer meshArgsBuffer;
    private ComputeBuffer physicalComputeIn;
    private ComputeBuffer GridDataBuffer;
    private ComputeBuffer CollisionGridDataBuffer;
    private ComputeBuffer NormalMixDataBuffer;
    private ComputeBuffer PointDataBuffer;
    private ComputeBuffer PointGridDataBuffer;
    private ComputeBuffer PointNormalMixDataBuffer;
    private ComputeBuffer TooManyThingInThereBuffer;
    Bounds viewBounds;
    public ComputeShader physicalCompute;
    //-------------------
    private int to_core1;
    private int to_updataTime;
    private int to_halfBound;
    private int to_boundTot;
    private int to_gravity;
    private int to_CH;
    private int to_centerBound;
    private int to_delateCount; 
    private int to_population;


    //--------------------------------------
    public Vector3 centerBound;
    public int3 bound = new int3(20,20,20);  //save
    int3 boundTot; // (全部,一層,一行),(一半的邊長)
    Vector3 halfBound;
    [HideInInspector]
    public int[][][] collisionGrid;  //save
    int[][][] SAVEcollisionGrid;

    //--------------------------------------
    System.Diagnostics.Stopwatch sw = new System.Diagnostics.Stopwatch();
    public bool keepLoadMesh;  //in

    //--------------------------------------
    //particalObj所有的控制項，比如說位置移動...顏色變化...，傳給buffer共particalObj.lenght個
    struct CellData
    {
        public Matrix4x4 mat;
        public float3 speed;
        public float4 color;
        public float mess;
        public float mixDis;
        public int grid;

        public static int size()
        {
            return sizeof(float) * (4 * 4 + 3 + 4 + 1 + 1) +
                   sizeof(int) * 1;
        }
    }
    
    //傳給buffer共boundTot.x+1(bound.x*bound.y*bound.z+1)個
    struct GridData
    {
        //what:collisionObj所有碰撞面依序格數逐漸增加，ex:0,2,5,7....320 (總共triangle數為320個，並且有boundTot.x+1格)
        //why:ComputeShader無法知道每個格子之間有幾個個數，並且因為矩陣限制，比需告訴ComputeShader哪個區域有幾個triangle碰撞點
        public int CollisionGridDataNOW;  
        //what:同上，但是這主要是用來探測particalObj之間的碰撞體，目前無用處
        public int ParticalGridDataNOW;  
        //what:每個格子有幾個particalObj，目前無用處
        //why:原本是用來探測每個格子有幾個particalObj，但是GPu的並行限制無法讓此具有互動性，已沒用處
        //ex:在第一個格子有兩個particalObj也，所以1+1等於二Ya，幹!怎麼只加一個
        public int toParticalGridData;
        public static int size()
        {
            return sizeof(int) * 3;
        }
    }
    
    //傳給buffer共所有靜態碰撞體的所在格子
    struct CollisionGridData
    {
        //what: 第幾個物體 ex:collisionObj[?]
        //why: 與 CollisionGrid 互相配對，
        public int js;
        //what: 那一個物體是對應地幾個 normal
        public int CollisionGrid;

        public static int size()
        {
            return sizeof(int) * 2;
        }
    }
    
    //傳給buffer共所有靜態碰撞體的碰撞數據
    struct NormalMixData
    {
        public Vector4 normal;
        public float value;

        public static int size()
        {
            return sizeof(float) * 5 ;
        }
    }
    
    //以下四項皆為無用
    struct PointData
    {
        public Vector4 points;
        public int pointLength;
        public int jsLength;
        public static int size()
        {
            return sizeof(float) * 4 +
                   sizeof(int) * 2;
        }
    }
    struct ParticalGridData
    {
        public int ParticalGrid;
        public int inGrid;
        public int gs;
        public static int size()
        {
            return sizeof(int) * 3;
        }
    }
    struct PointNormalMixData
    {
        public Vector4 normal;
        public Vector4 point;
        public static int size()
        {
            return sizeof(float) * 8;
        }

    }
    struct TooManyThingInThere
    {
        public Matrix4x4 s1;
        public Matrix4x4 s2;
        public Matrix4x4 s3;
        public Matrix4x4 s4;
        public static int size()
        {
            return sizeof(float)*64;
        }
    }
    
    Vector3[] particalVerticesPos;
    private void InitialSet()
    {
        //初始數據
        particalMesh = particalObj.GetComponent<MeshFilter>().mesh;
        particalVerticesPos = particalMesh.vertices;
        particalVerticesO =  new Vector4[particalMesh.vertices.Length];
        for (int i = 0; i < particalMesh.vertices.Length; i++)
        {
            particalVerticesO[i].x = particalMesh.vertices[i].x;
            particalVerticesO[i].y = particalMesh.vertices[i].y;
            particalVerticesO[i].z = particalMesh.vertices[i].z;
            particalVerticesO[i].w = 1f;
        }
        normalFunctionValue = new float[collisionObj.Length][];
        normalVector = new Vector3[collisionObj.Length][];
        collisionMesh = new Mesh[collisionObj.Length];
        viewBounds = new Bounds(Vector3.zero, Vector3.one * 50);

        //LoadData 數據的儲存，位置在Assets/StreamingAssets/MeshSave.json
        //why: 原本以為運算50萬個triangle就這麼滿了(10秒以上)，所以靜態碰撞一定要先運算好，
        //結果原來是因為我條用太多的gameObject取值了，弄好之後，延遲只剩不到10ms
        string path = Application.streamingAssetsPath + "/MeshSave.json";
        string jsonString = File.ReadAllText(path);
        MeshData meshData;
        meshData = JsonUtility.FromJson<MeshData>(jsonString);

        //設置grid來減少偵測的範圍
        Grid();

        //讀取網格及運算
        Load(meshData);

        //Save
        MeshData save = new MeshData(this);
        string jsonInfo = JsonUtility.ToJson(save);
        File.WriteAllText(path,jsonInfo);

        //接合particalObj重複的點
        //why: 因為一個三角形的組合為三個點三個normal，所以一定會有在相同位置的點
        MyLibrary.CombineRepeatPointOnMesh(ref particalVerticesO);

        //設置buffer
        MyLibrary.ComputeShaderSetMeshBuffer(ref meshArgsBuffer, particalMesh, population);
        SetNumInCompute();
        
        //存入數據
        sw = new System.Diagnostics.Stopwatch();
        SetColliderBuffer();
        material.enableInstancing = true;
        materialPropertyBlock = new MaterialPropertyBlock();
    }

    private void SetColliderBuffer()
    {
        //魚肉混雜，不須理解
        GridData[] GD = new GridData[collisionGrid.Length+1];
        int n = particalVerticesO.Length;
        if(n < collisionObj.Length+1)
        {
            n = collisionObj.Length+1;
        }
        PointData[] PD = new PointData[n];

        //---------------------------------------------------------------------------------------------PointGridData
        CellData[] CD = new CellData[population];
        ParticalGridData[] PGD = new ParticalGridData[population];
        PGDu = new ParticalGridData[population];

        int[][] objL = new int[boundTot.x][];
        int[][] SAVEobjL = new int[boundTot.x][];

        sw.Start();
        int boundSizeMin = boundTot.y*(int)(SpawnRangeMin.y+SpawnPos.y+halfBound.y-1);
        int boundSizeMax = boundTot.y*(int)(SpawnRangeMix.y+SpawnPos.y+halfBound.y+1);

        int pCon = 1+(int)(population/
                       ((SpawnRangeMix.x-SpawnRangeMin.x)*
                        (SpawnRangeMix.y-SpawnRangeMin.y)*
                        (SpawnRangeMix.z-SpawnRangeMin.z)) * boundTot.x/(boundSizeMax-boundSizeMin));  
        for (int i = boundSizeMin; i < boundSizeMax; i++)
        {
            objL[i] = new int[pCon];
        }

        for (int i = 0; i < population; i++)
        {
            float ran = (UnityEngine.Random.Range(0f,1f)+1.5f)/10;
            Vector3 sca = new Vector3(ran,ran,ran);
            quaternion rot = quaternion.Euler(new float3(UnityEngine.Random.Range(-1,1),
                                                         UnityEngine.Random.Range(-1,1),
                                                         UnityEngine.Random.Range(-1,1)));
            Vector3 pos = new Vector3(UnityEngine.Random.Range(SpawnRangeMin.x,SpawnRangeMix.x),
                                      UnityEngine.Random.Range(SpawnRangeMin.y,SpawnRangeMix.y),
                                      UnityEngine.Random.Range(SpawnRangeMin.z,SpawnRangeMix.z)) + SpawnPos;
            //Vector3 pos = new Vector3(0,2,0);
            Matrix4x4 mat = Matrix4x4.TRS(pos,rot,sca);
            CD[i].mat = mat;

            CD[i].speed = Vector3.zero;
            CD[i].color = new float4(UnityEngine.Random.Range(0f,1f),
                                     UnityEngine.Random.Range(0f,1f),
                                     UnityEngine.Random.Range(0f,1f),
                                     1f);
            //計算最遠的點
            MyLibrary.CalculationPointMixDistance(ref CD[i].mixDis,particalVerticesO, mat);

            int gridPos = (int)(pos.y + halfBound.y) * boundTot.y + 
                            (int)(pos.x + halfBound.x) * boundTot.z + 
                            (int)(pos.z + halfBound.z);

            if (pos.x < halfBound.x+centerBound.x && pos.x > -halfBound.x+centerBound.x &&
                pos.y < halfBound.y+centerBound.y && pos.y > -halfBound.y+centerBound.y &&
                pos.z < halfBound.z+centerBound.z && pos.z > -halfBound.z+centerBound.z)
            {
                CD[i].grid = gridPos; 
                PGD[i].inGrid = gridPos;
                GD[gridPos].toParticalGridData += 1;
                PGD[i].gs = GD[gridPos].toParticalGridData;
                int nowHave = GD[gridPos].toParticalGridData-1;
                if (GD[gridPos].toParticalGridData < pCon)
                {
                    objL[gridPos][nowHave] = i;
                }else
                {
                    SAVEobjL[gridPos] = objL[gridPos];

                    objL[gridPos] = new int[nowHave+1];
                    SAVEobjL[gridPos].CopyTo(objL[gridPos],0);

                    objL[gridPos][nowHave] = i; 
                } 
            }
        }
        Debug.Log(sw.ElapsedMilliseconds);

        int ss = 0;
        for (int i = 0; i < collisionGrid.Length; i++)
        {
            GD[i+1].ParticalGridDataNOW = 0;//-----

            for (int j = 0; j < GD[i].toParticalGridData; j++)
            {
                PGD[ss].ParticalGrid = objL[i][j];
                ss+=1;
            }
        }

        physicalComputeIn = new ComputeBuffer(population,CellData.size());
        physicalComputeIn.SetData(CD);
        physicalCompute.SetBuffer(to_core1,"_Properties",physicalComputeIn);
        material.SetBuffer("_Properties",physicalComputeIn);

        PointGridDataBuffer = new ComputeBuffer(population*particalVerticesO.Length,ParticalGridData.size());
        PointGridDataBuffer.SetData(PGD);
        physicalCompute.SetBuffer(to_core1,"_objGrid",PointGridDataBuffer);

        //---------------------------------------------------------------------------------------------PointNormalMixData
        int[] triangles = particalMesh.triangles;
        Vector3[] points = particalMesh.vertices;
        Vector3[] normals = particalMesh.normals;
        PointNormalMixData[] PNMD = new PointNormalMixData[triangles.Length/3];
        for (int i = 0; i < triangles.Length/3; i++)
        {
            Vector3 nor = normals[triangles[3*i]];
            Vector3 pos = (points[triangles[3*i]] + points[triangles[3*i+1]] + points[triangles[3*i+2]])/3;
            PNMD[i].normal = new Vector4(nor.x,nor.y,nor.z,0f);
            PNMD[i].point = new Vector4(pos.x,pos.y,pos.z,1f);
        }
        PointNormalMixDataBuffer = new ComputeBuffer(triangles.Length/3,PointNormalMixData.size());
        PointNormalMixDataBuffer.SetData(PNMD);
        physicalCompute.SetBuffer(to_core1,"_pointNormalMix",PointNormalMixDataBuffer);


        //---------------------------------------------------------------------------------------------CollisionGridData
        int gridTot = 0;
        for (int i = 0; i < collisionGrid.Length; i++)
        {
            for (int j = 0; j < collisionGrid[i].Length; j++)
            {
                gridTot += collisionGrid[i][j].Length;
            }
        }
        int C = 0;
        CollisionGridData[] CGD = new CollisionGridData[gridTot];
        for (int i = 0; i < collisionGrid.Length; i++)
        {
            GD[i].CollisionGridDataNOW = C;//-----
            for (int j = 0; j < collisionGrid[i].Length; j++)
            {
                for (int k = 0; k < collisionGrid[i][j].Length; k++)
                {
                    CGD[C].CollisionGrid = collisionGrid[i][j][k];
                    CGD[C].js = j; 
                    C++;
                }
            }
        }
        GD[collisionGrid.Length].CollisionGridDataNOW = C;//-----

        CollisionGridDataBuffer = new ComputeBuffer(C,CollisionGridData.size());
        CollisionGridDataBuffer.SetData(CGD);
        physicalCompute.SetBuffer(to_core1,"_CollisionGrid",CollisionGridDataBuffer);

        //---------------------------------------------------------------------------------------------NormalMixData
        int qq = 0;
        for(int j=0;j<collisionObj.Length;j++)
        {
            qq += normalFunctionValue[j].Length;
        }
        NormalMixData[] NMD = new NormalMixData[qq];
        int q = 0;
        for (int j = 0; j < collisionObj.Length; j++)
        {
            PD[j].jsLength = q;//-----
            for (int i = 0; i < normalFunctionValue[j].Length; i++)
            {
                NMD[q].normal = normalVector[j][i];
                NMD[q].value = normalFunctionValue[j][i];
                q++;
            }
        }
        PD[collisionObj.Length].jsLength = q;//-----
        NormalMixDataBuffer = new ComputeBuffer(qq,NormalMixData.size());
        NormalMixDataBuffer.SetData(NMD);
        physicalCompute.SetBuffer(to_core1,"_NormalMix",NormalMixDataBuffer);

        //---------------------------------------------------------------------------------------------PointData
        for (int i = 0; i < particalVerticesO.Length; i++)
        {
            PD[i].points = particalVerticesO[i];
            PD[i].pointLength = particalVerticesO.Length;
        }
        PointDataBuffer = new ComputeBuffer(particalVerticesO.Length,PointData.size());
        PointDataBuffer.SetData(PD);
        physicalCompute.SetBuffer(to_core1,"_point",PointDataBuffer);

        //---------------------------------------------------------------------------------------------GridData
        GridDataBuffer = new ComputeBuffer(boundTot.x+1,GridData.size());
        GridDataBuffer.SetData(GD);
        physicalCompute.SetBuffer(to_core1,"_Grid",GridDataBuffer);
    }
    
    private void SetNumInCompute()
    {
        //what: Shader.PropertyToID得到的值是可以直接套用於...
        to_core1 = physicalCompute.FindKernel("CSMain");
        to_updataTime = Shader.PropertyToID("updataTime");
        to_halfBound = Shader.PropertyToID("halfBound");
        to_boundTot = Shader.PropertyToID("boundTot");
        to_gravity = Shader.PropertyToID("gravity");
        to_CH = Shader.PropertyToID("CH");
        to_centerBound = Shader.PropertyToID("centerBound");
        to_delateCount = Shader.PropertyToID("delateCount");
        to_population = Shader.PropertyToID("population");

        physicalCompute.SetVector(to_halfBound,new Vector4(halfBound.x,halfBound.y,halfBound.z,0f));
        physicalCompute.SetVector(to_boundTot,new Vector4(boundTot.x,boundTot.y,boundTot.z,0f));
        physicalCompute.SetFloat(to_gravity,Physics.gravity.y/2);
        physicalCompute.SetVector(to_centerBound,Vector4.zero);
        physicalCompute.SetInt(to_population,population);
    }



    private void Load(MeshData meshData)
    {
        string[] collisionObjName = null;
        if (meshData.colliderName != null)
        {
            collisionObjName = meshData.colliderName;
        }

        for(int j=0;j<collisionObj.Length;j++)
        {
            if (collisionObjName !=null && collisionObjName.Length > j &&
                meshData.colliderName[j] == collisionObj[j].name && 
                meshData.bound[0] == bound[0] && meshData.bound[1] == bound[1] && meshData.bound[2] == bound[2])
            {
                Matrix4x4 mat = collisionObj[j].transform.localToWorldMatrix;
                Matrix4x4 matt = new Matrix4x4();
                for(int i=0;i<16;i++)
                {
                    matt[i] = meshData.mat[i+j*16];
                }
                if (mat == matt && keepLoadMesh == false)
                {
                    LoadMeshS(j,meshData);
                }else
                {
                    LoadMeshR(j);
                }
            }else
            {
                LoadMeshR(j);
            }
        }
    }

    private void Grid()
    {
        boundTot = new int3(bound.x*bound.y*bound.z,bound.x*bound.z,bound.z);
        collisionGrid = new int[boundTot.x][][];
        SAVEcollisionGrid = new int[boundTot.x][][];
        for (int i = 0; i < boundTot.x; i++)
        {
            collisionGrid[i] = new int[collisionObj.Length][];
            SAVEcollisionGrid[i] = new int[collisionObj.Length][];
        }
        for (int j = 0; j < collisionObj.Length; j++)
        {
            for (int i = 0; i < boundTot.x; i++)
            {
                collisionGrid[i][j] = new int[0];
            }
        }
        halfBound = new Vector3(bound.x/2,bound.y/2,bound.z/2);
    }

    private void LoadMeshR(int j)
    {
        collisionMesh[j] = collisionObj[j].GetComponent<MeshFilter>().mesh;
        int t = collisionMesh[j].triangles.Length/3;
        normalFunctionValue[j] = new float[t];

        normalVector[j] = new Vector3[t];
        Matrix4x4 mat = collisionObj[j].transform.localToWorldMatrix;

        int[] triC = collisionMesh[j].triangles;
        Vector3[] norC = collisionMesh[j].normals;
        Vector3[] poiC = collisionMesh[j].vertices;

        for (int i=0; i<t;i++)
        {
            Vector3 nor = norC[triC[i*3]];
            Vector3 poi = poiC[triC[i*3]];
            
            normalVector[j][i] = mat.MultiplyVector(nor).normalized;
            Vector3 poiN = mat.MultiplyPoint(poi);

            normalFunctionValue[j][i] = Vector3.Dot(poiN , normalVector[j][i]);

            Vector3[] pois = new Vector3[3]{poiN,mat.MultiplyPoint(poiC[triC[i*3+1]]),mat.MultiplyPoint(poiC[triC[i*3+2]])};
            Vector3[] r = new Vector3[3]{Vector3.zero,pois[1]-pois[0],pois[2]-pois[0]};
            Vector3 maxpoi = Vector3.zero;
            Vector3 minpoi = Vector3.zero;
            for(int k=0;k<3;k++)
            {
                if (pois[0][k]> pois[1][k] && pois[0][k] > pois[2][k])
                {
                    maxpoi[k] = poiN[k] + r[0][k];
                    minpoi[k] = poiN[k] + ((pois[1][k] < pois[2][k]) ? r[1][k]:r[2][k]);
                }
                else if ((pois[0][k] <= pois[1][k] && pois[0][k] >= pois[2][k]) ||
                         (pois[0][k] >= pois[1][k] && pois[0][k] <= pois[2][k]))
                {
                    maxpoi[k] = poiN[k] + ((pois[1][k] < pois[2][k]) ? r[2][k]:r[1][k]); 
                    minpoi[k] = poiN[k] + ((pois[1][k] < pois[2][k]) ? r[1][k]:r[2][k]);  
                }
                else
                {
                    maxpoi[k] = poiN[k] + ((pois[1][k] < pois[2][k]) ? r[2][k]:r[1][k]);
                    minpoi[k] = poiN[k] + r[0][k];
                }
            }

            maxpoi += halfBound;
            minpoi += halfBound;
            float3 Op = new float3(minpoi.x - (int)minpoi.x,minpoi.y - (int)minpoi.y,minpoi.z - (int)minpoi.z);
            for (int k = 0; k < 3; k++)
                if (Op[k] < 0.25f || Op[k] > 0.75f)
                    minpoi[k] -= 0.5f;

            Op = new float3(maxpoi.x - (int)maxpoi.x,maxpoi.y - (int)maxpoi.y,maxpoi.z - (int)maxpoi.z);
            for (int k = 0; k < 3; k++)
                if (Op[k] > 0.75f || Op[k] < 0.25f)
                    maxpoi[k] += 0.5f;

            Vector3 div = maxpoi - minpoi +Vector3.one/100;
            
            for (int y = 0; y < div.y; y++)
            {
                for (int x = 0; x < div.x; x++)
                {
                    for (int z = 0; z < div.z; z++)
                    {
                        int gridPos = ((int)minpoi.y + y) * boundTot.y + 
                                      ((int)minpoi.x + x) * boundTot.z + 
                                      ((int)minpoi.z + z);
                        if (gridPos<0 || gridPos>boundTot.x)
                        {
                            return;
                        }
                        MyLibrary.ArrayAddL(ref collisionGrid[gridPos][j],i);
                    }
                }
            }
        }
    }

    private void LoadMeshS(int j,MeshData data)
    {
        int qq = data.gs[j];
        int q = data.gs[j+1] - data.gs[j];

        normalFunctionValue[j] = new float[q];
        normalVector[j] = new Vector3[q];

        for(int i=0;i<q;i++)
        {
            int qqq = i*3+qq*3;
            normalFunctionValue[j][i] = data.normalFunctionValue[i+qq];
            normalVector[j][i] = new Vector3(data.normalVector[qqq],data.normalVector[qqq+1],data.normalVector[qqq+2]);
        }

        int p = data.collisionGrid.Length;
        for (int i = 0; i < p; i++)
        {    
            if (data.js[i] == j)             
            {
                int pp = data.collisionGrid[i];
                int nowHave = collisionGrid[data.gTo[i]][j].Length;
                MyLibrary.ArrayAddL(ref collisionGrid[data.gTo[i]][data.js[i]],pp);
            }
        }
    }

    float TimeD;
    int delateCount;
    void Start() 
    {
        InitialSet();
        delateCount = 0;
        //TooManyThingInThere[] too = new TooManyThingInThere[(int)(population/64)+1]; 
        //TooManyThingInThereBuffer = new ComputeBuffer((int)(population/64)+1,TooManyThingInThere.size());
        //TooManyThingInThereBuffer.SetData(too);
        //physicalCompute.SetBuffer(to_core1,"_too",TooManyThingInThereBuffer);
    }

    void Update()
    {
        if (Time.deltaTime>0.0166f)
        {
            TimeD = 0.0166f;
        }else
        {
            TimeD = Time.deltaTime;
        }

        if (delateCount == 2)
        {
            physicalCompute.SetFloat(to_updataTime,TimeD);
            physicalCompute.SetFloat(to_delateCount,1);

            physicalCompute.SetFloat(to_CH,0);
            physicalCompute.Dispatch(to_core1, Mathf.CeilToInt(population / 64f), 1, 1);
/*
            physicalCompute.SetFloat(to_CH,1);
            physicalCompute.Dispatch(to_core1, 1, 1, 1);
            physicalCompute.SetFloat(to_CH,2);
            physicalCompute.Dispatch(to_core1, Mathf.CeilToInt(population / 64f), 1, 1);
            delateCount = 0;

            PointGridDataBuffer.GetData(PGDu);
            int[] SAVEGDu = new int[population];
            for (int i = 0; i < PGDu.Length; i++)
            {
                GDu[PGDu[i].inGrid].ParticalGridDataNOW = i;
                SAVEGDu[PGDu[i].inGrid] += 1;
            }
            PointGridDataBuffer.SetData(PGDu);
*/
        }else
        {
            physicalCompute.SetFloat(to_updataTime,TimeD);
            physicalCompute.SetFloat(to_delateCount,0);

            physicalCompute.SetFloat(to_CH,0);
            physicalCompute.Dispatch(to_core1, Mathf.CeilToInt(population / 64f), 1, 1);
            delateCount++;
        }

        Graphics.DrawMeshInstancedIndirect(particalMesh, 0,material,viewBounds,meshArgsBuffer, 0, materialPropertyBlock);
        //TestCollider();
    }

/*  
    //減少內存回收
    Vector3 pos;
    Matrix4x4 mat;
    Vector3[] poik;
    bool iff;
    Vector3 colPos;
  
    bool isCollision = false;

    private void TestCollider()
    {
        
        pos = particalObj.transform.position;
        mat = particalObj.transform.localToWorldMatrix;
        poik = new Vector3[particalVerticesO.Length];
        iff = false;
        float a;

        for (int k=0;k<poik.Length;k++)
        {
            poik[k] = particalVerticesO[k];
            if (iff == false && isCollision == false)
            {   
                poik[k] = mat.MultiplyPoint(new Vector4(poik[k].x,poik[k].y,poik[k].z,0f));

                if (poik[k] == particalVerticesPos[k])
                {
                    isCollision = false;
                    iff = true;
                    break;
                }

                int gridPos = (int)(poik[k].y + halfBound.y) * boundTot.y + 
                              (int)(poik[k].x + halfBound.x) * boundTot.z + 
                              (int)(poik[k].z + halfBound.z);

                if (gridPos < 0 || gridPos > boundTot.x)
                    break;

                for (int j=0;j<collisionObj.Length;j++)
                {
                    bool isC = true;
                    int[] grid = collisionGrid[gridPos][j];

                    //每三個點為一個區間
                    for(int i=0;i < grid.Length ;i++)
                    {
                        Debug.Log("oooo");
                        a = Vector3.Dot(poik[k] , normalVector[j][grid[i]]);
                        if (a - normalFunctionValue[j][grid[i]] > 0)
                        {
                            isC = false;
                        }
                    }
                    if (isC == true && grid.Length != 0)
                    {
                        isCollision = true;
                        break;
                    }
                    
                }
            }
        }
        if (isCollision == true)
        {
            Debug.Log("Hii");
            isCollision = false;
        }
        particalVerticesPos = poik;
        
    }
    
    */
    private void OnDisable()
    {
        // Release gracefully.
        if (meshArgsBuffer != null)
        {
             meshArgsBuffer.Release();
        }
        meshArgsBuffer = null;

        if (physicalComputeIn != null)
        {
            physicalComputeIn.Release();
        }
        physicalComputeIn = null;

        if (CollisionGridDataBuffer != null)
        {
            CollisionGridDataBuffer.Release();
        }
        CollisionGridDataBuffer = null;

        if (NormalMixDataBuffer != null)
        {
            NormalMixDataBuffer.Release();
        }
        NormalMixDataBuffer = null;

        if (PointDataBuffer != null)
        {
            PointDataBuffer.Release();
        }
        PointDataBuffer = null;

        if (GridDataBuffer != null)
        {
            GridDataBuffer.Release();
        }
        GridDataBuffer = null;

        if (PointGridDataBuffer != null)
        {
            PointGridDataBuffer.Release();
        }
        PointGridDataBuffer = null;

        if (PointNormalMixDataBuffer != null)
        {
            PointNormalMixDataBuffer.Release();
        }
        PointNormalMixDataBuffer = null;

        if (TooManyThingInThereBuffer != null)
        {
            TooManyThingInThereBuffer.Release();
        }
        TooManyThingInThereBuffer = null;
    }
}
