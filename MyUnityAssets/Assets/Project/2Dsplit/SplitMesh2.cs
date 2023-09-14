using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Unity.Mathematics;

public class SplitMesh2 : MonoBehaviour
{
    public GameObject[] obj;
    public GameObject plane;
    Mesh[] objectMesh;
    Mesh planeMesh;
    SplitMeshData[] SMD;

    float planeValue;
    Vector3 planePoints;
    Vector3 planeNormal;
    Matrix4x4 planeMat;
    Matrix4x4 SAVEplaneMat;
    Matrix4x4[] ObjMat;
    Matrix4x4[] SAVEObjMat;

    System.Diagnostics.Stopwatch sw;

    
    // Start is called before the first frame update
    void Start()
    {
        planeMesh = plane.GetComponent<MeshFilter>().mesh;
        planePoints = planeMesh.vertices[0];

        objectMesh = new Mesh[obj.Length];
        SMD = new SplitMeshData[obj.Length];

        ObjMat = new Matrix4x4[obj.Length];
        SAVEObjMat = new Matrix4x4[obj.Length];
        for (int i = 0; i < obj.Length; i++)
        {
            objectMesh[i] = obj[i].GetComponent<MeshFilter>().mesh;
            SMD[i] = new SplitMeshData();
            SMD[i].InputMesh(objectMesh[i].vertices, objectMesh[i].normals, objectMesh[i].triangles, obj[i].transform.localToWorldMatrix); 
            ObjMat[i] = obj[i].transform.localToWorldMatrix;
            SAVEObjMat[i] = obj[i].transform.localToWorldMatrix;
        }    
    }

    void Update()
    {
        planeMat = plane.transform.localToWorldMatrix;
        if (SAVEplaneMat != planeMat)
        {
            planeNormal = planeMat.MultiplyVector(planeMesh.normals[0]).normalized;
            planeValue = Vector3.Dot(planeMat.MultiplyPoint(planePoints), planeNormal);

            for (int i = 0; i < obj.Length; i++)
            {
                IsObjMove(i);
                SMD[i].CutByFace(planeNormal, planeValue);

                objectMesh[i].Clear();
                objectMesh[i].vertices = SMD[i].ToOPP;
                objectMesh[i].normals = SMD[i].ToN;
                objectMesh[i].triangles = SMD[i].ToT;
            }
        }else{ 
            for (int i = 0; i < obj.Length; i++)
            {
                bool move = IsObjMove(i);
                if (move){
                    SMD[i].CutByFace(planeNormal, planeValue);

                    objectMesh[i].Clear();
                    objectMesh[i].vertices = SMD[i].ToOPP;
                    objectMesh[i].normals = SMD[i].ToN;
                    objectMesh[i].triangles = SMD[i].ToT;
                }
            }
        }
        SAVEplaneMat = planeMat;
        sw = new System.Diagnostics.Stopwatch();
    }

    //如果物體動了 =.=
    bool IsObjMove(int i){
        ObjMat[i] = obj[i].transform.localToWorldMatrix;

        bool objMove = (SAVEObjMat[i] != ObjMat[i]);
        if (objMove){
            SMD[i].ToWorldPosition(ObjMat[i]);
        }

        SAVEObjMat[i] = ObjMat[i];
        return objMove;
    }
}
