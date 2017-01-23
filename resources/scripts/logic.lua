-- logic.lua

-- Called when network response is available.
function eventResponse(response)
end

-- Called when GUI component listeners are activated.
function eventGUI(guiNode, eventType)
end

-- Handle initialization.
function start()
	print("Called start()")
	input = getInput()
	camera = getCamera()
    networkManager = getNetworkManager()
	db = getDB()

    plane = Model.new("floor")
    plane:setType("plane")
    plane:setAmbient("#FFFFFFFF")
    plane:setShader(Shader.new("temp_model"))
    plane:setTexture(Texture.new("lab_floor.png"))
    plane:setPosY(0.0)
    plane:setScaleXYZ(20.0, 1.0, 20.0)
    plane:enablePhysics()

	print "Loading cube"
	pun = Model.new("colored_cube")
	pun:setPosY(1.0)
	pun:setPosZ(-7.0)
	pun:setShader(Shader.new("default_model"))
	pun:setString("file", "cube.sm")
	pun:setType("mesh")
	pun:setRotZ(90)
	--pun:setScaleXYZ(3, 3, 3)
	pun:enablePhysics()

	print "Loading pun"
	pun = Model.new("pun")
	pun:setPosY(1.0)
	pun:setPosX(7.0)
	pun:setShader(Shader.new("default_model"))
	pun:setString("file", "pun.sm")
	pun:setType("mesh")
	pun:setRotZ(90)

	print "Loading elf"
	pun = Model.new("elf")
	pun:setPosY(1.0)
	pun:setPosX(-7.0)
	pun:setShader(Shader.new("default_model"))
	pun:setString("file", "elf.sm")
	pun:setString("cull_face", "false");
	pun:setType("mesh")

	print "Loading car_body"
	pun = Model.new("car_body")
	pun:setPosY(1.0)
	pun:setPosZ(-1.0)
	pun:setShader(Shader.new("default_model"))
	pun:setString("file", "car_body.sm")
	pun:setType("mesh")

	camera:setPosZ(5)
	camera:setPosY(1)
end

-- Called when program is brought to foreground.
function resume()
	print("Called resume()")
end

-- Called when program screen resolution is changed.
function resize()
	print("Called resize()")
	width = getScreenWidth()
	height = getScreenHeight()
end

-- Called every frame.
function update()
	local offset = 0.1
	if input:keyPressed(constants["W"]) then
		camera:moveZ(-offset)
	end
	if input:keyPressed(constants["S"]) then
		camera:moveZ(offset)
	end
	if input:keyPressed(constants["A"]) then
		camera:moveX(-offset)
	end
	if input:keyPressed(constants["D"]) then
		camera:moveX(offset)
	end

    camera:addRotX(input:getPointerDeltaY() * 0.2)
    camera:addRotY(input:getPointerDeltaX() * 0.2)
end

-- Called when program is brought to background.
function pause()
	print("Called pause()")
end

-- Called when program is about to quit.
function quit()
	print("Called quit()")
end