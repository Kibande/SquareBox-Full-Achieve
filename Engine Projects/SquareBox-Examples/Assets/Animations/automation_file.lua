local dynamics ={}

---lua co-routines
function DynamicBehaviour1(ObjectName)
	while true do 
	   _MoveObject(ObjectName,100.0,0.0,500.0)
	   coroutine.yield()
	   _MoveObject(ObjectName,0.0,-100.0,500.0)
	   coroutine.yield()
	   _MoveObject(ObjectName,-100.0,0.0,500.0)
	   coroutine.yield()
	   _MoveObject(ObjectName,0.0,100.0,500.0)
	   coroutine.yield()
	end
end


function DynamicBehaviour2(ObjectName)
	while true do 
	   _ChangeObjectTexture(ObjectName,0,2,50.0)
	   coroutine.yield()
	   _ChangeObjectTexture(ObjectName,2,2,50.0)
	   coroutine.yield()
	   _ChangeObjectTexture(ObjectName,4,2,50.0)
	   coroutine.yield()
	   _ChangeObjectTexture(ObjectName,6,2,50.0)
	   coroutine.yield()
	end
end

function IssueNextTask(ObjectName)
	if coroutine.status(dynamics[ObjectName].behaviour) ~= 'dead' then 
	print("Next Task Issued ")
	   coroutine.resume(dynamics[ObjectName].behaviour,ObjectName)
	end
end

--create the coroutine 
dynamics["targetBox"]={behaviour=coroutine.create(DynamicBehaviour1)}
dynamics["house"]={behaviour=coroutine.create(DynamicBehaviour2)}
--start the coroutine
IssueNextTask("targetBox")
IssueNextTask("house")