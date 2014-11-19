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
		--"abstract",
		--"abstract_2",
		--"abstract_3", -- Doesn't work on Galaxy S5, crashes on LG Optimus L3 II
		--"abstract_4",
		--"bouncing_ball",
		-- "bricks", -- Crashes on Galaxy S5
		--"bubbles",
		--"bubbles_2",
		--"bubbles_3",
		--"bubbles_4",
		--"cigarette_burn",
		-- "city", -- Crashes on LG Optimus L3 II
		--"clouds",
		-- "clouds_2", -- Crashes on Galaxy S5
		--"cobweb",
		--"fractal_2d",
		--"fractal_2d_2",
		-- "fractal_3d", -- Doesn't work on LG Optimus L3 II
		-- "fractal_3d_2", -- Crashes on LG Optimus L3 II
		-- "fractal_3d_3", -- Crashes on LG Optimus L3 II
		-- "fractal_3d_4", -- Too many constants on S5, Crashes on LG Optimus L3 II
		--"grass",
		--"grass_fields",
		--"lemons",
		--"line",
		"lines",
		--"matrix",
		"noise",
		-- "ocean",
		--"ocean_2",
		-- "octopus", -- Crashes on LG Optimus L3 II
		--"palm",
		--"smoke",
		--"space",
		--"space_2",
		--"space_3",
		--"sphere",
		"squares",
		--"sun",
		--"sun_2",
		-- "toon_fractal", -- Crashes on LG Optimus L3 II
		-- "tree", -- Crashes on LG Optimus L3 II
		--"tunnel",
		--"tunnel_2",
		--"vortex",
		-- "water", -- Crashes on LG Optimus L3 II
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
		if input:getPointerX() > width * 0.5 then
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
