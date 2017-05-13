function eventResponse(response)
end

function eventGUI(node, eventType)
	print ("Ok, wassup")
	loadScene(node:getName() .. ".lua")
	print("Loading script: " .. node:getName())
end

function start()
	print("Called start()")
	input = getInput()

	textButton = Button.new("text")
	textButton:setText("Sprites")
	--textButton:setBackground(Texture.new("grass_1.png"))
	textButton:setBackground("#ffffff")
	textButton:setColor("#ff00ff")
	textButton:setWidth(100.0)
	textButton:setHeight(25.0)
	textButton:setMarginLeft(100.0)
	textButton:setMarginBottom(100.0)

end

function resume()
	print("Called resume()")
end

function resize()
	print("Called resize()")
end

function update()
	if input:keyReleased(constants["N"]) then
		loadScene("second.lua")
		print "Loading second.lua"
	end
end

function pause()
	print("Called pause()")
end

function quit()
	print("Called quit()")
end