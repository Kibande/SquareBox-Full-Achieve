

function build()
	print("Build called from lua")
end


function onEntry()
	print("Entry called from lua")
end


function update(delatTime)
	
end


function draw()
   local time = os.clock()

   -- Calculate position values based on time
   local x = 0
   local y = 0
   local z = math.sin(time)
   local w = math.cos(time)
   desRect = { x = x, y = y, z = z, w = w }

   uvRect = { x = 1 , y = 1 , z = 1, w = 1}

	   -- Calculate color values based on time
   local currentTime = os.clock()
   local r = math.floor(math.sin(currentTime) * 127 + 128)
   local g = math.floor(math.sin(currentTime + math.pi / 2) * 127 + 128)
   local b = math.floor(math.sin(currentTime + math.pi) * 127 + 128)
   local a = 255

   color = { r = r, g = g, b = b, a = a }
   SpriteBatchDraw(desRect,uvRect,color)

end


function onExit()
	print("onExit called from lua")
end

function destroy()
	print("Destroy called from lua")
end