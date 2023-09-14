using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MeshData
{
    public string[] colliderName;
    public float[] normalFunctionValue; 
    public float[] normalVector;
    public int[] gs;
    public float[] mat;

    public int[] bound;
    public int[] collisionGrid;
    public int[] gTo;
    public int[] js;
    public MeshData(GPUPhysical gPUPhysical)
    {
        int objectLenght = gPUPhysical.collisionObj.Length;
        int qq = 0;
        for(int j=0;j<objectLenght;j++)
        {
            qq += gPUPhysical.normalFunctionValue[j].Length;
        }

        int collisionGridTot = gPUPhysical.bound.x * gPUPhysical.bound.y * gPUPhysical.bound.z;
        int gridTot = 0;
        for (int i = 0; i < collisionGridTot; i++)
        {
            for (int j = 0; j < objectLenght; j++)
            {
                gridTot += gPUPhysical.collisionGrid[i][j].Length;
            }
        }

        colliderName = new string[objectLenght];
        normalFunctionValue = new float[qq];
        normalVector = new float[qq*3];
        gs = new int[objectLenght+1];
        mat = new float[objectLenght*16];

        bound = new int[3];
        collisionGrid = new int[gridTot];
        gTo = new int[gridTot];
        js = new int[gridTot];

        int q = 0;
        for(int j=0;j<objectLenght;j++)
        {
            colliderName[j] = gPUPhysical.collisionObj[j].name;

            gs[j] = q;

            for(int i=0;i<gPUPhysical.normalFunctionValue[j].Length;i++)
            {
                normalFunctionValue[q] = gPUPhysical.normalFunctionValue[j][i];
                normalVector[q*3] = gPUPhysical.normalVector[j][i].x;
                normalVector[q*3+1] = gPUPhysical.normalVector[j][i].y;
                normalVector[q*3+2] = gPUPhysical.normalVector[j][i].z;
                q++;
            }

            Matrix4x4 matt = gPUPhysical.collisionObj[j].transform.localToWorldMatrix;
            for(int i=0;i<16;i++)
            {
                mat[i+j*16] = matt[i];
            }
        }
        gs[objectLenght] = q;

        bound[0] = gPUPhysical.bound.x;
        bound[1] = gPUPhysical.bound.y;
        bound[2] = gPUPhysical.bound.z;

        int C = 0;
        for (int i = 0; i < collisionGridTot; i++)
        {
            for (int j = 0; j < objectLenght; j++)
            {
                int n = gPUPhysical.collisionGrid[i][j].Length;
                for (int k = 0; k < n; k++)
                {
                    collisionGrid[C] = gPUPhysical.collisionGrid[i][j][k];
                    gTo[C] = i;
                    js[C] = j;
                    C++;
                }
            }
        }

    }
}
