-- Called when network respoonse is available.
function eventResponse(response)
end

-- Called when GUI component listeners are activated.
function eventGUI(guiNode, eventType)
end

-- Handle initialization.
function start()
	print("Called start()")
	input = getInput()
	-- Dimensions
	width = getScreenWidth()
	height = getScreenHeight()
	-- Shader array.
	shaderIndex = 1;
	shaders = {
		"abstract",
		"abstract_2",
		"abstract_3",
		"abstract_4",
		"abstract_5",
		"bouncing_ball",
		"bricks",
		"bubbles",
		"bubbles_2",
		"bubbles_3",
		"bubbles_4",
		"bubbles_5",
		"cigarette_burn",
		"city",
		"clouds",
		"clouds_2",
		"cobweb",
		"fractal_2d",
		"fractal_2d_2",
		"fractal_3d",
		"fractal_3d_2",
		"fractal_3d_3",
		"fractal_3d_4",
		"grass",
		"grass_fields",
		"lemons",
		"line",
		"lines",
		"matrix",
		"noise",
		"ocean",
		"ocean_2",
		"octopus",
		"palm",
		"smoke",
		"space",
		"space_2",
		"space_3",
		"sphere",
		"sun",
		"sun_2",
		"toon_fractal",
		"tree",
		"tunnel",
		"tunnel_2",
		"vortex",
		"water",
		"waves"
	}
	print("Shader count: " .. #shaders)
	-- Background
	background = Sprite.new("background_color")
	background:setScaleXYZ(width, height, 1.0)
	background:setPosXY(width / 2.0, height / 2.0)
	backgroundShader = Shader.new("wallpaper_" .. shaders[shaderIndex])
	background:setShader(backgroundShader)
	-- Message
	text = GUIText.new("text")	
	text:setColor("#FFFF00FF")
	text:setBackground("#99000099")
	text:setFontSize(12)
	text:setMarginLeft(100.0)
	text:setText("")
	-- FPS counter.
	fps = 0
	time = 0
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
	background:setScaleXYZ(width, height, 1.0)
	background:setPosXY(width / 2.0, height / 2.0)
end

-- Called every frame.
function update()
	time = time + getTimeDelta()
	if time > 1000.0 then
		text:setText("Use arrows or AD keys to navigate.\n"
			.. "Pres ALT + ENTER to enable/disable fullscreen mode\n"
			.. "Press H to show/hide text\n"
			.. "FPS: " .. fps .. "\nResolution: " .. width .. "x" .. height
			.. "\nShader: "
			.. shaders[shaderIndex])
		fps = 0
		time = 0
	end
	fps = fps + 1
	if input:keyReleased(constants["LEFT"]) or input:keyReleased(constants["A"]) then
		left()
	end
	if input:keyReleased(constants["RIGHT"]) or input:keyReleased(constants["D"]) then
		right()
	end
	if input:keyReleased(constants["H"]) then
		text:setVisibility(not text:getVisibility())
	end
	if input:keyReleased(constants["TOUCH"]) then
		if getPointerX() > width * 0.5 then
			right()
		else
			left()
		end
	end
end

function right()
	if shaderIndex == #shaders then
		shaderIndex = 1
	else
		shaderIndex = shaderIndex + 1
	end
	backgroundShader = Shader.new("wallpaper_" .. shaders[shaderIndex])
	background:setShader(backgroundShader)
	text:setText(shaders[shaderIndex])
	time = 1001
end

function left()
	if shaderIndex == 1 then
		shaderIndex = #shaders
	else
		shaderIndex = shaderIndex - 1
	end
	backgroundShader = Shader.new("wallpaper_" .. shaders[shaderIndex])
	background:setShader(backgroundShader)
	text:setText(shaders[shaderIndex])
	time = 1001
end

-- Called when program is brought to background.
function pause()
	print("Called pause()")
end

-- Called when program is about to quit.
function quit()
	print("Called quit()")
end
