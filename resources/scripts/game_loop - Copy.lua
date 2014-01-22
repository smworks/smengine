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
	camera:setPosY(25.0)
	input = getInput()
	networkManager = getNetworkManager()
	-- Create shader for models.
	shader = Shader.new("default_model")
	-- Create ground plane.
	groundModel = Model.new("groundPlane", "plane")
	groundModel:setShader(shader)
	groundModel:setScaleXZ(511.0, 511.0)
	groundModel:setMass(0.0)
	-- Create cube.
	--cube = Model.new("player", "cube", 10.0, 10.0, 10.0)
	-- Make camera follow ball.
	--camera:follow(sphere)
	--camera:lookAt(sphere)
    -- Play sound.
    playSound("memories.mp3", true)
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
	multiplier = getTimeDelta() / 10.0
	sign = 1.0
	if input:keyPressed(constants["CTRL"]) then
		multiplier = multiplier * 10.0
		sign = -1.0
	end
	if input:keyPressed(constants["UP"]) or input:keyPressed(constants["W"]) then
		camera:moveZ(-0.1 * multiplier)
		--sphere:accelerate(0.0, 0.0, -50.0)
	end
	if input:keyPressed(constants["DOWN"]) or input:keyPressed(constants["S"]) then
		camera:moveZ(0.1 * multiplier)
		--sphere:accelerate(0.0, 0.0, 50.0)
	end
	if input:keyPressed(constants["LEFT"]) or input:keyPressed(constants["A"]) then
		camera:moveX(-0.1 * multiplier)
		--sphere:accelerate(-50.0, 0.0, 0.0)
	end
	if input:keyPressed(constants["RIGHT"]) or input:keyPressed(constants["D"]) then
		camera:moveX(0.1 * multiplier)
		--sphere:accelerate(50.0, 0.0, 0.0)
	end
	if input:keyPressed(constants["ESC"]) then
		exit()
	end
	camera:addRotY(input:getPointerDeltaX())
	camera:addRotX(input:getPointerDeltaY())
end

-- Called when program is brought to background.
function pause()
	print("Called pause()")
end

-- Called when program is about to quit.
function quit()
	print("Called quit()")
end