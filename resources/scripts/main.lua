function eventResponse(response)
end

function eventGUI(node, eventType)
	if (node:getName() ~= "some text") then
		loadScene(node:getName() .. ".lua")
		print("Loading script: " .. node:getName())
	end	
end

function start()
	print("Called start()")
	input = getInput()
    print("Screen width in lua: " .. getScreenWidth() .. ", height: " .. getScreenHeight())
	
	textButton = Button.new("second")
	textButton:setText("Models")
    textButton:setBackground(Texture.new("tree_1.png"))
	--textButton:setBackground("#99ff9955")
	textButton:setBackgroundSelected("#00FF00FF")
	textButton:setColor("#ff00ff")
	textButton:setWidth(getScreenWidth())
    buttonHeight = getScreenHeight() * 0.2
	textButton:setPosY(200)
	textButton:setHeight(buttonHeight)
    textButton:setFontSize(buttonHeight * 0.6)

	randomText = Text.new("some text")
	randomText:setText("Hello")
	randomText:setWidth(300)
	randomText:setHeight(300)
	randomText:setPosX(400)
	randomText:setPosY(100)
	randomText:setShader(Shader.new("wallpaper_clouds_2"))
    
    fpsCounter = Button.new("text")
    fpsCounter:setColor("#FFFF00FF")
    fpsCounter:setBackground("#33333333")
	fpsCounter:setPosX(10)
	fpsCounter:setPosY(10)
    fontSize = 14
    fpsCounter:setFontSize(14)
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
	if input:keyReleased(constants["U"]) then
		fpsCounter:addPosX(0.5)
	end
	if input:keyReleased(constants["D"]) then
		fpsCounter:addPosX(-0.5)
	end 
    if input:keyReleased(constants["T"]) then
        fontSize = fontSize + 1
        fpsCounter:setFontSize(fontSize)
    end
    fpsTime = fpsTime + getTimeDelta()
	fpsCount = fpsCount + 1
    if fpsTime >= 1000 then
		print("SETTING")
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
