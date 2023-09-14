using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class Computer : MonoBehaviour
{
    public ComputeShader computeShader;
    public RenderTexture _rt;
    private int _kernelIndex;
    Image _image;

    // Start is called before the first frame update
    void Start()
    {
        InitShader();
    }

    private void InitShader()
    {
        _image = GetComponent<Image>();
        _kernelIndex = computeShader.FindKernel("CSMain");
        int width = 1024, height = 1024;
        _rt = new RenderTexture(width, height, 0) { enableRandomWrite = true };
        _rt.Create();
        Debug.Log(_kernelIndex);
        _image.material.SetTexture("_MainTex", _rt);
        computeShader.SetTexture(_kernelIndex, "Result", _rt);
        computeShader.Dispatch(_kernelIndex, width / 8, height / 8, 1);
    }
}
