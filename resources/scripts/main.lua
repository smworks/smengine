function eventResponse(response)
end

function eventGUI(node, eventType)
	loadScene(node:getName() .. ".lua")
	print("Loading script: " .. node:getName())
end

function start()
	print("Called start()")
	input = getInput()
    print("Screen width in lua: " .. getScreenWidth() .. ", height: " .. getScreenHeight())
	textButton = Button.new("second")
	textButton:setText("Models")
	--textButton:setBackground(Texture.new("tree_1.png"))
	textButton:setBackground("#99ff9955")
	textButton:setColor("#ff00ff")
	textButton:setWidth(getScreenWidth())
    buttonHeight = getScreenHeight() * 0.1
	textButton:setHeight(buttonHeight)
    textButton:setPosX(0)
    textButton:setPosY(buttonHeight * 2)
    textButton:setFontSize(buttonHeight * 0.6)

end

function resume()
	print("Called resume()")
end

function resize()
	print("Called resize()")
end

function update()
    --print("Updating scene")
	if input:keyReleased(constants["N"]) then
		loadScene("text.lua")
		print "Loading second.lua"
	end
end

function pause()
	print("Called pause()")
end

function quit()
	print("Called quit()")
end