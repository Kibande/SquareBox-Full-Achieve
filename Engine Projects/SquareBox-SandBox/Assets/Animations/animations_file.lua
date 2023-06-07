animations={}

animations[0]={
	TargetClusterObject="targetCapsule",
	AnimationType=0,
	ApplyDamping=false,
	MaxLinearVelocityX=1.5,
	MaxLinearVelocityY=1.5,
	AnimationSpecifications=
	{
		{
					--Standing Animation
					AnimationForceX=0.0,
					AnimationForceY=0.0,
					AnimationSpeed=1,
					Duration=0,
					ApplyForceAccordingToMass=false,
					MotionState=0,
					IsAnimationSpeedLinkedToHorizontalVelocity=false,
					IsAnimationSpeedLinkedToVerticalVelocity=false,
					IsForwardMovementOrientied=false,
					IsGroundDependant=true,
					IsReversedMovementOrientied=false,
					KeyActivationSquence=1,
					KeyCode=32,
					NumTiles=1,
					StartTile=0
				},
				{
					--Running Forward Animation
					AnimationForceX=10.0,
					AnimationForceY=0.0,
					AnimationSpeed=0.8,
					Duration=0,
					ApplyForceAccordingToMass=false,
					MotionState=1,
					IsAnimationSpeedLinkedToHorizontalVelocity=true,
					IsAnimationSpeedLinkedToVerticalVelocity=false,
					IsForwardMovementOrientied=true,
					IsGroundDependant=true,
					IsReversedMovementOrientied=false,
					KeyActivationSquence=1,
					KeyCode=100,
					NumTiles=6,
					StartTile=10
				},
				{
					--Running Backward Animation
					AnimationForceX=-10.0,
					AnimationForceY=0.0,
					AnimationSpeed=0.8,
					Duration=0,
					ApplyForceAccordingToMass=false,
					MotionState=2,
					IsAnimationSpeedLinkedToHorizontalVelocity=true,
					IsAnimationSpeedLinkedToVerticalVelocity=false,
					IsForwardMovementOrientied=false,
					IsGroundDependant=true,
					IsReversedMovementOrientied=true,
					KeyActivationSquence=1,
					KeyCode=97,
					NumTiles=6,
					StartTile=10
		},
		{
					--jumping Animation
					AnimationForceX=0.0,
					AnimationForceY=250.0,
					AnimationSpeed=0.1,
					Duration=0,
					ApplyForceAccordingToMass=true,
					MotionState=3,
					IsAnimationSpeedLinkedToHorizontalVelocity=false,
					IsAnimationSpeedLinkedToVerticalVelocity=false,
					IsForwardMovementOrientied=false,
					IsGroundDependant=true,
					IsReversedMovementOrientied=false,
					KeyActivationSquence=1,
					KeyCode=119,
					NumTiles=1,
					StartTile=16
		}
	}
}

animations[1]={
	TargetClusterObject="targetTransition",
	AnimationType=1,
	ApplyDamping=false,
	MaxLinearVelocityX=0,
	MaxLinearVelocityY=0,
	AnimationSpecifications=
	{
		{
					AnimationForceX=0.0,
					AnimationForceY=0.0,
					AnimationSpeed=0.0,
					Duration=1,
					ApplyForceAccordingToMass=false,
					MotionState=0,
					IsAnimationSpeedLinkedToHorizontalVelocity=false,
					IsAnimationSpeedLinkedToVerticalVelocity=false,
					IsForwardMovementOrientied=false,
					IsGroundDependant=false,
					IsReversedMovementOrientied=false,
					KeyActivationSquence=0,
					KeyCode=0,
					NumTiles=8,
					StartTile=0
		}


	}

}

function GetAnimation( n )
	return animations[n]
end


function GetAnimationSpecification( n,m )
-- because we are dealing with sets we have to offest our index by 1, i think
	return animations[n].AnimationSpecifications[m+1]
end


function tcout(luatable)
	local c = 1

	for i,v in ipairs(luatable) do
	c=c+1
	end
	return c
end


function GetNumAnimations()
	return tcout(animations)
end


function GetNumAnimationSpecification( n )
-- because we are dealing with sets we have to offest our index by 1, i think
	return tcout(animations[n].AnimationSpecifications)-1
end