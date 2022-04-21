local dynamics ={}

---lua co-routines
function DynamicBehaviour1(ObjectName)
	while true do 
	   _MoveObject(ObjectName,4.0,0.0,5.0)
	   coroutine.yield()
	   _MoveObject(ObjectName,0.0,-4.0,5.0)
	   coroutine.yield()
	   _MoveObject(ObjectName,-4.0,0.0,5.0)
	   coroutine.yield()
	   _MoveObject(ObjectName,0.0,4.0,5.0)
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
--start the coroutine
IssueNextTask("targetBox")