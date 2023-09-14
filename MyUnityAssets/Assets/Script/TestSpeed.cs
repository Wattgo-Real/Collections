using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Unity.Mathematics;


struct PD
{
    public float4x4 mat;
    public static int size()
    {
        return sizeof(float) * 16;
    }
};


public class TestSpeed : MonoBehaviour
{
    int to_b;
    // Start is called before the first frame update
    System.Diagnostics.Stopwatch sw;
    ComputeBuffer buffer;
    public ComputeShader shader;
    int population = 100000;
    int to_core1;
    PD[] PDs;
    void Start()
    {

        to_core1 = shader.FindKernel("CSMain");

        sw = new System.Diagnostics.Stopwatch();


        PD[] e = new PD[population];
        PDs = new PD[population];

        sw.Start();
        List<int> pe = new List<int>(); 
        for (int i = 0; i < 100000; i++)
        {
            pe.Add(i);
        }
        Debug.Log(sw.ElapsedMilliseconds);
        

        buffer = new ComputeBuffer(population,PD.size());
        buffer.SetData(e);

        shader.SetBuffer(to_core1,"_Properties",buffer);
        to_b = Shader.PropertyToID("b");
    }
    //int i =0 ;

    void Update() 
    {
        //shader.SetBool(to_b,true);

        //shader.Dispatch(to_core1, 1, 1, 1);

        //shader.SetBool(to_b,false);

        //shader.Dispatch(to_core1, Mathf.CeilToInt(population/64) , 1, 1);

    }

    private void OnDisable()
    {
        // Release gracefully.
        if (buffer != null)
        {
             buffer.Release();
        }
        buffer = null;
    }
}

