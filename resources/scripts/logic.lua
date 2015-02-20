-- Logic.lua

-- Called when network response is available.
function eventResponse(response)
--	if response:getId() == 0 then
--		picture = Texture.new("picture", response)
--		pictureSprite = Sprite.new("picture", picture)
--		pictureSprite:setPosXYZ(0.0, 0.0, 0.0)
--		pictureSprite:setScaleXYZ(128, 128, 1)
--	end
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
	db = getDB()
	-- Dimensions
	width = getScreenWidth()
	height = getScreenHeight()
	local offsetX = width * 0.5
	local offsetY = height * 0.5
	size = 128.0 * 0.5
	-- Load tiles from database into textures and
	-- create multidimensional array to represent map.
	map = {}
	local table = db:execute("SELECT * FROM map")
	for i, t in pairs(table) do
		local n = t["tile_id"] .. ".png"
		local text = getTexture(n) == nil and Texture.new(n) or getTexture(n)
		local tmpSprite = Sprite.new(n, text)
		tmpSprite:setPosXY(t["x"] * size + offsetX, t["y"] * size + offsetY)
		tmpSprite:setScaleXYZ(0.5, 0.5, 0.5)
		map[t["x"]] = {}
		map[t["x"]][t["y"]] = t["tile_type"]
	end
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
	local offset = 7.0
	if input:keyPressed(constants["W"]) then
		camera:addPosY(offset)
	end
	if input:keyPressed(constants["S"]) then
		camera:addPosY(-offset)
	end
	if input:keyPressed(constants["A"]) then
		camera:addPosX(-offset)
	end
	if input:keyPressed(constants["D"]) then
		camera:addPosX(offset)
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

function tablelength(T)
  local count = 0
  for _ in pairs(T) do count = count + 1 end
  return count
end