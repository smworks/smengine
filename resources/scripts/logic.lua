-- Called when network respoonse is available.
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
	-- Dimensions
	width = getScreenWidth()
	height = getScreenHeight()
	-- Textures
	grassTexture = Texture.new("grass_1_24.png", "rgb")
	for i = 0, 24, 1 do
		for j = 0, 10, 1 do
			local tmpSprite = Sprite.new("grass", grassTexture)
			tmpSprite:setPosXY(i * 128.0 + 64.0, j * 128.0 + 64.0)
		end
	end
	treeTexture = Texture.new("tree_1.png", "rgba")
	tree = Sprite.new("tree", treeTexture)
	tree:setPosXY(128.0 * 4 + 64.0, 128.0 * 2 + 64.0)
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
	--signIndicator:setMarginLeft(input:getPointerX())
	--signIndicator:setMarginBottom(input:getPointerY())
end

-- Called when program is brought to background.
function pause()
	print("Called pause()")
end

-- Called when program is about to quit.
function quit()
	print("Called quit()")
end