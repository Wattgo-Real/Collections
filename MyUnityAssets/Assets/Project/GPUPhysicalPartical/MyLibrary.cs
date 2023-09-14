using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Unity.Mathematics;

public static class MyLibrary
{
    //用於增加矩陣的個數。
    public static void ArrayAddL(ref int[] obj,int Num)
    {
        int nowHave = obj.Length;
        int[] SAVE = new int[nowHave];
        SAVE = obj;
        obj = new int[nowHave+1];
        SAVE.CopyTo(obj,0);
        obj[nowHave] = Num;
    }

    public static void ArrayAddL(ref float[] obj,float Num)
    {
        int nowHave = obj.Length;
        float[] SAVE = new float[nowHave];
        SAVE = obj;
        obj = new float[nowHave+1];
        SAVE.CopyTo(obj,0);
        obj[nowHave] = Num;
    }
    public static void ArrayAddL(ref float[][] obj)
    {
        int nowHave = obj.Length;
        float[][] SAVE = new float[nowHave][];
        SAVE = obj;
        obj = new float[nowHave+1][];
        SAVE.CopyTo(obj,0);
    }

    public static void ArrayAddL(ref Vector3[] obj,Vector3 Num)
    {
        int nowHave = obj.Length;
        Vector3[] SAVE = new Vector3[nowHave];
        SAVE = obj;
        obj = new Vector3[nowHave+1];
        SAVE.CopyTo(obj,0);
        obj[nowHave] = Num;
    }
    public static void ArrayAddL(ref Vector3[][] obj)
    {
        int nowHave = obj.Length;
        Vector3[][] SAVE = new Vector3[nowHave][];
        SAVE = obj;
        obj = new Vector3[nowHave+1][];
        SAVE.CopyTo(obj,0);
    }

    public static void ArrayAddL(ref int3[] obj,int3 Num)
    {
        int nowHave = obj.Length;
        int3[] SAVE = new int3[nowHave];
        SAVE = obj;
        obj = new int3[nowHave+1];
        SAVE.CopyTo(obj,0);
        obj[nowHave] = Num;
    }

    //用於找出一個物體與他距離最遠的點
    public static void CalculationPointMixDistance(ref float dis,Vector4[] vectors,Matrix4x4 mat)
    {
        for (int i = 0; i < vectors.Length; i++)
        {
            Vector3 pos = mat.MultiplyVector(vectors[i]);
            dis = (dis > pos.magnitude) ? dis : pos.magnitude;
        }
    }

    public static void CalculationPointMixDistance(ref float dis,Vector3[] vectors)
    {
        for (int i = 0; i < vectors.Length; i++)
        {
            Vector3 pos = vectors[i];
            dis = (dis > pos.magnitude) ? dis : pos.magnitude;
        }
    }

    public static void CalculationPointMixDistanceFromCenter(ref float dis,Vector3[] vectors,Vector3 center)
    {
        for (int i = 0; i < vectors.Length; i++)
        {
            float pos = Vector3.Distance(vectors[i],center) ;
            dis = (dis > pos) ? dis : pos;
        }
    }
    public static void FindCenter(out Vector3 center,Vector3[] vectors)
    {
        center = Vector3.zero;
        for (int i = 0; i < vectors.Length; i++)
        {
            center += vectors[i];
        }
        center /= vectors.Length;
    }

    //常用於偵測網格。
    public static void MeshTriangleSplicToNormalAndValue(out Vector4[] normal,out float[] value, Mesh mesh,Matrix4x4 mat)
    {
        int t = mesh.triangles.Length/3;

        value = new float[t];
        normal = new Vector4[t];

        int[] triC = mesh.triangles;
        Vector3[] norC = mesh.normals;
        Vector3[] poiC = mesh.vertices;

        for (int i=0; i<t;i++)
        {
            Vector3 nor = norC[triC[i*3]];
            Vector3 poi = poiC[triC[i*3]];
            
            normal[i] = mat.MultiplyVector(nor).normalized;
            Vector3 poiN = mat.MultiplyPoint(poi);

            value[i] = Vector3.Dot(poiN , normal[i]);
        }
    }

    //合併相同的頂點
    public static void CombineRepeatPointOnMesh(ref Vector4[] point)
    {
        Vector4[] point2 = point;
        int[] thf = new int[point.Length];
        int t = 0;
        for(int j=0;j<point.Length;j++)
        {
            for(int i=0;i<point.Length;i++)
            {
                if (point[j] == point[i] && j != i)
                    point[i] = Vector3.zero;
            }
            if (point[j] != Vector4.zero)
            {
                thf[t] = j;
                t++;
            }
        }
        Vector4[] nPoint = new Vector4[t];
        for (int i = 0; i < t; i++)
        {
            nPoint[i] = point[thf[i]];
        }
        point = new Vector4[t];
        point = nPoint;
    }

    //DrawMeshInstancedIndirect 固定式寫法
    public static void ComputeShaderSetMeshBuffer(ref ComputeBuffer buffer,Mesh mesh,int population)
    {
        uint[] args = new uint[5] { 0, 0, 0, 0, 0 };
        args[0] = (uint)mesh.GetIndexCount(0);
        args[1] = (uint)population;
        args[2] = (uint)mesh.GetIndexStart(0);
        args[3] = (uint)mesh.GetBaseVertex(0);
        buffer = new ComputeBuffer(1, args.Length * sizeof(uint), ComputeBufferType.IndirectArguments);
        buffer.SetData(args);
    }
}
