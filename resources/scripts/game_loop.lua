-- Called when network respoonse is available.
function eventResponse(response)
	if response:getId() == 0 then
		picture = Texture.new("picture", response)
		pictureSprite = Sprite.new("picture", picture)
		pictureSprite:setPosXYZ(0.0, 0.0, 0.0)
		pictureSprite:setScaleXYZ(128, 128, 1)
	end
end

-- Called when GUI component listeners are activated.
function eventGUI(guiElement, eventType)
	name = guiElement:getName()
	if eventType == constants["ACTION_CLICK"] then
		if name == "quit" then
			exit()
		end
	end
end

-- Handle initialization.
function start()
	print("Called start()")
	camera = getCamera()
	input = getInput()
	networkManager = getNetworkManager()
    -- Play sound.
    playSound("memories.mp3", true)
	-- Create ground plane.
	groundModel = Model.new("groundPlane", "plane")
	groundModel:setScaleXZ(256.0, 256.0)
	groundModel:setPosXZ(0.0, 0.0)
	groundModel:setMass(0.0)
	-- Send network requests.
	request = Request.new(
		"http://www.userlogos.org/files/logos/nooz/mrburns.png",
		"GET")
	request:setId(0)
	networkManager:execute(request)
	request = Request.new(
		"localhost/test/index.php",
		"POST",
		"FORM")
	request:setEntity(
		"Domain=mail.ru&Login=(login)&Password=(password)&level=0")
	request:setId(1)
	networkManager:execute(request)
	carNode = getNode("car")
	if carNode ~= nil then
		car = carNode:getVehicle()
	end
	camera:setPosXY(5.0, 5.0)
	camera:setRotXY(0.0, 0.0)
	second = 0
	counter = 0
	redShader = Shader.new("red")
end

-- Called when program is brought to foreground.
function resume()
	print("Called resume()")
end

-- Called when program screen resolution is changed.
function resize()
	print("Called resize()")
end

-- Called every frame.
function update()
	second = second + getTimeDelta()
	if second == 100 then
		counter = counter + 1
		second = 0
		local snowFlake = Model.new("snowFlake_" .. counter, "sphere", 10, 10)
		snowFlake:setPosXYZ((math.random() - 0.5) * 100.0, 100.0, (math.random() - 0.5) * 100.0)
		snowFlake:setScaleXYZ(0.1, 0.1, 0.1)
		snowFlake:setMass(1.0)
		snowFlake:setShader(redShader)
		print("Snowflake " .. counter .. " created")
	end
	multiplier = getTimeDelta() / 16.7
	if input:keyPressed(constants["UP"]) or input:keyPressed(constants["W"]) then
		car:accelerate(multiplier * 2500.0)
	end
	if input:keyPressed(constants["DOWN"]) or input:keyPressed(constants["S"]) then
		car:accelerate(multiplier * -2500.0)
	end
	if input:keyPressed(constants["LEFT"]) or input:keyPressed(constants["A"]) then
		car:turn(multiplier * 1.5)
	end
	if input:keyPressed(constants["RIGHT"]) or input:keyPressed(constants["D"]) then
		car:turn(-multiplier * 1.5)
	end
	if input:keyReleased(constants["ESC"]) then
		exit()
	end
	camera:addRotY(input:getPointerDeltaX() / 2)
	camera:addRotX(input:getPointerDeltaY() / 2)
	if carNode ~= nil then
		camera:setPosX(carNode:getPosX())
		camera:setPosY(carNode:getPosY())
		camera:setPosZ(carNode:getPosZ())
		camera:moveZ(10.0)
	end
end

-- Called when program is brought to background.
function pause()
	print("Called pause()")
end

-- Called when program is about to quit.
function quit()
	print("Called quit()")
end