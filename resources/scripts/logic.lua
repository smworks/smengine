-- logic.lua

-- Called when network response is available.
function eventResponse(response)
	if response:getId() == 0 then
		local picture = Texture.new("dante", response)
		local sprite = Sprite.new("dante", picture)
		sprite:setPosXY(256.0, 256.0)
	end
    --print("Content: " .. response:getContent())
    --print("Length: " .. string.len(response:getContent()))
    --print("Got response!")
    --print("Header: " .. response:getHeader())
    --print("Id: " .. response:getId())
end

-- Called when GUI component listeners are activated.
function eventGUI(guiNode, eventType)
	name = guiNode:getName()
	if eventType == constants["ACTION_CLICK"] then
		print(guiNode:getName())
	end
end

-- Handle initialization.
function start()
	print("Called start()")
	input = getInput()
	camera = getCamera()
    networkManager = getNetworkManager()
	db = getDB()
    -- HttpRequest
--    request = Request.new("http://upload.wikimedia.org/wikipedia/en/8/84/Dante_transparent.png")
--    request2 = Request.new("http://kentas.lt/index.php")
--    request2:setId(1)
--    networkManager:execute(request)
--    networkManager:execute(request2)
	-- Dimensions
--	width = getScreenWidth()
--	height = getScreenHeight()
--	local offsetX = width * 0.5
--	local offsetY = height * 0.5
--	size = 128.0 * 0.5
	-- Load tiles from database into textures and
	-- create multidimensional array to represent map.
--	map = {}
--	local table = db:execute("SELECT * FROM map")
--	for i, t in pairs(table) do
--		local n = t["texture"] .. ".png"
--		local text = getTexture(n) == nil and Texture.new(n) or getTexture(n)
--		local tmpSprite = Sprite.new(n, text)
--		tmpSprite:setPosXYZ(t["x"] * size + offsetX, t["y"] * size + offsetY, t["z"])
--		tmpSprite:setScaleXYZ(0.5, 0.5, 0.5)
--		map[t["x"]] = {}
--		map[t["x"]][t["y"]] = t["type"]
--	end

    plane = Model.new("floor")
    plane:setType("plane")
    plane:setAmbient("#FFFFFFFF")
    plane:setShader(Shader.new("temp_model"))
    plane:addTexture(Texture.new("lab_floor.png"))
    plane:setPosY(-2.0)
    plane:setScaleXYZ(100.0, 100.0, 100.0)

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
--	if input:keyReleased(constants["W"]) then
--		camera:addPosY(size)
--	end
--	if input:keyReleased(constants["S"]) then
--		camera:addPosY(-size)
--	end
--	if input:keyReleased(constants["A"]) then
--		camera:addPosX(-size)
--	end
--	if input:keyReleased(constants["D"]) then
--		camera:addPosX(size)
--	end

    camera:addRotX(input:getPointerDeltaY());
    camera:addRotY(input:getPointerDeltaX());

	local offset = 1.0

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

    offset = 14.0

	if input:keyPressed(constants["I"]) then
		camera:addPosY(-offset)
	end
	if input:keyPressed(constants["K"]) then
		camera:addPosY(offset)
	end
	if input:keyPressed(constants["J"]) then
		camera:addPosX(offset)
	end
	if input:keyPressed(constants["L"]) then
		camera:addPosX(-offset)
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