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
function eventGUI(guiElement, eventType)
	print("AFKJDSLKFJLKSFJ")
	print(guiElement:getName())
	name = guiElement:getName()
	if eventType == constants["ACTION_CLICK"] then
		if name == "buttonnode" then
			exit()
		end
	end
end

-- Handle initialization.
function start()
	print("Called start()")
	input = getInput()

	width = getScreenWidth()
	height = getScreenHeight()

	if width > height then
		edge = height / 3.0
	else
		edge = width / 3.0
	end

	topLeft = Button.new("topleft", edge, edge)
	topMiddle = Button.new("topmiddle", edge, edge)
	topRight = Button.new("topright", edge, edge)
	middleLeft = Button.new("middleleft", edge, edge)
	middle = Button.new("middle", edge, edge)
	middleRight = Button.new("middleright", edge, edge)
	bottomLeft = Button.new("bottomleft", edge, edge)
	bottomMiddle = Button.new("bottommiddle", edge, edge)
	bottomRight = Button.new("bottomright", edge, edge)
	
	--topleft:setColor("#ff0000")
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

end

-- Called when program is brought to background.
function pause()
	print("Called pause()")
end

-- Called when program is about to quit.
function quit()
	print("Called quit()")
end