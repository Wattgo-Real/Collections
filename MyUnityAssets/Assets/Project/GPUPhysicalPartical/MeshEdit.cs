using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MeshEdit
{
    public static Mesh CubeMesh()
    {
        Mesh m = new Mesh();

        Vector3[] point =
        {
          new Vector3(-0.5f, -0.5f, -0.5f), 
          new Vector3(-0.5f, -0.5f,  0.5f), 
          new Vector3( 0.5f, -0.5f, -0.5f), 
          new Vector3( 0.5f, -0.5f,  0.5f), 
          new Vector3(-0.5f,  0.5f, -0.5f), 
          new Vector3(-0.5f,  0.5f,  0.5f), 
          new Vector3( 0.5f,  0.5f, -0.5f), 
          new Vector3( 0.5f,  0.5f,  0.5f), 
        };

        Vector3[] vertices =
        {
            point[2],point[1],point[0],point[2],point[3],point[1],
            point[1],point[4],point[0],point[5],point[4],point[1],
            point[3],point[5],point[1],point[7],point[5],point[3],
            point[6],point[7],point[3],point[2],point[6],point[3],
            point[0],point[4],point[2],point[4],point[6],point[2],
            point[7],point[6],point[4],point[7],point[4],point[5]
        };

        Vector3[] AllND =
        {
            new Vector3(0f,-1f,0f),
            new Vector3(-1f,0f,0f),
            new Vector3(0f,0f,1f),
            new Vector3(1f,0f,0f),
            new Vector3(0f,0f,-1f),
            new Vector3(0f,1f,0f)
        };

        Vector3[] normals =
        {
            AllND[0],AllND[0],AllND[0],AllND[0],AllND[0],AllND[0],
            AllND[1],AllND[1],AllND[1],AllND[1],AllND[1],AllND[1],
            AllND[2],AllND[2],AllND[2],AllND[2],AllND[2],AllND[2],
            AllND[3],AllND[3],AllND[3],AllND[3],AllND[3],AllND[3],
            AllND[4],AllND[4],AllND[4],AllND[4],AllND[4],AllND[4],
            AllND[5],AllND[5],AllND[5],AllND[5],AllND[5],AllND[5]
        };

        int[] triangles = new int[vertices.Length];
        for (int i = 0; i < vertices.Length; ++i)
        {
          triangles[i] = i;
        }
  
        m.vertices = vertices;
        m.SetIndices(triangles, MeshTopology.Triangles, 0);
        m.normals = normals;
        return m;
    }

}
