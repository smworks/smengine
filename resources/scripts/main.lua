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
    
    fpsCounter = Button.new("fpsCounter")
    fpsCounter:setColor("#FFFF00FF")
    fpsCounter:setBackground("#33333333")
    fontSize = 14
    fpsCounter:setFontSize(14)
    fpsCounter:setWidth(256)
    fpsCounter:setHeight(64)
    fpsCount = 0
    fpsTime = 0
    
    texture = Texture.new("smworks.png")
    sprite = Sprite.new("Sprite name")
    sprite:setWidth(200);
    sprite:setHeight(200);
    sprite:setPosX(300)
    sprite:setPosY(300)
    sprite:setTexture(texture)
    

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
    if input:keyReleased(constants["T"]) then
        fontSize = fontSize + 1
        fpsCounter:setFontSize(fontSize)
    end
    fpsTime = fpsTime + getTimeDelta()
    if fpsTime >= 1000 then
        fpsCounter:setText("FPS: " .. fpsCount .. "\nResolution: " .. getScreenWidth() .. "x" .. getScreenHeight() .. "px")
        fpsCount = 0
        fpsTime = 0
    end
end

function pause()
	print("Called pause()")
end

function quit()
	print("Called quit()")
end