using UnityEngine;
using UnityEngine.Rendering;

public class CameraRenderer
{
	const string bufferName = "Render Camera";

	CommandBuffer buffer = new CommandBuffer 
	{
		name = bufferName
	};

	CullingResults cullingResults;

	bool Cull()
	{
		if (camera.TryGetCullingParameters(out ScriptableCullingParameters p))
		{
			cullingResults = context.Cull(ref p);
			return true;
		}
		return false;
	}

	ScriptableRenderContext context;

	Camera camera;

	public void Render(ScriptableRenderContext context, Camera camera)
	{
		this.context = context;
		this.camera = camera;

		if (!Cull())
		{
			return;
		}

		Setup();
		DrawVisibleGeometry();
		Submit();
	}

	void Setup()
	{
		context.SetupCameraProperties(camera);
		CameraClearFlags clearFlags = camera.clearFlags;
		buffer.ClearRenderTarget(
			(clearFlags & CameraClearFlags.Depth) != 0,
			(clearFlags & CameraClearFlags.Color) != 0,
			camera.backgroundColor);
		buffer.BeginSample(bufferName);
		ExecuteBuffer();
	}

	void DrawVisibleGeometry()
	{
		context.DrawSkybox(camera);
	}

	void Submit()
	{
		buffer.EndSample(bufferName);
		ExecuteBuffer();
		context.Submit();
	}

	void ExecuteBuffer()
	{
		context.ExecuteCommandBuffer(buffer);
		buffer.Clear();
	}
}