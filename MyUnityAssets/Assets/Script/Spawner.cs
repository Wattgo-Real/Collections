using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Linq;
using System;

public class ObjData
{
    public int id;
    public Vector3 pos;
    public Vector3 scale;
    public Quaternion rot;

    public Matrix4x4 matrix
    {
        get
        {
            return Matrix4x4.TRS(pos, rot, scale);

        }
    }
    public ObjData(int id,Vector3 pos, Vector3 scale, Quaternion rot)
    {
        this.id = id;
        this.pos = pos;
        this.scale = scale;
        this.rot = rot;
    }
}
public class Spawner : MonoBehaviour
{
    public int instances;
    public Vector3 maxPos;
    public Mesh objMesh;
    public Material objMat;

    private Dictionary<int, ObjData> OutputObjData = new Dictionary<int, ObjData>();
    private Matrix4x4[][] mat;
    private Matrix4x4[] mat2;

    int perTimes = 1000;
    void Start()
    {
        int times = instances / perTimes;
        mat = new Matrix4x4[times][];
        mat2 = new Matrix4x4[perTimes];

        for (int i = 0; i < instances; i++)
        {
            AddObj(OutputObjData, i);
        }

        for (int i = 0; i < times; i++)
        {
            for (int j = 0; j < perTimes; j++)
            {
                mat2[j] = OutputObjData[i * perTimes + j].matrix;
            }

            for (int j=0 ; j < perTimes ; j++)
            {
                mat[i] = (Matrix4x4[])mat2.Clone();
            }

            Array.Clear(mat2, 0, mat2.Length);
        }

    }

    void Update()
    {
        RenderBatches();
    }

    private void AddObj(Dictionary<int, ObjData> OutputObjData, int i)
    {
        Vector3 position = new Vector3(UnityEngine.Random.Range(-maxPos.x, maxPos.x), UnityEngine.Random.Range(-maxPos.y, maxPos.y), UnityEngine.Random.Range(-maxPos.z, maxPos.z));

        OutputObjData.Add(i, new ObjData(i, position, new Vector3(2, 2, 2), Quaternion.identity));
    }

    private Dictionary<int, ObjData> BuildNewDictionary()
    {
        return new Dictionary<int, ObjData>();
    }

    private void RenderBatches()
    {
        for (int i = 0;i < instances / perTimes; i++)
        Graphics.DrawMeshInstanced(objMesh, 0, objMat, mat[i]);
    }
}