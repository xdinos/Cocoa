﻿using JadeScriptRuntime;

public class RandomComponent : ScriptableComponent
{
	public override void Start()
	{
		Debug.LogInfo("This is the start method. Whoops, typo!");
	}

	override public void Update(float dt)
	{
		Debug.LogInfo($"Updating Random Component, dt: {dt}");
	}
}